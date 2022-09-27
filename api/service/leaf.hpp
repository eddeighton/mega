#ifndef LEAF_16_JUNE_2022
#define LEAF_16_JUNE_2022

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/sender.hpp"
#include "service/network/channel.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/node.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <vector>
#include <thread>

namespace mega
{
namespace service
{

class Leaf : public network::ConversationManager, public network::Sender
{
    friend class LeafRequestConversation;
    friend class LeafEnrole;

public:
    Leaf( network::Sender::Ptr pSender, network::Node::Type nodeType );
    ~Leaf();
    // void shutdown();
    bool running() { return !m_io_context.stopped(); }

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Header&       header,
                                                             const network::Message&      msg );

    network::Node::Type getType() const { return m_nodeType; }
    network::Sender&    getDaemonSender() { return m_client; }
    network::Sender&    getNodeChannelSender() { return *m_pSender; }

    // network::Sender
    virtual network::ConnectionID     getConnectionID() const { return m_pSelfSender->getConnectionID(); }
    virtual boost::system::error_code send( const network::ConversationID& conversationID, const network::Message& msg,
                                            boost::asio::yield_context& yield_ctx )
    {
        return m_pSelfSender->send( conversationID, msg, yield_ctx );
    }
   /*virtual boost::system::error_code post( const network::ConversationID& sourceID,
                                            const network::ConversationID& targetID, const network::Message& msg,
                                            boost::asio::yield_context& yield_ctx )
    {
        return m_pSelfSender->post( sourceID, targetID, msg, yield_ctx );
    }*/
    virtual void sendErrorResponse( const network::ConversationID& conversationID, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        m_pSelfSender->sendErrorResponse( conversationID, strErrorMsg, yield_ctx );
    }

private:
    network::Sender::Ptr     m_pSender;
    network::Sender::Ptr     m_pSelfSender;
    network::Node::Type      m_nodeType;
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;

    using ExecutorType = decltype( m_io_context.get_executor() );

    network::Client                                  m_client;
    boost::asio::executor_work_guard< ExecutorType > m_work_guard;
    std::thread                                      m_io_thread;
    mega::MP                                         m_mp;
    std::set< mega::MPO >                            m_mpos;
};

} // namespace service
} // namespace mega

#endif // LEAF_16_JUNE_2022