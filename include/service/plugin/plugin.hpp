
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

#ifndef GUARD_2023_March_07_plugin
#define GUARD_2023_March_07_plugin

#include "database.hpp"

#include "service/plugin/platform.hpp"
#include "service/plugin/player_network.hpp"
#include "service/plugin/plugin_state_machine.hpp"

#include "service/executor/executor.hpp"

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

class Plugin : public network::ConversationBase
{
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, network::ReceivedMsg ) >;

    mutable std::optional< network::ConnectionID > m_selfConnectionID;

public:
    using Ptr = std::shared_ptr< Plugin >;

    // Plugin
    Plugin( boost::asio::io_context& ioContext, U64 uiNumThreads );
    ~Plugin();

    Plugin( const Plugin& )            = delete;
    Plugin( Plugin&& )                 = delete;
    Plugin& operator=( const Plugin& ) = delete;
    Plugin& operator=( Plugin&& )      = delete;

    // Sender
    virtual network::ConnectionID     getConnectionID() const;
    virtual boost::system::error_code send( const network::Message& msg, boost::asio::yield_context& yield_ctx );
    void         sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg );
    virtual void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx );

    // network::ConversationBase
    virtual const network::ConversationID& getID() const;
    virtual void                           send( const network::ReceivedMsg& msg );

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

    void runOne()
    {
        std::promise< network::ReceivedMsg > pro;
        std::future< network::ReceivedMsg >  fut = pro.get_future();
        m_channel.async_receive(
            [ &pro ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
            {
                if( ec )
                {
                    SPDLOG_ERROR( "Failed to receive msg with error: {}", ec.what() );
                    THROW_RTE( "Failed to receive msg on channel: " << ec.what() );
                    pro.set_exception( std::make_exception_ptr( std::runtime_error( ec.what() ) ) );
                }
                else
                {
                    pro.set_value( msg );
                }
            } );

        dispatch( fut.get().msg );
    }

    Database* database()
    {
        return m_pDatabase.get();
    }

    log::Range* downstream()
    {
        tryRun();
        return m_stateMachine.getDownstream();
    }

    void upstream( float delta, void* pRange )
    {
        m_ct += delta;

        m_stateMachine.sendUpstream();
    }

    void tryRun()
    {
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
                dispatch( msgOpt.value().msg );
            }
            else
            {
                break;
            }
        }
    }

    void dispatch( const network::Message& msg );

    I64         network_count();
    const char* network_name( int networkID );
    void        network_connect( I64 networkID );
    void        network_disconnect();
    I64         network_current();

    void planet_create();
    void planet_destroy();
    bool planet_current();

private:
    mega::service::Executor                            m_executor;
    MessageChannel                                     m_channel;
    Platform::Ptr                                      m_pPlatform;
    PlayerNetwork::Ptr                                 m_pPlayerNetwork;
    std::optional< mega::network::PlatformState >      m_platformStateOpt;
    std::optional< mega::network::PlayerNetworkState > m_networkStateOpt;

    std::unique_ptr< Database > m_pDatabase;

    float                        m_ct               = 0.0f;
    float                        m_statusRate       = 1.0f;
    bool                         m_bNetworkRequest  = false;
    bool                         m_bPlatformRequest = false;
    std::optional< float >       m_lastPlatformStatus;
    std::optional< float >       m_lastNetworkStatus;
    PluginStateMachine< Plugin > m_stateMachine;
};
} // namespace mega::service

#endif // GUARD_2023_March_07_plugin
