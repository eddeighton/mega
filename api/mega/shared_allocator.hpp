
#ifndef SHARED_ALLOCATOR_8_SEPT
#define SHARED_ALLOCATOR_8_SEPT

#include "default_traits.hpp"

#include <cstddef>
#include <vector>
#include <memory>

namespace mega
{

template < typename TSharedMemoryManagerType, typename TSharedBuffer, typename THeapBuffer >
class IndexedBufferAllocator
{
public:
    using TSegmentManagerType = typename TSharedMemoryManagerType::segment_manager;
    using Index               = std::size_t;
    using SharedPtr           = boost::interprocess::offset_ptr< TSharedBuffer >;
    using HeapPtr             = std::unique_ptr< THeapBuffer >;

private:
    using BufferAllocator    = boost::interprocess::allocator< TSharedBuffer, TSegmentManagerType >;
    using SharedPtrAllocator = boost::interprocess::allocator< SharedPtr, TSegmentManagerType >;
    using IndexAllocator     = boost::interprocess::allocator< Index, TSegmentManagerType >;
    using SharedBufferVector = boost::interprocess::vector< SharedPtr, SharedPtrAllocator >;
    using HeapBufferVector   = std::vector< HeapPtr >;
    using FreeList           = boost::interprocess::vector< Index, IndexAllocator >;

public:
    IndexedBufferAllocator( TSegmentManagerType* pSegmentManager )
        : m_pSegmentManager( pSegmentManager )
        , m_bufferAllocator( pSegmentManager )
        , m_sharedBuffers( SharedPtrAllocator{ pSegmentManager } )
        , m_freeList( IndexAllocator{ pSegmentManager } )
    {
    }

    Index allocate()
    {
        SharedPtr pBuffer     = m_bufferAllocator.allocate_one();
        HeapPtr   pHeapBuffer = std::make_unique< THeapBuffer >();
        try
        {
            m_bufferAllocator.construct( pBuffer, m_pSegmentManager );

            if ( !m_freeList.empty() )
            {
                Index index              = m_freeList.back();
                m_sharedBuffers[ index ] = pBuffer;
                m_heapBuffers[ index ]   = std::move( pHeapBuffer );
                m_freeList.pop_back();
                return index;
            }
            else
            {
                m_sharedBuffers.push_back( pBuffer );
                m_heapBuffers.push_back( std::move( pHeapBuffer ) );
                return m_sharedBuffers.size() - 1U;
            }
        }
        catch ( boost::container::out_of_range& ex )
        {
            m_bufferAllocator.deallocate_one( pBuffer );
            throw boost::interprocess::bad_alloc{};
        }
        catch ( boost::interprocess::bad_alloc& ex )
        {
            m_bufferAllocator.deallocate_one( pBuffer );
            throw boost::interprocess::bad_alloc{};
        }
    }

    void deallocate( Index index )
    {
        m_freeList.push_back( index );
        SharedPtr pBuffer = m_sharedBuffers[ index ];
        m_bufferAllocator.destroy( pBuffer );
        m_bufferAllocator.deallocate_one( pBuffer );
        m_heapBuffers[ index ].reset();
    }

    SharedPtr    getShared( Index index ) const { return m_sharedBuffers[ index ]; }
    THeapBuffer* getHeap( Index index ) const { return m_heapBuffers[ index ].get(); }
    std::size_t  size() const { return m_sharedBuffers.size() - m_freeList.size(); }

private:
    TSegmentManagerType* m_pSegmentManager;
    BufferAllocator      m_bufferAllocator;
    SharedBufferVector   m_sharedBuffers;
    HeapBufferVector     m_heapBuffers;
    FreeList             m_freeList;
};

} // namespace mega

#endif // SHARED_ALLOCATOR_8_SEPT
