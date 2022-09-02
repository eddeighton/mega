

#include "runtime/runtime.hpp"

#include "jit.hpp"
#include "component_manager.hpp"
#include "database.hpp"
#include "code_generator.hpp"

#include "service/protocol/common/project.hpp"
#include "service/network/log.hpp"

#include "mega/common.hpp"
#include "mega/execution_context.hpp"
#include "mega/root.hpp"

#include "common/assert_verify.hpp"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <iostream>
#include <sstream>
#include <map>
#include <array>
#include <unordered_map>

namespace mega
{
namespace runtime
{

class ExecutionIndexMemory
{
public:
    using Allocator
        = boost::interprocess::allocator< int, boost::interprocess::managed_shared_memory::segment_manager >;
    using Vector = boost::interprocess::vector< int, Allocator >;

    ExecutionIndexMemory( const std::string& strMemoryName )
        : m_sharedMemory( boost::interprocess::open_only, strMemoryName.c_str() )
        , m_allocator( m_sharedMemory.get_segment_manager() )
        , m_vector( m_sharedMemory.construct< Vector >( "TheVector" )( m_allocator ) )
    {
    }

    boost::interprocess::managed_shared_memory m_sharedMemory;
    Allocator                                  m_allocator;
    Vector*                                    m_vector;
};

std::array< ExecutionIndexMemory*, mega::MAX_SIMULATIONS > m_memory;

namespace
{

std::array< Root*, mega::MAX_SIMULATIONS > m_roots;

class Runtime
{
    // LogicalAddress -> PhysicalAddress
    using AddressMapping = std::unordered_map< AddressStorage, AddressStorage >;

public:
    LogicalAddress allocate( ExecutionIndex executionIndex, TypeID objectTypeID )
    {
        ExecutionContext* pExecutionContext = ExecutionContext::execution_get();
        VERIFY_RTE( pExecutionContext );
        return pExecutionContext->allocate( executionIndex, objectTypeID );
    }

    PhysicalAddress logicalToPhysical( ExecutionIndex executionIndex, TypeID objectTypeID,
                                       LogicalAddress logicalAddress )
    {
        auto iFind = m_addressMapping.find( Address{ logicalAddress } );
        if ( iFind != m_addressMapping.end() )
        {
            return Address{ iFind->second }.physical;
        }
        else
        {
            PhysicalAddress result;

            return result;
        }
    }

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

    ::Root* allocateRoot( const mega::ExecutionIndex& executionIndex )
    {
        mega::reference ref;
        {
            ref.physical.type      = mega::PHYSICAL_ADDRESS;
            ref.physical.execution = executionIndex;
            ref.physical.object    = 0;
            ref.typeID             = mega::ROOT_TYPE_ID;
            ref.instance           = 0;
        }

        ::Root* pRoot = new ::Root( ref );

        m_roots[ executionIndex ] = pRoot;

        return pRoot;
    }

    void releaseRoot( ::Root* pRoot )
    {
        VERIFY_RTE( pRoot->data.getType() == mega::PHYSICAL_ADDRESS );
        m_roots[ pRoot->data.physical.execution ] = nullptr;
        delete pRoot;
    }

    void get_allocate( const char* pszUnitName, const mega::InvocationID& invocation, AllocateFunction* ppFunction )
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
                m_pCodeGenerator->generate_allocate( *m_database, invocation, osModule );
                pModule = m_pJITCompiler->compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocation, pModule ) );
            }
        }
        // ct3pt3__eg_ImpNoParams -> _Z22ct3pt3__eg_ImpNoParamsRKN4mega9referenceE
        std::ostringstream os;
        os << "_Z22" << invocation << "RKN4mega9referenceE";
        *ppFunction = pModule->getAllocate( os.str() );
    }

    void get_read( const char* pszUnitName, const mega::InvocationID& invocation, ReadFunction* ppFunction )
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
        os << "_Z23" << invocation << "RKN4mega9referenceE";
        *ppFunction = pModule->getRead( os.str() );
    }

private:
    std::unique_ptr< CodeGenerator >    m_pCodeGenerator;
    std::unique_ptr< JITCompiler >      m_pJITCompiler;
    ComponentManager::Ptr               m_componentManager;
    std::unique_ptr< DatabaseInstance > m_database;
    bool                                m_bInitialised = false;
    AddressMapping                      m_addressMapping;

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

// routines used by jit compiled functions
LogicalAddress allocate( ExecutionIndex executionIndex, TypeID objectTypeID )
{
    return getStaticRuntime().allocate( executionIndex, objectTypeID );
}

// public facing runtime interface
void initialiseRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                        const mega::network::Project&                   project )
{
    getStaticRuntime().reinitialise( megastructureInstallation, project );
}

::Root* allocateRoot( const mega::ExecutionIndex& executionIndex )
{
    return getStaticRuntime().allocateRoot( executionIndex );
}
void releaseRoot( ::Root* pRoot ) { getStaticRuntime().releaseRoot( pRoot ); }

void get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
{
    getStaticRuntime().get_allocate( pszUnitName, invocationID, ppFunction );
}
void get_read( const char* pszUnitName, const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    getStaticRuntime().get_read( pszUnitName, invocationID, ppFunction );
}

} // namespace runtime
} // namespace mega
