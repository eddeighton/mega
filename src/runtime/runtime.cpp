

#include "runtime/runtime.hpp"

#include "address_space.hpp"
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

        PhysicalAddress allocateShared( ExecutionIndex executionIndex )
        {
            return m_pAllocationShared( executionIndex );
        }

        void deAllocateShared( PhysicalAddress address ) { m_pDeAllocationShared( address ); }

        GetSharedFunction getGetter() const { return m_pGetShared; }

    private:
        mega::TypeID               m_objectTypeID;
        GetSharedFunction          m_pGetShared          = nullptr;
        AllocationSharedFunction   m_pAllocationShared   = nullptr;
        DeAllocationSharedFunction m_pDeAllocationShared = nullptr;
    };
    // LogicalAddress -> PhysicalAddress
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
    using ExecutionContextRoot        = std::array< mega::reference, mega::MAX_SIMULATIONS >;

public:
    LogicalAddress allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID )
    {
        SPDLOG_TRACE( "allocateLogical: {}", executionIndex, objectTypeID );

        ExecutionContext* pExecutionContext = ExecutionContext::get();
        VERIFY_RTE( pExecutionContext );
        return pExecutionContext->allocateLogical( executionIndex, objectTypeID );
    }

    ObjectTypeAllocator::Ptr getOrCreateObjectTypeAllocator( TypeID objectTypeID )
    {
        SPDLOG_TRACE( "getOrCreateObjectTypeAllocator: {}", objectTypeID );

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
        SPDLOG_TRACE( "logicalToPhysical: {} {} {}", executionIndex, objectTypeID, logicalAddress );

        AddressSpace::Lock lock( m_addressSpace.mutex() );

        auto resultOpt = m_addressSpace.find( logicalAddress );
        if ( resultOpt.has_value() )
        {
            // ensure the object type allocator is established
            getOrCreateObjectTypeAllocator( objectTypeID );
            return resultOpt.value();
        }
        else
        {
            ObjectTypeAllocator::Ptr pAllocator      = getOrCreateObjectTypeAllocator( objectTypeID );
            const PhysicalAddress    physicalAddress = pAllocator->allocateShared( executionIndex );
            m_addressSpace.insert( logicalAddress, physicalAddress );
            return physicalAddress;
        }
    }

    ManagedHeapMemory& getHeapMemoryManager( mega::ExecutionIndex executionIndex )
    {
        ExecutionContextMemory* pMemory = m_executionContextMemory[ executionIndex ].get();
        if ( pMemory == nullptr )
        {
            ExecutionContext* pExecutionContext = ExecutionContext::get();
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
            ExecutionContext* pExecutionContext = ExecutionContext::get();
            VERIFY_RTE( pExecutionContext );
            const std::string strMemory                = pExecutionContext->acquireMemory( executionIndex );
            m_executionContextMemory[ executionIndex ] = std::make_unique< ExecutionContextMemory >( strMemory );
            pMemory                                    = m_executionContextMemory[ executionIndex ].get();
        }
        return pMemory->getShared();
    }

    mega::reference allocateRoot( const mega::ExecutionIndex& executionIndex )
    {
        const TypeID          rootTypeID          = m_database.getRootTypeID();
        const LogicalAddress  rootLogicalAddress  = allocateLogical( executionIndex, rootTypeID );
        const PhysicalAddress rootPhysicalAddress = logicalToPhysical( executionIndex, rootTypeID, rootLogicalAddress );

        mega::reference ref;
        {
            ref.physical = rootPhysicalAddress;
            ref.typeID   = rootTypeID;
            ref.instance = 0;
        }

        m_executionContextRoot[ executionIndex ] = ref;

        return ref;
    }

    void deAllocateRoot( const mega::ExecutionIndex& executionIndex )
    {
        const mega::reference ref = m_executionContextRoot[ executionIndex ];
        VERIFY_RTE( ref.value != mega::INVALID_ADDRESS );

        ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( ref.typeID );
        // pAllocator->deAllocateShared( ref.physical );
    }

    void get_getter_shared( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( objectTypeID );
        *ppFunction                         = pAllocator->getGetter();
    }

    JITCompiler::Module::Ptr compile( const std::string& strCode )
    {
        auto                     startTime = std::chrono::steady_clock::now();
        JITCompiler::Module::Ptr pModule   = m_jitCompiler.compile( strCode );
        SPDLOG_TRACE( "JIT Compilation time: {}", std::chrono::steady_clock::now() - startTime );
        return pModule;
    }

    void get_allocation( mega::TypeID objectTypeID, GetSharedFunction* pGetSharedFunction,
                         AllocationSharedFunction*   ppAllocationShared,
                         DeAllocationSharedFunction* ppDeAllocationShared )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );

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
                m_codeGenerator.generate_allocation( m_database, objectTypeID, osModule );
                pModule = compile( osModule.str() );
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
        std::lock_guard< std::mutex > lock( m_jitMutex );

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
                m_codeGenerator.generate_allocate( m_database, invocation, osModule );
                pModule = compile( osModule.str() );
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
        std::lock_guard< std::mutex > lock( m_jitMutex );

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
                m_codeGenerator.generate_read( m_database, invocation, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocation, pModule ) );
            }
        }
        // _Z23 ct1ps12__eg_ImpNoParamsRKN4mega9referenceE
        // _Z24ct30ps18__eg_ImpNoParamsRKN4mega9referenceE
        std::ostringstream os;
        os << "_Z24" << invocation << "RKN4mega9referenceE";
        *ppFunction = pModule->getRead( os.str() );
    }

    Runtime( const mega::network::MegastructureInstallation& megastructureInstallation,
             const mega::network::Project&                   project )
        : m_jitMutex()
        , m_jitCompiler()
        , m_codeGenerator( megastructureInstallation, project )
        , m_database( project.getProjectDatabase() )
        , m_addressSpace()
        , m_objectTypeAllocatorMapping()
        , m_executionContextRoot()
        , m_executionContextMemory()
    {
    }

private:
    std::mutex                  m_jitMutex;
    JITCompiler                 m_jitCompiler;
    CodeGenerator               m_codeGenerator;
    DatabaseInstance            m_database;
    AddressSpace                m_addressSpace;
    ObjectTypeAllocatorMap      m_objectTypeAllocatorMapping;
    ExecutionContextRoot        m_executionContextRoot;
    ExecutionContextMemoryArray m_executionContextMemory;

    // use unordered_map
    using InvocationMap = std::map< mega::InvocationID, JITCompiler::Module::Ptr >;
    InvocationMap m_invocations;

    using AllocationsMap = std::map< mega::TypeID, JITCompiler::Module::Ptr >;
    AllocationsMap m_allocations;

    using FunctionPtrMap = std::multimap< const char*, void* >;
    FunctionPtrMap m_functionPointers;
};

static std::unique_ptr< Runtime > g_pRuntime;

void initialiseStaticRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                              const mega::network::Project&                   project )
{
    try
    {
        g_pRuntime.reset();

        if ( !project.isEmpty() )
        {
            if( !boost::filesystem::exists( project.getProjectDatabase() ) )
            {
                SPDLOG_ERROR( "Database missing at: {}", project.getProjectDatabase().string() );
            }
            else
            {
                g_pRuntime = std::make_unique< Runtime >( megastructureInstallation, project );
            }
        }
    }
    catch ( mega::io::DatabaseVersionException& ex )
    {
        SPDLOG_ERROR( "Database version exception: {}", project.getProjectInstallPath().string(), ex.what() );
        throw;
    }
    catch ( std::exception& ex )
    {
        SPDLOG_ERROR( "ComponentManager failed to initialise project: {} error: {}",
                      project.getProjectInstallPath().string(), ex.what() );
        throw;
    }
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
    return g_pRuntime->allocateLogical( executionIndex, objectTypeID );
}

PhysicalAddress logicalToPhysical( mega::ExecutionIndex executionIndex, mega::TypeID objectTypeID,
                                   mega::LogicalAddress logicalAddress )
{
    return g_pRuntime->logicalToPhysical( executionIndex, objectTypeID, logicalAddress );
}

ManagedHeapMemory& getHeapMemoryManager( mega::ExecutionIndex executionIndex )
{
    return g_pRuntime->getHeapMemoryManager( executionIndex );
}

ManagedSharedMemory& getSharedMemoryManager( mega::ExecutionIndex executionIndex )
{
    return g_pRuntime->getSharedMemoryManager( executionIndex );
}

void get_getter_shared( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
{
    g_pRuntime->get_getter_shared( pszUnitName, objectTypeID, ppFunction );
}

// public facing runtime interface
void initialiseRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                        const mega::network::Project&                   project )
{
    initialiseStaticRuntime( megastructureInstallation, project );
}

ExecutionRoot::ExecutionRoot( mega::ExecutionIndex executionIndex )
    : m_executionIndex( executionIndex )
    , m_root( g_pRuntime->allocateRoot( m_executionIndex ) )
{
}

ExecutionRoot::~ExecutionRoot() { g_pRuntime->deAllocateRoot( m_executionIndex ); }

void get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
{
    g_pRuntime->get_allocate( pszUnitName, invocationID, ppFunction );
}

void get_read( const char* pszUnitName, const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    g_pRuntime->get_read( pszUnitName, invocationID, ppFunction );
}

} // namespace runtime
} // namespace mega
