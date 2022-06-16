
#ifndef SERVICE_24_MAY_2022
#define SERVICE_24_MAY_2022

#include "leaf.hpp"

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/sender.hpp"
#include "service/network/channel.hpp"

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

class Terminal : public network::ConversationManager
{
    friend class TerminalRequestConversation;

public:
    Terminal( std::optional< const std::string > optName = std::nullopt );
    ~Terminal();
    void shutdown();
    bool running() { return !m_io_context.stopped(); }
    /*
        std::string                        GetVersion();
        std::vector< std::string >         ListTerminals();
        std::vector< network::ConversationID > listActivities();
        bool                               Shutdown();
    */
    network::PipelineResult PipelineRun( const pipeline::Configuration& pipelineConfig );

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                             const network::Header&         header,
                                                             const network::MessageVariant& msg );

    std::vector< std::string > listNetworkNodes();

    network::Sender& getLeafSender() { return m_leaf; }

private:
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    Leaf                     m_leaf;
};

} // namespace service
} // namespace mega

#endif // SERVICE_24_MAY_2022
