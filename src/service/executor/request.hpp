
#ifndef EXECUTOR_REQUEST_22_JUNE_2022
#define EXECUTOR_REQUEST_22_JUNE_2022

#include "service/network/conversation.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/leaf_worker.hxx"
#include "service/protocol/model/worker_leaf.hxx"

#include "runtime/runtime.hpp"

namespace mega
{
namespace service
{
class Executor;

class ExecutorRequestConversation : public network::ConcurrentConversation, public network::leaf_worker::Impl
{
protected:
    Executor& m_executor;

public:
    ExecutorRequestConversation( Executor& executor, const network::ConversationID& conversationID,
                                 const network::ConnectionID& originatingConnectionID );

    virtual bool dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx ) override;

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override;

    network::worker_leaf::Request_Encode  getLeafRequest( boost::asio::yield_context& yield_ctx );
    network::leaf_worker::Response_Encode getLeafResponse( boost::asio::yield_context& yield_ctx );

    virtual void RootListNetworkNodes( boost::asio::yield_context& yield_ctx ) override;

    virtual void RootPipelineStartJobs( const mega::pipeline::Configuration& configuration,
                                        const network::ConversationID&       rootConversationID,
                                        boost::asio::yield_context&          yield_ctx ) override;

    virtual void RootProjectUpdated( const mega::network::Project& project,
                                     boost::asio::yield_context&   yield_ctx ) override;

    virtual void RootSimList( boost::asio::yield_context& yield_ctx ) override;
    virtual void RootSimCreate( boost::asio::yield_context& yield_ctx ) override;

    virtual void RootSimReadLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override;
    virtual void RootSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // EXECUTOR_REQUEST_22_JUNE_2022