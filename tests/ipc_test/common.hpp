
#ifndef COMMON_SEPT_1_2022
#define COMMON_SEPT_1_2022

#include "mega/default_traits.hpp"

#include <boost/container/throw_exception.hpp>

static const char    SHARED_MEMORY_NAME[] = "MySharedMemory";
static constexpr int SHARED_MEMORY_SIZE   = 1024 * 1024 * 1;

using Allocator
    = boost::interprocess::allocator< mega::reference, typename mega::runtime::ManagedSharedMemory::segment_manager >;

struct _PartPlug
{
    using TSegmentManagerType = mega::runtime::ManagedSharedMemory::segment_manager;
    _PartPlug( TSegmentManagerType* pSegmentManager )
        : link_5( mega::DimensionTraits< mega::ReferenceVector >::init( pSegmentManager ) )
    {
    }
    mega::ReferenceVector link_5;
};
struct _PartSocket2
{
    using TSegmentManagerType = mega::runtime::ManagedSharedMemory::segment_manager;
    _PartSocket2( TSegmentManagerType* pSegmentManager )
        : link_4( Allocator( pSegmentManager ) )
    {
    }
    mega::ReferenceVector link_4;
};

// ObjectTypeID
struct SharedBuffer_3
{
    using TSegmentManagerType = mega::runtime::ManagedSharedMemory::segment_manager;
    SharedBuffer_3( TSegmentManagerType* pSegmentManager )
        : Plug{ { _PartPlug( pSegmentManager ) } }
        , Socket2{ { _PartSocket2( pSegmentManager ) } }
    {
    }
    _PartPlug    Plug[ 1 ];
    _PartSocket2 Socket2[ 1 ];
};
/*
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
};*/

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

using ExampleSharedBufferIndex = IndexedBufferAllocator< mega::runtime::ManagedSharedMemory, SharedBuffer_3 >;
// using ExampleHeapBufferIndex   = IndexedBufferAllocator< ManagedHeapMemory, SharedBuffer_3 >;

#endif // COMMON_SEPT_1_2022
