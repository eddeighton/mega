
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

#ifndef GUARD_2024_January_11_machine_id
#define GUARD_2024_January_11_machine_id

#include "mega/values/runtime/inline.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <ostream>
#include <istream>

namespace mega::runtime
{

class MachineID : public c_machine_id
{
public:
    using ValueType = U32;

    struct Hash
    {
        inline U64 operator()( const c_machine_id& value ) const noexcept
        {
            return value.value;
        }
    };

    constexpr inline MachineID()
        : c_machine_id{ 0U }
    {
    }

    constexpr inline explicit MachineID( c_machine_id value )
        : c_machine_id( value )
    {
    }

    constexpr inline explicit MachineID( ValueType value )
        : c_machine_id{ value }
    {
    }

    constexpr inline MachineID( const MachineID& cpy )            = default;
    constexpr inline MachineID( MachineID&& cpy )                 = default;
    constexpr inline MachineID& operator=( const MachineID& cpy ) = default;

    constexpr inline ValueType getValue() const { return value; }

    constexpr inline bool operator<( const MachineID& cpy ) const { return value < cpy.value; }
    constexpr inline bool operator==( const MachineID& cpy ) const { return value == cpy.value; }
    constexpr inline bool operator!=( const MachineID& cpy ) const { return !this->operator==( cpy ); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "machine_id", value );
        }
        else
        {
            archive& value;
        }
    }
};

static_assert( sizeof( MachineID ) == sizeof( MachineID::ValueType ), "Invalid MachineID Size" );

inline constexpr MachineID operator""_M( unsigned long long int value )
{
    return MachineID{ static_cast< MachineID::ValueType >( value ) };
}

static constexpr MachineID MACHINE_ZERO = 0x0000_M;

inline std::ostream& operator<<( std::ostream& os, const MachineID& instance )
{
    return os << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << instance.getValue() << std::dec << "_M";
}

inline std::istream& operator>>( std::istream& is, MachineID& instance )
{
    MachineID::ValueType value;
    is >> value;
    instance = MachineID{ value };
    return is;
}

} // namespace mega::runtime

#endif // GUARD_2024_January_11_machine_id
