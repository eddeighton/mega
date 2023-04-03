
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

#include "jit/allocator.hpp"
#include "jit/object_header.hpp"

#include "mega/memory.hpp"
#include "mega/reference_io.hpp"

#include <functional>
#include <memory>
#include <unordered_map>

namespace mega::runtime
{

class MemoryManager
{
    using HeapMap = std::unordered_map< reference, HeapBufferPtr, reference::Hash >;
    using NetMap  = std::unordered_map< reference, reference, reference::Hash >;

public:
    using GetAllocatorFPtr = std::function< Allocator::Ptr( TypeID ) >;

    template < typename TFunctor >
    MemoryManager( MPO mpo, TFunctor&& getAllocatorFunction )
        : m_mpo( mpo )
        , m_getAllocatorFPtr( std::move( getAllocatorFunction ) )
    {
    }

    AllocationID getAllocationID() const { return m_allocationIDCounter; }
    U64          getAllocationCount() const { return m_heapMap.size(); }

    reference networkToHeap( const reference& networkAddress ) const
    {
        ASSERT( networkAddress.isNetworkAddress() );
        auto    iFind = m_netMap.find( networkAddress.getObjectAddress() );
        using ::operator<<;
        VERIFY_RTE_MSG(
            iFind != m_netMap.end(), "Failed to locate network address entry for reference: " << networkAddress );
        return reference::make( iFind->second, networkAddress.getTypeInstance() );
    }

private:
    reference New( const reference& networkAddress )
    {
        const reference objectNetAddress = networkAddress.getObjectAddress();
        Allocator::Ptr  pAllocator       = m_getAllocatorFPtr( objectNetAddress.getType() );

        const mega::SizeAlignment sizeAlignment = pAllocator->getSizeAlignment();
        VERIFY_RTE_MSG( sizeAlignment.size > 0U, "Invalid size alignment" );

        HeapBufferPtr pHeapBuffer( sizeAlignment );

        const TimeStamp lockTime = 0U;

        new( pHeapBuffer.get() ) ObjectHeader{ ObjectHeaderBase{ objectNetAddress, lockTime }, pAllocator };

        // invoke the constructor
        pAllocator->getCtor()( pHeapBuffer.get() );

        reference heapAddress{ objectNetAddress.getTypeInstance(), m_mpo.getOwnerID(), pHeapBuffer.get() };

        VERIFY_RTE( m_heapMap.insert( { heapAddress, std::move( pHeapBuffer ) } ).second );
        VERIFY_RTE( m_netMap.insert( { objectNetAddress, heapAddress } ).second );

        return reference::make( heapAddress, networkAddress.getTypeInstance() );
    }

public:
    reference New( TypeID typeID )
    {
        // establish the header including the network address, lock timestamp and shared ownership of allocator
        const AllocationID allocationID = m_allocationIDCounter++;
        const reference    networkAddress{ TypeInstance{ typeID, 0 }, m_mpo, allocationID };
        ASSERT( ( typeID != ROOT_TYPE_ID ) || ( allocationID == ROOT_OBJECT_ID ) );
        return New( networkAddress );
    }

    void Delete( reference& ref )
    {
        ASSERT( ref.isHeapAddress() );

        auto    iFind = m_heapMap.find( ref.getObjectAddress() );
        using ::operator<<;
        VERIFY_RTE_MSG( iFind != m_heapMap.end(), "Failed to locate reference heap buffer: " << ref );

        // remove the network address entry
        {
            auto iFind2 = m_netMap.find( ref.getHeaderAddress() );
            VERIFY_RTE_MSG( iFind2 != m_netMap.end(), "Failed to locate network address entry for reference: " << ref );
            m_netMap.erase( iFind2 );
        }

        // get the object header
        auto pHeader = reinterpret_cast< ObjectHeader* >( iFind->second.get() );

        // invoke the destructor
        pHeader->m_pAllocator->getDtor()( pHeader );

        // delete the object header
        pHeader->~ObjectHeader();

        // remove the heap address entry
        m_heapMap.erase( iFind );

        // null out the reference
        ref = reference{};
    }

private:
    AllocationID     m_allocationIDCounter = ROOT_OBJECT_ID;
    MPO              m_mpo;
    GetAllocatorFPtr m_getAllocatorFPtr;
    HeapMap          m_heapMap;
    NetMap           m_netMap;
};

} // namespace mega::runtime

#endif // GUARD_2023_January_07_memory_manager
