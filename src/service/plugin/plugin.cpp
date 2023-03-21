
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

#include "service/plugin/plugin.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/platform.hxx"
#include "service/protocol/model/player_network.hxx"
#include "service/protocol/model/sim.hxx"

namespace mega::service
{
Plugin::Plugin( boost::asio::io_context& ioContext, U64 uiNumThreads )
    : m_executor( ioContext, uiNumThreads, mega::network::MegaDaemonPort(), this )
    , m_channel( ioContext )
    , m_stateMachine( *this )
{
    {
        m_pPlatform = std::make_shared< Platform >(
            m_executor, m_executor.createConversationID( m_executor.getLeafSender().getConnectionID() ), *this );
        m_executor.conversationInitiated( m_pPlatform, m_executor.getLeafSender() );
    }
    {
        m_pPlayerNetwork = std::make_shared< PlayerNetwork >(
            m_executor, m_executor.createConversationID( m_executor.getLeafSender().getConnectionID() ), *this );
        m_executor.conversationInitiated( m_pPlayerNetwork, m_executor.getLeafSender() );
    }
}

Plugin::~Plugin()
{
    SPDLOG_TRACE( "Plugin::~Plugin()" );

    if( m_pPlayerNetwork )
    {
        using namespace network::player_network;
        while( m_bNetworkRequest )
        {
            runOne();
        }
        m_bNetworkRequest = true;
        send( *m_pPlayerNetwork, MSG_PlayerNetworkDestroy_Request{} );
        while( m_bNetworkRequest )
        {
            runOne();
        }
        if( m_pPlayerNetwork )
        {
            m_executor.conversationCompleted( m_pPlayerNetwork );
            m_pPlayerNetwork.reset();
        }
    }
    if( m_pPlatform )
    {
        using namespace network::platform;
        while( m_bPlatformRequest )
        {
            runOne();
        }
        m_bPlatformRequest = true;
        send( *m_pPlatform, MSG_PlatformDestroy_Request{} );
        while( m_bPlatformRequest )
        {
            runOne();
        }
        if( m_pPlatform )
        {
            m_executor.conversationCompleted( m_pPlatform );
            m_pPlatform.reset();
        }
    }
}

network::ConnectionID Plugin::getConnectionID() const
{
    if( m_selfConnectionID.has_value() )
        return m_selfConnectionID.value();
    // synthesize a connectionID value
    std::ostringstream os;
    os << "self_" << getID();
    m_selfConnectionID = os.str();
    return m_selfConnectionID.value();
}

boost::system::error_code Plugin::send( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    const network::ReceivedMsg rMsg{ getConnectionID(), msg };
    send( rMsg );
    return boost::system::error_code{};
}

void Plugin::sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg )
{
    const network::ReceivedMsg rMsg{ getConnectionID(), make_error_msg( msg.msg.getReceiverID(), strErrorMsg ) };
    send( rMsg );
}

void Plugin::sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                boost::asio::yield_context& yield_ctx )
{
    sendErrorResponse( msg, strErrorMsg );
}

const network::ConversationID& Plugin::getID() const
{
    static network::ConversationID pluginID{ 0, "PLUGIN" };
    return pluginID;
}

void Plugin::send( const network::ReceivedMsg& msg )
{
    m_channel.async_send( boost::system::error_code(), msg,
                          [ &msg ]( boost::system::error_code ec )
                          {
                              if( ec )
                              {
                                  SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                                  THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                              }
                          } );
}

void Plugin::dispatch( const network::Message& msg )
{
    using namespace network::platform;
    using namespace network::player_network;
    using namespace network::sim;

    switch( msg.getID() )
    {
        // platform
        case MSG_PlatformStatus_Request::ID:
        {
            m_platformStateOpt   = MSG_PlatformStatus_Request::get( msg ).state;
            m_lastPlatformStatus = m_ct;
        }
        break;
        case MSG_PlatformDestroy_Response::ID:
        {
            m_bPlatformRequest = false;
        }
        break;

        // network
        case MSG_PlayerNetworkStatus_Request::ID:
        {
            SPDLOG_TRACE( "plugin::dispatch: {}", msg.getName() );
            m_networkStateOpt   = MSG_PlayerNetworkStatus_Request::get( msg ).state;
            m_lastNetworkStatus = m_ct;
        }
        break;
        case MSG_PlayerNetworkDestroy_Response::ID:
        {
            m_bNetworkRequest = false;
        }
        break;

        case MSG_PlayerNetworkConnect_Response::ID:
        {
            m_bNetworkRequest = false;
        }
        break;
        case MSG_PlayerNetworkDisconnect_Response::ID:
        {
            m_bNetworkRequest = false;
        }
        break;

        case MSG_PlayerNetworkCreatePlanet_Response::ID:
        {
            m_bNetworkRequest = false;
            //m_bPlanetActive   = true;
        }
        break;
        case MSG_PlayerNetworkDestroyPlanet_Response::ID:
        {
            m_bNetworkRequest = false;
            //m_bPlanetActive   = false;
        }
        break;

        // simulation clock
        case MSG_SimRegister_Request::ID:
        {
            m_stateMachine.simRegister( network::sim::MSG_SimRegister_Request::get( msg ) );
        }
        break;
        case MSG_SimUnregister_Request::ID:
        {
            m_stateMachine.simUnregister( network::sim::MSG_SimUnregister_Request::get( msg ) );
        }
        break;
        case MSG_SimClock_Request::ID:
        {
            m_stateMachine.simClock( network::sim::MSG_SimClock_Request::get( msg ) );
        }
        break;

        // errors
        case network::MSG_Error_Response::ID:
        {
            SPDLOG_ERROR( "{} {}", msg, network::MSG_Error_Response::get( msg ).what );
            THROW_RTE( network::MSG_Error_Response::get( msg ).what );
        }
        break;
        default:
            THROW_RTE( "plugin::dispatch Unsupported msg type: " << msg.getName() );
            break;
    }
}
I64 Plugin::network_count()
{
    if( m_platformStateOpt.has_value() )
    {
        return m_platformStateOpt.value().m_availableNetworks.size();
    }
    else
    {
        return 0;
    }
}

const char* Plugin::network_name( int networkID )
{
    if( m_platformStateOpt.has_value() )
    {
        const network::PlatformState& platform = m_platformStateOpt.value();
        const auto&                   networks = platform.m_availableNetworks;
        if( networkID >= 0 && networkID < networks.size() )
        {
            static std::string hmm;
            hmm = networks[ networkID ];
            return hmm.c_str();
        }
    }
    return nullptr;
}

void Plugin::network_connect( I64 networkID )
{
    SPDLOG_TRACE( "Plugin::network_connect: {}", networkID );

    using namespace network::player_network;
    if( m_pPlayerNetwork )
    {
        m_bNetworkRequest = true;
        send( *m_pPlayerNetwork, MSG_PlayerNetworkConnect_Request{ networkID } );
    }
}

void Plugin::network_disconnect()
{
    SPDLOG_TRACE( "Plugin::network_disconnect" );

    using namespace network::player_network;
    if( m_pPlayerNetwork )
    {
        m_bNetworkRequest = true;
        send( *m_pPlayerNetwork, MSG_PlayerNetworkDisconnect_Request{} );
    }
}

I64 Plugin::network_current()
{
    if( m_networkStateOpt.has_value() && m_platformStateOpt.has_value() )
    {
        const network::PlatformState&      platform = m_platformStateOpt.value();
        const network::PlayerNetworkState& network  = m_networkStateOpt.value();

        auto iFind = std::find(
            platform.m_availableNetworks.begin(), platform.m_availableNetworks.end(), network.m_currentNetwork );
        if( iFind != platform.m_availableNetworks.end() )
        {
            const I64 result = std::distance( platform.m_availableNetworks.begin(), iFind );
            // SPDLOG_TRACE( "Plugin::network_current: {}", result );
            return result;
        }
    }
    // SPDLOG_TRACE( "Plugin::network_current: {}", -1 );
    return -1;
}

void Plugin::planet_create()
{
    SPDLOG_TRACE( "Plugin::planet_create" );

    using namespace network::player_network;
    if( m_pPlayerNetwork )
    {
        m_bNetworkRequest = true;
        send( *m_pPlayerNetwork, MSG_PlayerNetworkCreatePlanet_Request{} );
    }
}

void Plugin::planet_destroy()
{
    SPDLOG_TRACE( "Plugin::planet_destroy" );

    using namespace network::player_network;
    if( m_pPlayerNetwork )
    {
        m_bNetworkRequest = true;
        send( *m_pPlayerNetwork, MSG_PlayerNetworkDestroyPlanet_Request{} );
    }
}
bool Plugin::planet_current()
{
    // SPDLOG_TRACE( "Plugin::planet_current" );
    if( m_networkStateOpt.has_value() )
    {
        return m_networkStateOpt.value().m_currentPlanet.has_value();
    }
    else
    {
        return false;
    }
}
} // namespace mega::service
