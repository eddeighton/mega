
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

#ifndef GUARD_2023_December_12_concrete_object_id
#define GUARD_2023_December_12_concrete_object_id

#include "mega/values/compilation/concrete/inline.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <istream>
#include <ostream>

namespace mega::concrete
{

class ObjectID : public c_concrete_object_id
{
public:
    using ValueType = U16;

    struct Hash
    {
        inline U64 operator()( const ObjectID& value ) const noexcept { return value.getValue(); }
    };

    constexpr inline ObjectID()
        : c_concrete_object_id{ 0 }
    {
    }

    constexpr inline explicit ObjectID( c_concrete_object_id value )
        : c_concrete_object_id( value )
    {
    }

    constexpr inline explicit ObjectID( ValueType value )
        : c_concrete_object_id{ value }
    {
    }

    constexpr inline ObjectID( const ObjectID& cpy ) = default;

    constexpr inline ValueType getValue() const { return value; }

    constexpr inline bool operator==( const ObjectID& cmp ) const { return getValue() == cmp.getValue(); }
    constexpr inline bool operator!=( const ObjectID& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const ObjectID& cmp ) const { return getValue() < cmp.getValue(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "concrete_object_id", value );
        }
        else
        {
            archive& value;
        }
    }
};

static_assert( sizeof( ObjectID ) == sizeof( U16 ), "Invalid Concrete ObjectID Size" );

inline constexpr ObjectID operator ""_CO( unsigned long long int value )
{
    return ObjectID{ static_cast< ObjectID::ValueType >( value ) };
}

static constexpr ObjectID NULL_OBJECT_ID = 0x0000_CO;
static constexpr ObjectID ROOT_OBJECT_ID = 0x0001_CO;

inline std::ostream& operator<<( std::ostream& os, const ObjectID& value )
{
    return os << "0x" << std::hex << std::setw( 4 ) << std::setfill( '0' ) << value.getValue() << "_CO";
}

inline std::istream& operator>>( std::istream& is, ObjectID& typeID )
{
    ObjectID::ValueType value;
    is >> std::hex >> value;
    typeID = ObjectID{ value };
    return is;
}

} // namespace mega

#endif // GUARD_2023_December_12_concrete_object_id