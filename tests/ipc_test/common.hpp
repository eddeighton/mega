
#ifndef COMMON_SEPT_1_2022
#define COMMON_SEPT_1_2022
/*
#include "mega/default_traits.hpp"
#include "mega/shared_allocator.hpp"

#include <boost/container/throw_exception.hpp>

static const char    SHARED_MEMORY_NAME[] = "MySharedMemory";
static constexpr int SHARED_MEMORY_SIZE   = 1024 * 1024 * 1;

struct HeapPart_Root
{
    class std::vector< int > m_testDimension2;
};
static_assert( std::is_standard_layout< HeapPart_Root >::value, "Invalid non-standard layout for HeapPart_Root" );
static_assert( sizeof( HeapPart_Root ) == 24, "Unexpected size for: HeapPart_Root" );
static_assert( offsetof( HeapPart_Root, m_testDimension2 ) == 0, "Unexpected offset for: m_testDimension2" );

struct HeapBuffer_3
{
    HeapPart_Root Root[ 1 ];
};
static_assert( std::is_standard_layout< HeapBuffer_3 >::value, "Invalid non-standard layout for HeapBuffer_3" );
static_assert( offsetof( HeapBuffer_3, Root ) == 0, "Unexpected offset for: Root" );

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

using ExampleBufferIndex
    = mega::IndexedBufferAllocator< mega::runtime::ManagedSharedMemory, SharedBuffer_3, HeapBuffer_3 >;
*/
#endif // COMMON_SEPT_1_2022
