
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

#ifndef GUARD_2023_April_25_json_io
#define GUARD_2023_April_25_json_io

#include "mega/type_id.hpp"
#include "mega/invocation_id.hpp"
#include "mega/relation_id.hpp"

#include "nlohmann/json.hpp"

#include "boost/filesystem/path.hpp"

namespace mega
{
    
inline void to_json( nlohmann::json& j, const mega::RelationID& relationID )
{
    j = nlohmann::json{
        { "lower", relationID.getLower().getSymbolID() }, { "upper", relationID.getUpper().getSymbolID() } };
}

inline void to_json( nlohmann::json& j, const mega::TypeID& typeID )
{
    std::ostringstream os;
    using ::           operator<<;
    os << typeID;
    j = nlohmann::json{ { "typeID", os.str() } };
}

inline void to_json( nlohmann::json& j, const mega::InvocationID& invocationID )
{
    j = nlohmann::json{ { "context", invocationID.m_context },
                        { "type_path", invocationID.m_type_path },
                        { "operation", invocationID.m_operation } };
}
} // namespace mega

namespace boost::filesystem
{
inline void to_json( nlohmann::json& j, const boost::filesystem::path& p )
{
    j = nlohmann::json{ { "path", p.string() } };
}
}

#endif //GUARD_2023_April_25_json_io
