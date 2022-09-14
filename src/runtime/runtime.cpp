

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
        friend class Runtime;

    public:
        using Ptr = std::shared_ptr< ObjectTypeAllocator >;

        ObjectTypeAllocator( Runtime& runtime, TypeID objectTypeID )
            : m_objectTypeID( objectTypeID )
        {
            runtime.get_allocation( m_objectTypeID, *this );
        }

        GetHeapFunction   getHeapGetter() const { return m_pGetHeap; }
        GetSharedFunction getSharedGetter() const { return m_pGetShared; }
        PhysicalAddress   allocate( ExecutionIndex executionIndex ) { return m_pAllocation( executionIndex ); }
        void              deAllocate( PhysicalAddress address ) { m_pDeAllocation( address ); }

    private:
        mega::TypeID         m_objectTypeID;
        GetHeapFunction      m_pGetHeap      = nullptr;
        GetSharedFunction    m_pGetShared    = nullptr;
        AllocationFunction   m_pAllocation   = nullptr;
        DeAllocationFunction m_pDeAllocation = nullptr;
    };
    // LogicalAddress -> PhysicalAddress
    using IndexArray             = std::array< void*, mega::MAX_SIMULATIONS >;
    using ObjectTypeAllocatorMap = std::unordered_map< TypeID, ObjectTypeAllocator::Ptr >;

    class ExecutionContextMemory
    {
    public:
        using Ptr = std::unique_ptr< ExecutionContextMemory >;

        ExecutionContextMemory( const std::string& strName )
            : m_shared( boost::interprocess::open_only, strName.c_str() )
        {
        }

        ManagedSharedMemory& getShared() { return m_shared; }

    private:
        ManagedSharedMemory m_shared;
    };
    using ExecutionContextMemoryArray = std::array< ExecutionContextMemory::Ptr, mega::MAX_SIMULATIONS >;
    using ExecutionContextRoot        = std::array< mega::reference, mega::MAX_SIMULATIONS >;

public:
    LogicalAddress allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID )
    {
        SPDLOG_TRACE( "RUNTIME: allocateLogical: {}", executionIndex, objectTypeID );

        ExecutionContext* pExecutionContext = ExecutionContext::get();
        VERIFY_RTE( pExecutionContext );
        return pExecutionContext->allocateLogical( executionIndex, objectTypeID );
    }

    ObjectTypeAllocator::Ptr getOrCreateObjectTypeAllocator( TypeID objectTypeID )
    {
        SPDLOG_TRACE( "RUNTIME: getOrCreateObjectTypeAllocator: {}", objectTypeID );

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
        SPDLOG_TRACE( "RUNTIME: logicalToPhysical: {} {} {}", executionIndex, objectTypeID, logicalAddress );

        AddressSpace::Lock lock( m_addressSpace.mutex() );

        ExecutionContext* pExecutionContext = ExecutionContext::get();
        VERIFY_RTE( pExecutionContext );
        if ( pExecutionContext->getThisExecutionIndex() != executionIndex )
        {
            // request write lock
            pExecutionContext->writeLock( executionIndex );
        }

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
            const PhysicalAddress    physicalAddress = pAllocator->allocate( executionIndex );
            m_addressSpace.insert( logicalAddress, physicalAddress );
            return physicalAddress;
        }
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
        SPDLOG_TRACE( "RUNTIME: allocateRoot: {}", executionIndex );

        const TypeID rootTypeID = m_database.getRootTypeID();

        ExecutionContext* pExecutionContext = ExecutionContext::get();
        VERIFY_RTE( pExecutionContext );
        const mega::ExecutionIndex thisIndex = pExecutionContext->getThisExecutionIndex();
        VERIFY_RTE( thisIndex == executionIndex );

        const LogicalAddress rootLogicalAddress = allocateLogical( executionIndex, rootTypeID );
        VERIFY_RTE( thisIndex == pExecutionContext->getThisExecutionIndex() );

        const PhysicalAddress rootPhysicalAddress = logicalToPhysical( executionIndex, rootTypeID, rootLogicalAddress );
        VERIFY_RTE( thisIndex == pExecutionContext->getThisExecutionIndex() );

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
        *ppFunction                         = pAllocator->getSharedGetter();
    }

    void get_getter_heap( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );
        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
        ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( objectTypeID );
        *ppFunction                         = pAllocator->getHeapGetter();
    }

    void get_getter_call( const char* pszUnitName, mega::TypeID objectTypeID, TypeErasedFunction* ppFunction )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );
        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
        *ppFunction = m_componentManager.getOperationFunctionPtr( objectTypeID );
    }

    JITCompiler::Module::Ptr compile( const std::string& strCode )
    {
        auto                     startTime = std::chrono::steady_clock::now();
        JITCompiler::Module::Ptr pModule   = m_jitCompiler.compile( strCode );
        SPDLOG_TRACE( "RUNTIME: JIT Compilation time: {}", std::chrono::steady_clock::now() - startTime );
        return pModule;
    }

    void symbolPrefix( const char* prefix, mega::TypeID objectTypeID, std::ostream& os )
    {
        std::ostringstream osTypeID;
        osTypeID << prefix << objectTypeID;
        os << "_Z" << osTypeID.str().size() << osTypeID.str();
    }

    void symbolPrefix( const mega::InvocationID& invocationID, std::ostream& os )
    {
        std::ostringstream osTypeID;
        osTypeID << invocationID;
        os << "_Z" << osTypeID.str().size() << osTypeID.str();
    }

    void get_allocation( mega::TypeID objectTypeID, ObjectTypeAllocator& objectTypeAllocator )
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

        {
            std::ostringstream os;
            symbolPrefix( "get_heap_", objectTypeID, os );
            os << "N4mega15PhysicalAddressE";
            objectTypeAllocator.m_pGetHeap = pModule->getGetShared( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "get_shared_", objectTypeID, os );
            os << "N4mega15PhysicalAddressE";
            objectTypeAllocator.m_pGetShared = pModule->getGetShared( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "alloc_", objectTypeID, os );
            os << "t";
            objectTypeAllocator.m_pAllocation = pModule->getAllocation( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "dealloc_", objectTypeID, os );
            os << "N4mega15PhysicalAddressE";
            objectTypeAllocator.m_pDeAllocation = pModule->getDeAllocation( os.str() );
        }
    }

    void get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        JITCompiler::Module::Ptr pModule;
        {
            auto iFind = m_invocations.find( invocationID );
            if ( iFind != m_invocations.end() )
            {
                pModule = iFind->second;
            }
            else
            {
                std::ostringstream osModule;
                m_codeGenerator.generate_allocate( m_database, invocationID, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocationID, pModule ) );
            }
        }
        std::ostringstream os;
        symbolPrefix( invocationID, os );
        os << "N4mega9referenceE";
        *ppFunction = pModule->getAllocate( os.str() );
    }

    void get_read( const char* pszUnitName, const mega::InvocationID& invocationID, ReadFunction* ppFunction )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        JITCompiler::Module::Ptr pModule;
        {
            auto iFind = m_invocations.find( invocationID );
            if ( iFind != m_invocations.end() )
            {
                pModule = iFind->second;
            }
            else
            {
                std::ostringstream osModule;
                m_codeGenerator.generate_read( m_database, invocationID, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocationID, pModule ) );
            }
        }
        std::ostringstream os;
        symbolPrefix( invocationID, os );
        os << "N4mega9referenceE";
        *ppFunction = pModule->getRead( os.str() );
    }

    void get_write( const char* pszUnitName, const mega::InvocationID& invocationID, WriteFunction* ppFunction )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        JITCompiler::Module::Ptr pModule;
        {
            auto iFind = m_invocations.find( invocationID );
            if ( iFind != m_invocations.end() )
            {
                pModule = iFind->second;
            }
            else
            {
                std::ostringstream osModule;
                m_codeGenerator.generate_write( m_database, invocationID, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocationID, pModule ) );
            }
        }
        std::ostringstream os;
        symbolPrefix( invocationID, os );
        os << "N4mega9referenceE";
        *ppFunction = pModule->getWrite( os.str() );
    }

    void get_call( const char* pszUnitName, const mega::InvocationID& invocationID, CallFunction* ppFunction )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );

        m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

        JITCompiler::Module::Ptr pModule;
        {
            auto iFind = m_invocations.find( invocationID );
            if ( iFind != m_invocations.end() )
            {
                pModule = iFind->second;
            }
            else
            {
                std::ostringstream osModule;
                m_codeGenerator.generate_call( m_database, invocationID, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocationID, pModule ) );
            }
        }
        std::ostringstream os;
        symbolPrefix( invocationID, os );
        os << "N4mega9referenceE";
        *ppFunction = pModule->getCall( os.str() );
    }

    Runtime( const mega::network::MegastructureInstallation& megastructureInstallation,
             const mega::network::Project&                   project )
        : m_megastructureInstallation( megastructureInstallation )
        , m_project( project )
        , m_jitMutex()
        , m_jitCompiler()
        , m_codeGenerator( m_megastructureInstallation, m_project )
        , m_database( m_project.getProjectDatabase() )
        , m_componentManager( m_project, m_database )
        , m_addressSpace()
        , m_objectTypeAllocatorMapping()
        , m_executionContextRoot()
        , m_executionContextMemory()
    {
        VERIFY_RTE_MSG( !m_project.isEmpty(), "Empty project" );
    }

private:
    const mega::network::MegastructureInstallation m_megastructureInstallation;
    const mega::network::Project                   m_project;
    std::mutex                                     m_jitMutex;
    JITCompiler                                    m_jitCompiler;
    CodeGenerator                                  m_codeGenerator;
    DatabaseInstance                               m_database;
    ComponentManager                               m_componentManager;
    AddressSpace                                   m_addressSpace;
    ObjectTypeAllocatorMap                         m_objectTypeAllocatorMapping;
    ExecutionContextRoot                           m_executionContextRoot;
    ExecutionContextMemoryArray                    m_executionContextMemory;

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
            if ( !boost::filesystem::exists( project.getProjectDatabase() ) )
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

bool isStaticRuntimeInitialised() { return g_pRuntime.get(); }

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

ManagedSharedMemory& getSharedMemoryManager( mega::ExecutionIndex executionIndex )
{
    return g_pRuntime->getSharedMemoryManager( executionIndex );
}

void get_getter_shared( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
{
    g_pRuntime->get_getter_shared( pszUnitName, objectTypeID, ppFunction );
}

void get_getter_heap( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
{
    g_pRuntime->get_getter_heap( pszUnitName, objectTypeID, ppFunction );
}

void get_getter_call( const char* pszUnitName, mega::TypeID objectTypeID, TypeErasedFunction* ppFunction )
{
    g_pRuntime->get_getter_call( pszUnitName, objectTypeID, ppFunction );
}

// public facing runtime interface
void initialiseRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                        const mega::network::Project&                   project )
{
    initialiseStaticRuntime( megastructureInstallation, project );
}
bool isRuntimeInitialised() { return isStaticRuntimeInitialised(); }

ExecutionRoot::ExecutionRoot( mega::ExecutionIndex executionIndex )
    : m_root( g_pRuntime->allocateRoot( executionIndex ) )
{
}

ExecutionRoot::~ExecutionRoot() { g_pRuntime->deAllocateRoot( m_root.physical.execution ); }

void get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
{
    g_pRuntime->get_allocate( pszUnitName, invocationID, ppFunction );
}

void get_read( const char* pszUnitName, const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    g_pRuntime->get_read( pszUnitName, invocationID, ppFunction );
}

void get_write( const char* pszUnitName, const mega::InvocationID& invocationID, WriteFunction* ppFunction )
{
    g_pRuntime->get_write( pszUnitName, invocationID, ppFunction );
}

void get_call( const char* pszUnitName, const mega::InvocationID& invocationID, CallFunction* ppFunction )
{
    g_pRuntime->get_call( pszUnitName, invocationID, ppFunction );
}

} // namespace runtime
} // namespace mega
