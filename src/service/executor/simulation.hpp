
#ifndef SIMULATION_22_JUNE_2022
#define SIMULATION_22_JUNE_2022

#include "request.hpp"

namespace mega
{
namespace service
{
class Executor;

class Simulation : public ExecutorRequestConversation
{
public:
    using Ptr = std::shared_ptr< Simulation >;

    Simulation( Executor& executor, const network::ConversationID& conversationID );

    virtual void run( boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // SIMULATION_22_JUNE_2022