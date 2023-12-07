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

#ifndef EG_DEFAULT_TRAITS
#define EG_DEFAULT_TRAITS

#include "mega/values/native_types.hpp"

#include "mega/values/runtime/reference.hpp"
#include "mega/values/runtime/maths_types.hpp"

#include <boost/dynamic_bitset.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

#include <vector>

// #include <stdbool.h>

#ifdef _WIN32
#define _Bool bool
#endif
/*
#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
*/
namespace mega
{

template < typename T >
concept IsReferenceType = std::is_base_of< mega::reference, T >::value;

template < typename T >
struct DimensionTraits;

using ReferenceVector = ::std::vector< mega::reference >;

static constexpr char psz_mega_reference[]        = "class mega::reference";
static constexpr char psz_mega_reference_vector[] = "class std::vector<class mega::reference>";

using LinkTypeVector = ::std::vector< mega::TypeID >;

static constexpr char psz_link_type[]        = "class mega::TypeID";
static constexpr char psz_link_type_vector[] = "class std::vector<class mega::TypeID>";

// template <typename Block = unsigned long,
//           typename Allocator = std::allocator<Block> >
// class dynamic_bitset;
using BitSet = ::boost::dynamic_bitset< mega::U64, std::allocator< mega::U64 > >;
static constexpr char psz_bitset[]
    = "class boost::dynamic_bitset<unsigned long long,class std::allocator<unsigned long long>>";

// reference types
template < IsReferenceType T >
struct DimensionTraits< T >
{
    using Type                              = T;
    using Read                              = const Type&;
    using Write                             = Type;
    using Erased                            = mega::reference;
    inline static const mega::U64 Hash      = typeid( Type ).hash_code();
    static const mega::U64        Size      = sizeof( Type );
    static const mega::U64        Alignment = alignof( Type );
    static const mega::U64        Simple    = std::is_trivially_copyable< Type >::value;
};

// container of reference types
template < IsReferenceType T >
struct DimensionTraits< std::vector< T > >
{
    using Type                              = std::vector< T >;
    using Read                              = const Type&;
    using Write                             = Type;
    using Erased                            = std::vector< mega::reference >;
    inline static const mega::U64 Hash      = typeid( Type ).hash_code();
    static const mega::U64        Size      = sizeof( Type );
    static const mega::U64        Alignment = alignof( Type );
    static const mega::U64        Simple    = std::is_trivially_copyable< T >::value;
};

template <>
struct DimensionTraits< BitSet >
{
    using Type                              = BitSet;
    using Read                              = const Type&;
    using Write                             = Type;
    using Erased                            = Type;
    inline static const mega::U64 Hash      = typeid( Type ).hash_code();
    static const mega::U64        Size      = sizeof( Type );
    static const mega::U64        Alignment = alignof( Type );
    static const mega::U64        Simple    = false;
};

// default dimension traits
template < typename T >
struct DimensionTraits
{
    using Type                              = T;
    using Read                              = const Type&;
    using Write                             = Type;
    using Erased                            = Type;
    inline static const mega::U64 Hash      = typeid( Type ).hash_code();
    static const mega::U64        Size      = sizeof( Type );
    static const mega::U64        Alignment = alignof( Type );
    static const mega::U64        Simple    = std::is_trivially_copyable< Type >::value;
};

} // namespace mega

#endif // EG_DEFAULT_TRAITS