
#include "database/types/operation.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace mega
{

static const OperationIDStringArray g_pszOperationStrings = {
    std::string( "__eg_ImpNoParams" ),
    std::string( "__eg_ImpParams" ),
    std::string( "Start" ),
    std::string( "Stop" ),
    std::string( "Pause" ),
    std::string( "Resume" ),
    std::string( "Wait" ),
    std::string( "Get" ),
    std::string( "Done" ),
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
    std::string( "Pause" ),
    std::string( "Resume" ),    
    std::string( "WaitAction" ),   
    std::string( "WaitDimension" ),
    std::string( "GetAction" ), 
    std::string( "GetDimension" ), 
    std::string( "Done" ),
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
