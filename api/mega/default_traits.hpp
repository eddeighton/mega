//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef EG_DEFAULT_TRAITS
#define EG_DEFAULT_TRAITS

#include "common.hpp"

#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
// #include <boost/interprocess/managed_heap_memory.hpp>
#include <boost/interprocess/segment_manager.hpp>
#include <boost/interprocess/sync/mutex_family.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace mega
{

// default dimension traits
template < typename T >
struct DimensionTraits
{
    using Read                       = const T&;
    using Write                      = T;
    using Get                        = T&;
    static const mega::U64 Size      = sizeof( T );
    static const mega::U64 Alignment = alignof( T );
    static const mega::U64 Simple    = std::is_trivially_copyable< T >::value;

    template < typename TSegmentManager >
    static T&& init( TSegmentManager* )
    {
        return std::move( T{} );
    }
};

namespace runtime
{

using VoidPtr = boost::interprocess::offset_ptr< void, long, unsigned long >;

// using ManagedHeapMemory = boost::interprocess::basic_managed_heap_memory<
//     char, boost::interprocess::rbtree_best_fit< boost::interprocess::null_mutex_family, VoidPtr >,
//     boost::interprocess::iset_index >;

using ManagedSharedMemory = boost::interprocess::basic_managed_shared_memory<
    char, boost::interprocess::rbtree_best_fit< boost::interprocess::null_mutex_family, VoidPtr >,
    boost::interprocess::iset_index >;

} // namespace runtime

using ReferenceVector = boost::interprocess::vector<
    mega::reference,
    boost::interprocess::allocator< mega::reference, typename mega::runtime::ManagedSharedMemory::segment_manager > >;

template <>
struct DimensionTraits< ReferenceVector >
{
    using Allocator
        = boost::interprocess::allocator< mega::reference, mega::runtime::ManagedSharedMemory::segment_manager >;

    using Read  = const ReferenceVector&;
    using Write = ReferenceVector;
    using Get   = ReferenceVector&;

    static const mega::U64 Size      = sizeof( ReferenceVector );
    static const mega::U64 Alignment = alignof( ReferenceVector );
    static const mega::U64 Simple    = true;

    static inline ReferenceVector init( mega::runtime::ManagedSharedMemory::segment_manager* pSegmentManager )
    {
        return ReferenceVector( Allocator( pSegmentManager ) );
    }
};

} // namespace mega

#endif // EG_DEFAULT_TRAITS