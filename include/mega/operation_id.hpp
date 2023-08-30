
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

#ifndef GUARD_2023_February_08_operation_id
#define GUARD_2023_February_08_operation_id

#include "type_id.hpp"

namespace mega
{

enum OperationID : TypeID::ValueType
{
    id_Imp_NoParams = TypeID::LOWEST_SYMBOL_ID, // std::numeric_limits< TypeID >::min(),
    id_Imp_Params,
    id_Start,
    id_Stop,
    id_Move,
    id_Get,
    id_Range,
    id_Raw,
    HIGHEST_OPERATION_TYPE
};

static const TypeID::ValueType TOTAL_OPERATION_TYPES
    = HIGHEST_OPERATION_TYPE - TypeID::LOWEST_SYMBOL_ID; // std::numeric_limits< TypeID >::min();

inline constexpr bool isOperationImplicit( OperationID operationType )
{
    return ( operationType == id_Imp_NoParams ) || ( operationType == id_Imp_Params );
}

inline constexpr bool isOperationArgs( OperationID operationType )
{
    switch( operationType )
    {
        case id_Imp_NoParams:
            return false;
        case id_Imp_Params:
            return true;
        case id_Start:
            return false;
        case id_Stop:
            return false;
        case id_Move:
            return true;
        case id_Get:
            return false;
        case id_Range:
            return false;
        case id_Raw:
            return false;
        case HIGHEST_OPERATION_TYPE:
            return false;
        default:
            return false;
    }
}

inline constexpr bool isOperationType( TypeID id )
{
    return id.isSymbolID() && ( id.getSymbolID() < HIGHEST_OPERATION_TYPE );
}

inline constexpr bool isOperationEnumeration( OperationID id )
{
    switch( id )
    {
        case id_Range:
        case id_Raw:
            return true;
        default:
            return false;
    }
}

enum InvocableID : TypeID::ValueType
{
    id_Variant = HIGHEST_OPERATION_TYPE,
    id_TypePath
};

enum ExplicitOperationID : TypeID::ValueType
{
    id_exp_Read,
    id_exp_Write,
    id_exp_Read_Link,
    id_exp_Write_Link,
    id_exp_Allocate,
    id_exp_Call,
    id_exp_Start,
    id_exp_Stop,
    id_exp_Move,
    id_exp_GetAction,
    id_exp_GetDimension,
    id_exp_Done,
    id_exp_Range,
    id_exp_Raw,
    HIGHEST_EXPLICIT_OPERATION_TYPE
};

} // namespace mega

#endif // GUARD_2023_February_08_operation_id
