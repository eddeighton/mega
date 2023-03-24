
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

#ifndef GUARD_2023_January_20_relation_io
#define GUARD_2023_January_20_relation_io

#include "mega/relation_id.hpp"
#include "mega/type_id_io.hpp"

#include "nlohmann/json.hpp"

#include <vector>
#include <array>
#include <ostream>

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::RelationID& relationID )
{
    j = nlohmann::json{ { "lower", relationID.getLower() }, { "upper", relationID.getUpper() } };
}
} // namespace mega

inline std::ostream& operator<<( std::ostream& os, const mega::RelationID& relationID )
{
    // this must work within a filename - used for relation code gen
    return os <<

           std::hex <<

           std::setw( 2 ) << std::setfill( '0' ) << static_cast< mega::U16 >( relationID.getLower().getObjectID() )

              << '_' <<

           std::setw( 2 ) << std::setfill( '0' ) << static_cast< mega::U16 >( relationID.getLower().getSubObjectID() )

              << '_' <<

           std::setw( 2 ) << std::setfill( '0' ) << static_cast< mega::U16 >( relationID.getUpper().getObjectID() )

              << '_' <<

           std::setw( 2 ) << std::setfill( '0' ) << static_cast< mega::U16 >( relationID.getUpper().getSubObjectID() )

              << std::dec;
}

#endif // GUARD_2023_January_20_relation_io
