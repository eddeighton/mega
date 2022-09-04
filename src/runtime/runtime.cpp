

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
#include "mega/default_traits.hpp"

#include "common/assert_verify.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <array>
#include <unordered_map>

namespace mega
{
namespace runtime
{

namespace
{

class Runtime
{
    class ObjectTypeAllocator
    {
    public:
        using Ptr = std::shared_ptr< ObjectTypeAllocator >;

        ObjectTypeAllocator( Runtime& runtime, TypeID objectTypeID )
            : m_objectTypeID( objectTypeID )
        {
            runtime.get_allocation( m_objectTypeID, &m_pGetShared, &m_pAllocationShared, &m_pDeAllocationShared );
        }

        PhysicalAddress acquire( ExecutionIndex executionIndex )
        {
            return m_pAllocationShared( executionIndex );
        }

    private:
        mega::TypeID               m_objectTypeID;
        GetSharedFunction          m_pGetShared          = nullptr;
        AllocationSharedFunction   m_pAllocationShared   = nullptr;
        DeAllocationSharedFunction m_pDeAllocationShared = nullptr;
    };
    // LogicalAddress -> PhysicalAddress
    using AddressMapping         = std::unordered_map< AddressStorage, AddressStorage >;
    using IndexArray             = std::array< void*, mega::MAX_SIMULATIONS >;
    using ObjectTypeAllocatorMap = std::unordered_map< TypeID, ObjectTypeAllocator::Ptr >;

    class ExecutionContextMemory
    {
    public:
        using Ptr = std::unique_ptr< ExecutionContextMemory >;

        static constexpr size_t SIZE = 1024 * 1024 * 4;

        ExecutionContextMemory( const std::string& strName )
            : m_heap( SIZE )
            , m_shared( boost::interprocess::open_only, strName.c_str() )
        {
        }

        ManagedHeapMemory&   getHeap() { return m_heap; }
        ManagedSharedMemory& getShared() { return m_shared; }

    private:
        ManagedHeapMemory   m_heap;
        ManagedSharedMemory m_shared;
    };
    using ExecutionContextMemoryArray = std::array< ExecutionContextMemory::Ptr, mega::MAX_SIMULATIONS >;
    using ExecutionContextRoot        = std::array< Root*, mega::MAX_SIMULATIONS >;

public:
    LogicalAddress allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID )
    {
        SPDLOG_INFO( "allocateLogical: {}", executionIndex, objectTypeID );

        ExecutionContext* pExecutionContext = ExecutionContext::execution_get();
        VERIFY_RTE( pExecutionContext );
        return pExecutionContext->allocateLogical( executionIndex, objectTypeID );
    }

    ObjectTypeAllocator::Ptr getOrCreateObjectTypeAllocator( TypeID objectTypeID )
    {
        SPDLOG_INFO( "getOrCreateObjectTypeAllocator: {}", objectTypeID );

        auto jFind = m_objectTypeAllocatorMapping.find( objectTypeID );
        if ( jFind != m_objectTypeAllocatorMapping.end() )
        {
            return jFind->second;
        }
        else
        {
            ObjectTypeAllocator::Ptr pAllocator = std::make_shared< ObjectTypeAllocator >( *this, objectTypeID );
            m_objectTypeAllocatorMapping.insert( { objectTypeID, pAllocator } );
            return pAllocator;
        }
    }

    PhysicalAddress logicalToPhysical( ExecutionIndex executionIndex, TypeID objectTypeID,
                                       LogicalAddress logicalAddress )
    {
        SPDLOG_INFO( "logicalToPhysical: {} {} {}", executionIndex, objectTypeID, logicalAddress );

        auto iFind = m_addressMapping.find( Address{ logicalAddress } );
        if ( iFind != m_addressMapping.end() )
        {
            // ensure the object type allocator is established
            getOrCreateObjectTypeAllocator( objectTypeID );
            return Address{ iFind->second }.physical;
        }
        else
        {
            ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( objectTypeID );
            const PhysicalAddress    result     = pAllocator->acquire( executionIndex );
            m_addressMapping.insert( { Address{ logicalAddress }, Address{ result } } );
            return result;
        }
    }

    ManagedHeapMemory& getHeapMemoryManager( mega::ExecutionIndex executionIndex )
    {
        ExecutionContextMemory* pMemory = m_executionContextMemory[ executionIndex ].get();
        if ( pMemory == nullptr )
        {
            ExecutionContext* pExecutionContext = ExecutionContext::execution_get();
            VERIFY_RTE( pExecutionContext );
            const std::string strMemory                = pExecutionContext->acquireMemory( executionIndex );
            m_executionContextMemory[ executionIndex ] = std::make_unique< ExecutionContextMemory >( strMemory );
            pMemory                                    = m_executionContextMemory[ executionIndex ].get();
        }
        return pMemory->getHeap();
    }

    ManagedSharedMemory& getSharedMemoryManager( mega::ExecutionIndex executionIndex )
    {
        ExecutionContextMemory* pMemory = m_executionContextMemory[ executionIndex ].get();
        if ( pMemory == nullptr )
        {
            ExecutionContext* pExecutionContext = ExecutionContext::execution_get();
            VERIFY_RTE( pExecutionContext );
            const std::string strMemory                = pExecutionContext->acquireMemory( executionIndex );
            m_executionContextMemory[ executionIndex ] = std::make_unique< ExecutionContextMemory >( strMemory );
            pMemory                                    = m_executionContextMemory[ executionIndex ].get();
        }
        return pMemory->getShared();
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

        m_executionContextRoot[ executionIndex ] = pRoot;

        return pRoot;
    }

    void releaseRoot( ::Root* pRoot )
    {
        VERIFY_RTE( pRoot->data.getType() == mega::PHYSICAL_ADDRESS );
        m_executionContextRoot[ pRoot->data.physical.execution ] = nullptr;
        delete pRoot;
    }

    void get_allocation( mega::TypeID objectTypeID, GetSharedFunction* pGetSharedFunction,
                         AllocationSharedFunction*   ppAllocationShared,
                         DeAllocationSharedFunction* ppDeAllocationShared )
    {
        VERIFY_RTE_MSG( m_bInitialised, "Runtime not initialised" );

        JITCompiler::Module::Ptr pModule;
        {
            auto iFind = m_allocations.find( objectTypeID );
            if ( iFind != m_allocations.end() )
            {
                pModule = iFind->second;
            }
            else
            {
                std::ostringstream osModule;
                m_pCodeGenerator->generate_allocation( *m_database, objectTypeID, osModule );
                pModule = m_pJITCompiler->compile( osModule.str() );
                m_allocations.insert( std::make_pair( objectTypeID, pModule ) );
            }
        }

        // get_shared_3 -> _Z12get_shared_3N4mega15PhysicalAddressE
        {
            std::ostringstream os;
            os << "_Z12"
               << "get_shared_" << objectTypeID << "N4mega15PhysicalAddressE";
            *pGetSharedFunction = pModule->getGetShared( os.str() );
        }

        // alloc_shared_3 -> _Z14alloc_shared_3t
        {
            std::ostringstream os;
            os << "_Z14"
               << "alloc_shared_" << objectTypeID << "t";
            *ppAllocationShared = pModule->getAllocationShared( os.str() );
        }

        // dealloc_shared_3 -> _Z16dealloc_shared_3N4mega15PhysicalAddressE
        {
            std::ostringstream os;
            os << "_Z16"
               << "dealloc_shared_" << objectTypeID << "N4mega15PhysicalAddressE";
            *ppDeAllocationShared = pModule->getDeAllocationShared( os.str() );
        }
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
    ObjectTypeAllocatorMap              m_objectTypeAllocatorMapping;
    ExecutionContextRoot                m_executionContextRoot;
    ExecutionContextMemoryArray         m_executionContextMemory;

    // use unordered_map
    using InvocationMap = std::map< mega::InvocationID, JITCompiler::Module::Ptr >;
    InvocationMap m_invocations;

    using AllocationsMap = std::map< mega::TypeID, JITCompiler::Module::Ptr >;
    AllocationsMap m_allocations;

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
LogicalAddress allocateLogical( mega::ExecutionIndex executionIndex, mega::TypeID objectTypeID )
{
    return getStaticRuntime().allocateLogical( executionIndex, objectTypeID );
}

PhysicalAddress logicalToPhysical( mega::ExecutionIndex executionIndex, mega::TypeID objectTypeID,
                                   mega::LogicalAddress logicalAddress )
{
    return getStaticRuntime().logicalToPhysical( executionIndex, objectTypeID, logicalAddress );
}

ManagedHeapMemory& getHeapMemoryManager( mega::ExecutionIndex executionIndex )
{
    return getStaticRuntime().getHeapMemoryManager( executionIndex );
}

ManagedSharedMemory& getSharedMemoryManager( mega::ExecutionIndex executionIndex )
{
    return getStaticRuntime().getSharedMemoryManager( executionIndex );
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
