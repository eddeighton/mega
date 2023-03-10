
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

#include "mega/native_types.hpp"
#include "mega/type_instance.hpp"

namespace mega
{

using MachineID = U32;
using ProcessID = U16;
using OwnerID   = U8;

class MPO;
class MP
{
    MachineID m_machineID; // 4
    ProcessID m_processID; // 2
    U16       m_padding;   // 2
public:
    struct Hash
    {
        inline U64 operator()( const MP& mp ) const noexcept { return mp.m_machineID + ( (U64)mp.m_processID << 4 ); }
    };

    MP() = default;

    constexpr MP( MachineID machineID, ProcessID processID )
        : m_machineID( machineID )
        , m_processID( processID )
        , m_padding( 0U )
    {
    }

    constexpr MP( const MP& mpo ) = default;
    constexpr MP( const MPO& mpo );

    constexpr inline MachineID getMachineID() const { return m_machineID; }
    constexpr inline ProcessID getProcessID() const { return m_processID; }

    constexpr inline bool operator==( const MP& cmp ) const
    {
        return m_machineID == cmp.m_machineID && m_processID == cmp.m_processID;
    }
    constexpr inline bool operator!=( const MP& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const MP& cmp ) const
    {
        return ( m_machineID != cmp.m_machineID ) ? ( m_machineID < cmp.m_machineID ) : ( m_processID < cmp.m_processID );
    }

    constexpr void setMachineID( MachineID machineID ) { m_machineID = machineID; }
    constexpr void setProcessID( ProcessID processID ) { m_processID = processID; }
};
static_assert( sizeof( MP ) == 8U, "Invalid MP Size" );

class MPO
{
    MachineID m_machineID; // 4
    ProcessID m_processID; // 2
    OwnerID   m_ownerID;   // 1
    U8        m_padding;   // 1
public:
    struct Hash
    {
        inline U64 operator()( const MPO& mpo ) const noexcept
        {
            return mpo.m_machineID + ( (U64)mpo.m_processID << 4 ) + ( (U64)mpo.m_ownerID << 6 );
        }
    };

    MPO() = default;
    constexpr MPO( MachineID machineID, ProcessID processID, OwnerID ownerID )
        : m_machineID( machineID )
        , m_processID( processID )
        , m_ownerID( ownerID )
        , m_padding( 0U )
    {
    }
    constexpr MPO( MP mp, OwnerID ownerID )
        : MPO{ mp.getMachineID(), mp.getProcessID(), ownerID }
    {
    }

    constexpr inline MachineID getMachineID() const { return m_machineID; }
    constexpr inline ProcessID getProcessID() const { return m_processID; }
    constexpr inline OwnerID   getOwnerID() const { return m_ownerID; }

    constexpr inline bool operator==( const MPO& cmp ) const
    {
        return m_machineID == cmp.m_machineID && m_processID == cmp.m_processID && m_ownerID == cmp.m_ownerID;
    }
    constexpr inline bool operator!=( const MPO& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const MPO& cmp ) const
    {
        return ( m_machineID != cmp.m_machineID )   ? ( m_machineID < cmp.m_machineID )
               : ( m_processID != cmp.m_processID ) ? ( m_processID < cmp.m_processID )
                                                : ( m_ownerID < cmp.m_ownerID );
    }

    constexpr void setMachineID( MachineID machineID ) { m_machineID = machineID; }
    constexpr void setProcessID( ProcessID processID ) { m_processID = processID; }
    constexpr void setExecutorID( OwnerID ownerID ) { m_ownerID = ownerID; }
};
static_assert( sizeof( MPO ) == 8U, "Invalid MPO Size" );

constexpr MP::MP( const MPO& mpo )
    : MP{ mpo.getMachineID(), mpo.getProcessID() }
{
}

}

#endif //GUARD_2023_January_07_mpo
