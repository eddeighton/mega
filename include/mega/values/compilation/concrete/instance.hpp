
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

#ifndef GUARD_2023_December_07_instance
#define GUARD_2023_December_07_instance

#include "mega/values/compilation/concrete/inline.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <ostream>
#include <istream>

namespace mega::concrete
{

class Instance : public c_concrete_instance
{
public:
    using ValueType = U16;

    struct Hash
    {
        inline U64 operator()( const Instance& ref ) const noexcept { return ref.getValue(); }
    };

    constexpr inline Instance()
        : c_concrete_instance{ 0U }
    {
    }

    constexpr inline explicit Instance( c_concrete_instance value )
        : c_concrete_instance( value )
    {
    }

    constexpr inline explicit Instance( ValueType value )
        : c_concrete_instance{ value }
    {
    }

    constexpr inline Instance( const Instance& cpy )            = default;
    constexpr inline Instance( Instance&& cpy )                 = default;
    constexpr inline Instance& operator=( const Instance& cpy ) = default;

    constexpr inline ValueType getValue() const { return value; }

    constexpr inline bool operator<( const Instance& cpy ) const { return value < cpy.value; }
    constexpr inline bool operator==( const Instance& cpy ) const { return value == cpy.value; }
    constexpr inline bool operator!=( const Instance& cpy ) const { return !this->operator==( cpy ); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "instance", value );
        }
        else
        {
            archive& value;
        }
    }
};

static_assert( sizeof( Instance ) == sizeof( Instance::ValueType ), "Invalid Instance Size" );

inline constexpr Instance operator""_CI( unsigned long long int value )
{
    return Instance{ static_cast< Instance::ValueType >( value ) };
}

static constexpr Instance INSTANCE_ZERO = 0x0000_CI;

inline std::ostream& operator<<( std::ostream& os, const Instance& instance )
{
    return os << "0x" << std::hex << std::setw( 4 ) << std::setfill( '0' ) << instance.getValue() << "_CI";
}

inline std::istream& operator>>( std::istream& is, Instance& instance )
{
    Instance::ValueType value;
    is >> value;
    instance = Instance{ value };
    return is;
}

} // namespace mega::concrete

#endif // GUARD_2023_December_07_instance
