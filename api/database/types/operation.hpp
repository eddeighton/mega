#ifndef OPERATIONS_10_MAY_2022
#define OPERATIONS_10_MAY_2022

#include "mega/common.hpp"

#include <array>
#include <string>

namespace mega
{

using OperationIDStringArray = std::array< std::string, TOTAL_OPERATION_TYPES >;
OperationID                   getOperationName( const std::string& strName );
const std::string&            getOperationString( OperationID op );
const OperationIDStringArray& getOperationStrings();

using ExplicitOperationIDStringArray = std::array< std::string, HIGHEST_EXPLICIT_OPERATION_TYPE >;
const std::string&                    getExplicitOperationString( ExplicitOperationID op );
ExplicitOperationID                   getExplicitOperationName( const std::string& strName );
const ExplicitOperationIDStringArray& getExplicitOperationStrings();

} // namespace mega

#endif // OPERATIONS_10_MAY_2022
