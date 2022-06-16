
#ifndef RECEIVER_24_MAY_2022
#define RECEIVER_24_MAY_2022

#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/end_point.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/spawn.hpp"

#include <functional>

namespace mega
{
namespace network
{

class SocketReceiver
{
public:
    SocketReceiver( ConversationManager& conversationManager, boost::asio::ip::tcp::socket& socket,
                    std::function< void() > disconnectHandler );
    ~SocketReceiver();

    template < typename TExecutor >
    void run( TExecutor& strandOrIOContext, const ConnectionID& connectionID )
    {
        m_connectionID           = connectionID;
        SocketReceiver& receiver = *this;
        boost::asio::spawn(
            strandOrIOContext, [ &receiver ]( boost::asio::yield_context yield ) { receiver.receive( yield ); } );
    }
    void stop() { m_bContinue = false; }

private:
    void receive( boost::asio::yield_context& yield_ctx );
    void onError( const boost::system::error_code& ec );

private:
    ConnectionID                  m_connectionID;
    bool                          m_bContinue = true;
    ConversationManager&          m_conversationManager;
    boost::asio::ip::tcp::socket& m_socket;
    std::function< void() >       m_disconnectHandler;
};

class ConcurrentChannelReceiver
{
public:
    ConcurrentChannelReceiver( ConversationManager& conversationManager, ConcurrentChannel& channel );
    ~ConcurrentChannelReceiver();

    template < typename TExecutor >
    void run( TExecutor& strandOrIOContext, const ConnectionID& connectionID )
    {
        m_connectionID                      = connectionID;
        ConcurrentChannelReceiver& receiver = *this;
        boost::asio::spawn(
            strandOrIOContext, [ &receiver ]( boost::asio::yield_context yield ) { receiver.receive( yield ); } );
    }
    void stop() { m_bContinue = false; }

private:
    void receive( boost::asio::yield_context& yield_ctx );
    void onError( const boost::system::error_code& ec );

private:
    ConnectionID            m_connectionID;
    bool                    m_bContinue = true;
    ConversationManager&    m_conversationManager;
    ConcurrentChannel&      m_channel;
};

} // namespace network
} // namespace mega

#endif // RECEIVER_24_MAY_2022
