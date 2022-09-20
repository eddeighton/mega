
#ifndef SHARED_ALLOCATOR_8_SEPT
#define SHARED_ALLOCATOR_8_SEPT

#include "mega/common.hpp"
#include "mega/default_traits.hpp"

#include <cstddef>
#include <vector>
#include <memory>
#include <utility>

namespace mega
{
namespace runtime
{

class IndexedBufferAllocator
{
public:
    using TSegmentManagerType = typename ManagedSharedMemory::segment_manager;
    using Index               = ObjectID;
    using SharedPtr           = VoidPtr;
    using HeapPtr             = std::unique_ptr< char[] >;

private:
    using SharedPtrAllocator = boost::interprocess::allocator< SharedPtr, TSegmentManagerType >;
    using SharedBufferVector = boost::interprocess::vector< SharedPtr, SharedPtrAllocator >;
    using IndexAllocator     = boost::interprocess::allocator< Index, TSegmentManagerType >;
    using FreeList           = boost::interprocess::vector< Index, IndexAllocator >;
    using HeapBufferVector   = std::vector< HeapPtr >;

    IndexedBufferAllocator( const IndexedBufferAllocator& )            = delete;
    IndexedBufferAllocator& operator=( const IndexedBufferAllocator& ) = delete;

public:
    IndexedBufferAllocator( ManagedSharedMemory& sharedMemory );
    ~IndexedBufferAllocator();

    struct AllocationResult
    {
        void* pShared;
        void* pHeap;
        Index object;
    };
    AllocationResult     allocate( mega::U64 szSharedSize, mega::U64 szSharedAlign, mega::U64 szHeapSize,
                                   mega::U64 szHeapAlign );
    void                 deallocate( Index index );
    void*                getShared( Index index ) const;
    void*                getHeap( Index index ) const;
    TSegmentManagerType* getSegmentManager() const { return m_pSegmentManager; }

private:
    TSegmentManagerType* m_pSegmentManager;
    SharedBufferVector   m_sharedBuffers;
    FreeList             m_freeList;
    HeapBufferVector     m_heapBuffers;
};

} // namespace runtime
} // namespace mega

#endif // SHARED_ALLOCATOR_8_SEPT
