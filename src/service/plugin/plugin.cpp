
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

#include "service/executor/executor.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/platform.hxx"

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

public:
    using Ptr = std::shared_ptr< Plugin >;

    // Sender
    virtual network::ConnectionID     getConnectionID() const { THROW_TODO; }
    virtual boost::system::error_code send( const network::Message& msg, boost::asio::yield_context& yield_ctx )
    {
        THROW_TODO;
    }
    virtual void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        THROW_TODO;
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

    class Activity
    {
        enum State
        {
            eSending,
            eReceiving,
            eReceived,
            TOTAL_STATES
        };

    public:
        using Ptr       = std::shared_ptr< Activity >;
        using PtrVector = std::vector< Ptr >;

        Activity( Plugin& plugin, ConversationBase& sender, network::Message&& requestMsg )
            : m_plugin( plugin )
            , m_sender( sender )
            , m_request( requestMsg )
        {
        }

        bool                                         isReceived() const { return m_state == eReceived; }
        const std::optional< network::ReceivedMsg >& getResponse() const { return m_responseOpt; }

        void update( std::optional< network::ReceivedMsg >& msgOpt )
        {
            switch( m_state )
            {
                case eSending:
                {
                    SPDLOG_TRACE( "Activity sending msg request: {}", m_request.getName() );

                    network::ConversationBase::RequestStack stack(
                        "steam::MSG_SteamDestroy_Request", m_plugin.shared_from_this(), m_sender.getConnectionID() );

                    const network::ReceivedMsg rMsg{ m_sender.getConnectionID(), m_request };

                    m_sender.send( rMsg );

                    m_state = eReceiving;
                }
                break;
                case eReceiving:
                {
                    if( msgOpt.has_value() )
                    {
                        const network::ReceivedMsg& msg = msgOpt.value();

                        if( !network::isRequest( msg.msg ) )
                        {
                            if( msg.msg.getID() == m_request.getID() + 1 )
                            {
                                SPDLOG_TRACE( "Activity received msg response: {}", m_request.getName() );
                                m_responseOpt.swap( msgOpt );
                                m_state = eReceived;
                            }
                        }
                    }
                }
                break;
                case eReceived:
                {
                    // do nothing
                }
                break;
                case TOTAL_STATES:
                default:
                {
                    THROW_RTE( "Invalid activity state" );
                }
                break;
            }
        }

    private:
        Plugin&                               m_plugin;
        ConversationBase&                     m_sender;
        State                                 m_state = eSending;
        network::Message                      m_request;
        std::optional< network::ReceivedMsg > m_responseOpt;
    };

    static Ptr makePlugin( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
    {
        auto pPlugin = std::make_shared< mega::service::Plugin >( pszConsoleLogLevel, pszFileLogLevel );
        // work around shared_from_this in constructor issues
        //pPlugin->m_executor.externalConversationInitiated( pPlugin );
        return pPlugin;
    }

    static void releasePlugin( Ptr& pPlugin )
    {
        // work around shared_from_this in constructor issues
        pPlugin->shutdown();
        //pPlugin->m_executor.conversationCompleted( pPlugin );
        pPlugin.reset();
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
                m_executor, m_executor.createConversationID( m_executor.getLeafSender().getConnectionID() ) );
            m_executor.conversationInitiated( m_pPlatform, m_executor.getLeafSender() );
        }
    }

    ~Plugin()
    {
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

    void shutdown()
    {
        SPDLOG_TRACE( "Plugin::shutdown" );
        
        using namespace network::platform;
        Activity::Ptr pActivity = std::make_shared< Activity >(
            *this, *m_pPlatform, MSG_PlatformDestroy_Request::make( getID(), getID(), MSG_PlatformDestroy_Request{} ) );

        m_activities.push_back( pActivity );

        while( !pActivity->isReceived() )
            update();

        m_activities.clear();
    }

    void update()
    {
        std::optional< network::ReceivedMsg > msgOpt;

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

        for( auto& pActivity : m_activities )
        {
            pActivity->update( msgOpt );
        }

        VERIFY_RTE_MSG( !msgOpt.has_value(), "Failed to dispatch message: " << msgOpt.value().msg.getName() );
    }

private:
    using NumThreadsType              = decltype( std::thread::hardware_concurrency() );
    NumThreadsType m_uiNumThreads     = std::thread::hardware_concurrency();
    short          m_daemonPortNumber = mega::network::MegaDaemonPort();

    boost::asio::io_context    m_ioContext;
    mega::service::Executor    m_executor;
    MessageChannel             m_channel;
    std::vector< std::thread > m_threads;

    Activity::PtrVector m_activities;

    Platform::Ptr m_pPlatform;
};

static Plugin::Ptr g_pPlugin;

} // namespace

} // namespace mega::service

void mp_initialise( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
{
    mega::service::g_pPlugin = mega::service::Plugin::makePlugin( pszConsoleLogLevel, pszFileLogLevel );
}

void mp_update()
{
    mega::service::g_pPlugin->update();
}

void mp_shutdown()
{
    mega::service::Plugin::releasePlugin( mega::service::g_pPlugin );
}

int mp_network_count()
{
    return 1;
}

const char* mp_network_name( int networkID )
{
    return "Single Player";
}

void mp_network_connect( int networkID )
{
}

void mp_network_disconnect()
{
}