
#include "database/types/operation.hpp"

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
    OperationIDStringArray::const_iterator iFind = std::find( g_pszOperationStrings.begin(), g_pszOperationStrings.end(), strName );
    if ( iFind == g_pszOperationStrings.end() )
        return HIGHEST_OPERATION_TYPE;
    else
        return static_cast< OperationID >(
            std::numeric_limits< TypeID >::min() + std::distance( g_pszOperationStrings.begin(), iFind ) );
}

const OperationIDStringArray& getOperationStrings()
{
    return g_pszOperationStrings;
}

}
