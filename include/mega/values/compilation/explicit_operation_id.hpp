
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

#include "mega/values/compilation/interface/symbol_id.hpp"

#include <array>
#include <string>

namespace mega
{
enum ExplicitOperationID : interface::SymbolID::ValueType
{
    id_exp_Read,
    id_exp_Write,

    id_exp_Link_Read,
    id_exp_Link_Add,
    id_exp_Link_Remove,
    id_exp_Link_Clear,

    id_exp_Call,
    id_exp_Signal,
    id_exp_Start,
    id_exp_Move,
    id_exp_GetContext,
    id_exp_Range,
    HIGHEST_EXPLICIT_OPERATION_TYPE
};

using ExplicitOperationIDStringArray = std::array< std::string, HIGHEST_EXPLICIT_OPERATION_TYPE >;
const std::string&                    getExplicitOperationString( ExplicitOperationID op );
ExplicitOperationID                   getExplicitOperationName( const std::string& strName );
const ExplicitOperationIDStringArray& getExplicitOperationStrings();

} // namespace mega

#endif // GUARD_2023_February_08_operation_id
