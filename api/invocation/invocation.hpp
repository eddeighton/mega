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
#include "database/types/invocation_id.hpp"

#include "database/model/OperationsStage.hxx"

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

template < class T >
inline std::vector< T > uniquify_without_reorder( const std::vector< T >& ids )
{
    /*
    not this...
    std::sort( ids.begin(), ids.end() );
    auto last = std::unique( ids.begin(), ids.end() );
    ids.erase( last, ids.end() );
    */

    std::vector< T > result;
    std::set< T >    uniq;
    for ( const T& value : ids )
    {
        if ( uniq.count( value ) == 0 )
        {
            result.push_back( value );
            uniq.insert( value );
        }
    }
    return result;
}

using InterfaceVariantVector       = std::vector< OperationsStage::Operations::InterfaceVariant* >;
using InterfaceVariantVectorVector = std::vector< InterfaceVariantVector >;

InterfaceVariantVectorVector
symbolVectorToInterfaceVariantVector( OperationsStage::Database&                                    database,
                                      const std::vector< OperationsStage::Symbols::SymbolTypeID* >& symbols );

std::vector< OperationsStage::Operations::ElementVector* >
toElementVector( OperationsStage::Database&          database,
                 const InterfaceVariantVectorVector& interfaceVariantVectorVector );

OperationsStage::Operations::Invocation* construct( io::Environment& environment, const mega::InvocationID& id,
                                                    OperationsStage::Database&    database,
                                                    const mega::io::megaFilePath& sourceFile );

} // namespace mega::invocation

#endif // INVOCATION_6_JUNE_2022
