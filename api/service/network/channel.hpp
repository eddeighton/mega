


#ifndef CHANNEL_16_JUNE_2022
#define CHANNEL_16_JUNE_2022

#include "end_point.hpp"
#include "receiver.hpp"
#include "sender.hpp"

#include <optional>

namespace mega
{
namespace network
{

class ReceiverChannel
{
public:
    ReceiverChannel( boost::asio::io_context& ioContext, ConversationManager& conversationManager )
        : m_ioContext( ioContext )
        , m_channel( ioContext )
        , m_receiver( conversationManager, m_channel )
    {
    }

    void run( const ConnectionID& connectionID )
    {
        m_connectionIDOpt = connectionID;
        m_receiver.run( m_ioContext, m_connectionIDOpt.value() );
    }

    void stop()
    {
        m_receiver.stop();
        m_channel.cancel();
        m_channel.close();
    }

    Sender::Ptr getSender()
    {
        VERIFY_RTE( m_connectionIDOpt.has_value() );
        return make_current_channel_sender( m_channel, m_connectionIDOpt.value() );
    }

private:
    boost::asio::io_context&      m_ioContext;
    ConcurrentChannel             m_channel;
    ConcurrentChannelReceiver     m_receiver;
    std::optional< ConnectionID > m_connectionIDOpt;
};

} // namespace network
} // namespace mega

#endif // CHANNEL_16_JUNE_2022