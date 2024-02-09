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

#include "request.hpp"

#include "log/log.hpp"

namespace mega::service
{
// network::project::Impl
network::Status DaemonRequestLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                       boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::LogicalThreadID > logicalthreads;
        for( const auto& [ id, pCon ] : m_daemon.m_logicalthreads )
        {
            if( id != getID() )
            {
                logicalthreads.push_back( id );
            }
        }
        status.setLogicalThreadID( logicalthreads );
        status.setMachineID( m_daemon.m_machineID );
        status.setDescription( m_daemon.m_strProcessName );
        status.setLogFile( m_daemon.getLog().logFile.string() );
    }

    return status;
}

std::string DaemonRequestLogicalThread::Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping reached: " << common::ProcessID::get() << " got: " << strMsg.size() << " bytes";
    return os.str();
}

mega::reports::Container DaemonRequestLogicalThread::GetReport( const mega::reports::URL&                      url,
                                                                const std::vector< mega::reports::Container >& report,
                                                                boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::GetReport" );
    using namespace mega::reports;
    using namespace std::string_literals;

    reports::Branch daemon{ { m_daemon.getProcessName() } };

    m_daemon.getGeneralStatusReport( url, daemon );

    for( const auto& child : report )
    {
        daemon.m_elements.push_back( child );
    }

    return daemon;
}
} // namespace mega::service
