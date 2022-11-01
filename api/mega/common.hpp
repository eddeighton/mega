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



#ifndef EG_COMMON_22_04_2019
#define EG_COMMON_22_04_2019

#include "reference.hpp"

namespace mega
{

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
    id_Save,
    id_Load,
    id_Files,
    id_Get,
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
    id_exp_Save,
    id_exp_Load,
    id_exp_Files,
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