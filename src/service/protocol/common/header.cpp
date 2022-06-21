
#include "service/protocol/common/header.hpp"


namespace mega
{
namespace network
{

std::ostream& operator<<( std::ostream& os, const ConversationID& conversationID )
{
    return os << conversationID.getID() << "_" << conversationID.getConnectionID();
}

}}
