

#include "runtime/runtime.hpp"
#include "runtime/runtime_api.hpp"
#include "runtime/component_manager.hpp"

#include "service/protocol/common/project.hpp"

#include "service/network/log.hpp"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/common/manifest_data.hpp"

#include "database/types/invocation_id.hpp"

#include "database/model/FinalStage.hxx"
#include "database/model/manifest.hxx"

#include "mega/common.hpp"

#include "common/assert_verify.hpp"

#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"

#include "llvm/Support/TargetSelect.h"

#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "llvm/Target/TargetMachine.h"

#include <iostream>
#include <sstream>

namespace
{

class Runtime
{
    class DatabaseInstance
    {
    public:
        DatabaseInstance( const boost::filesystem::path& projectDatabasePath )
            : m_environment( projectDatabasePath )
            , m_manifest( m_environment, m_environment.project_manifest() )
            , m_database( m_environment, m_manifest.getManifestFilePath() )
            , m_components( m_database.many< FinalStage::Components::Component >( m_manifest.getManifestFilePath() ) )
            , m_pSymbolTable( m_database.one< FinalStage::Symbols::SymbolTable >( m_manifest.getManifestFilePath() ) )
        {
        }

        const FinalStage::Operations::Invocation* getInvocation( const mega::invocation::ID& invocation )
        {
            using namespace FinalStage;
            using InvocationMap = std::map< mega::invocation::ID, Operations::Invocation* >;

            for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
            {
                const Operations::Invocations* pInvocations
                    = m_database.one< Operations::Invocations >( sourceFilePath );
                const InvocationMap invocations = pInvocations->get_invocations();
                InvocationMap::const_iterator iFind = invocations.find( invocation );
                if ( iFind != invocations.end() )
                {
                    const Operations::Invocation* pInvocation = iFind->second;
                    return pInvocation;
                }
            }
            for ( const mega::io::cppFilePath& sourceFilePath : m_manifest.getCppSourceFiles() )
            {
                const Operations::Invocations* pInvocations
                    = m_database.one< Operations::Invocations >( sourceFilePath );
                const InvocationMap invocations = pInvocations->get_invocations();
                InvocationMap::const_iterator iFind = invocations.find( invocation );
                if ( iFind != invocations.end() )
                {
                    const Operations::Invocation* pInvocation = iFind->second;
                    return pInvocation;
                }
            }
            THROW_RTE( "Failed to resolve invocation: " << invocation );
        }

    private:
        mega::io::ArchiveEnvironment                      m_environment;
        mega::io::Manifest                                m_manifest;
        FinalStage::Database                              m_database;
        std::vector< FinalStage::Components::Component* > m_components;
        FinalStage::Symbols::SymbolTable*                 m_pSymbolTable;
    };

public:
    Runtime()
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
    }

    void reinitialise( const mega::network::Project& project )
    {
        if ( !m_componentManager )
        {
            m_componentManager = std::make_unique< mega::runtime::ComponentManager >();
        }

        try
        {
            m_database.reset();
            m_database.reset( new DatabaseInstance( project.getProjectDatabase() ) );

            SPDLOG_INFO( "ComponentManager initialised with project: {}", project.getProjectInstallPath().string() );

            m_bInitialised = true;
        }
        catch ( mega::io::DatabaseVersionException& ex )
        {
            SPDLOG_ERROR( "ComponentManager failed to initialise project: {} database version exception: {}",
                          project.getProjectInstallPath().string(), ex.what() );
        }
        catch ( std::exception& ex )
        {
            SPDLOG_ERROR( "ComponentManager failed to initialise project: {} error: {}",
                          project.getProjectInstallPath().string(), ex.what() );
        }
    }

    void* resolveInvocationImpl( const mega::invocation::ID& invocation )
    {
        VERIFY_RTE_MSG( m_bInitialised, "Runtime not initialised" );

        const FinalStage::Operations::Invocation* pInvocation = m_database->getInvocation( invocation );

        void* pFunction = nullptr;

        return pFunction;
    }

private:
    llvm::LLVMContext                    m_llvmContext;
    std::unique_ptr< DatabaseInstance >  m_database;
    mega::runtime::ComponentManager::Ptr m_componentManager;
    bool                                 m_bInitialised = false;
};

Runtime& getStaticRuntime()
{
    static Runtime runtime;
    return runtime;
}

} // namespace

namespace mega
{
namespace runtime
{

void initialiseRuntime( const mega::network::Project& project ) { getStaticRuntime().reinitialise( project ); }

} // namespace runtime
} // namespace mega

extern "C"
{
    const int& madeUpFunction( mega::reference data )
    {
        std::cout << "madeUpFunction called" << std::endl;
        static int i = 321;
        return i;
    }

    void* resolveOperation( int typeID )
    {
        //
        return nullptr;
    }

    void resolveInvocation( mega::TypeID typeID, const mega::TypeID* typePath, mega::TypeID typePathSize,
                            mega::TypeID operationID, void** ppFunction )
    {
        const mega::SymbolID*      pIter = reinterpret_cast< const mega::SymbolID* >( typePath );
        const mega::invocation::ID invocationID{ { static_cast< mega::SymbolID >( typeID ) },
                                                 std::vector< mega::SymbolID >( pIter, pIter + typePathSize ),
                                                 static_cast< mega::OperationID >( operationID ) };
        *ppFunction = getStaticRuntime().resolveInvocationImpl( invocationID );

        //
        std::cout << "Setting function pointer" << std::endl;
        *ppFunction = ( void* )&madeUpFunction;
    }
}
