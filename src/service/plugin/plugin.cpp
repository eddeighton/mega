
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

#include <memory>

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/project.hxx"
#include "service/protocol/model/platform.hxx"
#include "service/protocol/model/player_network.hxx"
#include "service/protocol/model/sim.hxx"

namespace mega::service
{
Plugin::Plugin( boost::asio::io_context& ioContext, U64 uiNumThreads )
    : m_channel( ioContext )
    , m_executor( ioContext, uiNumThreads, mega::network::MegaDaemonPort(), this, network::Node::Plugin )
    , m_stateMachine( *this )
{
    SPDLOG_TRACE( "Plugin::Plugin()" );
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
        send( *m_pPlayerNetwork, MSG_PlayerNetworkStatus_Response{ false } );
        m_pPlayerNetwork.reset();
    }
    if( m_pPlatform )
    {
        using namespace network::platform;
        while( m_bPlatformRequest )
        {
            runOne();
        }
        m_bPlatformRequest = true;
        send( *m_pPlatform, MSG_PlatformStatus_Response{ false } );
        m_pPlatform.reset();
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
    m_channel.async_send(
        boost::system::error_code(), msg,
        [ &msg ]( boost::system::error_code ec )
        {
            if( ec )
            {
                if( ec.value() == boost::asio::error::eof )
                {
                }
                else if( ec.value() == boost::asio::error::operation_aborted )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_closed )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_cancelled )
                {
                }
                else if( ec.failed() )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
            }
        } );
}

void Plugin::runOne()
{
    std::promise< network::ReceivedMsg > pro;
    std::future< network::ReceivedMsg >  fut = pro.get_future();
    m_channel.async_receive(
        [ &pro ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
        {
            if( ec )
            {
                SPDLOG_ERROR( "Failed to receive msg with error: {}", ec.what() );
                pro.set_exception( std::make_exception_ptr( std::runtime_error( ec.what() ) ) );
            }
            else
            {
                pro.set_value( msg );
            }
        } );

    dispatch( fut.get().msg );
}

void Plugin::tryRun()
{
    if( m_pPlatform )
    {
        if( m_lastPlatformStatus.has_value() && !m_bPlatformRequest
            && ( ( m_ct - m_lastPlatformStatus.value() ) > m_statusRate ) )
        {
            using namespace network::platform;
            send( *m_pPlatform, MSG_PlatformStatus_Response{ true } );
            m_lastPlatformStatus.reset();
        }
    }

    if( m_pPlayerNetwork )
    {
        if( m_lastNetworkStatus.has_value() && !m_bNetworkRequest
            && ( ( m_ct - m_lastNetworkStatus.value() ) > m_statusRate ) )
        {
            using namespace network::player_network;
            send( *m_pPlayerNetwork, MSG_PlayerNetworkStatus_Response{ true } );
            m_lastNetworkStatus.reset();
        }
    }

    std::optional< network::ReceivedMsg > msgOpt;
    while( true )
    {
        msgOpt.reset();
        m_channel.try_receive(
            [ &msgOpt ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
            {
                if( !ec )
                {
                    msgOpt = msg;
                }
                else
                {
                    THROW_TODO;
                }
            } );

        if( msgOpt.has_value() )
        {
            dispatch( msgOpt.value().msg );
        }
        else
        {
            break;
        }
    }
}

void Plugin::dispatch( const network::Message& msg )
{
    using namespace network::project;
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

        // network
        case MSG_PlayerNetworkStatus_Request::ID:
        {
            // SPDLOG_TRACE( "plugin::dispatch: {}", msg.getName() );
            m_networkStateOpt   = MSG_PlayerNetworkStatus_Request::get( msg ).state;
            m_lastNetworkStatus = m_ct;
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
            // m_bPlanetActive   = true;
        }
        break;
        case MSG_PlayerNetworkDestroyPlanet_Response::ID:
        {
            m_bNetworkRequest = false;
            // m_bPlanetActive   = false;
        }
        break;

        // simulation clock
        case MSG_SimRegister_Request::ID:
        {
            const network::sim::MSG_SimRegister_Request& msg_ = MSG_SimRegister_Request::get( msg );
            m_stateMachine.simRegister( msg_ );
            m_memoryDescription.onRegister( msg_.senderRef );
        }
        break;
        case MSG_SimUnregister_Request::ID:
        {
            const auto& msg_ = MSG_SimUnregister_Request::get( msg );
            m_stateMachine.simUnregister( msg_ );
            m_memoryDescription.onUnregister( msg_.mpo );
        }
        break;
        case MSG_SimClock_Request::ID:
        {
            m_stateMachine.simClock( MSG_SimClock_Request::get( msg ) );
        }
        break;

        // project
        case MSG_SetProject_Request::ID:
        {
            const auto& projectMsg = MSG_SetProject_Request::get( msg );
            SPDLOG_INFO( "plugin::dispatch: Set project request received for project: {}",
                         projectMsg.project.getProjectInstallPath().string() );
            boost::filesystem::path unityDatabasePath = projectMsg.project.getProjectBin() / "unityDatabase.json";
            VERIFY_RTE_MSG( boost::filesystem::exists( unityDatabasePath ),
                            "Failed to locate unity database: " << unityDatabasePath.string() );
            m_strDatabasePath  = unityDatabasePath.string();
            m_databaseHashcode = task::FileHash( unityDatabasePath );
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
