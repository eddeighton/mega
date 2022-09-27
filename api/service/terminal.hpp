
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
    mega::MPO                          SimCreate( const mega::MP& mp );
    void                               SimDestroy( const mega::MPO& mpo );
    std::string                        PingMP( const mega::MP& mp );
    std::string                        PingMPO( const mega::MPO& mpo );

    network::Sender& getLeafSender() { return m_leaf; }

private:
    using Router        = std::function< network::Message( const network::Message& ) >;
    using RouterFactory = std::function< Router(
        network::ConversationBase&, network::Sender&, boost::asio::yield_context& yield_ctx ) >;

    RouterFactory makeTermRoot();
    RouterFactory makeMP( mega::MP mp );
    RouterFactory makeMPO( mega::MPO mpo );

    network::Message routeGenericRequest( const network::Message& message, RouterFactory router );

    template < typename RequestType >
    RequestType getRootRequest()
    {
        using namespace std::placeholders;
        return RequestType( std::bind( &Terminal::routeGenericRequest, this, _1, makeTermRoot() ) );
    }

    template < typename RequestType >
    RequestType getMPRequest( mega::MP mp )
    {
        using namespace std::placeholders;
        return RequestType( std::bind( &Terminal::routeGenericRequest, this, _1, makeMP( mp ) ) );
    }

    template < typename RequestType >
    RequestType getMPORequest( mega::MPO mpo )
    {
        using namespace std::placeholders;
        return RequestType( std::bind( &Terminal::routeGenericRequest, this, _1, makeMPO( mpo ) ) );
    }

private:
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    Leaf                     m_leaf;
};

} // namespace service
} // namespace mega

#endif // SERVICE_24_MAY_2022
