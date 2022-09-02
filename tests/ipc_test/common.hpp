
#ifndef COMMON_SEPT_1_2022
#define COMMON_SEPT_1_2022

#include <boost/container/throw_exception.hpp>

#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/segment_manager.hpp>
#include <boost/interprocess/sync/mutex_family.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

static const char    SHARED_MEMORY_NAME[] = "MySharedMemory";
static constexpr int SHARED_MEMORY_SIZE   = 1024 * 1024 * 1;

using VoidPtr = boost::interprocess::offset_ptr< void, long, unsigned long >;

using ManagedHeapMemory = boost::interprocess::basic_managed_heap_memory<
    char, boost::interprocess::rbtree_best_fit< boost::interprocess::null_mutex_family, VoidPtr >,
    boost::interprocess::flat_map_index >;

using ManagedSharedMemory = boost::interprocess::basic_managed_shared_memory<
    char, boost::interprocess::rbtree_best_fit< boost::interprocess::null_mutex_family, VoidPtr >,
    boost::interprocess::iset_index >;

template < typename TMemoryType >
class ExampleBuffer
{
public:
    using TSegmentManagerType = typename TMemoryType::segment_manager;
    using Ptr                 = boost::interprocess::offset_ptr< ExampleBuffer >;
    using IntAllocator        = boost::interprocess::allocator< int, TSegmentManagerType >;
    using Vector              = boost::interprocess::vector< Ptr, IntAllocator >;

    ExampleBuffer( TSegmentManagerType* pSegmentManager )
        : m_vector( IntAllocator{ pSegmentManager } )
    {
    }

    int                                              m_int = 123;
    boost::interprocess::vector< int, IntAllocator > m_vector;
};

template < typename TMemoryType, typename TBuffer >
class IndexedBufferAllocator
{
public:
    using TSegmentManagerType = typename TMemoryType::segment_manager;
    using Index               = std::size_t;
    using Ptr                 = boost::interprocess::offset_ptr< TBuffer >;

private:
    using BufferAllocator = boost::interprocess::allocator< TBuffer, TSegmentManagerType >;
    using PtrAllocator    = boost::interprocess::allocator< Ptr, TSegmentManagerType >;
    using IndexAllocator  = boost::interprocess::allocator< Index, TSegmentManagerType >;
    using BufferVector    = boost::interprocess::vector< Ptr, PtrAllocator >;
    using FreeList        = boost::interprocess::vector< Index, IndexAllocator >;

public:
    IndexedBufferAllocator( TSegmentManagerType* pSegmentManager )
        : m_pSegmentManager( pSegmentManager )
        , m_bufferAllocator( pSegmentManager )
        , m_buffers( PtrAllocator{ pSegmentManager } )
        , m_freeList( IndexAllocator{ pSegmentManager } )
    {
    }

    Index allocate()
    {
        Ptr pBuffer = m_bufferAllocator.allocate_one();
        try
        {
            m_bufferAllocator.construct( pBuffer, m_pSegmentManager );

            if ( !m_freeList.empty() )
            {
                Index index        = m_freeList.back();
                m_buffers[ index ] = pBuffer;
                m_freeList.pop_back();
                return index;
            }
            else
            {
                m_buffers.push_back( pBuffer );
                return m_buffers.size() - 1U;
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

    void free( Index index )
    {
        m_freeList.push_back( index );
        Ptr pBuffer = m_buffers[ index ];
        m_bufferAllocator.destroy( pBuffer );
        m_bufferAllocator.deallocate_one( pBuffer );
    }

    Ptr get( Index index ) const { return m_buffers[ index ]; }

    std::size_t size() const { return m_buffers.size() - m_freeList.size(); }

private:
    TSegmentManagerType* m_pSegmentManager;
    BufferAllocator      m_bufferAllocator;
    BufferVector         m_buffers;
    FreeList             m_freeList;
};

using ExampleSharedBufferIndex = IndexedBufferAllocator< ManagedSharedMemory, ExampleBuffer< ManagedSharedMemory > >;
using ExampleHeapBufferIndex   = IndexedBufferAllocator< ManagedHeapMemory, ExampleBuffer< ManagedHeapMemory > >;

#endif // COMMON_SEPT_1_2022
