
#include "service/executor/simulation.hpp"

#include "service/executor.hpp"

namespace mega
{
namespace service
{

Simulation::Simulation( Executor& executor, const network::ConversationID& conversationID )
    : ExecutorRequestConversation( executor, conversationID, conversationID.getConnectionID() )
{
}

void Simulation::run( boost::asio::yield_context& yield_ctx )
{
    //
}

} // namespace service
} // namespace mega