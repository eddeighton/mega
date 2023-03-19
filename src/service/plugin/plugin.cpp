
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
    Plugin( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
        : m_ioContext()
        , m_executor( m_ioContext, m_uiNumThreads, m_daemonPortNumber )
        , m_channel( m_ioContext )
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

        for( int i = 0; i < m_uiNumThreads; ++i )
        {
            m_threads.emplace_back( std::move( std::thread( [ &ioContext = m_ioContext ]() { ioContext.run(); } ) ) );
        }

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

        {
            using namespace network::player_network;
            send( *m_pPlayerNetwork, MSG_PlayerNetworkDestroy_Request{} );
            while( m_pPlayerNetwork )
            {
                update();
            }
        }
        {
            using namespace network::platform;
            send( *m_pPlatform, MSG_PlatformDestroy_Request{} );
            while( m_pPlatform )
            {
                update();
            }
        }

        m_executor.shutdown();
        for( std::thread& thread : m_threads )
        {
            thread.join();
        }
    }

    Plugin( const Plugin& )            = delete;
    Plugin( Plugin&& )                 = delete;
    Plugin& operator=( const Plugin& ) = delete;
    Plugin& operator=( Plugin&& )      = delete;

    void update()
    {
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

                switch( msg.getID() )
                {
                    case MSG_PlatformStatus_Request::ID:
                    {
                        m_platformStateOpt = MSG_PlatformStatus_Request::get( msg ).state;
                        if( m_pPlatform )
                        {
                            send( *m_pPlatform, MSG_PlatformStatus_Response{} );
                        }
                    }
                    break;
                    case MSG_PlatformDestroy_Response::ID:
                    {
                        m_pPlatform.reset();
                    }
                    break;

                    case MSG_PlayerNetworkStatus_Request::ID:
                    {
                        m_networkStateOpt = MSG_PlayerNetworkStatus_Request::get( msg ).state;
                        if( m_pPlatform )
                        {
                            send( *m_pPlatform, MSG_PlayerNetworkStatus_Response{} );
                        }
                    }
                    break;
                    case MSG_PlayerNetworkConnect_Response::ID:
                    {
                    }
                    break;
                    case MSG_PlayerNetworkDisconnect_Response::ID:
                    {
                    }
                    break;
                    case MSG_PlayerNetworkDestroy_Response::ID:
                    {
                        m_pPlayerNetwork.reset();
                    }
                    break;

                    case MSG_PlayerNetworkCreatePlanet_Response::ID:
                    {
                    }
                    break;
                    case MSG_PlayerNetworkDestroyPlanet_Response::ID:
                    {
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

    mega::U64 network_count()
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
            const auto& networks = m_platformStateOpt.value().m_availableNetworks;
            if( networkID >= 0 && networkID < networks.size() )
            {
                return networks[ networkID ].c_str();
            }
        }
        return nullptr;
    }

    void network_connect( mega::U64 networkID )
    {
        SPDLOG_TRACE( "Plugin::network_connect: {}", networkID );

        using namespace network::player_network;
        if( m_pPlayerNetwork )
        {
            send( *m_pPlayerNetwork, MSG_PlayerNetworkConnect_Request{ networkID } );
        }
    }

    void network_disconnect()
    {
        SPDLOG_TRACE( "Plugin::network_disconnect" );

        using namespace network::player_network;
        if( m_pPlayerNetwork )
        {
            send( *m_pPlayerNetwork, MSG_PlayerNetworkDisconnect_Request{} );
        }
    }

    void planet_create()
    {
        SPDLOG_TRACE( "Plugin::planet_create" );

        using namespace network::player_network;
        if( m_pPlayerNetwork )
        {
            send( *m_pPlayerNetwork, MSG_PlayerNetworkCreatePlanet_Request{} );
        }
    }

    void planet_destroy()
    {
        SPDLOG_TRACE( "Plugin::planet_destroy" );

        using namespace network::player_network;
        if( m_pPlayerNetwork )
        {
            send( *m_pPlayerNetwork, MSG_PlayerNetworkDestroyPlanet_Request{} );
        }
    }

private:
    decltype( std::thread::hardware_concurrency() ) m_uiNumThreads     = std::thread::hardware_concurrency();
    short                                           m_daemonPortNumber = mega::network::MegaDaemonPort();

    boost::asio::io_context    m_ioContext;
    mega::service::Executor    m_executor;
    MessageChannel             m_channel;
    std::vector< std::thread > m_threads;
    Platform::Ptr              m_pPlatform;
    PlayerNetwork::Ptr         m_pPlayerNetwork;

    std::optional< mega::network::PlatformState >      m_platformStateOpt;
    std::optional< mega::network::PlayerNetworkState > m_networkStateOpt;
};

static Plugin::Ptr g_pPlugin;

} // namespace

} // namespace mega::service

void mp_initialise( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
{
    mega::service::g_pPlugin.reset();
    mega::service::g_pPlugin = std::make_shared< mega::service::Plugin >( pszConsoleLogLevel, pszFileLogLevel );
}

void mp_update()
{
    mega::service::g_pPlugin->update();
}

void mp_shutdown()
{
    mega::service::g_pPlugin.reset();
}

mega::U64 mp_network_count()
{
    return mega::service::g_pPlugin->network_count();
}

const char* mp_network_name( mega::U64 networkID )
{
    return mega::service::g_pPlugin->network_name( networkID );
}

void mp_network_connect( mega::U64 networkID )
{
    mega::service::g_pPlugin->network_connect( networkID );
}

void mp_network_disconnect()
{
    mega::service::g_pPlugin->network_disconnect();
}

void mp_planet_create()
{
    mega::service::g_pPlugin->planet_create();
}

void mp_planet_destroy()
{
    mega::service::g_pPlugin->planet_destroy();
}