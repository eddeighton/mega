
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

#include "report_server.hpp"
#include "http_logical_thread.hpp"

#include "request.hpp"

#include "service/mpo_context.hpp"

#include "service/network/logical_thread.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "log/log.hpp"

#include "mega/values/service/logical_thread_id.hpp"

#include "request.hpp"

#include "event/file_log.hpp"

#include <spdlog/spdlog.h>

#include <boost/filesystem.hpp>

#include <thread>

namespace mega::service::report
{

ReportServer::ReportServer( boost::asio::io_context& io_context, network::Log log, short daemonPortNumber, int iTimeoutSeconds,
                const boost::asio::ip::tcp::endpoint& httpEndPoint )
    : network::LogicalThreadManager( network::Node::makeProcessName( network::Node::Report ), io_context )
    , m_io_context( io_context )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf( std::move( log ), m_receiverChannel.getSender(), network::Node::Report, daemonPortNumber )
    , m_iTimeoutSeconds( iTimeoutSeconds )
    , m_httpEndPoint( httpEndPoint )
{
    m_receiverChannel.run( m_leaf.getLeafSender() );
    m_leaf.startup();
    m_megastructureInstallation = m_leaf.getMegastructureInstallation();
}

ReportServer::~ReportServer()
{
    m_receiverChannel.stop();
}

void ReportServer::shutdown()
{
    // TODO ?
}

network::LogicalThreadBase::Ptr ReportServer::joinLogicalThread( const network::Message& msg )
{
    return network::LogicalThreadBase::Ptr( new ReportRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
}

void ReportServer::createReport( boost::asio::ip::tcp::socket& socket )
{
    HTTPLogicalThread::Ptr pMPOLogicalThread
        = std::make_shared< HTTPLogicalThread >( *this, createLogicalThreadID(), socket );
    logicalthreadInitiated( pMPOLogicalThread );
}

} // namespace mega::service::report
