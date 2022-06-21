
#ifndef SIMULATION_22_JUNE_2022
#define SIMULATION_22_JUNE_2022

#include "mega/common.hpp"
#include "request.hpp"
#include "service/network/sender.hpp"
#include "service/protocol/common/header.hpp"

#include "service/protocol/model/exe_sim.hxx"

namespace mega
{
namespace service
{
class Executor;

class Simulation : public ExecutorRequestConversation, public network::exe_sim::Impl
{
public:
    using Ptr = std::shared_ptr< Simulation >;

    Simulation( Executor& executor, const network::ConversationID& conversationID );

    virtual bool dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx ) override;

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override;
                        
    virtual void run( boost::asio::yield_context& yield_ctx ) override;

    network::Sender& getRequestSender()
    {
        if ( !m_pRequestChannelSender )
        {
            m_pRequestChannelSender
                = network::make_current_channel_sender( m_requestChannel, network::ConnectionID{ "test" } );
        }
        return *m_pRequestChannelSender;
    }

    virtual void ExeSimReadLockAcquire( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimReadLockRelease( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimWriteLockAcquire( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimWriteLockRelease( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override;

private:
    network::ConcurrentChannel m_requestChannel;
    network::Sender::Ptr       m_pRequestChannelSender;
};

} // namespace service
} // namespace mega

#endif // SIMULATION_22_JUNE_2022