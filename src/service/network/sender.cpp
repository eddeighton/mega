
#include "service/network/sender.hpp"

#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/messages.hxx"

#include "common/assert_verify.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/experimental/channel_error.hpp>
#include <boost/asio/write.hpp>

#include "boost/interprocess/interprocess_fwd.hpp"
#include "boost/interprocess/streams/vectorstream.hpp"

#include "boost/archive/binary_oarchive.hpp"
#include <algorithm>
#include <sstream>

namespace mega
{
namespace network
{

Sender::~Sender() {}

class SocketSender : public Sender
{
    boost::asio::ip::tcp::socket& m_socket;
    ConnectionID                  m_connectionID;

public:
    SocketSender( boost::asio::ip::tcp::socket& socket, const ConnectionID& connectionID )
        : m_socket( socket )
        , m_connectionID( connectionID )
    {
    }

    virtual ~SocketSender() {}

    virtual ConnectionID getConnectionID() const { return m_connectionID; }

    virtual boost::system::error_code send( const ConversationID& conversationID, const Message& msg,
                                            boost::asio::yield_context& yield_ctx )
    {
        using SendBuffer = std::vector< char >;
        SendBuffer buffer;
        {
            boost::interprocess::basic_vectorbuf< SendBuffer > os;
            {
                const Header                    header( getMsgID( msg ), conversationID );
                boost::archive::binary_oarchive oa( os );
                oa&                             header;
                encode( oa, msg );
            }
            const MessageSize size = os.vector().size();
            std::string_view  sizeView( reinterpret_cast< const char* >( &size ), sizeof( MessageSize ) );
            buffer.reserve( size + sizeof( MessageSize ) );
            std::copy( sizeView.begin(), sizeView.end(), std::back_inserter( buffer ) );
            std::copy( os.vector().begin(), os.vector().end(), std::back_inserter( buffer ) );
        }

        boost::system::error_code ec;
        {
            const std::size_t szBytesWritten
                = boost::asio::async_write( m_socket, boost::asio::buffer( buffer ), yield_ctx[ ec ] ); //
            if ( !ec )
            {
                VERIFY_RTE( szBytesWritten == buffer.size() );
            }
            return ec;
        }
    }

    virtual void sendErrorResponse( const ConversationID& conversationID, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        Message msg = make_error_msg( strErrorMsg );
        if ( const boost::system::error_code ec = send( conversationID, msg, yield_ctx ) )
        {
            THROW_RTE( "Error sending: " << ec.what() );
        }
        else
        {
            SPDLOG_TRACE( "Sent error response for conversation: {} msg: {}", conversationID, strErrorMsg );
        }
    }
};

Sender::Ptr make_socket_sender( boost::asio::ip::tcp::socket& socket, const ConnectionID& connectionID )
{
    return std::make_unique< SocketSender >( socket, connectionID );
}

class ConcurrentChannelSender : public Sender
{
    ConcurrentChannel& m_channel;
    ConnectionID       m_connectionID;

public:
    ConcurrentChannelSender( ConcurrentChannel& channel, const ConnectionID& connectionID )
        : m_channel( channel )
        , m_connectionID( connectionID )
    {
    }

    virtual ~ConcurrentChannelSender() {}

    virtual ConnectionID getConnectionID() const { return m_connectionID; }

    virtual boost::system::error_code send( const ConversationID& conversationID, const Message& msg,
                                            boost::asio::yield_context& yield_ctx )
    {
        const ChannelMsg channelMsg{ Header{ static_cast< MessageID >( getMsgID( msg ) ), conversationID }, msg };
        boost::system::error_code ec;
        VERIFY_RTE_MSG( m_channel.is_open(), "Channel NOT open" );
        m_channel.async_send( ec, channelMsg, yield_ctx );

        if ( ec != boost::system::error_code() )
        {
            if ( ( ec != boost::asio::experimental::error::channel_cancelled )
                 && ( ec != boost::asio::experimental::error::channel_closed ) )
            {
                SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg, ec.what() );
                THROW_RTE( "Failed to send request on channel: " << msg << " : " << ec.what() );
            }
            else
            {
                SPDLOG_ERROR( "Failed to send request due to channel closed: {}", ec.what() );
                THROW_RTE( "Failed to send request due to channel closed: " << ec.what() );
            }
        }

        return ec;
    }

    virtual void sendErrorResponse( const ConversationID& conversationID, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        Message msg = make_error_msg( strErrorMsg );
        if ( const boost::system::error_code ec = send( conversationID, msg, yield_ctx ) )
        {
            THROW_RTE( "Error sending: " << ec.what() );
        }
        else
        {
            SPDLOG_TRACE( "Sent error response for conversation: {} msg: {}", conversationID, strErrorMsg );
        }
    }
};

Sender::Ptr make_current_channel_sender( ConcurrentChannel& channel, const ConnectionID& connectionID )
{
    return std::make_unique< ConcurrentChannelSender >( channel, connectionID );
}

class ChannelSender : public Sender
{
    Channel&     m_channel;
    ConnectionID m_connectionID;

public:
    ChannelSender( Channel& channel, const ConnectionID& connectionID )
        : m_channel( channel )
        , m_connectionID( connectionID )
    {
    }

    virtual ~ChannelSender() {}

    virtual ConnectionID getConnectionID() const { return m_connectionID; }

    virtual boost::system::error_code send( const ConversationID& conversationID, const Message& msg,
                                            boost::asio::yield_context& yield_ctx )
    {
        const ChannelMsg channelMsg{ Header{ static_cast< MessageID >( getMsgID( msg ) ), conversationID }, msg };
        boost::system::error_code ec;
        VERIFY_RTE_MSG( m_channel.is_open(), "Channel NOT open" );
        m_channel.async_send( ec, channelMsg, yield_ctx );

        if ( ec != boost::system::error_code() )
        {
            if ( ( ec != boost::asio::experimental::error::channel_cancelled )
                 && ( ec != boost::asio::experimental::error::channel_closed ) )
            {
                SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg, ec.what() );
                THROW_RTE( "Failed to send request on channel: " << msg << " : " << ec.what() );
            }
            else
            {
                SPDLOG_ERROR( "Failed to send request due to channel closed: {}", ec.what() );
                THROW_RTE( "Failed to send request due to channel closed: " << ec.what() );
            }
        }

        return ec;
    }

    virtual void sendErrorResponse( const ConversationID& conversationID, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        Message msg = make_error_msg( strErrorMsg );
        if ( const boost::system::error_code ec = send( conversationID, msg, yield_ctx ) )
        {
            THROW_RTE( "Error sending: " << ec.what() );
        }
        else
        {
            SPDLOG_TRACE( "Sent error response for conversation: {} msg: {}", conversationID, strErrorMsg );
        }
    }
};

Sender::Ptr make_channel_sender( Channel& channel, const ConnectionID& connectionID )
{
    return std::make_unique< ChannelSender >( channel, connectionID );
}

} // namespace network
} // namespace mega