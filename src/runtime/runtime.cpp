

#include "runtime/runtime.hpp"

#include "jit.hpp"
#include "component_manager.hpp"
#include "database.hpp"
#include "code_generator.hpp"

#include "service/protocol/common/project.hpp"
#include "service/network/log.hpp"

#include "mega/common.hpp"
#include "mega/root.hpp"

#include "common/assert_verify.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <array>
//#include <unordered_map>

std::array< Root*, mega::MAX_SIMULATIONS > m_roots;

namespace mega
{
namespace runtime
{
namespace
{

void* dummy_read( mega::ExecutionContext& executionContext, const mega::reference& )
{
    static int x = 456;
    return &x;
}


class Runtime
{
public:
    void reinitialise( const mega::network::MegastructureInstallation& megastructureInstallation,
                       const mega::network::Project&                   project )
    {
        if ( !m_componentManager )
        {
            m_componentManager = std::make_unique< ComponentManager >();
        }
        if ( !m_pJITCompiler )
        {
            m_pJITCompiler = std::make_unique< JITCompiler >();
        }
        if ( !m_pCodeGenerator )
        {
            m_pCodeGenerator = std::make_unique< CodeGenerator >( megastructureInstallation, project );
        }

        m_database.reset();

        try
        {
            if ( !project.isEmpty() )
            {
                if ( boost::filesystem::exists( project.getProjectDatabase() ) )
                {
                    m_database.reset( new DatabaseInstance( project.getProjectDatabase() ) );
                    SPDLOG_INFO(
                        "ComponentManager initialised with project: {}", project.getProjectInstallPath().string() );
                    m_bInitialised = true;
                }
            }
        }
        catch ( mega::io::DatabaseVersionException& ex )
        {
            m_bInitialised = false;
            SPDLOG_ERROR( "ComponentManager failed to initialise project: {} database version exception: {}",
                          project.getProjectInstallPath().string(), ex.what() );
        }
        catch ( std::exception& ex )
        {
            m_bInitialised = false;
            SPDLOG_ERROR( "ComponentManager failed to initialise project: {} error: {}",
                          project.getProjectInstallPath().string(), ex.what() );
        }
    }

    ::Root* allocateRoot( mega::ExecutionContext& executionContext, const mega::network::ConversationID& simulationID )
    {
        mega::reference ref   = {};
        ::Root*         pRoot = new ::Root( ref );

        // const std::string strBufferName = executionContext.mapBuffer( ref );
        // executionContext

        return pRoot;
    }

    void releaseRoot( mega::ExecutionContext& executionContext, ::Root* pRoot )
    {
        //
        delete pRoot;
    }

    void get_allocate( const char* pszUnitName, mega::ExecutionContext& executionContext,
                       const mega::InvocationID& invocation, AllocateFunction* ppFunction )
    {
        VERIFY_RTE_MSG( m_bInitialised, "Runtime not initialised" );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        /*{
            mega::reference   ref           = {};
            const std::string strBufferName = executionContext.mapBuffer( ref );
        }*/

        JITCompiler::Module::Ptr pModule;
        {
            auto iFind = m_invocations.find( invocation );
            if ( iFind != m_invocations.end() )
            {
                pModule = iFind->second;
            }
            else
            {
                std::ostringstream osModule;
                m_pCodeGenerator->generate_allocate( *m_database, invocation, osModule );
                pModule = m_pJITCompiler->compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocation, pModule ) );
            }
        }

        // _Z22ct1pt1__eg_ImpNoParamsRN4mega16ExecutionContextERKNS_9referenceE
        std::ostringstream os;
        os << "_Z22" << invocation << "RN4mega16ExecutionContextERKNS_9referenceE";
        *ppFunction = pModule->getAllocate( os.str() );
    }

    void get_read( const char* pszUnitName, mega::ExecutionContext& executionContext,
                   const mega::InvocationID& invocation, ReadFunction* ppFunction )
    {
        VERIFY_RTE_MSG( m_bInitialised, "Runtime not initialised" );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        JITCompiler::Module::Ptr pModule;
        {
            auto iFind = m_invocations.find( invocation );
            if ( iFind != m_invocations.end() )
            {
                pModule = iFind->second;
            }
            else
            {
                std::ostringstream osModule;
                m_pCodeGenerator->generate_read( *m_database, invocation, osModule );
                pModule = m_pJITCompiler->compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocation, pModule ) );
            }
        }
        // _Z23ct1ps12__eg_ImpNoParamsRKN4mega9referenceE
        std::ostringstream os;
        os << "_Z23" << invocation << "RN4mega16ExecutionContextERKNS_9referenceE";
        *ppFunction = pModule->getRead( os.str() );
    }

private:
    std::unique_ptr< CodeGenerator >    m_pCodeGenerator;
    std::unique_ptr< JITCompiler >      m_pJITCompiler;
    ComponentManager::Ptr               m_componentManager;
    std::unique_ptr< DatabaseInstance > m_database;
    bool                                m_bInitialised = false;

    // use unordered_map
    using InvocationMap = std::map< mega::InvocationID, JITCompiler::Module::Ptr >;
    InvocationMap m_invocations;

    using FunctionPtrMap = std::multimap< const char*, void* >;
    FunctionPtrMap m_functionPointers;
};

Runtime& getStaticRuntime()
{
    static Runtime runtime;
    return runtime;
}

} // namespace
} // namespace runtime
} // namespace mega

namespace mega
{
namespace runtime
{

void initialiseRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                        const mega::network::Project&                   project )
{
    getStaticRuntime().reinitialise( megastructureInstallation, project );
}

::Root* allocateRoot( mega::ExecutionContext& executionContext, const mega::network::ConversationID& simulationID )
{
    return getStaticRuntime().allocateRoot( executionContext, simulationID );
}
void releaseRoot( mega::ExecutionContext& executionContext, ::Root* pRoot )
{
    getStaticRuntime().releaseRoot( executionContext, pRoot );
}

void get_allocate( const char* pszUnitName, mega::ExecutionContext& executionContext,
                   const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
{
    getStaticRuntime().get_allocate( pszUnitName, executionContext, invocationID, ppFunction );
}
void get_read( const char* pszUnitName, mega::ExecutionContext& executionContext,
               const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    getStaticRuntime().get_read( pszUnitName, executionContext, invocationID, ppFunction );
}

} // namespace runtime
} // namespace mega
