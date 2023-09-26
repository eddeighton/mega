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

#ifndef INVOCATION_6_JUNE_2022
#define INVOCATION_6_JUNE_2022

#include "database/model/environment.hxx"
#include "database/model/OperationsStage.hxx"

#include "mega/invocation_id.hpp"

#include <stdexcept>
#include <vector>
#include <set>

namespace mega::invocation
{

class Exception : public std::runtime_error
{
public:
    Exception( const std::string& strMessage )
        : std::runtime_error( strMessage )
    {
    }
};

#define THROW_INVOCATION_EXCEPTION( msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _os; _os << msg; throw Exception( _os.str() ); )

struct SymbolTables
{
    SymbolTables( OperationsStage::Symbols::SymbolTable* pSymbolTable );

    using SymbolTypeIDMap    = std::map< mega::TypeID, OperationsStage::Symbols::SymbolTypeID* >;
    using InterfaceTypeIDMap = std::map< mega::TypeID, OperationsStage::Symbols::InterfaceTypeID* >;
    SymbolTypeIDMap    symbolIDMap;
    InterfaceTypeIDMap interfaceIDMap;
};

OperationsStage::Operations::Invocation* compileInvocation( OperationsStage::Database& database,
                                                            const SymbolTables&        symbolTables,
                                                            const mega::InvocationID&  id );

} // namespace mega::invocation

#endif // INVOCATION_6_JUNE_2022
