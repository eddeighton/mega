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

#ifndef SHARED_ALLOCATOR_8_SEPT
#define SHARED_ALLOCATOR_8_SEPT

#include "mega/common.hpp"
#include "mega/default_traits.hpp"

#include <cstddef>
#include <vector>
#include <memory>
#include <utility>

namespace mega::runtime
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
    using IndexPtr           = boost::interprocess::offset_ptr< SharedBufferVector >;
    using FreeListPtr        = boost::interprocess::offset_ptr< FreeList >;
    using HeapBufferVector   = std::vector< HeapPtr >;

    IndexedBufferAllocator( const IndexedBufferAllocator& )            = delete;
    IndexedBufferAllocator& operator=( const IndexedBufferAllocator& ) = delete;

public:
    IndexedBufferAllocator( TypeID objectTypeID, ManagedSharedMemory& sharedMemory );
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
    std::string          m_strSharedName;
    IndexPtr             m_pSharedIndex;
    FreeListPtr          m_pFreeList;
    HeapBufferVector     m_heapBuffers;
};

} // namespace mega::runtime

#endif // SHARED_ALLOCATOR_8_SEPT
