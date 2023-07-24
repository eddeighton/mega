
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

#include "service/plugin/platform.hpp"
#include "service/plugin/player_network.hpp"
#include "service/plugin/plugin_state_machine.hpp"

#include "service/executor/clock.hpp"
#include "service/executor/executor.hpp"

#include "service/protocol/common/platform_state.hpp"

#include "common/assert_verify.hpp"
#include "common/stash.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

namespace mega::service
{

class Plugin : public network::LogicalThreadBase, public ProcessClock
{
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, network::ReceivedMessage ) >;

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
    virtual boost::system::error_code send( const network::Message& msg );
    virtual boost::system::error_code send( const network::Message& msg, boost::asio::yield_context& yield_ctx );

    // ProcessClock
    virtual void setActiveProject( const Project& project, U64 dbHashCode ) override;
    virtual void registerMPO( network::SenderRef sender ) override;
    virtual void unregisterMPO( network::SenderRef sender ) override;
    virtual void requestClock( network::LogicalThreadBase* pSender, MPO mpo, log::Range range ) override;

    // network::LogicalThreadBase
    virtual const network::LogicalThreadID& getID() const override;

    virtual network::Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override
    {
        THROW_TODO;
    }
    virtual void               run( boost::asio::yield_context& yield_ctx ) override { THROW_TODO; }
    virtual const std::string& getProcessName() const override { THROW_TODO; }
    virtual U64                getStackSize() const override { THROW_TODO; }
    virtual void               onDisconnect( network::Sender::Ptr pRequestResponseSender ) override { THROW_TODO; }
    virtual void               requestStarted( network::Sender::Ptr pRequestResponseSender ) override { ; }
    virtual void               requestCompleted() override { ; }

    virtual void                   receive( const network::ReceivedMessage& msg )   override
    {
        THROW_TODO;
    }
    /*
        void send( LogicalThreadBase& sender, network::Message&& requestMsg )
        {
            // SPDLOG_TRACE( "plugin::send: {}", requestMsg.getName() );
            const network::ReceivedMessage rMsg{ , requestMsg };
            sender.send( rMsg );
        }
        template < typename MsgType >
        void send( LogicalThreadBase& sender, MsgType&& msg )
        {
            send( sender, MsgType::make( getID(), sender.getID(), std::move( msg ) ) );
        }*/

    U64         database_hashcode() { return m_databaseHashcode; }
    const char* database() { return m_strDatabasePath.c_str(); }

    const Downstream* downstream()
    {
        tryRun();
        return m_stateMachine.getDownstream();
    }

    void upstream( float delta, void* pRange )
    {
        m_ct += delta;
        const network::ClockTick clockTick{ ++m_cycle, m_ct, delta };
        m_stateMachine.sendUpstream( clockTick );
    }

    void runOne();
    bool tryRun();
    void dispatch( const network::Message& msg );

    U64         network_count();
    const char* network_name( U64 networkID );
    void        network_connect( U64 networkID );
    void        network_disconnect();
    U64         network_current();

    void planet_create();
    void planet_destroy();
    bool planet_current();

private:
    network::LogicalThreadID m_conID;
    MessageChannel           m_channel;
    mega::service::Executor  m_executor;
    // Platform::Ptr                                m_pPlatform;
    // PlayerNetwork::Ptr                           m_pPlayerNetwork;
    // std::optional< network::PlatformState >      m_platformStateOpt;
    // std::optional< network::PlayerNetworkState > m_networkStateOpt;
    U64         m_databaseHashcode = 0U; // MUS BE zero for interop to do nothing
    std::string m_strDatabasePath;
    U64         m_memoryHashcode;
    // MemoryDescription                            m_memoryDescription;
    mega::TimeStamp m_cycle = 0;
    float           m_ct    = 0.0f;
    // float                        m_statusRate       = 1.0f;
    // bool                         m_bNetworkRequest  = false;
    // bool                         m_bPlatformRequest = false;
    // std::optional< float >       m_lastPlatformStatus;
    // std::optional< float >       m_lastNetworkStatus;
    PluginStateMachine< Plugin > m_stateMachine;
};
} // namespace mega::service

#endif // GUARD_2023_March_07_plugin
