
#ifndef EXECUTOR_27_MAY_2022
#define EXECUTOR_27_MAY_2022

#include "service/leaf/leaf.hpp"

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"

#include "parser/parser.hpp"
#include "service/protocol/common/header.hpp"

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
class Simulation;

class Executor : public network::ConversationManager
{
    friend class ExecutorRequestConversation;
    friend class JobConversation;
    friend class Simulation;

public:
    using SimulationMap = std::map< network::ConversationID, std::shared_ptr< Simulation > >;

    Executor( boost::asio::io_context& io_context, int numThreads );
    ~Executor();
    void shutdown();

    int getNumThreads() const { return m_numThreads; }

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Header&       header,
                                                             const network::Message&      msg );

    network::Sender& getLeafSender() { return m_leaf; }

private:
    boost::asio::io_context&                 m_io_context;
    int                                      m_numThreads;
    boost::shared_ptr< EG_PARSER_INTERFACE > m_pParser;
    network::ReceiverChannel                 m_receiverChannel;
    Leaf                                     m_leaf;
    SimulationMap                            m_simulations;
};

} // namespace service
} // namespace mega

#endif // EXECUTOR_27_MAY_2022
