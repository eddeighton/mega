
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

#ifndef GUARD_2023_January_09_remote_memory_manager
#define GUARD_2023_January_09_remote_memory_manager

#include "jit/allocator.hpp"
#include "jit/object_header.hpp"
#include "jit/code_generator.hpp"

#include "mega/reference_io.hpp"

#include "service/protocol/common/sender_ref.hpp"

#include <functional>
#include <memory>
#include <unordered_map>

namespace mega::runtime
{

class RemoteMemoryManager
{
    using HeapMap = std::unordered_map< reference, HeapBufferPtr, reference::Hash >;
    using NetMap  = std::unordered_map< reference, reference, reference::Hash >;

public:
    using GetAllocatorFPtr = std::function< Allocator::Ptr( TypeID, runtime::CodeGenerator::LLVMCompiler& ) >;

    template < typename TFunctor >
    RemoteMemoryManager( MP mp, TFunctor&& getAllocatorFunction )
        : m_mp( mp )
        , m_getAllocatorFPtr( std::move( getAllocatorFunction ) )
    {
    }

    network::MemoryStatus getStatus() const
    {
        network::MemoryStatus status;
        status.m_heap   = m_usedHeapMemory;
        status.m_object = m_heapMap.size();
        return status;
    }

    void MPODestroyed( const MPO& mpo )
    {
        std::vector< reference > heapAddresses;
        {
            for( auto i = m_heapMap.begin(), iEnd = m_heapMap.end(); i != iEnd; ++i )
            {
                if( i->first.getMPO() == mpo )
                {
                    heapAddresses.push_back( i->first );
                }
            }
        }
        for( const reference& ref : heapAddresses )
        {
            m_heapMap.erase( ref );
            m_netMap.erase( ref.getHeaderAddress() );
        }
    }

    bool tryNetworkToHeap( reference& networkAddress ) const
    {
        auto iFind = m_netMap.find( networkAddress.getObjectAddress() );
        if( iFind != m_netMap.end() )
        {
            networkAddress = reference::make( iFind->second, networkAddress.getTypeInstance() );
            return true;
        }
        return false;
    }

    reference networkToHeap( const reference& networkAddress, runtime::CodeGenerator::LLVMCompiler& llvmCompiler )
    {
        reference objectAddress = networkAddress.getObjectAddress();
        if( tryNetworkToHeap( objectAddress ) )
        {
            return objectAddress;
        }

        Allocator::Ptr pAllocator = m_getAllocatorFPtr( objectAddress.getType(), llvmCompiler );

        const mega::SizeAlignment sizeAlignment = pAllocator->getSizeAlignment();

        HeapBufferPtr pHeapBuffer( sizeAlignment );
        m_usedHeapMemory += sizeAlignment.size;

        // establish the header including the network address, lock timestamp and shared ownership of allocator
        const TimeStamp lockTime = 0U;

        new( pHeapBuffer.get() ) ObjectHeader{ ObjectHeaderBase{ objectAddress, lockTime }, pAllocator };

        // invoke the constructor
        pAllocator->getCtor()( pHeapBuffer.get() );

        const reference objectHeapAddress
            = reference{ objectAddress.getTypeInstance(), objectAddress.getOwnerID(), pHeapBuffer.get() };

        m_heapMap.insert( { objectHeapAddress, std::move( pHeapBuffer ) } );
        m_netMap.insert( { objectAddress, objectHeapAddress } );

        return reference::make( objectHeapAddress, networkAddress.getTypeInstance() );
    }
/*
    void Delete( reference& ref )
    {
        auto iFind = m_heapMap.find( ref.getObjectAddress() );

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
        pHeader->m_pAllocator->getDtor()( ref.getObjectAddress(), pHeader );

        // delete the object header
        pHeader->~ObjectHeader();

        // remove the heap address entry
        m_heapMap.erase( iFind );

        // null out the reference
        ref = reference{};
    }
*/
private:
    U64              m_usedHeapMemory = 0U;
    MP               m_mp;
    GetAllocatorFPtr m_getAllocatorFPtr;
    HeapMap          m_heapMap;
    NetMap           m_netMap;
};
} // namespace mega::runtime

#endif // GUARD_2023_January_09_remote_memory_manager
