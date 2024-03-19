
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

#ifndef GUARD_2024_January_11_process_id
#define GUARD_2024_January_11_process_id

#include "mega/values/runtime/inline.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <ostream>
#include <istream>

namespace mega::runtime
{

class ProcessID : public c_process_id
{
public:
    using ValueType = U16;

    constexpr inline ProcessID()
        : c_process_id{ 0U }
    {
    }

    constexpr inline explicit ProcessID( c_process_id value )
        : c_process_id( value )
    {
    }

    constexpr inline explicit ProcessID( ValueType value )
        : c_process_id{ value }
    {
    }

    constexpr inline ProcessID( const ProcessID& cpy )            = default;
    constexpr inline ProcessID( ProcessID&& cpy )                 = default;
    constexpr inline ProcessID& operator=( const ProcessID& cpy ) = default;

    constexpr inline ValueType getValue() const { return value; }

    constexpr inline bool operator<( const ProcessID& cpy ) const { return value < cpy.value; }
    constexpr inline bool operator==( const ProcessID& cpy ) const { return value == cpy.value; }
    constexpr inline bool operator!=( const ProcessID& cpy ) const { return !this->operator==( cpy ); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "process_id", value );
        }
        else
        {
            archive& value;
        }
    }
};

static_assert( sizeof( ProcessID ) == sizeof( ProcessID::ValueType ), "Invalid ProcessID Size" );

inline constexpr ProcessID operator""_P( unsigned long long int value )
{
    return ProcessID{ static_cast< ProcessID::ValueType >( value ) };
}

static constexpr ProcessID PROCESS_ZERO = 0x0000_P;

inline std::ostream& operator<<( std::ostream& os, const ProcessID& instance )
{
    return os << "0x" << std::hex << std::setw( 4 ) << std::setfill( '0' ) << instance.getValue() << std::dec << "_P";
}

inline std::istream& operator>>( std::istream& is, ProcessID& instance )
{
    ProcessID::ValueType value;
    is >> value;
    instance = ProcessID{ value };
    return is;
}

} // namespace mega::runtime

#endif // GUARD_2024_January_11_process_id
