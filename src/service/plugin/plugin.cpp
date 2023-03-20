
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

#include "platform.hpp"
#include "player_network.hpp"

#include "service/executor/executor.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/platform.hxx"
#include "service/protocol/model/player_network.hxx"
#include "service/protocol/model/sim.hxx"

#include "service/protocol/common/platform_state.hpp"

#include "common/assert_verify.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

namespace mega::service
{
namespace
{

class Plugin : public network::ConversationBase
{
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, network::ReceivedMsg ) >;

    mutable std::optional< network::ConnectionID > m_selfConnectionID;

public:
    using Ptr = std::shared_ptr< Plugin >;

    // Sender
    virtual network::ConnectionID getConnectionID() const
    {
        if( m_selfConnectionID.has_value() )
            return m_selfConnectionID.value();
        // synthesize a connectionID value
        std::ostringstream os;
        os << "self_" << getID();
        m_selfConnectionID = os.str();
        return m_selfConnectionID.value();
    }

    virtual boost::system::error_code send( const network::Message& msg, boost::asio::yield_context& yield_ctx )
    {
        const network::ReceivedMsg rMsg{ getConnectionID(), msg };
        send( rMsg );
        return boost::system::error_code{};
    }

    void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg )
    {
        const network::ReceivedMsg rMsg{ getConnectionID(), make_error_msg( msg.msg.getReceiverID(), strErrorMsg ) };
        send( rMsg );
    }

    virtual void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        sendErrorResponse( msg, strErrorMsg );
    }

    // network::ConversationBase
    virtual const ID& getID() const
    {
        static network::ConversationID pluginID{ 0, "PLUGIN" };
        return pluginID;
    }

    virtual void send( const network::ReceivedMsg& msg )
    {
        m_channel.async_send(
            boost::system::error_code(), msg,
            [ &msg ]( boost::system::error_code ec )
            {
                if( ec )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
            } );
    }
    virtual network::Message   dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) { THROW_TODO; }
    virtual void               run( boost::asio::yield_context& yield_ctx ) { THROW_TODO; }
    virtual const std::string& getProcessName() const { THROW_TODO; }
    virtual U64                getStackSize() const { THROW_TODO; }
    virtual void               onDisconnect( const network::ConnectionID& connectionID ) { THROW_TODO; }
    virtual void               requestStarted( const network::ConnectionID& connectionID ) { ; }
    virtual void               requestCompleted() { ; }

    void send( ConversationBase& sender, network::Message&& requestMsg )
    {
        // SPDLOG_TRACE( "plugin::send: {}", requestMsg.getName() );
        const network::ReceivedMsg rMsg{ sender.getConnectionID(), requestMsg };
        sender.send( rMsg );
    }
    template < typename MsgType >
    void send( ConversationBase& sender, MsgType&& msg )
    {
        send( sender, MsgType::make( getID(), sender.getID(), std::move( msg ) ) );
    }

    // Plugin
    Plugin( boost::asio::io_context& ioContext, U64 uiNumThreads )
        : m_executor( ioContext, uiNumThreads, mega::network::MegaDaemonPort(), this )
        , m_channel( ioContext )
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

    ~Plugin()
    {
        SPDLOG_TRACE( "Plugin::~Plugin()" );

        if( m_pPlayerNetwork )
        {
            using namespace network::player_network;
            while( m_bNetworkRequest )
            {
                update( 1.0f );
            }
            m_bNetworkRequest = true;
            send( *m_pPlayerNetwork, MSG_PlayerNetworkDestroy_Request{} );
            while( m_bNetworkRequest )
            {
                update( 1.0f );
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
                update( 1.0f );
            }
            m_bPlatformRequest = true;
            send( *m_pPlatform, MSG_PlatformDestroy_Request{} );
            while( m_bPlatformRequest )
            {
                update( 1.0f );
            }
            if( m_pPlatform )
            {
                m_executor.conversationCompleted( m_pPlatform );
                m_pPlatform.reset();
            }
        }
    }

    Plugin( const Plugin& )            = delete;
    Plugin( Plugin&& )                 = delete;
    Plugin& operator=( const Plugin& ) = delete;
    Plugin& operator=( Plugin&& )      = delete;

    float                  m_ct         = 0.0f;
    float                  m_statusRate = 1.0f;
    std::optional< float > m_lastPlatformStatus;
    std::optional< float > m_lastNetworkStatus;
    bool                   m_bNetworkRequest  = false;
    bool                   m_bPlatformRequest = false;

    void update( float dt )
    {
        float last_ct = m_ct;
        m_ct += dt;

        if( m_pPlatform )
        {
            if( m_lastPlatformStatus.has_value() && !m_bPlatformRequest
                && ( ( m_ct - m_lastPlatformStatus.value() ) > m_statusRate ) )
            {
                using namespace network::platform;
                send( *m_pPlatform, MSG_PlatformStatus_Response{} );
                m_lastPlatformStatus.reset();
            }
        }

        if( m_pPlayerNetwork )
        {
            if( m_lastNetworkStatus.has_value() && !m_bNetworkRequest
                && ( ( m_ct - m_lastNetworkStatus.value() ) > m_statusRate ) )
            {
                using namespace network::player_network;
                send( *m_pPlayerNetwork, MSG_PlayerNetworkStatus_Response{} );
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
                const network::Message& msg = msgOpt.value().msg;
                // SPDLOG_TRACE( "plugin::update: {}", msg.getName() );

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
                        SPDLOG_TRACE( "plugin::update: {}", msg.getName() );
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
                    }
                    break;
                    case MSG_PlayerNetworkDestroyPlanet_Response::ID:
                    {
                        m_bNetworkRequest = false;
                    }
                    break;

                    // simulation clock
                    case MSG_SimClock_Request::ID:
                    {
                        if( ConversationBase::Ptr pSim = m_executor.findExistingConversation( msg.getSenderID() ) )
                        {
                            send( *pSim, MSG_SimClock_Response{} );
                        }
                        else
                        {
                            THROW_RTE( "Failed to resolve simulation" );
                        }
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
                        THROW_RTE( "Unsupported msg type: " << msg.getName() );
                        break;
                }
            }
            else
            {
                break;
            }
        }
    }

    I64 network_count()
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

    const char* network_name( int networkID )
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

    void network_connect( I64 networkID )
    {
        SPDLOG_TRACE( "Plugin::network_connect: {}", networkID );

        using namespace network::player_network;
        if( m_pPlayerNetwork )
        {
            m_bNetworkRequest = true;
            send( *m_pPlayerNetwork, MSG_PlayerNetworkConnect_Request{ networkID } );
        }
    }

    void network_disconnect()
    {
        SPDLOG_TRACE( "Plugin::network_disconnect" );

        using namespace network::player_network;
        if( m_pPlayerNetwork )
        {
            m_bNetworkRequest = true;
            send( *m_pPlayerNetwork, MSG_PlayerNetworkDisconnect_Request{} );
        }
    }

    I64 network_current()
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

    void planet_create()
    {
        SPDLOG_TRACE( "Plugin::planet_create" );

        using namespace network::player_network;
        if( m_pPlayerNetwork )
        {
            m_bNetworkRequest = true;
            send( *m_pPlayerNetwork, MSG_PlayerNetworkCreatePlanet_Request{} );
        }
    }

    void planet_destroy()
    {
        SPDLOG_TRACE( "Plugin::planet_destroy" );

        using namespace network::player_network;
        if( m_pPlayerNetwork )
        {
            m_bNetworkRequest = true;
            send( *m_pPlayerNetwork, MSG_PlayerNetworkDestroyPlanet_Request{} );
        }
    }
    bool planet_current()
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

private:
    mega::service::Executor                            m_executor;
    MessageChannel                                     m_channel;
    Platform::Ptr                                      m_pPlatform;
    PlayerNetwork::Ptr                                 m_pPlayerNetwork;
    std::optional< mega::network::PlatformState >      m_platformStateOpt;
    std::optional< mega::network::PlayerNetworkState > m_networkStateOpt;
};

class PluginWrapper
{
public:
    using Ptr = std::unique_ptr< PluginWrapper >;

    PluginWrapper( const char* pszConsoleLogLevel, const char* pszFileLogLevel,
                   U64 uiNumThreads = std::thread::hardware_concurrency() )
    {
        {
            boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
            std::string             strConsoleLogLevel = "warn";
            std::string             strLogFileLevel    = "warn";
            if( pszConsoleLogLevel )
                strConsoleLogLevel = pszConsoleLogLevel;
            if( pszFileLogLevel )
                strLogFileLevel = pszFileLogLevel;
            mega::network::configureLog( logFolder, "executor", mega::network::fromStr( strConsoleLogLevel ),
                                         mega::network::fromStr( strLogFileLevel ) );
        }

        m_pPlugin = std::make_shared< mega::service::Plugin >( m_ioContext, uiNumThreads );

        for( int i = 0; i < uiNumThreads; ++i )
        {
            m_threads.emplace_back( std::move( std::thread( [ &ioContext = m_ioContext ]() { ioContext.run(); } ) ) );
        }
    }
    ~PluginWrapper()
    {
        m_pPlugin.reset();

        m_ioContext.stop();

        for( std::thread& thread : m_threads )
        {
            thread.join();
        }
    }

    boost::asio::io_context    m_ioContext;
    std::vector< std::thread > m_threads;
    Plugin::Ptr                m_pPlugin;
};

static PluginWrapper::Ptr g_pPluginWrapper;

} // namespace

} // namespace mega::service

void mp_initialise( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
{
    mega::service::g_pPluginWrapper.reset();
    mega::service::g_pPluginWrapper
        = std::make_unique< mega::service::PluginWrapper >( pszConsoleLogLevel, pszFileLogLevel );
}

void mp_update( float dt )
{
    mega::service::g_pPluginWrapper->m_pPlugin->update( dt );
}

void mp_shutdown()
{
    mega::service::g_pPluginWrapper.reset();
}

mega::I64 mp_network_count()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->network_count();
}

const char* mp_network_name( mega::I64 networkID )
{
    return mega::service::g_pPluginWrapper->m_pPlugin->network_name( networkID );
}

void mp_network_connect( mega::I64 networkID )
{
    mega::service::g_pPluginWrapper->m_pPlugin->network_connect( networkID );
}

void mp_network_disconnect()
{
    mega::service::g_pPluginWrapper->m_pPlugin->network_disconnect();
}

mega::I64 mp_network_current()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->network_current();
}

void mp_planet_create()
{
    mega::service::g_pPluginWrapper->m_pPlugin->planet_create();
}

void mp_planet_destroy()
{
    mega::service::g_pPluginWrapper->m_pPlugin->planet_destroy();
}

bool mp_planet_current()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->planet_current();
}