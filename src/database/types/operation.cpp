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


#include "database/types/operation.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>
#include <limits>

namespace mega
{

static const OperationIDStringArray g_pszOperationStrings = {
    std::string( "__eg_ImpNoParams" ),
    std::string( "__eg_ImpParams" ),
    std::string( "Start" ),
    std::string( "Stop" ),
    std::string( "Save" ),
    std::string( "Load" ),
    std::string( "Files" ),
    std::string( "Get" ),
    std::string( "Range" ),
    std::string( "Raw" ),
};

const std::string& getOperationString( OperationID op )
{
    return g_pszOperationStrings[ op - std::numeric_limits< TypeID >::min() ];
}

OperationID getOperationName( const std::string& strName )
{
    OperationIDStringArray::const_iterator iFind
        = std::find( g_pszOperationStrings.begin(), g_pszOperationStrings.end(), strName );
    if ( iFind == g_pszOperationStrings.end() )
        return HIGHEST_OPERATION_TYPE;
    else
        return static_cast< OperationID >( std::numeric_limits< TypeID >::min()
                                           + std::distance( g_pszOperationStrings.begin(), iFind ) );
}

const OperationIDStringArray& getOperationStrings() { return g_pszOperationStrings; }

// clang-format off
static const ExplicitOperationIDStringArray g_pszExplicitOperationStrings = 
{ 
    std::string( "Read" ),      
    std::string( "Write" ),
    std::string( "Allocate" ),
    std::string( "Call" ),
    std::string( "Start" ),     
    std::string( "Stop" ),         
    std::string( "Save" ),
    std::string( "Load" ),    
    std::string( "Files" ),   
    std::string( "GetAction" ), 
    std::string( "GetDimension" ), 
    std::string( "Range" ),     
    std::string( "Raw" ) 
};
// clang-format on

const std::string& getExplicitOperationString( ExplicitOperationID op )
{
    ASSERT( ( static_cast< int >( op ) >= 0 )
            && ( static_cast< int >( op ) < static_cast< int >( HIGHEST_EXPLICIT_OPERATION_TYPE ) ) );
    return g_pszExplicitOperationStrings[ op ];
}

ExplicitOperationID getExplicitOperationName( const std::string& strName )
{
    ExplicitOperationIDStringArray::const_iterator iFind
        = std::find( g_pszExplicitOperationStrings.begin(), g_pszExplicitOperationStrings.end(), strName );
    if ( iFind == g_pszExplicitOperationStrings.end() )
        return HIGHEST_EXPLICIT_OPERATION_TYPE;
    else
        return static_cast< ExplicitOperationID >( std::numeric_limits< TypeID >::min()
                                                   + std::distance( g_pszExplicitOperationStrings.begin(), iFind ) );
}

const ExplicitOperationIDStringArray& getExplicitOperationStrings() { return g_pszExplicitOperationStrings; }

} // namespace mega
