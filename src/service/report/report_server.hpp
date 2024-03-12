
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

#ifndef GUARD_2023_October_17
#define GUARD_2023_October_17

#include "service/leaf.hpp"

#include "service/network/client.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "service/network/sender_factory.hpp"
#include "service/network/receiver_channel.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <memory>
#include <vector>
#include <thread>
#include <functional>

namespace mega::service::report
{

class HTTPLogicalThread;

class ReportServer : public network::LogicalThreadManager
{
    friend class PythonRequestLogicalThread;

public:
    ReportServer( boost::asio::io_context& io_context, network::Log log, short daemonPortNumber, int iTimeoutSeconds,
            const boost::asio::ip::tcp::endpoint& httpEndPoint );
    ~ReportServer();

    void shutdown();

    // network::LogicalThreadManager
    virtual network::LogicalThreadBase::Ptr joinLogicalThread( const network::Message& msg );

    network::Sender::Ptr getLeafSender() { return m_leaf.getLeafSender(); }
    runtime::MP          getMP() const { return m_leaf.getMP(); }

    int                                   getTimeoutSeconds() const { return m_iTimeoutSeconds; }
    const boost::asio::ip::tcp::endpoint& getHTTPEndPoint() const { return m_httpEndPoint; }
    const MegastructureInstallation&      getMegastructureInstallation() const { return m_megastructureInstallation; }

    void createReport( boost::asio::ip::tcp::socket& socket );

    boost::asio::io_context& getIOContext() const { return m_ioContext; }

private:
    network::Log                   m_log;
    boost::asio::io_context&       m_io_context;
    network::ReceiverChannel       m_receiverChannel;
    Leaf                           m_leaf;
    int                            m_iTimeoutSeconds;
    boost::asio::ip::tcp::endpoint m_httpEndPoint;
    MegastructureInstallation      m_megastructureInstallation;
};
} // namespace mega::service::report

#endif // GUARD_2023_October_17
