#ifndef TOOL_16_JUNE_2022
#define TOOL_16_JUNE_2022

#include "leaf.hpp"

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/sender.hpp"
#include "service/network/channel.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <vector>
#include <thread>

namespace mega
{
namespace service
{

class Tool : public network::ConversationManager
{
public:
    Tool();
    ~Tool();

    void shutdown();
    bool running() { return !m_io_context.stopped(); }

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                             const network::Header&         header,
                                                             const network::MessageVariant& msg );

private:
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    using ExecutorType = decltype( m_io_context.get_executor() );
    boost::asio::executor_work_guard< ExecutorType > m_work_guard;
    Leaf                                             m_leaf;
};

} // namespace service
} // namespace mega

#endif // TOOL_16_JUNE_2022