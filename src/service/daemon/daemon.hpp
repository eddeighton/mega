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

#ifndef DAEMON_25_MAY_2022
#define DAEMON_25_MAY_2022

#include "service/network/client.hpp"
#include "service/network/server.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "log/log.hpp"

#include "service/network/network.hpp"

#include <boost/asio/io_context.hpp>

namespace mega::service
{

class Daemon : public network::LogicalThreadManager
{
    friend class DaemonRequestLogicalThread;
    friend class DaemonEnrole;

public:
    Daemon( boost::asio::io_context& ioContext, network::Log log, const std::string& strRootIP,
            short rootPortNumber   = mega::network::MegaRootPort(),
            short daemonPortNumber = mega::network::MegaDaemonPort() );

    void shutdown();

    // network::LogicalThreadManager
    virtual network::LogicalThreadBase::Ptr joinLogicalThread( const network::Message& msg );

    void getGeneralStatusReport( const URL& url, Branch& report );

    const auto& getLog() const { return m_log; }

private:
    void onLeafDisconnect( mega::runtime::MP mp );

    network::Log       m_log;
    network::Client    m_rootClient;
    network::Server    m_server;
    runtime::MachineID m_machineID;
};

} // namespace mega::service

#endif // DAEMON_25_MAY_2022
