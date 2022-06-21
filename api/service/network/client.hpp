#ifndef CLIENT_24_MAY_2022
#define CLIENT_24_MAY_2022

#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/receiver.hpp"
#include "service/network/sender.hpp"

#include "common/assert_verify.hpp"

#include "boost/asio/strand.hpp"
#include "boost/asio/ip/tcp.hpp"

#include <boost/asio/execution_context.hpp>
#include <string>
#include <thread>
#include <functional>
#include <iostream>

namespace mega
{
namespace network
{

class Client : public Sender
{
    using Strand = boost::asio::strand< boost::asio::io_context::executor_type >;

public:
    Client( boost::asio::io_context& ioContext, ConversationManager& conversationManager,
            const std::string& strServiceIP, const std::string& strServiceName );
    ~Client();

    boost::asio::io_context& getIOContext() const { return m_ioContext; }

    void stop();
    void disconnected();

    // Sender
    virtual ConnectionID              getConnectionID() const { return m_pSender->getConnectionID(); }
    virtual boost::system::error_code send( const ConversationID& conversationID, const Message& msg,
                                            boost::asio::yield_context& yield_ctx )
    {
        return m_pSender->send( conversationID, msg, yield_ctx );
    }
    virtual void sendErrorResponse( const ConversationID& conversationID, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        m_pSender->sendErrorResponse( conversationID, strErrorMsg, yield_ctx );
    }

private:
    boost::asio::io_context&       m_ioContext;
    boost::asio::ip::tcp::resolver m_resolver;
    Strand                         m_strand;
    boost::asio::ip::tcp::socket   m_socket;
    boost::asio::ip::tcp::endpoint m_endPoint;
    SocketReceiver                 m_receiver;
    ConnectionID                   m_connectionID;
    Sender::Ptr                    m_pSender;
};

} // namespace network
} // namespace mega

#endif // CLIENT_24_MAY_2022
