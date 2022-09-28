


#ifndef INVOCATION_ID_7_JUNE_2022
#define INVOCATION_ID_7_JUNE_2022

#include "mega/invocation_id.hpp"

#include "nlohmann/json.hpp"

#include <vector>
#include <array>
#include <ostream>

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::InvocationID& invocationID )
{
    j = nlohmann::json{ { "context", invocationID.m_context },
                        { "type_path", invocationID.m_type_path },
                        { "operation", invocationID.m_operation } };
}
} // namespace mega

std::ostream& operator<<( std::ostream& os, const mega::InvocationID& invocationID );

#endif // INVOCATION_ID_7_JUNE_2022
