
#include "shared_allocator.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace runtime
{

IndexedBufferAllocator::IndexedBufferAllocator( ManagedSharedMemory& sharedMemory )
    : m_pSegmentManager( sharedMemory.get_segment_manager() )
    // sharedMemory.find_or_construct< SharedBufferVector >()
    , m_sharedBuffers( SharedPtrAllocator{ m_pSegmentManager } )
    , m_freeList( IndexAllocator{ m_pSegmentManager } )
{
    SPDLOG_TRACE( "IndexedBufferAllocator" );
}

IndexedBufferAllocator::~IndexedBufferAllocator() {}

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
        if ( reinterpret_cast< U64 >( pHeap.get() ) % szHeapAlign != 0 )
        {
            throw std::runtime_error( "Alignment error" );
        }
        // SPDLOG_TRACE( "IndexedBufferAllocator::allocate heap {}", pHeap.get() );
    }

    try
    {
        AllocationResult result{ pShared.get(), pHeap.get(), 0U };
        {
            if ( !m_freeList.empty() )
            {
                result.object = m_freeList.back();
                m_freeList.pop_back();
                m_sharedBuffers[ result.object ] = pShared;
                m_heapBuffers[ result.object ]   = std::move( pHeap );
            }
            else
            {
                result.object = static_cast< ObjectID >( m_sharedBuffers.size() );
                m_sharedBuffers.push_back( pShared );
                m_heapBuffers.push_back( std::move( pHeap ) );
            }
        }
        // SPDLOG_TRACE( "IndexedBufferAllocator::allocate shared {} {} {} {}", (void*)this, result.pShared,
        // result.object, m_sharedBuffers[ result.object ].get() );
        return result;
    }
    catch ( boost::container::out_of_range& ex )
    {
        m_pSegmentManager->deallocate( pShared.get() );
        throw boost::interprocess::bad_alloc{};
    }
    catch ( boost::interprocess::bad_alloc& ex )
    {
        m_pSegmentManager->deallocate( pShared.get() );
        throw boost::interprocess::bad_alloc{};
    }
}

void IndexedBufferAllocator::deallocate( Index index )
{
    // SPDLOG_TRACE( "IndexedBufferAllocator::allocate {}", index );
    SharedPtr pShared        = m_sharedBuffers[ index ];
    m_sharedBuffers[ index ] = nullptr;
    m_pSegmentManager->deallocate( pShared.get() );
    m_heapBuffers[ index ].reset();
    m_freeList.push_back( index );
}

void* IndexedBufferAllocator::getShared( Index index ) const
{
    void* pAddress = m_sharedBuffers[ index ].get();
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