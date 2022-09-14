

#include "jit.hpp"

#include "common/string.hpp"
#include "common/assert_verify.hpp"

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

namespace mega
{
namespace runtime
{
llvm::ExitOnError ExitOnErr;

JITCompiler::Module::~Module() {}

class ModuleImpl : public JITCompiler::Module
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
        if ( auto M = parseIR( MemoryBufferRef( Source, Name ), Err, *Ctx ) )
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

        auto module = ExitOnErr( parseModule( strModule, m_name ) );
        ExitOnErr( m_jit.addIRModule( m_jitDynLib, std::move( module ) ) );
    }

    ~ModuleImpl() { ExitOnErr( m_jit.getExecutionSession().removeJITDylib( m_jitDynLib ) ); }
    /*
        std::string mangleName( llvm::StringRef UnmangledName )
        {
            std::string MangledName;
            {
                llvm::raw_string_ostream MangledNameStream( MangledName );
                llvm::Mangler::getNameWithPrefix( MangledNameStream, UnmangledName, m_jit.getDataLayout() );
            }
            return MangledName;
        }

        llvm::JITTargetAddress findSymbolAddress( const std::string& strSymbol )
        {
            auto&                          executionSession = m_jit.getExecutionSession();
            llvm::orc::SymbolStringPtr     pNamePtr         = executionSession.intern( mangleName( strSymbol ) );

            llvm::orc::JITDylibSearchOrder jitDynListSearchList{
                std::pair< llvm::orc::JITDylib*, llvm::orc::JITDylibLookupFlags >{
                    &m_jitDynLib, llvm::orc::JITDylibLookupFlags::MatchAllSymbols } };

            llvm::JITEvaluatedSymbol jitEvaluatedSymbol
                = ExitOnErr( executionSession.lookup( jitDynListSearchList, pNamePtr ) );
            VERIFY_RTE( jitEvaluatedSymbol );

            llvm::JITTargetAddress jitAddress = jitEvaluatedSymbol.getAddress();
            VERIFY_RTE( jitAddress );
            return jitAddress;
        }
    */
    template < typename TFunctionPtrType >
    TFunctionPtrType getFunctionPtr( const std::string& strSymbol )
    {
        auto functionPtr = ExitOnErr( m_jit.lookup( m_jitDynLib, strSymbol ) );
        VERIFY_RTE( functionPtr );

        TFunctionPtrType pResultFunction = functionPtr.toPtr< TFunctionPtrType >();
        VERIFY_RTE( pResultFunction );
        return pResultFunction;
    }

    virtual mega::runtime::GetHeapFunction getGetHeap( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::GetHeapFunction >( strSymbol );
    }

    virtual mega::runtime::GetSharedFunction getGetShared( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::GetSharedFunction >( strSymbol );
    }

    virtual mega::runtime::SetAllocatorFunction getSetAllocator( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::SetAllocatorFunction >( strSymbol );
    }
    virtual mega::runtime::SharedCtorFunction getSharedCtor( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::SharedCtorFunction >( strSymbol );
    }
    virtual mega::runtime::SharedDtorFunction getSharedDtor( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::SharedDtorFunction >( strSymbol );
    }
    virtual mega::runtime::HeapCtorFunction getHeapCtor( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::HeapCtorFunction >( strSymbol );
    }
    virtual mega::runtime::HeapDtorFunction getHeapDtor( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::HeapDtorFunction >( strSymbol );
    }

    virtual mega::runtime::AllocateFunction getAllocate( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::AllocateFunction >( strSymbol );
    }

    virtual mega::runtime::ReadFunction getRead( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::ReadFunction >( strSymbol );
    }

    virtual mega::runtime::WriteFunction getWrite( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::WriteFunction >( strSymbol );
    }

    virtual mega::runtime::CallFunction getCall( const std::string& strSymbol ) override
    {
        return getFunctionPtr< mega::runtime::CallFunction >( strSymbol );
    }

private:
    const std::string    m_name;
    llvm::orc::LLJIT&    m_jit;
    llvm::orc::JITDylib& m_jitDynLib;
};

JITCompiler::StaticInit::StaticInit()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}
JITCompiler::StaticInit JITCompiler::m_staticInit;

class JITCompiler::Pimpl
{
public:
    Pimpl()
        : m_pLLJit( ExitOnErr( llvm::orc::LLJITBuilder().create() ) )
    {
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

    std::unique_ptr< llvm::orc::LLJIT > m_pLLJit;
    std::set< Module* >                 m_modules;
};

JITCompiler::JITCompiler()
    : m_pPimpl( std::make_shared< Pimpl >() )
{
}

JITCompiler::Module::Ptr JITCompiler::compile( const std::string& strModule ) { return m_pPimpl->compile( strModule ); }

} // namespace runtime
} // namespace mega
