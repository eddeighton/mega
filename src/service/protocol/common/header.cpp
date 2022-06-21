
#include "service/protocol/common/header.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace mega
{
namespace network
{

std::ostream& operator<<( std::ostream& os, const ConversationID& conversationID )
{
    return os << conversationID.getID() << "_" << conversationID.getConnectionID();
}

std::istream& operator>>( std::istream& is, ConversationID& conversationID )
{
    std::string str;
    is >> str;

    auto iter = std::find( str.begin(), str.end(), '_' );

    VERIFY_RTE( iter != str.end() );

    const std::string strCon( iter + 1, str.end() );

    ConversationID::ID id;
    {
        std::string strInt( str.begin(), iter );
        std::istringstream isInt( strInt );
        isInt >> id;
    }

    conversationID = ConversationID{ id, strCon };

    return is;
}

} // namespace network
} // namespace mega
