
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

#ifndef GUARD_2024_January_12_allocation_id
#define GUARD_2024_January_12_allocation_id

#include "mega/values/runtime/inline.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <ostream>
#include <istream>

namespace mega::runtime
{

class AllocationID : public c_allocation_id
{
public:
    using ValueType = U16;

    struct Hash
    {
        inline U64 operator()( const c_allocation_id& value ) const noexcept
        {
            return value.value;
        }
    };

    constexpr inline AllocationID()
        : c_allocation_id{ 0U }
    {
    }

    constexpr inline explicit AllocationID( c_allocation_id _value )
        : c_allocation_id( _value )
    {
    }

    constexpr inline explicit AllocationID( ValueType _value )
        : c_allocation_id{ _value }
    {
    }

    constexpr inline AllocationID( const AllocationID& cpy )            = default;
    constexpr inline AllocationID( AllocationID&& cpy )                 = default;
    constexpr inline AllocationID& operator=( const AllocationID& cpy ) = default;

    constexpr inline ValueType getValue() const { return value; }

    constexpr inline bool operator<( const AllocationID& cpy ) const { return value < cpy.value; }
    constexpr inline bool operator==( const AllocationID& cpy ) const { return value == cpy.value; }
    constexpr inline bool operator!=( const AllocationID& cpy ) const { return !this->operator==( cpy ); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "allocation_id", value );
        }
        else
        {
            archive& value;
        }
    }
};

static_assert( sizeof( AllocationID ) == sizeof( AllocationID::ValueType ), "Invalid AllocationID Size" );

inline constexpr AllocationID operator""_A( unsigned long long int value )
{
    return AllocationID{ static_cast< AllocationID::ValueType >( value ) };
}

static constexpr AllocationID ALLOCATION_ZERO = 0x0000_A;

inline std::ostream& operator<<( std::ostream& os, const AllocationID& instance )
{
    return os << "0x" << std::hex << std::setw( 4 ) << std::setfill( '0' ) << instance.getValue() << "_A";
}

inline std::istream& operator>>( std::istream& is, AllocationID& instance )
{
    AllocationID::ValueType value;
    is >> value;
    instance = AllocationID{ value };
    return is;
}

} // namespace mega::runtime

#endif //GUARD_2024_January_12_allocation_id
