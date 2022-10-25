//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef SERVICE_24_MAY_2022
#define SERVICE_24_MAY_2022

#include "service/leaf.hpp"

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "service/network/network.hpp"

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

namespace mega::service
{

class Terminal : public network::ConversationManager
{
    friend class TerminalRequestConversation;

public:
    Terminal( short daemonPortNumber = mega::network::MegaDaemonPort() );
    ~Terminal();

    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Message&      msg );

    network::MegastructureInstallation GetMegastructureInstallation();
    network::Project                   GetProject();
    void                               SetProject( const network::Project& project );
    void                               ClearStash();
    network::Status                    GetNetworkStatus();
    network::PipelineResult            PipelineRun( const pipeline::Configuration& pipelineConfig );
    mega::MPO                          SimCreate( const mega::MP& mp );
    void                               SimDestroy( const mega::MPO& mpo );
    TimeStamp                          SimRead( const mega::MPO& from, const mega::MPO& to );
    TimeStamp                          SimWrite( const mega::MPO& from, const mega::MPO& to );
    void                               SimRelease( const mega::MPO& from, const mega::MPO& to );
    std::string                        PingMP( const mega::MP& mp, const std::string& strMsg );
    std::string                        PingMPO( const mega::MPO& mpo, const std::string& strMsg );

    network::Sender& getLeafSender() { return m_leaf; }

private:
    using Router        = std::function< network::Message( const network::Message& ) >;
    using RouterFactory = std::function< Router(
        network::ConversationBase&, network::Sender&, boost::asio::yield_context& yield_ctx ) >;

    RouterFactory makeTermRoot();
    RouterFactory makeMP( mega::MP mp );
    RouterFactory makeMPO( mega::MPO mpo );

    network::Message routeGenericRequest( const network::ConversationID& conversationID,
                                          const network::Message&        message,
                                          RouterFactory                  router );

    template < typename RequestType >
    RequestType getRootRequest()
    {
        const auto conversationID = createConversationID( getLeafSender().getConnectionID() );
        using namespace std::placeholders;
        return RequestType(
            std::bind( &Terminal::routeGenericRequest, this, conversationID, _1, makeTermRoot() ), conversationID );
    }

    template < typename RequestType >
    RequestType getMPRequest( mega::MP mp )
    {
        const auto conversationID = createConversationID( getLeafSender().getConnectionID() );
        using namespace std::placeholders;
        return RequestType(
            std::bind( &Terminal::routeGenericRequest, this, conversationID, _1, makeMP( mp ) ), conversationID );
    }

    template < typename RequestType >
    RequestType getMPORequest( mega::MPO mpo )
    {
        const auto conversationID = createConversationID( getLeafSender().getConnectionID() );
        using namespace std::placeholders;
        return RequestType(
            std::bind( &Terminal::routeGenericRequest, this, conversationID, _1, makeMPO( mpo ) ), conversationID );
    }

private:
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    Leaf                     m_leaf;
};

} // namespace mega::service

#endif // SERVICE_24_MAY_2022
