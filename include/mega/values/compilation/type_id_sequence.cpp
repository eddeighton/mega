
#include "mega/values/compilation/type_id_sequence.hpp"

#include "common/string.hpp"

namespace mega
{
namespace interface
{
std::ostream& operator<<( std::ostream& os, const SymbolIDSequence& symbolIDSequence )
{
    common::delimit( symbolIDSequence.begin(), symbolIDSequence.end(), ".", os );
    return os;
}
std::ostream& operator<<( std::ostream& os, const TypeIDSequence& typeIDSequence )
{
    common::delimit( typeIDSequence.begin(), typeIDSequence.end(), ".", os );
    return os;
}
} // namespace interface
namespace concrete
{
std::ostream& operator<<( std::ostream& os, const TypeIDSequence& typeIDSequence )
{
    common::delimit( typeIDSequence.begin(), typeIDSequence.end(), ".", os );
    return os;
}
} // namespace concrete
} // namespace mega
