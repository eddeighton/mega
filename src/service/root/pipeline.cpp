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

#include "pipeline.hpp"

#include "root.hpp"

#include "service/protocol/model/job.hxx"

#include "spdlog/stopwatch.h"

namespace mega::service
{

RootPipelineConversation::RootPipelineConversation( Root&                          root,
                                                    const network::ConversationID& conversationID,
                                                    const network::ConnectionID&   originatingConnectionID )
    : RootRequestConversation( root, conversationID, originatingConnectionID )
    , m_taskReady( root.getIOContext(), CHANNEL_SIZE )
    , m_taskComplete( root.getIOContext(), CHANNEL_SIZE )
{
}

network::Message RootPipelineConversation::dispatchRequest( const network::Message&     msg,
                                                            boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::pipeline::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return RootRequestConversation::dispatchRequest( msg, yield_ctx );
}

mega::pipeline::PipelineResult RootPipelineConversation::PipelineRun( const mega::pipeline::Configuration& configuration,
                                                                     boost::asio::yield_context&          yield_ctx )
{
    if ( !m_root.m_megastructureInstallationOpt.has_value() )
        m_root.m_megastructureInstallationOpt = m_root.getMegastructureInstallation();
    VERIFY_RTE_MSG(
        m_root.m_megastructureInstallationOpt.has_value(), "Megastructure Installation Toolchain unspecified" );
    const auto toolChain = m_root.m_megastructureInstallationOpt.value().getToolchain();

    spdlog::stopwatch             sw;
    mega::pipeline::Pipeline::Ptr pPipeline;
    {
        std::ostringstream osLog;
        pPipeline = pipeline::Registry::getPipeline( toolChain, configuration, osLog );
        if ( !pPipeline )
        {
            SPDLOG_ERROR( "Failed to load pipeline: {}", configuration.getPipelineID() );
            THROW_RTE( "Root: Failed to load pipeline: " << configuration.get() );
        }
        else
        {
            SPDLOG_INFO( "{}", osLog.str() );
        }
    }

    m_root.m_buildHashCodes.reset();

    // acquire jobs
    {
        const std::vector< network::ConversationID > jobs
            = getExeBroadcastRequest< network::job::Request_Encoder >( yield_ctx )
                  .JobStart( toolChain, configuration, getID(), {} );
        for ( const network::ConversationID& id : jobs )
            m_jobs.insert( id );
    }

    if ( m_jobs.empty() )
    {
        SPDLOG_WARN( "Failed to find executors for pipeline: {}", configuration.getPipelineID() );
        THROW_RTE( "Root: Failed to find executors for pipeline" );
    }
    SPDLOG_TRACE( "Found {} jobs for pipeline {}", m_jobs.size(), configuration.getPipelineID() );

    mega::pipeline::Schedule                   schedule = pPipeline->getSchedule( *this, *this );
    std::set< mega::pipeline::TaskDescriptor > scheduledTasks, activeTasks;
    bool                                       bScheduleFailed = false;
    {
        while ( !schedule.isComplete() && !bScheduleFailed )
        {
            for ( const mega::pipeline::TaskDescriptor& task : schedule.getReady() )
            {
                VERIFY_RTE( task != mega::pipeline::TaskDescriptor() );
                if ( activeTasks.size() < CHANNEL_SIZE )
                {
                    if ( !scheduledTasks.count( task ) )
                    {
                        scheduledTasks.insert( task );
                        VERIFY_RTE( activeTasks.insert( task ).second );
                        m_taskReady.async_send( boost::system::error_code(), task, yield_ctx );
                    }
                }
                else
                    break;
            }

            while ( !activeTasks.empty() )
            {
                const TaskCompletion taskCompletion = m_taskComplete.async_receive( yield_ctx );
                VERIFY_RTE( activeTasks.erase( taskCompletion.task ) == 1U );
                if ( taskCompletion.bSuccess )
                {
                    schedule.complete( taskCompletion.task );
                }
                else
                {
                    SPDLOG_WARN( "Pipeline failed at task: {}", taskCompletion.task.getName() );
                    bScheduleFailed = true;
                    break;
                }
                if ( !m_taskComplete.ready() )
                    break;
            }
        }
    }

    // close out remaining active tasks
    while ( !activeTasks.empty() )
    {
        TaskCompletion taskCompletion = m_taskComplete.async_receive( yield_ctx );
        VERIFY_RTE( activeTasks.erase( taskCompletion.task ) == 1U );
        if ( taskCompletion.bSuccess )
        {
            schedule.complete( taskCompletion.task );
        }
        else
        {
            SPDLOG_WARN( "Pipeline failed at task: {}", taskCompletion.task.getName() );
        }
    }

    // send termination task to each job
    for ( const network::ConversationID& jobID : m_jobs )
    {
        m_taskReady.async_send( boost::system::error_code(), mega::pipeline::TaskDescriptor(), yield_ctx );
    }
    for ( const network::ConversationID& jobID : m_jobs )
    {
        m_taskComplete.async_receive( yield_ctx );
    }

    {
        std::ostringstream os;
        if ( bScheduleFailed )
        {
            SPDLOG_WARN( "FAILURE: Pipeline {} failed in: {}ms", configuration.getPipelineID(),
                         std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
            os << "Pipeline: " << configuration.getPipelineID() << " failed";
            return pipeline::PipelineResult( false, os.str(), m_root.m_buildHashCodes.get() );
        }
        else
        {
            SPDLOG_INFO( "SUCCESS: Pipeline {} succeeded in: {}", configuration.getPipelineID(),
                         std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
            os << "Pipeline: " << configuration.getPipelineID() << " succeeded";
            return pipeline::PipelineResult( true, os.str(), m_root.m_buildHashCodes.get() );
        }
    }
}

} // namespace mega::service
