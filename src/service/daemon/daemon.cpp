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

#include "mega/values/service/url.hpp"

#include "service/network/logical_thread.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "log/log.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/asio/ip/host_name.hpp>

#include <iostream>

namespace mega::service
{

////////////////////////////////////////////////////////////////

class DaemonEnrole : public DaemonRequestLogicalThread
{
    std::promise< void >& m_promise;

public:
    DaemonEnrole( Daemon& daemon, std::promise< void >& promise )
        : DaemonRequestLogicalThread( daemon, daemon.createLogicalThreadID() )
        , m_promise( promise )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        m_daemon.m_machineID = getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleDaemon();
        //m_daemon.setActiveProject( getRootRequest< network::project::Request_Encoder >( yield_ctx ).GetProject() );

        std::ostringstream os;
        os << network::Node( network::Node::Daemon ) << " " << m_daemon.m_machineID;
        common::ProcessID::setDescription( os.str().c_str() );

        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
    }
};

////////////////////////////////////////////////////////////////
// Daemon
Daemon::Daemon( boost::asio::io_context& ioContext, network::Log log, const std::string& strRootIP,
                short rootPortNumber, short daemonPortNumber )
    : network::LogicalThreadManager( network::Node::makeProcessName( network::Node::Daemon ), ioContext )
    , m_log( log )
    , m_rootClient( ioContext, *this, strRootIP, rootPortNumber )
    , m_server( ioContext, *this, daemonPortNumber )
{
    m_server.waitForConnection();

    // immediately enrole
    {
        std::promise< void > promise;
        std::future< void >  future = promise.get_future();
        logicalthreadInitiated( std::make_shared< DaemonEnrole >( *this, promise ) );
        using namespace std::chrono_literals;
        while( std::future_status::timeout == future.wait_for( 0s ) )
        {
            ioContext.run_one();
        }
    }
}

void Daemon::getGeneralStatusReport( const mega::reports::URL& url, mega::reports::Branch& report )
{
    using namespace mega::reports;
    using namespace std::string_literals;

    Table table;
    // clang-format off
    table.m_rows.push_back( { Line{ "    Hostname: "s }, Line{ boost::asio::ip::host_name() } } );
    table.m_rows.push_back( { Line{ "     Process: "s }, Line{ m_strProcessName } } );
    table.m_rows.push_back( { Line{ "          IP: "s }, Line{ m_server.getEndPoint().address().to_string() } } );
    table.m_rows.push_back( { Line{ "        PORT: "s }, Line{ std::to_string( m_server.getEndPoint().port() ) } } );
    table.m_rows.push_back( { Line{ "  Machine ID: "s }, Line{ m_machineID } } );
    table.m_rows.push_back( { Line{ "    Log File: "s }, Line{ m_log.logFile, makeFileURL( url, m_log.logFile ) } } );
    // clang-format on

    report.m_elements.push_back( table );
}

void Daemon::onLeafDisconnect( mega::runtime::MP leafMP )
{
    m_server.unLabelConnection( leafMP );

    // onDisconnect();

    class DaemonLeafDisconnect : public DaemonRequestLogicalThread
    {
        mega::runtime::MP m_leafMP;

    public:
        DaemonLeafDisconnect( Daemon& daemon, mega::runtime::MP leafMP )
            : DaemonRequestLogicalThread( daemon, daemon.createLogicalThreadID() )
            , m_leafMP( leafMP )
        {
        }
        void run( boost::asio::yield_context& yield_ctx )
        {
            getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleLeafDisconnect( m_leafMP );
            SPDLOG_TRACE( "DaemonLeafDisconnect {}", m_leafMP );
        }
    };
    logicalthreadInitiated( std::make_shared< DaemonLeafDisconnect >( *this, leafMP ) );
}

void Daemon::shutdown()
{
    m_rootClient.stop();
    m_server.stop();
}

network::LogicalThreadBase::Ptr Daemon::joinLogicalThread( const network::Message& msg )
{
    return network::LogicalThreadBase::Ptr( new DaemonRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
}

} // namespace mega::service
