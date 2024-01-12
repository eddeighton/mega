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

#include "job.hpp"

#include "root.hpp"
#include "pipeline.hpp"

#include "spdlog/stopwatch.h"

namespace mega::service
{

RootJobLogicalThread::RootJobLogicalThread( Root& root, const network::LogicalThreadID& logicalthreadID )
    : RootRequestLogicalThread( root, logicalthreadID )
{
}

network::Message RootJobLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                               boost::asio::yield_context& yield_ctx )
{
    return RootRequestLogicalThread::dispatchInBoundRequest( msg, yield_ctx );
}

void RootJobLogicalThread::JobReadyForWork( const network::LogicalThreadID& rootLogicalThreadID,
                                            boost::asio::yield_context&     yield_ctx )
{
    std::shared_ptr< RootPipelineLogicalThread > pCoordinator = std::dynamic_pointer_cast< RootPipelineLogicalThread >(
        m_root.findExistingLogicalThread( rootLogicalThreadID ) );
    VERIFY_RTE( pCoordinator );

    {
        auto              exeRequest = getExeRequest< network::job::Request_Encoder >( yield_ctx );
        spdlog::stopwatch sw;
        while( true )
        {
            const mega::pipeline::TaskDescriptor task = pCoordinator->getTask( yield_ctx );
            if( task == mega::pipeline::TaskDescriptor() )
            {
                pCoordinator->completeTask( mega::pipeline::TaskDescriptor(), true, yield_ctx );
                break;
            }
            else
            {
                try
                {
                    sw.reset();
                    pipeline::PipelineResult result = exeRequest.JobStartTask( task );
                    network::logLinesSuccessFail( result.getMessage(), result.getSuccess(),
                                                  std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
                    pCoordinator->completeTask( task, result.getSuccess(), yield_ctx );
                }
                catch( std::exception& ex )
                {
                    network::logLinesWarn( task.getName(), ex.what() );
                    pCoordinator->completeTask( task, false, yield_ctx );
                }
            }
        }
    }
}

void RootJobLogicalThread::JobProgress( const std::string& message, boost::asio::yield_context& yield_ctx )
{
    network::logLinesInfo( message );
}

} // namespace mega::service
