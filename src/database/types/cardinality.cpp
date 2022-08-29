
#include "database/types/cardinality.hpp"

std::ostream& operator<<( std::ostream& os, const mega::CardinalityRange& cardinalityRange )
{
    if( cardinalityRange.minimum().isNullAllowed() )
        os << '!';
    if( cardinalityRange.minimum().isMany() )
        os << "*";
    else
        os << cardinalityRange.minimum().getNumber();
    os << ":";
    if( cardinalityRange.maximum().isNullAllowed() )
        os << '!';
    if( cardinalityRange.maximum().isMany() )
        os << "*";
    else
        os << cardinalityRange.maximum().getNumber();
    return os;
}
