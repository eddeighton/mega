

#include "runtime/runtime.hpp"

#include "address_space.hpp"
#include "jit.hpp"
#include "component_manager.hpp"
#include "database.hpp"
#include "code_generator.hpp"

#include "service/protocol/common/project.hpp"
#include "service/network/log.hpp"

#include "mega/common.hpp"
#include "mega/default_traits.hpp"
#include "mega/shared_allocator.hpp"
#include "mega/mpo_context.hpp"

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
    class ObjectTypeAllocator : public Allocator
    {
        friend class Runtime;

    public:
        using Ptr         = std::shared_ptr< ObjectTypeAllocator >;
        using IndexPtr    = std::shared_ptr< IndexedBufferAllocator >;
        using IndexPtrMap = std::unordered_map< MPO, IndexPtr, MPO::Hash >;

        ObjectTypeAllocator( Runtime& runtime, TypeID objectTypeID )
            : m_runtime( runtime )
            , m_objectTypeID( objectTypeID )
            , m_szSizeShared( 0U )
            , m_szSizeHeap( 0U )
        {
            const FinalStage::Concrete::Object* pObject = runtime.m_database.getObject( m_objectTypeID );
            using namespace FinalStage;
            for ( auto pBuffer : pObject->get_buffers() )
            {
                if ( dynamic_database_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
                {
                    VERIFY_RTE( m_szSizeShared == 0U );
                    m_szSizeShared = pBuffer->get_size();
                }
                else if ( dynamic_database_cast< MemoryLayout::NonSimpleBuffer >( pBuffer ) )
                {
                    VERIFY_RTE( m_szSizeHeap == 0U );
                    m_szSizeHeap = pBuffer->get_size();
                }
                else
                {
                    THROW_RTE( "Unsupported buffer type" );
                }
            }

            runtime.get_allocation( m_objectTypeID, *this );
            m_pSetAllocator( this );
        }

        IndexPtr getIndex( MPO mpo )
        {
            IndexPtr pAllocator;
            {
                auto iFind = m_index.find( mpo );
                if ( iFind != m_index.end() )
                {
                    pAllocator = iFind->second;
                }
                else
                {
                    mega::runtime::ManagedSharedMemory& managedSharedMemory = m_runtime.getSharedMemoryManager( mpo );
                    pAllocator
                        = std::make_shared< IndexedBufferAllocator >( managedSharedMemory.get_segment_manager() );
                    m_index.insert( { mpo, pAllocator } );
                }
            }
            return pAllocator;
        }

        reference get( MachineAddress machineAddress )
        {
            VERIFY_RTE( machineAddress.isMachine() );
            void* pShared = getIndex( machineAddress )->getShared( machineAddress.object ).get();
            return reference( TypeInstance( 0, m_objectTypeID ), machineAddress, pShared );
        }

        reference allocate( MPO mpo )
        {
            VERIFY_RTE( mpo.isMachine() );
            const IndexedBufferAllocator::AllocationResult result
                = getIndex( mpo )->allocate( m_szSizeShared, m_szSizeHeap );
            m_pSharedCtor( result.pShared );
            m_pHeapCtor( result.pHeap );
            return reference( TypeInstance( 0, m_objectTypeID ), MachineAddress{ result.object, mpo }, result.pShared );
        }

        void deAllocate( MachineAddress machineAddress )
        {
            auto pIndex = getIndex( machineAddress );
            m_pSharedDtor( pIndex->getShared( machineAddress.object ).get() );
            m_pHeapDtor( pIndex->getHeap( machineAddress.object ) );
            pIndex->deallocate( machineAddress.object );
        }

        // Allocator
        virtual void* get_shared( mega::MachineAddress machineAddress )
        {
            auto pIndex = getIndex( machineAddress );
            return pIndex->getShared( machineAddress.object ).get();
        }

        virtual void* get_heap( mega::MachineAddress machineAddress )
        {
            auto pIndex = getIndex( machineAddress );
            return pIndex->getHeap( machineAddress.object );
        }

        GetHeapFunction   getHeapGetter() { return m_pHeapGetter; }
        GetSharedFunction getSharedGetter() { return m_pSharedGetter; }

    private:
        Runtime&     m_runtime;
        mega::TypeID m_objectTypeID;
        mega::U64  m_szSizeShared, m_szSizeHeap;
        IndexPtrMap  m_index;

        SetAllocatorFunction m_pSetAllocator = nullptr;
        SharedCtorFunction   m_pSharedCtor   = nullptr;
        SharedDtorFunction   m_pSharedDtor   = nullptr;
        HeapCtorFunction     m_pHeapCtor     = nullptr;
        HeapDtorFunction     m_pHeapDtor     = nullptr;
        GetHeapFunction      m_pHeapGetter   = nullptr;
        GetSharedFunction    m_pSharedGetter = nullptr;
    };
    // NetworkAddress -> MachineAddress
    using ObjectTypeAllocatorMap = std::unordered_map< TypeID, ObjectTypeAllocator::Ptr >;

    class MPOContextMemory
    {
    public:
        using Ptr = std::unique_ptr< MPOContextMemory >;

        MPOContextMemory( const std::string& strName )
            : m_shared( boost::interprocess::open_only, strName.c_str() )
        {
        }

        ::mega::runtime::ManagedSharedMemory& getShared() { return m_shared; }

    private:
        ::mega::runtime::ManagedSharedMemory m_shared;
    };

    ManagedSharedMemory& getSharedMemoryManager( mega::MPO mpo )
    {
        auto iFind = m_executionContextMemory.find( mpo );
        if ( iFind != m_executionContextMemory.end() )
        {
            return iFind->second->getShared();
        }
        else
        {
            MPOContext* pMPOContext = MPOContext::get();
            VERIFY_RTE( pMPOContext );
            const std::string                         strMemory = pMPOContext->acquireMemory( mpo );
            std::unique_ptr< MPOContextMemory > pMemoryPtr
                = std::make_unique< MPOContextMemory >( strMemory );
            MPOContextMemory* pMemory = pMemoryPtr.get();
            m_executionContextMemory.insert( { mpo, std::move( pMemoryPtr ) } );
            return pMemory->getShared();
        }
    }

    using MPOContextMemoryMap = std::unordered_map< MPO, MPOContextMemory::Ptr, MPO::Hash >;
    using MPOContextRoot      = std::unordered_map< MPO, mega::reference, MPO::Hash >;

public:
    NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID )
    {
        SPDLOG_TRACE( "RUNTIME: allocateNetworkAddress: {} {}", mpo, objectTypeID );

        MPOContext* pMPOContext = MPOContext::get();
        VERIFY_RTE( pMPOContext );
        return pMPOContext->allocateNetworkAddress( mpo, objectTypeID );
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

    reference networkToMachine( MPO mpo, TypeID objectTypeID, NetworkAddress networkAddress )
    {
        SPDLOG_TRACE( "RUNTIME: networkToMachine: {} {} {}", mpo, objectTypeID, networkAddress );

        AddressSpace::Lock lock( m_addressSpace.mutex() );

        MPOContext* pMPOContext = MPOContext::get();
        VERIFY_RTE( pMPOContext );
        if ( pMPOContext->getThisMPO() != mpo )
        {
            // request write lock
            //pMPOContext->writeLock( mpo );
        }

        ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( objectTypeID );

        auto resultOpt = m_addressSpace.find( networkAddress );
        if ( resultOpt.has_value() )
        {
            // ensure the object type allocator is established
            return pAllocator->get( resultOpt.value() );
        }
        else
        {
            const reference ref = pAllocator->allocate( mpo );
            m_addressSpace.insert( networkAddress, static_cast< const MachineAddress& >( ref ) );
            return ref;
        }
    }

    mega::reference allocateRoot( const mega::MPO& mpo )
    {
        SPDLOG_TRACE( "RUNTIME: allocateRoot: {}", mpo );

        const TypeID         rootType           = m_database.getRootTypeID();
        const NetworkAddress rootNetworkAddress = allocateNetworkAddress( mpo, rootType );
        const reference      ref                = networkToMachine( mpo, rootType, rootNetworkAddress );
        VERIFY_RTE( mpo == ref );
        m_executionContextRoot.insert( { mpo, ref } );

        SPDLOG_TRACE( "RUNTIME: allocateRoot: {}", ref );
        return ref;
    }

    void deAllocateRoot( const mega::MPO& mpo )
    {
        SPDLOG_TRACE( "RUNTIME: deAllocateRoot: {}", mpo );

        const mega::reference ref = m_executionContextRoot[ mpo ];
        VERIFY_RTE( ref.isMachine() );
        ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( ref.type );
        pAllocator->deAllocate( ref );
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

        SPDLOG_TRACE( "RUNTIME: get_allocation: {}", objectTypeID );

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
            symbolPrefix( "set_allocator_", objectTypeID, os );
            os << "PN4mega7runtime9AllocatorE";
            objectTypeAllocator.m_pSetAllocator = pModule->getSetAllocator( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "get_heap_", objectTypeID, os );
            os << "N4mega14MachineAddressE";
            objectTypeAllocator.m_pHeapGetter = pModule->getGetHeap( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "get_shared_", objectTypeID, os );
            os << "N4mega14MachineAddressE";
            objectTypeAllocator.m_pSharedGetter = pModule->getGetShared( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "shared_ctor_", objectTypeID, os );
            os << "Pv";
            objectTypeAllocator.m_pSharedCtor = pModule->getSharedCtor( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "shared_dtor_", objectTypeID, os );
            os << "Pv";
            objectTypeAllocator.m_pSharedDtor = pModule->getSharedDtor( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "heap_ctor_", objectTypeID, os );
            os << "Pv";
            objectTypeAllocator.m_pHeapCtor = pModule->getHeapCtor( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "heap_dtor_", objectTypeID, os );
            os << "Pv";
            objectTypeAllocator.m_pHeapDtor = pModule->getHeapDtor( os.str() );
        }
    }

    void get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
    {
        std::lock_guard< std::mutex > lock( m_jitMutex );

        SPDLOG_TRACE( "RUNTIME: get_allocate: {} {}", pszUnitName, invocationID );

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

        SPDLOG_TRACE( "RUNTIME: get_read: {} {}", pszUnitName, invocationID );

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

        SPDLOG_TRACE( "RUNTIME: get_write: {} {}", pszUnitName, invocationID );

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

        SPDLOG_TRACE( "RUNTIME: get_call: {} {}", pszUnitName, invocationID );

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

    // these three are order dependent - m_executionContextMemory owns the shared memory
    MPOContextMemoryMap                      m_executionContextMemory;
    ObjectTypeAllocatorMap                         m_objectTypeAllocatorMapping;
    MPOContextRoot                           m_executionContextRoot;

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
NetworkAddress allocateNetworkAddress( mega::MPO mpo, mega::TypeID objectTypeID )
{
    return g_pRuntime->allocateNetworkAddress( mpo, objectTypeID );
}

reference networkToMachine( mega::MPO mpo, mega::TypeID objectTypeID, mega::NetworkAddress networkAddress )
{
    return g_pRuntime->networkToMachine( mpo, objectTypeID, networkAddress );
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

MPORoot::MPORoot( mega::MPO mpo )
    : m_root( g_pRuntime->allocateRoot( mpo ) )
{
}

MPORoot::~MPORoot() { g_pRuntime->deAllocateRoot( m_root ); }

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
