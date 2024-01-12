//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "runtime/orc.hpp"

#include "common/string.hpp"
#include "common/assert_verify.hpp"
#include "common/clang_warnings.hpp"

#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"

#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"

#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"

#include "llvm/IRReader/IRReader.h"

#include "llvm/Target/TargetMachine.h"

#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/SourceMgr.h"

namespace mega::runtime
{
namespace
{
struct StaticInit
{
    StaticInit()
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
    }
};
static StaticInit m_staticInit;
} // namespace

llvm::ExitOnError ExitOnErr;

Orc::Module::~Module() = default;

class ModuleImpl : public Orc::Module
{
    // A function object that creates a simple pass pipeline to apply to each
    // module as it passes through the IRTransformLayer.
    /*class MyOptimizationTransform
    {
    public:
        MyOptimizationTransform()
            : PM( std::make_unique< llvm::legacy::PassManager >() )
        {
            PM->add( llvm::createTailCallEliminationPass() );
            PM->add( llvm::createFunctionInliningPass() );
            PM->add( llvm::createIndVarSimplifyPass() );
            PM->add( llvm::createCFGSimplificationPass() );
        }
        llvm::Expected< llvm::orc::ThreadSafeModule > operator()( llvm::orc::ThreadSafeModule               TSM,
                                                                  llvm::orc::MaterializationResponsibility& R )
        {
            TSM.withModuleDo( [ this ]( llvm::Module& M ) { PM->run( M ); } );
            return std::move( TSM );
        }
    private:
        std::unique_ptr< llvm::legacy::PassManager > PM;
    };*/

    llvm::Error createSMDiagnosticError( llvm::SMDiagnostic& Diag )
    {
        using namespace llvm;
        std::string Msg;
        {
            raw_string_ostream OS( Msg );
            Diag.print( "", OS );
        }
        return make_error< StringError >( std::move( Msg ), inconvertibleErrorCode() );
    }

    llvm::Expected< llvm::orc::ThreadSafeModule > parseModule( llvm::StringRef Source, llvm::StringRef Name )
    {
        using namespace llvm;
        auto         Ctx = std::make_unique< LLVMContext >();
        SMDiagnostic Err;
        if( auto M = parseIR( MemoryBufferRef( Source, Name ), Err, *Ctx ) )
        {
            return orc::ThreadSafeModule( std::move( M ), std::move( Ctx ) );
        }
        return createSMDiagnosticError( Err );
    }

public:
    ModuleImpl( llvm::orc::LLJIT& jit, const std::string& strModule )
        : m_name( common::uuid() )
        , m_jit( jit )
        , m_jitDynLib( ExitOnErr( m_jit.createJITDylib( m_name ) ) )
    {
        // m_jit.getIRTransformLayer().setTransform( MyOptimizationTransform() );

        m_jitDynLib.addGenerator( ExitOnErr( llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
            m_jit.getDataLayout().getGlobalPrefix() ) ) );

        auto jitModule = ExitOnErr( parseModule( strModule, m_name ) );
        ExitOnErr( m_jit.addIRModule( m_jitDynLib, std::move( jitModule ) ) );
    }

    ~ModuleImpl() { ExitOnErr( m_jit.getExecutionSession().removeJITDylib( m_jitDynLib ) ); }

    virtual void* getRawFunctionPtr( const std::string& strSymbol )
    {
        auto functionPtr = ExitOnErr( m_jit.lookup( m_jitDynLib, strSymbol ) );
        VERIFY_RTE_MSG( functionPtr, "Failed to locate symbol: " << strSymbol );
        return reinterpret_cast< void* >( functionPtr.getValue() );
    }

private:
    const std::string    m_name;
    llvm::orc::LLJIT&    m_jit;
    llvm::orc::JITDylib& m_jitDynLib;
};

class Orc::Pimpl
{
public:
    Pimpl( const mega::MegastructureInstallation& megastructureInstallation )
        : m_megastructureInstallation( megastructureInstallation )
        , m_pLLJit( ExitOnErr( llvm::orc::LLJITBuilder().create() ) )
    {
        // ensure symbols available for megastructure libs
        /*{
            m_pLLJit->getMainJITDylib().addGenerator( ExitOnErr( llvm::orc::DynamicLibrarySearchGenerator::Load(
                m_megastructureInstallation.getToolchain().megaManglePath.string().c_str(),
                m_pLLJit->getDataLayout().getGlobalPrefix() ) ) );

            m_pLLJit->getMainJITDylib().addGenerator( ExitOnErr( llvm::orc::DynamicLibrarySearchGenerator::Load(
                m_megastructureInstallation.getToolchain().leafPath.string().c_str(),
                m_pLLJit->getDataLayout().getGlobalPrefix() ) ) );
        }*/

        m_pLLJit->getMainJITDylib().addGenerator(
            ExitOnErr( llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
                m_pLLJit->getDataLayout().getGlobalPrefix() ) ) );
    }

    std::shared_ptr< ModuleImpl > compile( const std::string& strModule )
    {
        Pimpl*                        pThis = this;
        std::shared_ptr< ModuleImpl > pModule(
            new ModuleImpl( *m_pLLJit, strModule ), [ pThis ]( Module* pModule ) { pThis->unload( pModule ); } );
        m_modules.insert( pModule.get() );
        return pModule;
    }

    void unload( Module* pModule )
    {
        auto iFind = m_modules.find( pModule );
        VERIFY_RTE( iFind != m_modules.end() );
        m_modules.erase( pModule );
        delete pModule;
    }

    mega::MegastructureInstallation     m_megastructureInstallation;
    std::unique_ptr< llvm::orc::LLJIT > m_pLLJit;
    std::set< Module* >                 m_modules;
};

Orc::Orc( const mega::MegastructureInstallation& megastructureInstallation )
    : m_pPimpl( std::make_shared< Pimpl >( megastructureInstallation ) )
{
}

Orc::Module::Ptr Orc::compile( const std::string& strModule )
{
    return m_pPimpl->compile( strModule );
}

} // namespace mega::runtime
