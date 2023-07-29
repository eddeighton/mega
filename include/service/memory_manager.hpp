
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef GUARD_2023_January_07_memory_manager
#define GUARD_2023_January_07_memory_manager

#include "fixed_allocator.hpp"

#include "jit/allocator.hpp"
#include "jit/object_header.hpp"

#include "database/mpo_database.hpp"

#include "service/protocol/common/sender_ref.hpp"
// #include "service/network/log.hpp"

#include "mega/memory.hpp"
#include "mega/reference_io.hpp"

#include <functional>
#include <memory>
#include <unordered_map>

namespace mega::runtime
{

class MemoryManager
{
    using PtrVariant = std::variant< HeapBufferPtr, FixedAllocator::FixedPtr >;
    inline void* get( const PtrVariant& ptr )
    {
        struct GetPtrVisitor
        {
            void* operator()( const HeapBufferPtr& heapPtr ) const { return heapPtr.get(); }
            void* operator()( const FixedAllocator::FixedPtr& fixedPtr ) const { return fixedPtr.get(); }
        };
        return std::visit( GetPtrVisitor{}, ptr );
    }

    using HeapMap       = std::unordered_map< reference, PtrVariant, reference::Hash >;
    using NetMap        = std::unordered_map< reference, reference, reference::Hash >;
    using Allocators    = std::map< TypeID, FixedAllocator::Ptr >;
    using AllocatorMap  = std::unordered_map< TypeID, FixedAllocator*, TypeID::Hash >;
    using OldHeapVector = std::vector< PtrVariant >;

public:
    using GetAllocatorFPtr = std::function< Allocator::Ptr( TypeID ) >;

    network::MemoryStatus getStatus() const
    {
        network::MemoryStatus status;
        status.m_heap   = m_usedHeapMemory;
        status.m_object = m_heapMap.size();

        auto i = status.m_allocators.begin();
        for( const auto& [ typeID, pAllocator ] : m_allocators )
        {
            i->typeID = typeID;
            i->status = pAllocator->getStatus();
            ++i;
        }
        return status;
    }

    template < typename TFunctor >
    inline MemoryManager( MPODatabase& database, MPO mpo, TFunctor&& getAllocatorFunction )
        : m_database( database )
        , m_mpo( mpo )
        , m_getAllocatorFPtr( std::move( getAllocatorFunction ) )
    {
        for( const auto& [ typeID, pMapping ] : database.getMemoryMappings() )
        {
            FixedAllocator::Ptr pAllocator = std::make_unique< FixedAllocator >(
                pMapping->get_block_size(), pMapping->get_fixed_allocation(), pMapping->get_block_alignment() );

            // register with all types
            for( FinalStage::Concrete::Object* pObject : pMapping->get_concrete() )
            {
                // SPDLOG_TRACE( "Fixed allocations for: {} of {}", typeID, pObject->get_concrete_id() );
                VERIFY_RTE( m_allocatorsMap.insert( { pObject->get_concrete_id(), pAllocator.get() } ).second );
            }

            m_allocators.insert( { typeID, std::move( pAllocator ) } );
        }
    }
/*
    inline network::SenderRef::AllocatorBaseArray getAllocators() const
    {
        network::SenderRef::AllocatorBaseArray result;
        auto                                   i = result.begin();

        for( const auto& [ typeID, pAllocator ] : m_allocators )
        {
            VERIFY_RTE_MSG( i != result.end(), "Too many allocators for SenderRef::AllocatorBaseArray" );
            i->type  = typeID;
            i->pBase = pAllocator->getDoubleBuffer();
            ++i;
        }

        return result;
    }

    inline void doubleBuffer()
    {
        for( const auto& [ _, pAllocator ] : m_allocators )
        {
            pAllocator->copyToDoubleBuffer();
        }
    }*/

    inline AllocationID getAllocationID() const { return m_allocationIDCounter; }
    inline U64          getAllocationCount() const { return m_heapMap.size(); }

    inline reference networkToHeap( const reference& networkAddress ) const
    {
        ASSERT( networkAddress.isNetworkAddress() );
        auto    iFind = m_netMap.find( networkAddress.getObjectAddress() );
        using ::operator<<;
        VERIFY_RTE_MSG( iFind != m_netMap.end(),
                        "Failed to locate network address entry for reference: " << networkAddress.getObjectAddress() );
        return reference::make( iFind->second, networkAddress.getTypeInstance() );
    }

private:
    inline reference construct( const reference& networkAddress, PtrVariant&& memory, Allocator::Ptr pAllocator )
    {
        using ::operator<<;
        VERIFY_RTE_MSG( networkAddress.isNetworkAddress(), "construct given heap address: " << networkAddress );

        const reference objectNetAddress = networkAddress.getObjectAddress();
        void*           pAddress         = get( memory );

        // establish the header including the network address, lock timestamp and shared ownership of allocator
        new( pAddress ) ObjectHeader{ ObjectHeaderBase{ objectNetAddress }, pAllocator };

        // invoke the constructor
        pAllocator->getCtor()( pAddress );

        const reference heapAddress{ objectNetAddress.getTypeInstance(), pAddress };

        VERIFY_RTE( m_heapMap.insert( { heapAddress, std::move( memory ) } ).second );
        VERIFY_RTE( m_netMap.insert( { objectNetAddress, heapAddress } ).second );

        return reference::make( heapAddress, networkAddress.getTypeInstance() );
    }

public:
    inline reference New( TypeID typeID )
    {
        // must acquire allocator EVERYTIME for now - which means request to JIT
        const TypeID   objectType = TypeID::make_object_from_typeID( typeID );
        Allocator::Ptr pAllocator = m_getAllocatorFPtr( objectType );
        auto           iFind      = m_allocatorsMap.find( objectType );
        if( iFind != m_allocatorsMap.end() )
        {
            // SPDLOG_TRACE( "Memory Manager:New fixed allocation for: {} of {}", typeID, objectType );
            FixedAllocator::FixedPtr pFixed = iFind->second->allocate();
            const reference          networkAddress{ TypeInstance{ typeID, 0 }, m_mpo, pFixed.getID() };
            return construct( networkAddress, PtrVariant{ std::move( pFixed ) }, pAllocator );
        }
        else
        {
            // SPDLOG_TRACE( "Memory Manager:New heap allocation for: {} of {}", typeID, objectType );
            auto          sizeAlign = pAllocator->getSizeAlignment();
            HeapBufferPtr memory( sizeAlign );
            m_usedHeapMemory += sizeAlign.size;
            const AllocationID allocationID = m_allocationIDCounter++;
            const reference    networkAddress{ TypeInstance{ typeID, 0 }, m_mpo, allocationID };
            ASSERT( ( typeID != ROOT_TYPE_ID ) || ( allocationID == ROOT_OBJECT_ID ) );
            return construct( networkAddress, PtrVariant{ std::move( memory ) }, pAllocator );
        }
    }

    inline void Delete( const reference& ref )
    {
        ASSERT( ref.isHeapAddress() );

        auto heapEntry = m_heapMap.extract( ref.getObjectAddress() );

        // remove the network address entry
        {
            auto    iFind2 = m_netMap.find( ref.getHeaderAddress() );
            using ::operator<<;
            VERIFY_RTE_MSG( iFind2 != m_netMap.end(), "Failed to locate network address entry for reference: " << ref );
            m_netMap.erase( iFind2 );
        }

        // get the object header
        auto pHeader = reinterpret_cast< ObjectHeader* >( get( heapEntry.mapped() ) );

        // invoke the destructor
        pHeader->m_pAllocator->getDtor()( ref.getObjectAddress(), pHeader );

        m_oldHeap.emplace_back( std::move( heapEntry.mapped() ) );
    }

    inline void Garbage()
    {
        for( auto& pHeap : m_oldHeap )
        {
            auto pHeader = reinterpret_cast< ObjectHeader* >( get( pHeap ) );
            pHeader->~ObjectHeader();
        }
        m_oldHeap.clear();
    }

    using MemoryEntry = HeapMap::node_type;

    inline MemoryEntry Move( const reference& ref )
    {
        ASSERT( ref.isHeapAddress() );

        // remove the network address entry
        {
            auto    iFind2 = m_netMap.find( ref.getHeaderAddress() );
            using ::operator<<;
            VERIFY_RTE_MSG( iFind2 != m_netMap.end(), "Failed to locate network address entry for reference: " << ref );
            m_netMap.erase( iFind2 );
        }
        return m_heapMap.extract( ref.getObjectAddress() );
    }

private:
    AllocationID     m_allocationIDCounter = ROOT_OBJECT_ID;
    U64              m_usedHeapMemory      = 0U;
    MPODatabase      m_database;
    MPO              m_mpo;
    GetAllocatorFPtr m_getAllocatorFPtr;
    HeapMap          m_heapMap;
    NetMap           m_netMap;
    Allocators       m_allocators;
    AllocatorMap     m_allocatorsMap;
    OldHeapVector    m_oldHeap;
};

} // namespace mega::runtime

#endif // GUARD_2023_January_07_memory_manager
