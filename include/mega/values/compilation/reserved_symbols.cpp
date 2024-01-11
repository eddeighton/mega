
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

#include "mega/values/compilation/reserved_symbols.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

namespace mega::interface
{

namespace
{
using namespace std::string_literals;

static const ReservedSymbolNameArray g_strReservedSymbols = {

    ""s,
    std::string( EG_ROOT ),
    std::string( EG_OWNER ),
    std::string( EG_STATE ),

    "__eg_ImpNoParams"s,
    "__eg_ImpParams"s,

    "MOVE"s,
    "GET"s,
    "RANGE"s,
    "REMOVE"s,
    "CLEAR"s

};

static_assert( TOTAL_RESERVED_SYMBOLS == g_strReservedSymbols.size(), "Incorrect operation strings" );

} // namespace

bool isOperationType( SymbolID symbolID )
{
    switch( symbolID.getValue() )
    {
        case e_NULL_SYMBOL_ID:
        case e_ROOT_SYMBOL_ID:
        case e_OWNER_SYMBOL_ID:
        case e_STATE_SYMBOL_ID:
        case TOTAL_RESERVED_SYMBOLS:
        default:
            return false;
        case e_OPERATION_IMP_NOPARAMS:
        case e_OPERATION_IMP_PARAMS:
        case e_OPERATION_MOVE:
        case e_OPERATION_GET:
        case e_OPERATION_RANGE:
        case e_OPERATION_REMOVE:
        case e_OPERATION_CLEAR:
            return true;
    }
}
/*
SymbolID getReservedSymbolID( const std::string& strName )
{
    auto iFind = std::find( g_strReservedSymbols.begin(), g_strReservedSymbols.end(), strName );
    VERIFY_RTE_MSG( iFind != g_strReservedSymbols.end(), "Faield to find reserved symbol: " << strName );
    return SymbolID{ static_cast< SymbolID::ValueType >( std::distance( g_strReservedSymbols.begin(), iFind ) ) };
}
*/
bool getReservedSymbolIDMaybe( const std::string& strName, SymbolID& symbolID )
{
    auto iFind = std::find( g_strReservedSymbols.begin(), g_strReservedSymbols.end(), strName );
    if( iFind != g_strReservedSymbols.end() )
    {
        symbolID
            = SymbolID{ static_cast< SymbolID::ValueType >( std::distance( g_strReservedSymbols.begin(), iFind ) ) };
        return true;
    }
    else
    {
        return false;
    }
}
const std::string& getReservedSymbol( SymbolID symbolID )
{
    ASSERT( symbolID.getValue() < TOTAL_RESERVED_SYMBOLS );
    return g_strReservedSymbols[ symbolID.getValue() ];
}

const ReservedSymbolNameArray& getReservedSymbols()
{
    return g_strReservedSymbols;
}

} // namespace mega::interface
