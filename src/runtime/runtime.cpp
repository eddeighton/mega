

#include "runtime/runtime.hpp"

#include "jit.hpp"
#include "component_manager.hpp"
#include "database.hpp"
#include "code_generator.hpp"

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

    void get_allocate( const char* pszUnitName, mega::ExecutionContext executionContext,
                      const mega::InvocationID& invocation, AllocateFunction* ppFunction )
    {
        VERIFY_RTE_MSG( m_bInitialised, "Runtime not initialised" );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        auto iFind = m_invocations.find( invocation );
        if ( iFind != m_invocations.end() )
        {
            //*ppFunction = iFind->second->getAllocate( "_Z8testFuncRKN4mega9referenceE" );
        }
        else
        {
            std::ostringstream osModule;
            m_pCodeGenerator->generate_allocate( *m_database, invocation, osModule );

            JITCompiler::Module::Ptr pModule = m_pJITCompiler->compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocation, pModule ) );

            //*ppFunction = pModule->getAllocate( "_Z8testFuncRKN4mega9referenceE" );
        }
    }

    void get_read( const char* pszUnitName, mega::ExecutionContext executionContext,
                      const mega::InvocationID& invocation, ReadFunction* ppFunction )
    {
        VERIFY_RTE_MSG( m_bInitialised, "Runtime not initialised" );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        std::ostringstream os;
        //os << invocation << "(mega::reference const&)";
        //os << invocation;
        //os << "_Z24ct15ps10__eg_ImpNoParamsRKN4mega9referenceE";
        os << "_Z24" << invocation << "RKN4mega9referenceE";

        auto iFind = m_invocations.find( invocation );
        if ( iFind != m_invocations.end() )
        {
            *ppFunction = iFind->second->getRead( os.str() );
        }
        else
        {
            std::ostringstream osModule;
            m_pCodeGenerator->generate_read( *m_database, invocation, osModule );

            JITCompiler::Module::Ptr pModule = m_pJITCompiler->compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocation, pModule ) );

            *ppFunction = pModule->getRead( os.str() );
        }
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

extern void get_allocate( const char* pszUnitName, mega::ExecutionContext executionContext,
                          const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
{
    getStaticRuntime().get_allocate( pszUnitName, executionContext, invocationID, ppFunction );
}
extern void get_read( const char* pszUnitName, mega::ExecutionContext executionContext,
                      const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    getStaticRuntime().get_read( pszUnitName, executionContext, invocationID, ppFunction );
}

} // namespace runtime
} // namespace mega
