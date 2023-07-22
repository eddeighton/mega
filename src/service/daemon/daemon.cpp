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

#include "daemon.hpp"
#include "request.hpp"

#include "mega/operation_id.hpp"

#include "service/network/conversation.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include <boost/filesystem/operations.hpp>

#include <iostream>

namespace mega::service
{

////////////////////////////////////////////////////////////////

class DaemonEnrole : public DaemonRequestConversation
{
    std::promise< void >& m_promise;

public:
    DaemonEnrole( Daemon& daemon, const network::ConnectionID& originatingConnectionID, std::promise< void >& promise )
        : DaemonRequestConversation(
            daemon, daemon.createConversationID(), originatingConnectionID )
        , m_promise( promise )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        m_daemon.m_machineID = getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleDaemon();
        m_daemon.setActiveProject( getRootRequest< network::project::Request_Encoder >( yield_ctx ).GetProject() );

        std::ostringstream os;
        os << network::Node::toStr( network::Node::Daemon ) << " " << m_daemon.m_machineID;
        common::ProcessID::setDescription( os.str().c_str() );

        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
    }
};

////////////////////////////////////////////////////////////////
// Daemon
Daemon::Daemon( boost::asio::io_context& ioContext,
                const std::string&       strRootIP,
                short                    rootPortNumber,
                short                    daemonPortNumber )
    : network::ConversationManager( network::makeProcessName( network::Node::Daemon ), ioContext )
    , m_rootClient( ioContext, *this, strRootIP, rootPortNumber )
    , m_server( ioContext, *this, daemonPortNumber )
{
    m_server.waitForConnection();

    // immediately enrole
    {
        std::promise< void > promise;
        std::future< void >  future = promise.get_future();
        conversationInitiated(
            std::make_shared< DaemonEnrole >( *this, m_rootClient.getConnectionID(), promise ), m_rootClient );
        using namespace std::chrono_literals;
        while( std::future_status::timeout == future.wait_for( 0s ) )
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

void Daemon::setActiveProject( const Project& project )
{
    m_activeProject = project;
}

void Daemon::onLeafDisconnect( const network::ConnectionID& connectionID, mega::MP leafMP )
{
    m_server.unLabelConnection( leafMP );

    onDisconnect( connectionID );

    class DaemonLeafDisconnect : public DaemonRequestConversation
    {
        mega::MP m_leafMP;

    public:
        DaemonLeafDisconnect( Daemon& daemon, const network::ConnectionID& originatingConnectionID, mega::MP leafMP )
            : DaemonRequestConversation(
                daemon, daemon.createConversationID(), originatingConnectionID )
            , m_leafMP( leafMP )
        {
        }
        void run( boost::asio::yield_context& yield_ctx )
        {
            getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleLeafDisconnect( m_leafMP );
            SPDLOG_TRACE( "DaemonLeafDisconnect {}", m_leafMP );
        }
    };
    conversationInitiated(
        std::make_shared< DaemonLeafDisconnect >( *this, m_rootClient.getConnectionID(), leafMP ), m_rootClient );
}

void Daemon::shutdown()
{
    m_rootClient.stop();
    m_server.stop();
}

network::ConversationBase::Ptr Daemon::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                         const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new DaemonRequestConversation( *this, msg.getReceiverID(), originatingConnectionID ) );
}

} // namespace mega::service
