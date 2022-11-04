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

#ifndef MEGA_REFERENCE_18_SEPT_2022
#define MEGA_REFERENCE_18_SEPT_2022

#include "mega/native_types.hpp"

namespace mega
{

using Instance = U16;
using SymbolID = I16;
using TypeID   = I16;

static constexpr const char* ROOT_TYPE_NAME = "Root";
static constexpr TypeID      ROOT_TYPE_ID   = 1;

struct TypeInstance
{
    Instance instance = 0U;
    TypeID   type     = 0;

    TypeInstance() = default;
    constexpr TypeInstance( Instance instance, TypeID type )
        : instance( instance )
        , type( type )
    {
    }

    static constexpr TypeInstance Object( TypeID type ) { return { 0, type }; }
    static constexpr TypeInstance Root() { return Object( ROOT_TYPE_ID ); }

    constexpr inline bool operator==( const TypeInstance& cmp ) const
    {
        return ( instance == cmp.instance ) && ( type == cmp.type );
    }
    constexpr inline bool operator!=( const TypeInstance& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const TypeInstance& cmp ) const
    {
        return ( instance != cmp.instance ) ? ( instance < cmp.instance )
               : ( type != cmp.type )       ? ( type < cmp.type )
                                            : false;
    }

    constexpr inline bool is_valid() const { return type != 0; }
};
static_assert( sizeof( TypeInstance ) == 4U, "Invalid TypeInstance Size" );

using AddressStorage                         = U64; // recheck numeric_limits if change
static constexpr AddressStorage NULL_ADDRESS = 0x0;

using ProcessAddress = I64;
using NetworkAddress = U64;

inline ProcessAddress toProcessAddress( void* pMemoryBase, void* pMemory )
{
    return reinterpret_cast< char* >( pMemory ) - reinterpret_cast< char* >( pMemoryBase );
}
inline void* fromProcessAddress( void* pMemoryBase, ProcessAddress processAddress )
{
    return reinterpret_cast< char* >( pMemoryBase ) + processAddress;
}

struct NetworkOrProcessAddress
{
    union
    {
        AddressStorage nop_storage;
        ProcessAddress pointer;
        NetworkAddress network;
    };
    constexpr NetworkOrProcessAddress()
        : nop_storage( NULL_ADDRESS )
    {
    }
    constexpr NetworkOrProcessAddress( NetworkAddress networkAddress )
        : network( networkAddress )
    {
    }
    constexpr NetworkOrProcessAddress( ProcessAddress processAddress )
        : pointer( processAddress )
    {
    }

    constexpr inline bool is_null() const { return nop_storage == NULL_ADDRESS; }
    constexpr inline      operator AddressStorage() const { return nop_storage; }
    constexpr inline bool operator==( const NetworkOrProcessAddress& cmp ) const
    {
        return ( nop_storage == cmp.nop_storage );
    }
    constexpr inline bool operator!=( const NetworkOrProcessAddress& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const NetworkOrProcessAddress& cmp ) const
    {
        return nop_storage < cmp.nop_storage;
    }
};
static_assert( sizeof( NetworkOrProcessAddress ) == 8U, "Invalid NetworkAddress Size" );

using ProcessID = U8;
using OwnerID   = U16;

//#define MAX_MACHINES_BITS 20
#define MAX_MACHINES_BITS 21
#define MAX_PROCESS_PER_MACHINE_BITS 3
#define MAX_OWNER_PER_PROCESS_BITS 7
#define MAX_MPO_REMAINING_BITS 1
static_assert( MAX_MACHINES_BITS + MAX_PROCESS_PER_MACHINE_BITS + MAX_OWNER_PER_PROCESS_BITS + MAX_MPO_REMAINING_BITS
               == 32 );

// NOTE: MAX_PROCESS_PER_MACHINE used by the shared memory header to set size of heap memory pointer array - ( so keep small )
static constexpr U64 MAX_MACHINES            = 2 << ( MAX_MACHINES_BITS - 1 );            // 2097152
static constexpr U64 MAX_PROCESS_PER_MACHINE = 2 << ( MAX_PROCESS_PER_MACHINE_BITS - 1 ); // 8
static constexpr U64 MAX_OWNER_PER_PROCESS   = 2 << ( MAX_OWNER_PER_PROCESS_BITS - 1 );   // 128

static constexpr U64 TOTAL_MACHINES  = MAX_MACHINES;                                                   // 2,097,152
static constexpr U64 TOTAL_PROCESSES = MAX_MACHINES * MAX_PROCESS_PER_MACHINE;                         // 16,777,216
static constexpr U64 TOTAL_OWNERS    = MAX_MACHINES * MAX_PROCESS_PER_MACHINE * MAX_OWNER_PER_PROCESS; // 2,147,483,648

class MachineID
{
public:
    MachineID() = default;
    
    constexpr MachineID( U32 value )
        : m_storage{ value }
    {
    }

    constexpr MachineID( const MachineID& cpy ) = default;
    constexpr MachineID& operator=( const MachineID& cpy ) = default;

    struct Hash
    {
        inline U64 operator()( const MachineID& machineID ) const noexcept { return machineID.m_storage; }
    };

    constexpr operator U32() const { return m_storage; }

    constexpr inline bool operator==( const MachineID& cmp ) const { return m_storage == cmp.m_storage; }
    constexpr inline bool operator!=( const MachineID& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const MachineID& cmp ) const { return m_storage < cmp.m_storage; }

private:
    U32 m_storage;
};

class MP
{
    using MPStorageType = U32;

    // clang-format off
    struct MachineProcess
    {
        U32 process : MAX_PROCESS_PER_MACHINE_BITS, 
            machine : MAX_MACHINES_BITS,
            reserved: 8; // NOTE: generally ensure ALL bits get initialised or mp_storage comparison can fail
    };
    // clang-format on
    static_assert( sizeof( MachineProcess ) == 4U, "Invalid MachineProcess Size" );
    static_assert( sizeof( MachineProcess ) == sizeof( MPStorageType ), "Invalid MachineProcess Size" );

public:
    union
    {
        MachineProcess mp;
        MPStorageType  mp_storage;
    };

    struct Hash
    {
        inline U64 operator()( const MP& mp ) const noexcept { return mp.mp_storage; }
    };

    MP() = default;

    constexpr MP( MachineID machineID, ProcessID processID )
        : mp{ processID, machineID, 0U }
    {
    }

    template < typename T >
    constexpr MP( const T& mpo )
        : mp{ mpo.getProcessID(), mpo.getMachineID(), 0U }
    {
    }

    constexpr inline MachineID getMachineID() const { return mp.machine; }
    constexpr inline ProcessID getProcessID() const { return mp.process; }

    constexpr inline bool operator==( const MP& cmp ) const { return mp_storage == cmp.mp_storage; }
    constexpr inline bool operator!=( const MP& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const MP& cmp ) const { return mp_storage < cmp.mp_storage; }

    constexpr void setMachineID( MachineID machineID ) { mp.machine = machineID; }
    constexpr void setProcessID( ProcessID processID ) { mp.process = processID; }
};
static_assert( sizeof( MP ) == 4U, "Invalid MP Size" );

class MPO
{
    using MPOStorageType = U32;

    enum AddressType : MPOStorageType
    {
        NETWORK_ADDRESS = 0,
        MACHINE_ADDRESS = 1
    };

    // clang-format off
    struct MachineProcessOwner
    {
        MPOStorageType  owner           : MAX_OWNER_PER_PROCESS_BITS,
                        process         : MAX_PROCESS_PER_MACHINE_BITS,
                        machine         : MAX_MACHINES_BITS,
                        address_type    : MAX_MPO_REMAINING_BITS;
    };
    // clang-format on
    static_assert( sizeof( MachineProcessOwner ) == 4U, "Invalid MachineProcessOwner Size" );
    static_assert( sizeof( MachineProcessOwner ) == sizeof( MPOStorageType ), "Invalid MachineProcessOwner Size" );

public:
    union
    {
        MachineProcessOwner mpo;
        MPOStorageType      mpo_storage;
    };

    struct Hash
    {
        inline U64 operator()( const MPO& mpo ) const noexcept { return mpo.mpo_storage; }
    };

    MPO() = default;
    constexpr MPO( MachineID machineID, ProcessID processID, OwnerID ownerID )
        : mpo{ ownerID, processID, machineID, MACHINE_ADDRESS }
    {
    }
    constexpr MPO( MP mp, OwnerID ownerID )
        : mpo{ ownerID, mp.getProcessID(), mp.getMachineID(), MACHINE_ADDRESS }
    {
    }

    constexpr inline MachineID getMachineID() const { return mpo.machine; }
    constexpr inline ProcessID getProcessID() const { return mpo.process; }
    constexpr inline OwnerID   getOwnerID() const { return mpo.owner; }
    constexpr inline bool      isNetwork() const { return mpo.address_type == NETWORK_ADDRESS; }
    constexpr inline bool      isMachine() const { return mpo.address_type == MACHINE_ADDRESS; }

    constexpr inline bool operator==( const MPO& cmp ) const { return mpo_storage == cmp.mpo_storage; }
    constexpr inline bool operator!=( const MPO& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const MPO& cmp ) const { return mpo_storage < cmp.mpo_storage; }

    constexpr void setMachineID( MachineID machineID ) { mpo.machine = machineID; }
    constexpr void setProcessID( ProcessID processID ) { mpo.process = processID; }
    constexpr void setExecutorID( OwnerID ownerID ) { mpo.owner = ownerID; }
    constexpr void setIsNetwork() { mpo.address_type = NETWORK_ADDRESS; }
    constexpr void setIsMachine() { mpo.address_type = MACHINE_ADDRESS; }
};
static_assert( sizeof( MPO ) == 4U, "Invalid MPO Size" );

struct reference : TypeInstance, MPO, NetworkOrProcessAddress
{
    inline reference() = default;

    constexpr reference( TypeInstance typeInstance, MPO mpo, ProcessAddress process )
        : TypeInstance( typeInstance )
        , MPO( mpo )
        , NetworkOrProcessAddress( process )
    {
        setIsMachine();
    }
    constexpr reference( TypeInstance typeInstance, MPO mpo, NetworkAddress networkAddress )
        : TypeInstance( typeInstance )
        , MPO( mpo )
        , NetworkOrProcessAddress( networkAddress )
    {
        setIsNetwork();
    }

    constexpr inline bool operator==( const reference& cmp ) const
    {
        if ( isMachine() && cmp.isMachine() )
        {
            return TypeInstance::operator==( cmp ) && MPO::operator==( cmp );
        }
        else if ( isNetwork() && cmp.isNetwork() )
        {
            return TypeInstance::operator==( cmp ) && NetworkOrProcessAddress::operator==( cmp );
        }
        else
        {
            return false;
        }
    }
    constexpr inline bool operator!=( const reference& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const reference& cmp ) const
    {
        if ( isMachine() && cmp.isMachine() )
        {
            return MPO::operator!=( cmp ) ? MPO::operator<( cmp ) : TypeInstance::operator<( cmp );
        }
        else if ( isNetwork() && cmp.isNetwork() )
        {
            return NetworkOrProcessAddress::operator!=( cmp ) ? NetworkOrProcessAddress::operator<( cmp )
                                                              : TypeInstance::           operator<( cmp );
        }
        else
        {
            return isMachine();
        }
    }

    constexpr inline bool is_valid() const
    {
        if ( isNetwork() )
            return !NetworkOrProcessAddress::is_null();
        else
            return TypeInstance::is_valid();
    }
};
static_assert( sizeof( reference ) == 16U, "Invalid reference Size" );

} // namespace mega

#endif // MEGA_REFERENCE_18_SEPT_2022
