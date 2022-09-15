//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef EG_COMMON_22_04_2019
#define EG_COMMON_22_04_2019

#include <cstdint>
//#include <limits>

namespace mega
{

using Instance       = std::uint16_t;
using SymbolID       = std::int16_t;
using TypeID         = std::int16_t;
using TimeStamp      = std::uint32_t;
using event_iterator = std::uint64_t;

struct TypeInstance
{
    Instance instance = 0U;
    TypeID   type     = 0;

    TypeInstance() {}
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

using AddressStorage                         = std::uint64_t; // recheck numeric_limits if change
static constexpr AddressStorage NULL_ADDRESS = 0x0;           // std::numeric_limits< AddressStorage >::max();

using ProcessAddress = void*;
using NetworkAddress = std::uint64_t;

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

using MachineID  = std::uint8_t;
using ProcessID  = std::uint8_t;
using ExecutorID = std::uint16_t;
using ObjectID   = std::uint16_t;

static constexpr MachineID  MAX_MACHINES             = 8;
static constexpr ProcessID  MAX_PROCESS_PER_MACHINE  = 16;
static constexpr ExecutorID MAX_EXECUTOR_PER_PROCESS = 256;
static constexpr auto       MAX_OBJECTS_PER_EXECUTOR = 65536;

static constexpr auto TOTAL_MACHINES  = MAX_MACHINES;                                                      // 8
static constexpr auto TOTAL_PROCESSES = MAX_MACHINES * MAX_PROCESS_PER_MACHINE;                            // 128
static constexpr auto TOTAL_EXECUTORS = MAX_MACHINES * MAX_PROCESS_PER_MACHINE * MAX_EXECUTOR_PER_PROCESS; // 32768

class MPE
{
    using MPEStorageType = std::uint16_t;

    enum AddressType : MPEStorageType
    {
        NETWORK_ADDRESS = 0,
        MACHINE_ADDRESS = 1
    };
    struct MachineProcessExecutor
    {
        MPEStorageType executor : 8, process : 4, machine : 3, address_type : 1;
    };
    static_assert( sizeof( MachineProcessExecutor ) == 2U, "Invalid MachineAddress Size" );

    union
    {
        MachineProcessExecutor mpe;
        MPEStorageType         mpe_storage;
    };

    // inline                operator MPEStorageType() const { return mpe_storage; }
    // inline MPEStorageType getStorage() const { return mpe_storage; }
public:
    struct Hash
    {
        inline std::size_t operator()( const MPE& mpe ) const noexcept { return mpe.mpe_storage; }
    };

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& mpe_storage;
    }

    MPE() {}
    MPE( MachineID machineID, ProcessID processID, ExecutorID executorID )
        : mpe{ executorID, processID, machineID, MACHINE_ADDRESS }
    {
    }
    
    inline MachineID  getMachineID() const { return mpe.machine; }
    inline ProcessID  getProcessID() const { return mpe.process; }
    inline ExecutorID getExecutorID() const { return mpe.executor; }
    inline bool       isNetwork() const { return mpe.address_type == NETWORK_ADDRESS; }
    inline bool       isMachine() const { return mpe.address_type == MACHINE_ADDRESS; }

    inline bool operator==( const MPE& cmp ) const { return mpe_storage == cmp.mpe_storage; }
    inline bool operator!=( const MPE& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const MPE& cmp ) const { return mpe_storage < cmp.mpe_storage; }

    void setMachineID( MachineID machineID ) { mpe.machine = machineID; }
    void setProcessID( ProcessID processID ) { mpe.process = processID; }
    void setExecutorID( ExecutorID executorID ) { mpe.executor = executorID; }
    void setIsNetwork() { mpe.address_type = NETWORK_ADDRESS; }
    void setIsMachine() { mpe.address_type = MACHINE_ADDRESS; }
};
static_assert( sizeof( MPE ) == 2U, "Invalid MPE Size" );

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

struct MachineAddress : ObjectAddress, MPE
{
    inline bool operator==( const MachineAddress& cmp ) const
    {
        return ObjectAddress::operator==( cmp ) && MPE::operator==( cmp );
    }
    inline bool operator!=( const MachineAddress& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const MachineAddress& cmp ) const
    {
        return ObjectAddress::operator!=( cmp ) ? ObjectAddress::operator<( cmp ) : MPE::operator<( cmp );
    }
};
static_assert( sizeof( MachineAddress ) == 4U, "Invalid MachineAddress Size" );

struct reference : TypeInstance, MachineAddress, NetworkOrProcessAddress
{
    inline reference() {}
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
            return TypeInstance::operator==( cmp ) && MPE::operator==( cmp );
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
            return MPE::operator!=( cmp ) ? MPE::operator<( cmp ) : TypeInstance::operator<( cmp );
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

enum ActionState
{
    action_stopped = 0,
    action_running,
    action_paused,
    TOTAL_ACTION_STATES
};

inline const char* getActionState( ActionState state )
{
    switch ( state )
    {
        case action_running:
            return "::mega::action_running";
        case action_paused:
            return "::mega::action_paused";
        case action_stopped:
            return "::mega::action_stopped";
        case TOTAL_ACTION_STATES:
        default:
            return "";
    }
}

enum OperationID : TypeID
{
    id_Imp_NoParams = -32768, // std::numeric_limits< TypeID >::min(),
    id_Imp_Params,
    id_Start,
    id_Stop,
    id_Pause,
    id_Resume,
    id_Wait,
    id_Get,
    id_Done,
    id_Range,
    id_Raw,
    HIGHEST_OPERATION_TYPE
};

enum ExplicitOperationID : TypeID
{
    id_exp_Read,
    id_exp_Write,
    id_exp_Allocate,
    id_exp_Call,
    id_exp_Start,
    id_exp_Stop,
    id_exp_Pause,
    id_exp_Resume,
    id_exp_WaitAction,
    id_exp_WaitDimension,
    id_exp_GetAction,
    id_exp_GetDimension,
    id_exp_Done,
    id_exp_Range,
    id_exp_Raw,
    HIGHEST_EXPLICIT_OPERATION_TYPE
};

static const TypeID TOTAL_OPERATION_TYPES = HIGHEST_OPERATION_TYPE - -32768; // std::numeric_limits< TypeID >::min();

inline bool isOperationImplicit( OperationID operationType )
{
    return ( operationType == id_Imp_NoParams ) || ( operationType == id_Imp_Params );
}

inline bool isOperationType( TypeID id ) { return id < HIGHEST_OPERATION_TYPE; }

inline bool isOperationEnumeration( OperationID id )
{
    switch ( id )
    {
        case id_Range:
        case id_Raw:
            return true;
        default:
            return false;
    }
}

enum InvocableID : TypeID
{
    id_Variant = HIGHEST_OPERATION_TYPE,
    id_TypePath
};

enum ReferenceState
{
    Null,
    Invalid,
    Stopped,
    Paused,
    Running
};

} // namespace mega

#endif // EG_COMMON_22_04_2019