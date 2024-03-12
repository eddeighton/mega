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

#include "service/executor/request.hpp"

#include "log/log.hpp"

#include "service/executor/executor.hpp"

namespace mega::service
{

// network::project::Impl
network::Status ExecutorRequestLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                         boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestLogicalThread::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::LogicalThreadID > logicalthreads;
        {
            for( const auto& id : m_executor.reportLogicalThreads() )
            {
                if( id != getID() )
                {
                    logicalthreads.push_back( id );
                }
            }
        }
        status.setLogicalThreadID( logicalthreads );

        std::ostringstream os;
        os << m_executor.getProcessName() << " with threads: " << m_executor.getNumThreads();
        status.setDescription( os.str() );
    }

    return status;
}

std::string ExecutorRequestLogicalThread::Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping reached: " << common::ProcessID::get() << " got: " << strMsg.size() << " bytes";
    return os.str();
}

Report ExecutorRequestLogicalThread::GetReport( const URL&           url,
                                                const std::vector< Report >& report,
                                                boost::asio::yield_context&  yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestLogicalThread::GetReport" );
    using namespace std::string_literals;

    Branch exe{ { m_executor.getProcessName() } };

    m_executor.getGeneralStatusReport( url, exe );

    for( const auto& child : report )
    {
        exe.m_elements.push_back( child );
    }

    return exe;
}
} // namespace mega::service
