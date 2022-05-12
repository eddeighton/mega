
#include "database/types/cardinality.hpp"

std::ostream& operator<<( std::ostream& os, const mega::CardinalityRange& cardinalityRange )
{
    if( cardinalityRange.lower().isNullAllowed() )
        os << '!';
    if( cardinalityRange.lower().isMany() )
        os << "*";
    else
        os << cardinalityRange.lower().getNumber();
    os << ":";
    if( cardinalityRange.upper().isNullAllowed() )
        os << '!';
    if( cardinalityRange.upper().isMany() )
        os << "*";
    else
        os << cardinalityRange.upper().getNumber();
    return os;
}
