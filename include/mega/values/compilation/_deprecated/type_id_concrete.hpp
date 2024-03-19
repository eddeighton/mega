
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

#ifndef GUARD_03_NOV_2023_CONCRETE_TYPE_ID
#define GUARD_03_NOV_2023_CONCRETE_TYPE_ID

#include "mega/values/compilation/type_id.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>

namespace mega
{

class concrete::TypeID : public c_concrete_type_id
{
public:
    using ValueType   = U32;
    using ObjectID    = U16;
    using SubObjectID = U16;

    struct Hash
    {
        inline U64 operator()( const concrete::TypeID& ref ) const noexcept { return ref.getValue(); }
    };

    constexpr concrete::TypeID()
        : c_concrete_type_id{ 0, 0 }
    {
    }
    constexpr explicit concrete::TypeID( ObjectID object, SubObjectID subObject )
        : c_concrete_type_id{ object, subObject }
    {
    }

    explicit concrete::TypeID( ValueType value )
        : c_concrete_type_id( c_concrete_type_id_from_int( value ) )
    {
    }

    constexpr concrete::TypeID( const concrete::TypeID& cpy ) = default;

    constexpr inline bool isObject() const { return sub_object_id.value == 0; }

    inline ValueType             getValue() const { return c_concrete_type_id_as_int( *this ); }
    constexpr inline ObjectID    getObjectID() const { return object_id.value; }
    constexpr inline SubObjectID getSubObjectID() const { return sub_object_id.value; }

    inline      operator ValueType() const { return getValue(); }
    inline bool valid() const { return getValue() != 0; }

    inline bool operator==( const concrete::TypeID& cmp ) const { return getValue() == cmp.getValue(); }
    inline bool operator!=( const concrete::TypeID& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const concrete::TypeID& cmp ) const { return getValue() < cmp.getValue(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "object_id", object_id.value );
            archive& boost::serialization::make_nvp( "sub_object_id", sub_object_id.value );
        }
        else
        {
            archive& object_id.value;
            archive& sub_object_id.value;
        }
    }
};

static_assert( sizeof( concrete::TypeID ) == sizeof( U32 ), "Invalid concrete::TypeID Size" );
std::ostream& operator<<( std::ostream& os, const mega::concrete::TypeID& typeID );
std::istream& operator>>( std::istream& is, mega::concrete::TypeID& typeID );

static constexpr concrete::TypeID NULL_CONCRETE_TYPE_ID = concrete::TypeID{ 0, 0 };
static constexpr concrete::TypeID ROOT_CONCRETE_TYPE_ID = concrete::TypeID{ 1, 0 };

} // namespace mega

#endif // GUARD_03_NOV_2023_CONCRETE_TYPE_ID
