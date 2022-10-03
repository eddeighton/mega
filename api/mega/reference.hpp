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

using Instance       = U16;
using SymbolID       = I16;
using TypeID         = I16;

static constexpr char   ROOT_TYPE_NAME[] = "Root";
static constexpr TypeID ROOT_TYPE_ID     = 1;

struct TypeInstance
{
    Instance instance = 0U;
    TypeID   type     = 0;

    TypeInstance() = default;
    TypeInstance( Instance instance, TypeID type )
        : instance( instance )
        , type( type )
    {
    }

    inline bool operator==( const TypeInstance& cmp ) const
    {
        return ( instance == cmp.instance ) && ( type == cmp.type );
    }
    inline bool operator!=( const TypeInstance& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const TypeInstance& cmp ) const
    {
        return ( instance != cmp.instance ) ? ( instance < cmp.instance )
               : ( type != cmp.type )       ? ( type < cmp.type )
                                            : false;
    }

    inline bool is_valid() const { return type != 0; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& instance;
        archive& type;
    }
};
static_assert( sizeof( TypeInstance ) == 4U, "Invalid TypeInstance Size" );

using AddressStorage                         = U64; // recheck numeric_limits if change
static constexpr AddressStorage NULL_ADDRESS = 0x0;

using ProcessAddress = void*;
using NetworkAddress = U64;

struct NetworkOrProcessAddress
{
    union
    {
        AddressStorage nop_storage;
        ProcessAddress pointer;
        NetworkAddress network;
    };
    inline NetworkOrProcessAddress()
        : nop_storage( NULL_ADDRESS )
    {
    }
    inline NetworkOrProcessAddress( ProcessAddress processAddress )
        : pointer( processAddress )
    {
    }
    inline NetworkOrProcessAddress( NetworkAddress networkAddress )
        : network( networkAddress )
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& nop_storage;
    }

    inline bool is_null() const { return nop_storage == NULL_ADDRESS; }
    inline      operator AddressStorage() const { return nop_storage; }
    inline bool operator==( const NetworkOrProcessAddress& cmp ) const { return ( nop_storage == cmp.nop_storage ); }
    inline bool operator!=( const NetworkOrProcessAddress& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const NetworkOrProcessAddress& cmp ) const { return nop_storage < cmp.nop_storage; }
};
static_assert( sizeof( NetworkOrProcessAddress ) == 8U, "Invalid NetworkAddress Size" );

using MachineID = U8;
using ProcessID = U8;
using OwnerID   = U16;

static constexpr MachineID MAX_MACHINES            = 8;
static constexpr ProcessID MAX_PROCESS_PER_MACHINE = 16;
static constexpr OwnerID   MAX_OWNER_PER_PROCESS   = 256;

static constexpr auto TOTAL_MACHINES  = MAX_MACHINES;                                                   // 8
static constexpr auto TOTAL_PROCESSES = MAX_MACHINES * MAX_PROCESS_PER_MACHINE;                         // 128
static constexpr auto TOTAL_OWNERS    = MAX_MACHINES * MAX_PROCESS_PER_MACHINE * MAX_OWNER_PER_PROCESS; // 32768

class MP
{
    using MPStorageType = U8;

    struct MachineProcess
    {
        U8 process : 4, machine : 3, is_daemon : 1;
    };
    static_assert( sizeof( MachineProcess ) == 1U, "Invalid MachineProcess Size" );

    union
    {
        MachineProcess mp;
        MPStorageType  mp_storage;
    };

public:
    struct Hash
    {
        inline U64 operator()( const MP& mp ) const noexcept { return mp.mp_storage; }
    };

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& mp_storage;
    }

    MP() = default;
    MP( MachineID machineID, ProcessID processID, bool isDeamon )
        : mp{ processID, machineID, isDeamon }
    {
    }

    inline MachineID getMachineID() const { return mp.machine; }
    inline ProcessID getProcessID() const { return mp.process; }
    inline bool      getIsDaemon() const { return mp.is_daemon; }

    inline bool operator==( const MP& cmp ) const { return mp_storage == cmp.mp_storage; }
    inline bool operator!=( const MP& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const MP& cmp ) const { return mp_storage < cmp.mp_storage; }

    void setMachineID( MachineID machineID ) { mp.machine = machineID; }
    void setProcessID( ProcessID processID ) { mp.process = processID; }
    void setIsDaemon( bool bIsDaemon ) { mp.is_daemon = bIsDaemon; }
};
static_assert( sizeof( MP ) == 1U, "Invalid MP Size" );

class MPO
{
    using MPOStorageType = U16;

    enum AddressType : MPOStorageType
    {
        NETWORK_ADDRESS = 0,
        MACHINE_ADDRESS = 1
    };
    struct MachineProcessOwner
    {
        MPOStorageType owner : 8, process : 4, machine : 3, address_type : 1;
    };
    static_assert( sizeof( MachineProcessOwner ) == 2U, "Invalid MachineAddress Size" );

    union
    {
        MachineProcessOwner mpo;
        MPOStorageType      mpo_storage;
    };

public:
    struct Hash
    {
        inline U64 operator()( const MPO& mpo ) const noexcept { return mpo.mpo_storage; }
    };

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& mpo_storage;
    }

    MPO() {}
    MPO( MachineID machineID, ProcessID processID, OwnerID ownerID )
        : mpo{ ownerID, processID, machineID, MACHINE_ADDRESS }
    {
    }

    inline MachineID getMachineID() const { return mpo.machine; }
    inline ProcessID getProcessID() const { return mpo.process; }
    inline OwnerID   getOwnerID() const { return mpo.owner; }
    inline bool      isNetwork() const { return mpo.address_type == NETWORK_ADDRESS; }
    inline bool      isMachine() const { return mpo.address_type == MACHINE_ADDRESS; }

    inline bool operator==( const MPO& cmp ) const { return mpo_storage == cmp.mpo_storage; }
    inline bool operator!=( const MPO& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const MPO& cmp ) const { return mpo_storage < cmp.mpo_storage; }

    void setMachineID( MachineID machineID ) { mpo.machine = machineID; }
    void setProcessID( ProcessID processID ) { mpo.process = processID; }
    void setExecutorID( OwnerID ownerID ) { mpo.owner = ownerID; }
    void setIsNetwork() { mpo.address_type = NETWORK_ADDRESS; }
    void setIsMachine() { mpo.address_type = MACHINE_ADDRESS; }
};
static_assert( sizeof( MPO ) == 2U, "Invalid MPO Size" );

using ObjectID = U16;

struct ObjectAddress
{
    ObjectID    object;
    inline bool operator==( const ObjectAddress& cmp ) const { return ( object == cmp.object ); }
    inline bool operator!=( const ObjectAddress& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const ObjectAddress& cmp ) const { return object < cmp.object; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& object;
    }
};
static_assert( sizeof( ObjectAddress ) == 2U, "Invalid ObjectAddress Size" );

struct MachineAddress : ObjectAddress, MPO
{
    inline bool operator==( const MachineAddress& cmp ) const
    {
        return ObjectAddress::operator==( cmp ) && MPO::operator==( cmp );
    }
    inline bool operator!=( const MachineAddress& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const MachineAddress& cmp ) const
    {
        return ObjectAddress::operator!=( cmp ) ? ObjectAddress::operator<( cmp ) : MPO::operator<( cmp );
    }
};
static_assert( sizeof( MachineAddress ) == 4U, "Invalid MachineAddress Size" );

struct reference : TypeInstance, MachineAddress, NetworkOrProcessAddress
{
    inline reference() = default;
    inline reference( TypeInstance typeInstance, MachineAddress machineAddress )
        : TypeInstance( typeInstance )
        , MachineAddress( machineAddress )
    {
        setIsMachine();
    }
    inline reference( TypeInstance typeInstance, MachineAddress machineAddress, ProcessAddress process )
        : TypeInstance( typeInstance )
        , MachineAddress( machineAddress )
        , NetworkOrProcessAddress( process )
    {
        setIsMachine();
    }
    inline reference( TypeInstance typeInstance, NetworkAddress networkAddress )
        : TypeInstance( typeInstance )
        , NetworkOrProcessAddress( networkAddress )
    {
        setIsNetwork();
    }

    inline bool operator==( const reference& cmp ) const
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
    inline bool operator!=( const reference& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const reference& cmp ) const
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

    inline bool is_valid() const
    {
        if ( isNetwork() )
            return !NetworkOrProcessAddress::is_null();
        else
            return TypeInstance::is_valid();
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& static_cast< TypeInstance& >( *this );
        archive& static_cast< MachineAddress& >( *this );
        archive& static_cast< NetworkOrProcessAddress& >( *this );
    }
};
static_assert( sizeof( reference ) == 16U, "Invalid reference Size" );

} // namespace mega

#endif // MEGA_REFERENCE_18_SEPT_2022
