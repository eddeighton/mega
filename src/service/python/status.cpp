
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

#include "service/network/log.hpp"

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

        using MPOTimeStampVec = std::vector< std::pair< mega::MPO, TimeStamp > >;
        using MPOVec          = std::vector< mega::MPO >;
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

        status.setMemory( m_pMemoryManager->getStatus() );
    }

    return status;
}

mega::reports::Container MPOLogicalThread::GetReport( const mega::reports::URL&                      url,
                                                      const std::vector< mega::reports::Container >& report,
                                                      boost::asio::yield_context&                    yield_ctx )
{
    SPDLOG_TRACE( "MPOLogicalThread::GetReport" );
    VERIFY_RTE( report.empty() );
    using namespace mega::reports;
    using namespace std::string_literals;
    Table table;
    table.m_rows.push_back( { Line{ "   Thread ID: "s }, Line{ getID() } } );
    MPOContext::getBasicReport( table );
    return table;
}

network::Status PythonRequestLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                       boost::asio::yield_context&           yield_ctx )
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

std::string PythonRequestLogicalThread::Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx )
{
    using ::           operator<<;
    std::ostringstream os;
    os << "Ping reached: " << common::ProcessID::get() << " got: " << strMsg.size() << " bytes";
    return os.str();
}

mega::reports::Container PythonRequestLogicalThread::GetReport( const mega::reports::URL&                      url,
                                                                const std::vector< mega::reports::Container >& report,
                                                                boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "PythonRequestLogicalThread::GetReport" );
    using namespace mega::reports;
    using namespace std::string_literals;
    reports::Branch branch{ { getID(), " "s, m_python.getProcessName() }, report };
    return branch;
}
} // namespace mega::service::python
