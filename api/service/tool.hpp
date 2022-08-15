#ifndef TOOL_16_JUNE_2022
#define TOOL_16_JUNE_2022

#include "service/leaf/leaf.hpp"

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/sender.hpp"
#include "service/network/channel.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <vector>
#include <thread>
#include <functional>

namespace mega
{
namespace service
{

class Tool : public network::ConversationManager
{
    friend class ToolRequestConversation;
public:
    Tool();
    ~Tool();

    void shutdown();
    bool running() { return !m_io_context.stopped(); }

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Header&       header,
                                                             const network::Message&      msg );

    using Functor = std::function< void( boost::asio::yield_context& yield_ctx ) >;

    void run( Functor& function );

    network::Sender& getLeafSender() { return m_leaf; }
private:
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    //using ExecutorType = decltype( m_io_context.get_executor() );
    //boost::asio::executor_work_guard< ExecutorType > m_work_guard;
    Leaf                                             m_leaf;
};

} // namespace service
} // namespace mega

#endif // TOOL_16_JUNE_2022