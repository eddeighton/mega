#ifndef SENDER_15_JUNE_2022
#define SENDER_15_JUNE_2022

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/messages.hxx"

#include "service/network/end_point.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/spawn.hpp"
#include "boost/asio/experimental/concurrent_channel.hpp"
#include "boost/asio/experimental/channel.hpp"

#include <memory>

namespace mega
{
namespace network
{

class Sender
{
public:
    using Ptr         = std::unique_ptr< Sender >;
    virtual ~Sender() = 0;

    virtual ConnectionID getConnectionID() const = 0;

    virtual boost::system::error_code send( const ConversationID& conversationID, const MessageVariant& msg,
                                            boost::asio::yield_context& yield_ctx )
        = 0;
    virtual void sendErrorResponse( const ConversationID& conversationID, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
        = 0;
};

Sender::Ptr make_socket_sender( boost::asio::ip::tcp::socket& socket, const ConnectionID& connectionID );
Sender::Ptr make_current_channel_sender( ConcurrentChannel& channel, const ConnectionID& connectionID );
Sender::Ptr make_channel_sender( Channel& channel, const ConnectionID& connectionID );

} // namespace network
} // namespace mega

#endif // SENDER_15_JUNE_2022
