
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

#ifndef GUARD_2024_January_11_owner_id
#define GUARD_2024_January_11_owner_id

#include "mega/values/runtime/inline.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <ostream>
#include <istream>

namespace mega::runtime
{

class OwnerID : public c_owner_id
{
public:
    using ValueType = U8;

    struct Hash
    {
        inline U64 operator()( const c_owner_id& value ) const noexcept
        {
            return value.value;
        }
    };

    constexpr inline OwnerID()
        : c_owner_id{ 0U }
    {
    }

    constexpr inline explicit OwnerID( c_owner_id _value )
        : c_owner_id( _value )
    {
    }

    constexpr inline explicit OwnerID( ValueType _value )
        : c_owner_id{ _value }
    {
    }

    constexpr inline OwnerID( const OwnerID& cpy )            = default;
    constexpr inline OwnerID( OwnerID&& cpy )                 = default;
    constexpr inline OwnerID& operator=( const OwnerID& cpy ) = default;

    constexpr inline ValueType getValue() const { return value; }

    constexpr inline bool operator<( const OwnerID& cpy ) const { return value < cpy.value; }
    constexpr inline bool operator==( const OwnerID& cpy ) const { return value == cpy.value; }
    constexpr inline bool operator!=( const OwnerID& cpy ) const { return !this->operator==( cpy ); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "owner_id", value );
        }
        else
        {
            archive& value;
        }
    }
};

static_assert( sizeof( OwnerID ) == sizeof( OwnerID::ValueType ), "Invalid OwnerID Size" );

inline constexpr OwnerID operator""_O( unsigned long long int value )
{
    return OwnerID{ static_cast< OwnerID::ValueType >( value ) };
}

static constexpr OwnerID OWNER_ZERO = 0x0000_O;

inline std::ostream& operator<<( std::ostream& os, const OwnerID& instance )
{
    return os << "0x" << std::hex << std::setw( 2 ) << std::setfill( '0' ) << 
        static_cast< U32 >( instance.getValue() ) << "_O";
}

inline std::istream& operator>>( std::istream& is, OwnerID& instance )
{
    U32 value;
    is >> value;
    instance = OwnerID{ static_cast< OwnerID::ValueType >( value ) };
    return is;
}

} // namespace mega::runtime

#endif // GUARD_2024_January_11_owner_id
