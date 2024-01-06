
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

#ifndef GUARD_2023_October_20_type_id_sequence_123
#define GUARD_2023_October_20_type_id_sequence_123

#include "mega/values/compilation/interface/symbol_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"
#include "mega/values/compilation/concrete/type_id.hpp"

#include "common/string.hpp"

#include <vector>
#include <ostream>

namespace mega
{
namespace interface
{
using SymbolIDSequence = std::vector< SymbolID >;
inline std::ostream& operator<<( std::ostream& os, const SymbolIDSequence& symbolIDSequence )
{
    common::delimit( symbolIDSequence.begin(), symbolIDSequence.end(), ".", os );
    return os;
}
using TypeIDSequence = std::vector< TypeID >;
inline std::ostream& operator<<( std::ostream& os, const TypeIDSequence& typeIDSequence )
{
    common::delimit( typeIDSequence.begin(), typeIDSequence.end(), ".", os );
    return os;
}
} // namespace interface
namespace concrete
{
using TypeIDSequence = std::vector< TypeID >;
inline std::ostream& operator<<( std::ostream& os, const TypeIDSequence& typeIDSequence )
{
    common::delimit( typeIDSequence.begin(), typeIDSequence.end(), ".", os );
    return os;
}
} // namespace concrete
} // namespace mega

#endif // GUARD_2023_October_20_type_id_sequence_123
