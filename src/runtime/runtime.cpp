

#include "runtime/runtime.hpp"

#include "jit.hpp"
#include "component_manager.hpp"
#include "database.hpp"

#include "service/protocol/common/project.hpp"
#include "service/network/log.hpp"

#include "mega/common.hpp"

#include "common/assert_verify.hpp"

#include <iostream>
#include <sstream>
#include <map>
//#include <unordered_map>

namespace mega
{
namespace runtime
{
namespace
{

class Runtime
{
public:
    void reinitialise( const mega::network::Project& project )
    {
        if ( !m_componentManager )
        {
            m_componentManager = std::make_unique< ComponentManager >();
        }
        if ( !m_pJITCompiler )
        {
            m_pJITCompiler = std::make_unique< JITCompiler >();
        }

        m_database.reset();

        try
        {
            if ( boost::filesystem::exists( project.getProjectDatabase() ) )
            {
                m_database.reset( new DatabaseInstance( project.getProjectDatabase() ) );
                SPDLOG_INFO(
                    "ComponentManager initialised with project: {}", project.getProjectInstallPath().string() );
                m_bInitialised = true;
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

    void resolveRead( const char* pszUnitName, mega::ExecutionContext executionContext, 
            const mega::InvocationID& invocation, ReadFunction* ppFunction )
    {
        VERIFY_RTE_MSG( m_bInitialised, "Runtime not initialised" );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        auto iFind = m_invocations.find( invocation );
        if ( iFind != m_invocations.end() )
        {
            *ppFunction = reinterpret_cast< ReadFunction >( iFind->second );
        }
        else
        {
            ReadFunction pFunction = m_pJITCompiler->compile_read( *m_database, invocation );
            m_invocations.insert( std::make_pair( invocation, ( void* )pFunction ) );
            *ppFunction = pFunction;
        }
    }

private:
    std::unique_ptr< JITCompiler >      m_pJITCompiler;
    ComponentManager::Ptr               m_componentManager;
    std::unique_ptr< DatabaseInstance > m_database;
    bool                                m_bInitialised = false;

    // use unordered_map
    using InvocationMap = std::map< mega::InvocationID, void* >;
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

void initialiseRuntime( const mega::network::Project& project ) { getStaticRuntime().reinitialise( project ); }

__attribute__((used)) extern void get_read( const char* pszUnitName, mega::ExecutionContext executionContext,
               const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    getStaticRuntime().resolveRead( pszUnitName, executionContext, invocationID, ppFunction );
}

} // namespace runtime
} // namespace mega
