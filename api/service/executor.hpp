
#ifndef EXECUTOR_27_MAY_2022
#define EXECUTOR_27_MAY_2022

#include "leaf.hpp"

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"

#include "parser/parser.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <optional>
#include <string>
#include <optional>
#include <vector>
#include <thread>

namespace mega
{
namespace service
{

class Executor : public network::ConversationManager
{
    friend class ExecutorRequestConversation;
    friend class JobConversation;
public:
    Executor( boost::asio::io_context& io_context, int numThreads );
    ~Executor();
    void shutdown();

    int getNumThreads() const { return m_numThreads; }

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                             const network::Header&         header,
                                                             const network::MessageVariant& msg );

    network::Sender& getLeafSender() { return m_leaf; }
private:
    boost::asio::io_context&                 m_io_context;
    int                                      m_numThreads;
    boost::shared_ptr< EG_PARSER_INTERFACE > m_pParser;
    network::ReceiverChannel                 m_receiverChannel;
    Leaf                                     m_leaf;
};

} // namespace service
} // namespace mega

#endif // EXECUTOR_27_MAY_2022
