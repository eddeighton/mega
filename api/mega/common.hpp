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
#include <limits>
#include <string>

namespace mega
{

using Instance       = std::uint16_t;
using SymbolID       = std::int16_t;
using TypeID         = std::int16_t;
using TimeStamp      = std::uint32_t;
using AddressStorage = std::uint32_t;
using ExecutionIndex = std::uint16_t;
using ObjectIndex    = std::uint32_t;
using event_iterator = std::uint64_t;

static constexpr ExecutionIndex MAX_SIMULATIONS = 512;
static constexpr ObjectIndex    MAX_OBJECTS     = 4194304;
static constexpr std::uint32_t  INVALID_ADDRESS = std::numeric_limits< std::uint32_t >::max();

struct TypeInstance
{
    Instance instance = 0U;
    TypeID   typeID   = 0;

    inline bool operator==( const TypeInstance& cmp ) const
    {
        return ( instance == cmp.instance ) && ( typeID == cmp.typeID );
    }
    inline bool operator!=( const TypeInstance& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const TypeInstance& cmp ) const
    {
        return ( instance != cmp.instance ) ? ( instance < cmp.instance )
               : ( typeID != cmp.typeID )   ? ( typeID < cmp.typeID )
                                            : false;
    }
};
static_assert( sizeof( TypeInstance ) == 4U, "Invalid TypeInstance Size" );

enum AddressType
{
    LOGICAL_ADDRESS  = 0,
    PHYSICAL_ADDRESS = 1
};

struct PhysicalAddress
{
    // 2 ^ 9    == 512
    // 2 ^ 22   == 4194304
    AddressStorage execution : 9, object : 22, type : 1;
};
static_assert( sizeof( PhysicalAddress ) == 4U, "Invalid PhysicalAddress Size");

struct LogicalAddress
{
    // 2 ^ 31   == 2147483648
    AddressStorage address : 31, type : 1;
};
static_assert( sizeof( LogicalAddress ) == 4U, "Invalid LogicalAddress Size" );

struct Address
{
    union
    {
        AddressStorage  value;
        LogicalAddress  logical;
        PhysicalAddress physical;
    };
    Address()
        : value( INVALID_ADDRESS )
    {
    }
    Address( const AddressStorage& value )
        : value( value )
    {
    }
    Address( const LogicalAddress& logicalAddress )
        : logical( logicalAddress )
    {
    }
    Address( const PhysicalAddress& physicalAddress )
        : physical( physicalAddress )
    {
    }

    inline bool        valid() const { return value != INVALID_ADDRESS; }
    inline AddressType getType() const { return static_cast< AddressType >( logical.type ); }
    inline             operator AddressStorage() const { return value; }
    inline bool        operator==( const Address& cmp ) const { return ( value == cmp.value ); }
    inline bool        operator!=( const Address& cmp ) const { return !( *this == cmp ); }
    inline bool        operator<( const Address& cmp ) const { return value < cmp.value; }
};
static_assert( sizeof( Address ) == 4U, "Invalid Address Size" );

struct reference : TypeInstance, Address
{
    inline bool operator==( const reference& cmp ) const
    {
        return ( !valid() && !cmp.valid() ) || ( TypeInstance::operator==( cmp ) && Address::operator==( cmp ) );
    }
    inline bool operator!=( const reference& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const reference& cmp ) const
    {
        return TypeInstance::operator!=( cmp ) ? TypeInstance::operator<( cmp )
               : Address::operator!=( cmp )    ? Address::operator<( cmp )
                                               : false;
    }
};
static_assert( sizeof( reference ) == 8U, "Invalid reference Size" );

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
    id_Imp_NoParams = std::numeric_limits< TypeID >::min(),
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

static const TypeID TOTAL_OPERATION_TYPES = HIGHEST_OPERATION_TYPE - std::numeric_limits< TypeID >::min();

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