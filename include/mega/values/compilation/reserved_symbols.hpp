
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

#ifndef GUARD_2024_January_10_reserved_symbol
#define GUARD_2024_January_10_reserved_symbol

#include "mega/values/compilation/interface/symbol_id.hpp"

#include <array>
#include <string>

namespace mega::interface
{

enum ReservedSymbol : SymbolID::ValueType
{
    e_NULL_SYMBOL_ID,
    e_ROOT_SYMBOL_ID,
    e_OWNER_SYMBOL_ID,
    e_STATE_SYMBOL_ID,

    e_OPERATION_IMP_NOPARAMS,
    e_OPERATION_IMP_PARAMS,

    e_OPERATION_MOVE,
    e_OPERATION_GET,
    e_OPERATION_RANGE,
    e_OPERATION_REMOVE,
    e_OPERATION_CLEAR,

    TOTAL_RESERVED_SYMBOLS
};

static_assert( TOTAL_RESERVED_SYMBOLS == OPERATION_CLEAR.getValue() + 1 );
static_assert( e_OPERATION_IMP_NOPARAMS == OPERATION_IMP_NOPARAMS.getValue() );
static_assert( e_OPERATION_IMP_PARAMS == OPERATION_IMP_PARAMS.getValue() );

using ReservedSymbolNameArray = std::array< std::string, TOTAL_RESERVED_SYMBOLS >;

bool                           isOperationType( SymbolID symbolID );
bool                           getReservedSymbolIDMaybe( const std::string& strName, SymbolID& symbolID );
const std::string&             getReservedSymbol( SymbolID symbolID );
const ReservedSymbolNameArray& getReservedSymbols();

} // namespace mega::interface

#endif // GUARD_2024_January_10_reserved_symbol
