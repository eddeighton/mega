
#ifndef SHARED_ALLOCATOR_8_SEPT
#define SHARED_ALLOCATOR_8_SEPT

#include "common.hpp"
#include "default_traits.hpp"

#include <cstddef>
#include <vector>
#include <memory>
#include <utility>

namespace mega
{

class IndexedBufferAllocator
{
public:
    using TSegmentManagerType = typename runtime::ManagedSharedMemory::segment_manager;
    using Index               = ObjectID;
    using SharedPtr           = boost::interprocess::offset_ptr< void >;
    using HeapPtr             = std::unique_ptr< char[] >;

private:
    using SharedPtrAllocator = boost::interprocess::allocator< SharedPtr, TSegmentManagerType >;
    using IndexAllocator     = boost::interprocess::allocator< Index, TSegmentManagerType >;
    using SharedBufferVector = boost::interprocess::vector< SharedPtr, SharedPtrAllocator >;
    using HeapBufferVector   = std::vector< HeapPtr >;
    using FreeList           = boost::interprocess::vector< Index, IndexAllocator >;

public:
    IndexedBufferAllocator( TSegmentManagerType* pSegmentManager )
        : m_pSegmentManager( pSegmentManager )
        , m_sharedBuffers( SharedPtrAllocator{ pSegmentManager } )
        , m_freeList( IndexAllocator{ pSegmentManager } )
    {
    }

    struct AllocationResult
    {
        void* pShared;
        void* pHeap;
        Index object;
    };
    AllocationResult allocate( std::size_t szShared, std::size_t szHeap )
    {
        SharedPtr pShared{ m_pSegmentManager->allocate( szShared ) };
        HeapPtr   pHeap{ new char[ szHeap ] };

        try
        {
            if ( !m_freeList.empty() )
            {
                Index index              = m_freeList.back();
                m_sharedBuffers[ index ] = pShared;
                m_heapBuffers[ index ]   = std::move( pHeap );
                m_freeList.pop_back();
                return AllocationResult{ pShared.get(), pHeap.get(), index };
            }
            else
            {
                const ObjectID object = static_cast< ObjectID >( m_sharedBuffers.size() );
                m_sharedBuffers.push_back( pShared );
                m_heapBuffers.push_back( std::move( pHeap ) );
                return AllocationResult{ pShared.get(), pHeap.get(), object };
            }
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

    void deallocate( Index index )
    {
        m_freeList.push_back( index );
        m_pSegmentManager->deallocate( m_sharedBuffers[ index ].get() );
        m_heapBuffers[ index ].reset();
    }

    SharedPtr   getShared( Index index ) const { return m_sharedBuffers[ index ]; }
    void*       getHeap( Index index ) const { return m_heapBuffers[ index ].get(); }
    std::size_t size() const { return m_sharedBuffers.size() - m_freeList.size(); }

private:
    TSegmentManagerType* m_pSegmentManager;
    SharedBufferVector   m_sharedBuffers;
    HeapBufferVector     m_heapBuffers;
    FreeList             m_freeList;
};

} // namespace mega

#endif // SHARED_ALLOCATOR_8_SEPT
