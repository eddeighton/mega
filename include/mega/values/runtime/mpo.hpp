
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

#ifndef GUARD_2023_January_07_mpo
#define GUARD_2023_January_07_mpo

#include "mega/values/runtime/inline.h"

#include "mega/values/runtime/mp.hpp"

#include "mega/values/native_types.hpp"
#include "mega/values/runtime/machine_id.hpp"
#include "mega/values/runtime/process_id.hpp"
#include "mega/values/runtime/owner_id.hpp"
#include "mega/values/runtime/mp.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>
#include <iomanip>

namespace mega::runtime
{

class MPO : public c_machine_process_owner_id
{
public:
    using ValueType = U64;

    struct Hash
    {
        inline U64 operator()( const MPO& ref ) const noexcept { return ref.getValue(); }
    };

    constexpr inline MPO()
        : c_machine_process_owner_id{ 0, 0, 0, 0 }
    {
    }

    constexpr inline explicit MPO( c_machine_id machineID, c_process_id processID, c_owner_id ownerID )
        : c_machine_process_owner_id(
            c_machine_process_owner_id_make( machineID.value, processID.value, ownerID.value ) )
    {
    }

    constexpr inline explicit MPO( MachineID machineID, ProcessID processID, OwnerID ownerID )
        : c_machine_process_owner_id(
            c_machine_process_owner_id_make( machineID.getValue(), processID.getValue(), ownerID.getValue() ) )
    {
    }

    constexpr inline explicit MPO( MP mp, OwnerID ownerID )
        : c_machine_process_owner_id( c_machine_process_owner_id_make(
            mp.getMachineID().getValue(), mp.getProcessID().getValue(), ownerID.getValue() ) )
    {
    }

    constexpr inline explicit MPO( ValueType _value )
        : c_machine_process_owner_id( c_machine_process_owner_id_from_int( _value ) )
    {
    }

    constexpr inline MPO( const MPO& cpy ) = default;

    constexpr inline ValueType getValue() const { return c_machine_process_owner_id_as_int( *this ); }
    constexpr inline MachineID getMachineID() const { return MachineID{ m_machine_id }; }
    constexpr inline ProcessID getProcessID() const { return ProcessID{ m_process_id }; }
    constexpr inline OwnerID   getOwnerID() const { return OwnerID{ m_owner_id }; }

    constexpr inline MP getMP() const { return MP{ m_machine_id, m_process_id }; }

    constexpr inline bool valid() const { return getValue() != 0; }

    constexpr inline bool operator==( const MPO& cmp ) const { return getValue() == cmp.getValue(); }
    constexpr inline bool operator!=( const MPO& cmp ) const { return !this->operator==( cmp ); }
    constexpr inline bool operator<( const MPO& cmp ) const { return getValue() < cmp.getValue(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "machine_id", m_machine_id.value );
            archive& boost::serialization::make_nvp( "process_id", m_process_id.value );
            archive& boost::serialization::make_nvp( "owner_id", m_owner_id.value );
        }
        else
        {
            archive& m_machine_id.value;
            archive& m_process_id.value;
            archive& m_owner_id.value;
        }
    }
};

static_assert( sizeof( MPO ) == sizeof( MPO::ValueType ), "Invalid Concrete ObjectID Size" );

inline std::ostream& operator<<( std::ostream& os, const MPO& value )
{
    return os << value.getMachineID() << '.' << value.getProcessID() << '.' << value.getOwnerID();
}

inline std::istream& operator>>( std::istream& is, MPO& typeID )
{
    MachineID machineID;
    ProcessID processID;
    OwnerID   ownerID;
    char      c;
    is >> machineID >> c >> processID >> c >> ownerID;
    typeID = MPO{ machineID, processID, ownerID };
    return is;
}

} // namespace mega::runtime

#endif // GUARD_2023_January_07_mpo
