
#include "daemon.hpp"
#include "request.hpp"

#include "mega/common.hpp"
#include "service/network/conversation.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"


#include <iostream>

namespace mega
{
namespace service
{

////////////////////////////////////////////////////////////////

class DaemonEnrole : public DaemonRequestConversation
{
    std::promise< void >& m_promise;

public:
    DaemonEnrole( Daemon& daemon, const network::ConnectionID& originatingConnectionID, std::promise< void >& promise )
        : DaemonRequestConversation(
            daemon, daemon.createConversationID( originatingConnectionID ), originatingConnectionID )
        , m_promise( promise )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        m_daemon.m_mpo = getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleDaemon();
        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
    }
};

////////////////////////////////////////////////////////////////
// Daemon
Daemon::Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP )
    : network::ConversationManager( network::makeProcessName( network::Node::Daemon ), ioContext )
    , m_rootClient( ioContext, *this, strRootIP, mega::network::MegaRootServiceName() )
    , m_leafServer( ioContext, *this, network::MegaDaemonPort() )
{
    m_leafServer.waitForConnection();

    // immediately enrole
    {
        std::promise< void > promise;
        std::future< void >  future = promise.get_future();
        conversationInitiated(
            std::make_shared< DaemonEnrole >( *this, m_rootClient.getConnectionID(), promise ), m_rootClient );
        using namespace std::chrono_literals;
        while ( std::future_status::timeout == future.wait_for( 0s ) )
        {
            ioContext.run_one();
        }
    }
}

Daemon::~Daemon()
{
    //
    // SPDLOG_TRACE( "Daemon shutdown" );
}

void Daemon::onLeafDisconnect( const network::ConnectionID& connectionID, mega::MPO leafMPO )
{
    onDisconnect( connectionID );

    class DaemonLeafDisconnect : public DaemonRequestConversation
    {
        mega::MPO m_leafMPO;

    public:
        DaemonLeafDisconnect( Daemon& daemon, const network::ConnectionID& originatingConnectionID, mega::MPO leafMPO )
            : DaemonRequestConversation(
                daemon, daemon.createConversationID( originatingConnectionID ), originatingConnectionID )
            , m_leafMPO( leafMPO )
        {
        }
        void run( boost::asio::yield_context& yield_ctx )
        {
            getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleLeafDisconnect( m_leafMPO );
            SPDLOG_TRACE( "DaemonLeafDisconnect {}", m_leafMPO );
        }
    };
    conversationInitiated(
        std::make_shared< DaemonLeafDisconnect >( *this, m_rootClient.getConnectionID(), leafMPO ), m_rootClient );
}

void Daemon::shutdown()
{
    m_rootClient.stop();
    m_leafServer.stop();
}

network::ConversationBase::Ptr Daemon::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                         const network::Header&       header,
                                                         const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new DaemonRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
}

} // namespace service
} // namespace mega