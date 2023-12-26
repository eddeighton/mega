
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

#ifndef GUARD_2023_December_12_concrete_sub_object_id_instance
#define GUARD_2023_December_12_concrete_sub_object_id_instance

#include "mega/values/compilation/concrete/inline.h"

#include "mega/values/compilation/concrete/sub_object_id.hpp"
#include "mega/values/compilation/concrete/instance.hpp"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>
#include <iomanip>

namespace mega::concrete
{

class SubObjectIDInstance : public c_concrete_sub_object_id_instance
{
public:
    using ValueType = U32;

    struct Hash
    {
        inline U64 operator()( const SubObjectIDInstance& ref ) const noexcept { return ref.getValue(); }
    };

    constexpr inline SubObjectIDInstance()
        : c_concrete_sub_object_id_instance{ 0, 0 }
    {
    }
    constexpr inline explicit SubObjectIDInstance( c_concrete_sub_object_id_instance value )
        : c_concrete_sub_object_id_instance( value )
    {
    }
    constexpr inline explicit SubObjectIDInstance( SubObjectID subObjectID, Instance instance )
        : c_concrete_sub_object_id_instance(
            c_concrete_sub_object_id_instance_make( subObjectID.getValue(), instance.getValue() ) )
    {
    }
    constexpr inline explicit SubObjectIDInstance( ValueType value )
        : c_concrete_sub_object_id_instance( c_concrete_sub_object_id_instance_from_int( value ) )
    {
    }

    constexpr inline SubObjectIDInstance( const SubObjectIDInstance& cpy ) = default;

    constexpr inline bool isObject() const { return sub_object_id.value == 0; }

    constexpr inline ValueType   getValue() const { return c_concrete_sub_object_id_instance_as_int( *this ); }
    constexpr inline SubObjectID getSubObjectID() const { return SubObjectID{ sub_object_id }; }
    constexpr inline Instance    getInstance() const { return Instance{ instance }; }

    constexpr inline bool valid() const { return getValue() != 0; }

    constexpr inline bool operator==( const SubObjectIDInstance& cmp ) const { return getValue() == cmp.getValue(); }
    constexpr inline bool operator!=( const SubObjectIDInstance& cmp ) const { return !this->operator==( cmp ); }
    constexpr inline bool operator<( const SubObjectIDInstance& cmp ) const { return getValue() < cmp.getValue(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "concrete_sub_object_id", sub_object_id.value );
            archive& boost::serialization::make_nvp( "concrete_instance", instance.value );
        }
        else
        {
            archive& sub_object_id.value;
            archive& instance.value;
        }
    }
};

static_assert( sizeof( SubObjectIDInstance ) == sizeof( U32 ), "Invalid Concrete ObjectID Size" );

inline constexpr SubObjectIDInstance operator""_CSI( unsigned long long int value )
{
    return SubObjectIDInstance( static_cast< SubObjectIDInstance::ValueType >( value ) );
}

inline std::ostream& operator<<( std::ostream& os, const SubObjectIDInstance& value )
{
    return os << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << value.getValue() << std::dec << "_CSI";
}

inline std::istream& operator>>( std::istream& is, SubObjectIDInstance& typeID )
{
    SubObjectIDInstance::ValueType value;
    is >> std::hex >> value >> std::dec;
    typeID = SubObjectIDInstance{ value };
    return is;
}

} // namespace mega::concrete

#endif // GUARD_2023_December_12_concrete_sub_object_id_instance
