
#ifndef SERVICE_24_MAY_2022
#define SERVICE_24_MAY_2022

#include "service/leaf/leaf.hpp"

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/sender.hpp"
#include "service/network/channel.hpp"
#include "service/protocol/common/header.hpp"
#include "service/protocol/common/megastructure_installation.hpp"

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

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Header&       header,
                                                             const network::Message&      msg );

    std::vector< std::string >             ListNetworkNodes();
    network::PipelineResult                PipelineRun( const pipeline::Configuration& pipelineConfig );
    network::MegastructureInstallation     GetMegastructureInstallation();
    bool                                   SetProject( const mega::network::Project& project );
    mega::network::Project                 GetProject();
    bool                                   NewInstallation( const mega::network::Project& project );
    network::ConversationID                SimNew();
    void                                   SimDestroy( const network::ConversationID& simID );
    std::vector< network::ConversationID > SimList();
    bool        SimRead( const network::ConversationID& owningID, const network::ConversationID& simID );
    bool        SimWrite( const network::ConversationID& owningID, const network::ConversationID& simID );
    void        SimRelease( const network::ConversationID& owningID, const network::ConversationID& simID );
    void        ClearStash();
    mega::U64 Capacity();
    void        Shutdown();

    network::Sender& getLeafSender() { return m_leaf; }

private:
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    Leaf                     m_leaf;
};

} // namespace service
} // namespace mega

#endif // SERVICE_24_MAY_2022
