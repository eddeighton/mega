
#include "database/types/operation.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace mega
{

static const OperationIDStringArray g_pszOperationStrings = {
    std::string( "__eg_ImpNoParams" ), // id_Imp_NoParams (-2147483648)
    std::string( "__eg_ImpParams" ),   // id_Imp_Params   (-2147483647)
    std::string( "Start" ),            // id_Start        (-2147483646)
    std::string( "Stop" ),             // id_Stop         (-2147483645)
    std::string( "Pause" ),            // id_Pause        (-2147483644)
    std::string( "Resume" ),           // id_Resume       (-2147483643)
    std::string( "Wait" ),             // id_Wait         (-2147483642)
    std::string( "Get" ),              // id_Get          (-2147483641)
    std::string( "Done" ),             // id_Done         (-2147483640)
    std::string( "Range" ),            // id_Range        (-2147483639)
    std::string( "Raw" ),              // id_Raw          (-2147483638)
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
