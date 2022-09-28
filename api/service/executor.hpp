



#ifndef EXECUTOR_27_MAY_2022
#define EXECUTOR_27_MAY_2022

#include "service/leaf.hpp"

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/protocol/common/megastructure_installation.hpp"

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
    using SimulationMap = std::unordered_map< mega::MPO, std::shared_ptr< Simulation >, mega::MPO::Hash >;

    Executor( boost::asio::io_context& io_context, int numThreads );
    ~Executor();
    void shutdown();

    int getNumThreads() const { return m_numThreads; }

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Message&      msg );

    network::Sender& getLeafSender() { return m_leaf; }

    std::shared_ptr< Simulation > getSimulation( const mega::MPO& mpo ) const;
    mega::MPO createSimulation( network::ConversationBase& callingConversation, boost::asio::yield_context& yield_ctx );
    void      simulationTerminating( std::shared_ptr< Simulation > pSimulation );
    virtual void conversationCompleted( network::ConversationBase::Ptr pConversation );

private:
    boost::asio::io_context&                 m_io_context;
    int                                      m_numThreads;
    boost::shared_ptr< EG_PARSER_INTERFACE > m_pParser;
    network::ReceiverChannel                 m_receiverChannel;
    Leaf                                     m_leaf;
    SimulationMap                            m_simulations;
    network::MegastructureInstallation       m_megastructureInstallation;
};

} // namespace service
} // namespace mega

#endif // EXECUTOR_27_MAY_2022
