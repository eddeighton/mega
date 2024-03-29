
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
#include "mpo_logical_thread.hpp"

#include "log/log.hpp"

namespace mega::service::python
{

// network::project::Impl

network::Status MPOLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                             boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "MPOLogicalThread::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::LogicalThreadID > logicalthreads;
        {
            for( const auto& id : m_python.reportLogicalThreads() )
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
        status.setDescription( m_python.getProcessName() );

        using MPOTimeStampVec = std::vector< std::pair< runtime::MPO, runtime::TimeStamp > >;
        if( const auto& reads = m_lockTracker.getReads(); !reads.empty() )
            status.setReads( MPOTimeStampVec{ reads.begin(), reads.end() } );
        if( const auto& writes = m_lockTracker.getWrites(); !writes.empty() )
            status.setWrites( MPOTimeStampVec{ writes.begin(), writes.end() } );

        {
            std::ostringstream os;
            os << "Python: " << getLog().getTimeStamp();
        }

        status.setLogIterator( getLog().getIterator() );
        status.setLogFolder( getLog().getLogFolderPath().string() );
    }

    return status;
}

Report MPOLogicalThread::GetReport( const URL& url, const std::vector< Report >& report, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOLogicalThread::GetReport" );
    VERIFY_RTE( report.empty() );
    using namespace std::string_literals;
    Table table;
    table.m_rows.push_back( { Line{ "   Thread ID: "s }, Line{ getID() } } );
    runtime::MPOContext::getBasicReport( url, table );
    return table;
}

network::Status PythonRequestLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                       boost::asio::yield_context& )
{
    SPDLOG_TRACE( "PythonRequestLogicalThread::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::LogicalThreadID > logicalthreads;
        {
            for( const auto& id : m_python.reportLogicalThreads() )
            {
                if( id != getID() )
                {
                    logicalthreads.push_back( id );
                }
            }
        }
        status.setLogicalThreadID( logicalthreads );
        status.setDescription( m_python.getProcessName() );
    }

    return status;
}

std::string PythonRequestLogicalThread::Ping( const std::string& strMsg, boost::asio::yield_context& )
{
    std::ostringstream os;
    os << "Ping reached: " << common::ProcessID::get() << " got: " << strMsg.size() << " bytes";
    return os.str();
}

Report PythonRequestLogicalThread::GetReport( const URL&                   url,
                                              const std::vector< Report >& report,
                                              boost::asio::yield_context& )
{
    SPDLOG_TRACE( "PythonRequestLogicalThread::GetReport" );
    using namespace std::string_literals;
    Branch branch{ { getID(), " "s, m_python.getProcessName() }, report };
    return branch;
}
} // namespace mega::service::python
