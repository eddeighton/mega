
#include "database/types/type_id.hpp"

#include "common/string.hpp"

namespace mega
{

}

std::ostream& operator<<( std::ostream& os, const mega::TypeIDSequence& typeIDSequence )
{
    common::delimit( typeIDSequence.begin(), typeIDSequence.end(), ".", os );
    return os;
}

std::ostream& operator<<( std::ostream& os, const mega::TypeIDSequencePair& typeIDSequencePair )
{
    return os << typeIDSequencePair.first << "-" << typeIDSequencePair.second;
}