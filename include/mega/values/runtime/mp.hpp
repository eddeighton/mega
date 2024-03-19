
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

#ifndef GUARD_2024_January_11_mp
#define GUARD_2024_January_11_mp

#include "mega/values/runtime/inline.h"

#include "mega/values/native_types.hpp"
#include "mega/values/runtime/machine_id.hpp"
#include "mega/values/runtime/process_id.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>
#include <iomanip>

namespace mega::runtime
{

class MP : public c_machine_process_id
{
public:
    using ValueType = U64;

    struct Hash
    {
        inline U64 operator()( const MP& ref ) const noexcept { return ref.getValue(); }
    };

    constexpr inline MP()
        : c_machine_process_id{ 0, 0, 0 }
    {
    }

    constexpr inline explicit MP( c_machine_id machineID, c_process_id processID )
        : c_machine_process_id( c_machine_process_id_make( machineID.value, processID.value ) )
    {
    }

    constexpr inline explicit MP( MachineID machineID, ProcessID processID )
        : c_machine_process_id( c_machine_process_id_make( machineID.getValue(), processID.getValue() ) )
    {
    }

    constexpr inline explicit MP( ValueType value )
        : c_machine_process_id( c_machine_process_id_from_int( value ) )
    {
    }

    constexpr inline MP( const MP& cpy ) = default;

    constexpr inline ValueType getValue() const { return c_machine_process_id_as_int( *this ); }
    constexpr inline MachineID getMachineID() const { return MachineID{ m_machine_id }; }
    constexpr inline ProcessID getProcessID() const { return ProcessID{ m_process_id }; }

    constexpr inline bool valid() const { return getValue() != 0; }

    constexpr inline bool operator==( const MP& cmp ) const { return getValue() == cmp.getValue(); }
    constexpr inline bool operator!=( const MP& cmp ) const { return !this->operator==( cmp ); }
    constexpr inline bool operator<( const MP& cmp ) const { return getValue() < cmp.getValue(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "machine_id", m_machine_id.value );
            archive& boost::serialization::make_nvp( "process_id", m_process_id.value );
        }
        else
        {
            archive& m_machine_id.value;
            archive& m_process_id.value;
        }
    }
};

static_assert( sizeof( MP ) == sizeof( MP::ValueType ), "Invalid Concrete ObjectID Size" );

inline std::ostream& operator<<( std::ostream& os, const MP& value )
{
    return os << value.getMachineID() << '.' << value.getProcessID();
}

inline std::istream& operator>>( std::istream& is, MP& typeID )
{
    MachineID machineID;
    ProcessID processID;
    char      c;
    is >> machineID >> c >> processID;
    typeID = MP{ machineID, processID };
    return is;
}

} // namespace mega::runtime

#endif // GUARD_2024_January_11_mp
