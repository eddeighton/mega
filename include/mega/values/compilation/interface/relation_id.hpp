
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

#ifndef GUARD_2023_January_19_relation_id
#define GUARD_2023_January_19_relation_id

#include "mega/values/compilation/interface/inline.h"

#include "mega/values/compilation/interface/object_id.hpp"
#include "mega/values/compilation/interface/sub_object_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>
#include <iomanip>

namespace mega::interface
{

struct RelationID : public c_interface_relation_id
{
public:
    using ValueType = U64;

    struct Hash
    {
        inline U64 operator()( const RelationID& ref ) const noexcept { return ref.getValue(); }
    };

    constexpr inline RelationID()
        : c_interface_relation_id( c_interface_relation_id_make( 0, 0 ) )
    {
    }
    constexpr inline explicit RelationID( c_interface_relation_id value )
        : c_interface_relation_id( value )
    {
    }
    constexpr inline explicit RelationID( TypeID up, TypeID lo )
        : c_interface_relation_id( c_interface_relation_id_make( up.getValue(), lo.getValue() ) )
    {
    }
    constexpr inline explicit RelationID( ValueType value )
        : c_interface_relation_id( c_interface_relation_id_from_int( value ) )
    {
    }

    constexpr inline RelationID( const RelationID& cpy ) = default;

    constexpr inline ValueType getValue() const { return c_interface_relation_id_as_int( *this ); }

    constexpr inline TypeID getUpper() const { return TypeID{ upper }; }
    constexpr inline TypeID getLower() const { return TypeID{ lower }; }

    constexpr inline bool operator==( const RelationID& cmp ) const { return getValue() == cmp.getValue(); }
    constexpr inline bool operator!=( const RelationID& cmp ) const { return !this->operator==( cmp ); }
    constexpr inline bool operator<( const RelationID& cmp ) const { return getValue() < cmp.getValue(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "lower_object_id", lower.object_id.value );
            archive& boost::serialization::make_nvp( "lower_sub_object_id", lower.sub_object_id.value );
            archive& boost::serialization::make_nvp( "upper_object_id", upper.object_id.value );
            archive& boost::serialization::make_nvp( "upper_sub_object_id", upper.sub_object_id.value );
        }
        else
        {
            archive& lower.object_id.value;
            archive& lower.sub_object_id.value;
            archive& upper.object_id.value;
            archive& upper.sub_object_id.value;
        }
    }
};

static_assert( sizeof( RelationID ) == 8U, "Invalid RelationID Size" );
static_assert( sizeof( RelationID ) == sizeof( RelationID::ValueType ), "Invalid RelationID Size" );

inline constexpr RelationID operator""_R( unsigned long long int value )
{
    return RelationID( static_cast< RelationID::ValueType >( value ) );
}

static constexpr RelationID OWNERSHIP_RELATION_ID = 0x0000000000000000_R;
static constexpr RelationID TEST_RELATION_ID      = 0x0001000400020003_R;

static_assert( TEST_RELATION_ID.getUpper().getObjectID() == 1_IO );
static_assert( TEST_RELATION_ID.getUpper().getSubObjectID() == 4_ISO );
static_assert( TEST_RELATION_ID.getLower().getObjectID() == 2_IO );
static_assert( TEST_RELATION_ID.getLower().getSubObjectID() == 3_ISO );

inline std::ostream& operator<<( std::ostream& os, const RelationID& relationID )
{
    // this must work within a filename - used for relation code gen
    return os << "0x" << std::hex << std::setw( 16 ) << std::setfill( '0' ) << relationID.getValue() << std::dec << "_R";
}
} // namespace mega::interface

#endif // GUARD_2023_January_19_relation_id
