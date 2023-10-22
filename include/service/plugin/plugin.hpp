
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

#include "service/plugin/plugin_state_machine.hpp"

#include "service/executor/clock.hpp"
#include "service/executor/executor.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

namespace mega::service
{

class Plugin : public ProcessClock
{
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, network::Message ) >;

public:
    using Ptr = std::shared_ptr< Plugin >;

    // Plugin
    Plugin( boost::asio::io_context& ioContext, network::Log log, U64 uiNumThreads );
    ~Plugin();

    Plugin( const Plugin& )            = delete;
    Plugin( Plugin&& )                 = delete;
    Plugin& operator=( const Plugin& ) = delete;
    Plugin& operator=( Plugin&& )      = delete;

    void send( const network::Message& msg );

    // ProcessClock
    virtual void setActiveProject( const Project& project, U64 dbHashCode ) override;
    virtual void registerMPO( network::SenderRef sender ) override;
    virtual void unregisterMPO( network::SenderRef sender ) override;
    virtual void requestClock( network::LogicalThreadBase* pSender, MPO mpo, log::Range range ) override;
    virtual bool unrequestClock( network::LogicalThreadBase* pSender, MPO mpo ) override;
    virtual void requestMove( network::LogicalThreadBase* pSender, MPO mpo ) override;

    const network::LogicalThreadID& getLogicalThreadID() const { return m_logicalThreadID; }

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

private:
    network::LogicalThreadID     m_logicalThreadID;
    MessageChannel               m_channel;
    mega::service::Executor      m_executor;
    U64                          m_databaseHashcode = 0U; // MUST BE zero for interop to do nothing
    std::string                  m_strDatabasePath;
    mega::TimeStamp              m_cycle = 0;
    float                        m_ct    = 0.0f;
    PluginStateMachine< Plugin > m_stateMachine;
};
} // namespace mega::service

#endif // GUARD_2023_March_07_plugin
