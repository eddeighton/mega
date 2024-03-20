
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
#include "http_logical_thread.hpp"

#include "log/log.hpp"

namespace mega::service::report
{

// network::project::Impl

network::Status HTTPLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                              boost::asio::yield_context& )
{
    SPDLOG_TRACE( "HTTPLogicalThread::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::LogicalThreadID > logicalthreads;
        {
            for( const auto& id : m_reportServer.reportLogicalThreads() )
            {
                if( id != getID() )
                {
                    logicalthreads.push_back( id );
                }
            }
        }
        status.setLogicalThreadID( logicalthreads );
        if( m_mpo.has_value() )
            status.setMPO( m_mpo.value() );
        status.setDescription( m_reportServer.getProcessName() );

        using MPOTimeStampVec = std::vector< std::pair< runtime::MPO, runtime::TimeStamp > >;
        if( const auto& reads = m_lockTracker.getReads(); !reads.empty() )
            status.setReads( MPOTimeStampVec{ reads.begin(), reads.end() } );
        if( const auto& writes = m_lockTracker.getWrites(); !writes.empty() )
            status.setWrites( MPOTimeStampVec{ writes.begin(), writes.end() } );

        {
            std::ostringstream os;
            os << "Report: " << getLog().getTimeStamp();
        }

        status.setLogIterator( getLog().getIterator() );
        status.setLogFolder( getLog().getLogFolderPath().string() );
    }

    return status;
}

Report HTTPLogicalThread::GetReport( const URL& url, const std::vector< Report >&, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "HTTPLogicalThread::GetReport" );
    using namespace std::string_literals;
    Table table;
    table.m_rows.push_back( { Line{ "   Thread ID: "s }, Line{ getID() } } );
    runtime::MPOContext::getBasicReport( url, table );
    return table;
}

network::Status ReportRequestLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                       boost::asio::yield_context& )
{
    SPDLOG_TRACE( "ReportRequestLogicalThread::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::LogicalThreadID > logicalthreads;
        {
            for( const auto& id : m_reportServer.reportLogicalThreads() )
            {
                if( id != getID() )
                {
                    logicalthreads.push_back( id );
                }
            }
        }
        status.setLogicalThreadID( logicalthreads );
        status.setDescription( m_reportServer.getProcessName() );
        status.setMP( m_reportServer.getMP() );
    }

    return status;
}

std::string ReportRequestLogicalThread::Ping( const std::string& strMsg, boost::asio::yield_context& )
{
    std::ostringstream os;
    os << "Ping reached: " << common::ProcessID::get() << " got: " << strMsg.size() << " bytes";
    return os.str();
}

Report
ReportRequestLogicalThread::GetReport( const URL&, const std::vector< Report >& report, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "ReportRequestLogicalThread::GetReport" );
    Branch branch{
        { getID(), m_reportServer.getProcessName(), m_reportServer.getHTTPEndPoint().address().to_string() }, report };
    return branch;
}

} // namespace mega::service::report
