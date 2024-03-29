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
#include "service/remote_memory_manager.hpp"

namespace mega::service
{

// network::project::Impl
network::Status LeafRequestLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                     boost::asio::yield_context& )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::LogicalThreadID > logicalthreads;
        {
            for( const auto& [ id, pCon ] : m_leaf.m_logicalthreads )
            {
                if( id != getID() )
                {
                    logicalthreads.push_back( id );
                }
            }
        }
        status.setLogicalThreadID( logicalthreads );
        status.setMP( m_leaf.m_mp );
        std::ostringstream os;
        os << m_leaf.getProcessName() << " of type: " << m_leaf.m_nodeType;
        status.setDescription( os.str() );

        status.setLogFile( m_leaf.getLog().logFile.string() );

        if( auto program = m_leaf.getRuntime().getProgram(); !program.empty() )
        {
            status.setProgram( program );
        }
    }

    return status;
}

std::string LeafRequestLogicalThread::Ping( const std::string& strMsg, boost::asio::yield_context& )
{
    std::ostringstream os;
    os << "Ping reached: " << common::ProcessID::get() << " got: " << strMsg.size() << " bytes";
    return os.str();
}

Report
LeafRequestLogicalThread::GetReport( const URL& url, const std::vector< Report >& report, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::GetReport" );
    using namespace std::string_literals;

    Branch leaf{ { m_leaf.getProcessName() } };

    m_leaf.getGeneralStatusReport( url, leaf );

    for( const auto& child : report )
    {
        leaf.m_elements.push_back( child );
    }

    return leaf;
}
} // namespace mega::service
