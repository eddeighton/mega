
#ifndef SERVICE_24_MAY_2022
#define SERVICE_24_MAY_2022

#include "service/leaf.hpp"

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "service/network/conversation_manager.hpp"
#include "service/protocol/common/header.hpp"
#include "service/protocol/common/megastructure_installation.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <optional>
#include <string>
#include <optional>
#include <vector>
#include <thread>
#include <functional>

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

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Header&       header,
                                                             const network::Message&      msg );

    network::MegastructureInstallation GetMegastructureInstallation();
    network::Project                   GetProject();
    void                               SetProject( const network::Project& project );
    void                               ClearStash();
    network::Status                    GetNetworkStatus();
    network::PipelineResult            PipelineRun( const pipeline::Configuration& pipelineConfig );

    network::Sender& getLeafSender() { return m_leaf; }

private:
    network::Message rootRequest( const network::Message& message );

    template < typename RequestType >
    RequestType getRootRequest()
    {
        using namespace std::placeholders;
        return RequestType( std::bind( &Terminal::rootRequest, this, _1 ) );
    }

private:
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    Leaf                     m_leaf;
};

} // namespace service
} // namespace mega

#endif // SERVICE_24_MAY_2022
