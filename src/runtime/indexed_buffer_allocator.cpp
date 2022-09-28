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


#include "indexed_buffer_allocator.hpp"

#include "service/network/log.hpp"

#include <sstream>

namespace mega
{
namespace runtime
{
namespace
{
std::string generateSharedName( TypeID objectTypeID )
{
    std::ostringstream os;
    os << objectTypeID;
    return os.str();
}
} // namespace

IndexedBufferAllocator::IndexedBufferAllocator( TypeID objectTypeID, ManagedSharedMemory& sharedMemory )
    : m_pSegmentManager( sharedMemory.get_segment_manager() )
    , m_strSharedName( generateSharedName( objectTypeID ) )
    , m_pSharedIndex( sharedMemory.find_or_construct< SharedBufferVector >( m_strSharedName.c_str() )(
          SharedPtrAllocator{ m_pSegmentManager } ) )
    , m_pFreeList(
          sharedMemory.find_or_construct< FreeList >( m_strSharedName.c_str() )( IndexAllocator{ m_pSegmentManager } ) )

{
    // SPDLOG_TRACE( "IndexedBufferAllocator" );
}

IndexedBufferAllocator::~IndexedBufferAllocator() 
{
    //
}

IndexedBufferAllocator::AllocationResult IndexedBufferAllocator::allocate( mega::U64 szSharedSize,
                                                                           mega::U64 szSharedAlign,
                                                                           mega::U64 szHeapSize, mega::U64 szHeapAlign )
{
    // SPDLOG_TRACE(
    //     "IndexedBufferAllocator::allocate {} {} {} {}", szSharedSize, szSharedAlign, szHeapSize, szHeapAlign );
    SharedPtr pShared;
    if ( szSharedSize )
    {
        void* pAllocated = m_pSegmentManager->allocate_aligned( szSharedSize, szSharedAlign );
        // SPDLOG_TRACE( "IndexedBufferAllocator::allocate shared memory at {}", pAllocated );
        pShared = pAllocated;
    }

    HeapPtr pHeap;
    if ( szHeapSize )
    {
        pHeap.reset( new ( std::align_val_t( szHeapAlign ) ) char[ szHeapSize ] );
        // SPDLOG_TRACE( "IndexedBufferAllocator::allocate heap {}", pHeap.get() );
    }

    try
    {
        AllocationResult result{ pShared.get(), pHeap.get(), 0U };
        {
            if ( !m_pFreeList->empty() )
            {
                result.object = m_pFreeList->back();
                m_pFreeList->pop_back();
                m_pSharedIndex->at( result.object ) = pShared;
                m_heapBuffers[ result.object ]      = std::move( pHeap );
            }
            else
            {
                result.object = static_cast< ObjectID >( m_pSharedIndex->size() );
                m_pSharedIndex->push_back( pShared );
                m_heapBuffers.push_back( std::move( pHeap ) );
            }
        }
        // SPDLOG_TRACE( "IndexedBufferAllocator::allocate shared {} {} {} {}", (void*)this, result.pShared,
        // result.object, m_sharedBuffers[ result.object ].get() );
        return result;
    }
    catch ( boost::container::out_of_range& ex )
    {
        SPDLOG_ERROR( "Error allocating {}", m_strSharedName );
        m_pSegmentManager->deallocate( pShared.get() );
        throw boost::interprocess::bad_alloc{};
    }
    catch ( boost::interprocess::bad_alloc& ex )
    {
        SPDLOG_ERROR( "Error allocating {}", m_strSharedName );
        m_pSegmentManager->deallocate( pShared.get() );
        throw boost::interprocess::bad_alloc{};
    }
}

void IndexedBufferAllocator::deallocate( Index index )
{
    // SPDLOG_TRACE( "IndexedBufferAllocator::allocate {}", index );
    SharedPtr pShared           = m_pSharedIndex->at( index );
    m_pSharedIndex->at( index ) = nullptr;
    m_pSegmentManager->deallocate( pShared.get() );
    m_heapBuffers[ index ].reset();
    m_pFreeList->push_back( index );
}

void* IndexedBufferAllocator::getShared( Index index ) const
{
    void* pAddress = m_pSharedIndex->at( index ).get();
    // SPDLOG_TRACE( "IndexedBufferAllocator::getShared {} {} {}", (void*)this, index, pAddress );
    return pAddress;
}

void* IndexedBufferAllocator::getHeap( Index index ) const
{
    // SPDLOG_TRACE( "IndexedBufferAllocator::getHeap {}", index );
    return m_heapBuffers[ index ].get();
}

} // namespace runtime
} // namespace mega