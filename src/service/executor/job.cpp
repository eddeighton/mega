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

#include "service/executor.hpp"

#include "service/protocol/model/stash.hxx"

#include "service/network/log.hpp"

namespace mega::service
{

std::vector< network::ConversationID >
ExecutorRequestConversation::JobStart( const mega::utilities::ToolChain&                            toolChain,
                                       const mega::pipeline::Configuration&                         configuration,
                                       const network::ConversationID&                               rootConversationID,
                                       const std::vector< std::vector< network::ConversationID > >& resultJobs,
                                       boost::asio::yield_context&                                  yield_ctx )
{
    mega::pipeline::Pipeline::Ptr pPipeline;
    {
        std::ostringstream osLog;
        pPipeline = pipeline::Registry::getPipeline( toolChain, configuration, osLog );
        if ( !pPipeline )
        {
            SPDLOG_ERROR( "PIPELINE: Executor: Failed to load pipeline: {}", configuration.getPipelineID() );
            THROW_RTE( "Executor: Failed to load pipeline: " << configuration.get() );
        }
        else
        {
            SPDLOG_TRACE( "PIPELINE: {}", osLog.str() );
        }
    }

    std::vector< network::ConversationID > jobIDs;
    std::vector< JobConversation::Ptr >    jobs;
    for ( int i = 0; i < m_executor.getNumThreads(); ++i )
    {
        JobConversation::Ptr pJob = std::make_shared< JobConversation >(
            m_executor, m_executor.createConversationID( m_executor.getLeafSender().getConnectionID() ), pPipeline,
            rootConversationID );
        jobIDs.push_back( pJob->getID() );
        jobs.push_back( pJob );
    }

    for ( JobConversation::Ptr pJob : jobs )
    {
        m_executor.conversationInitiated( pJob, m_executor.getLeafSender() );
    }

    return jobIDs;
}

JobConversation::JobConversation( Executor& executor, const network::ConversationID& conversationID,
                                  mega::pipeline::Pipeline::Ptr  pPipeline,
                                  const network::ConversationID& rootConversationID )
    : ExecutorRequestConversation( executor, conversationID, std::nullopt )
    , m_pPipeline( pPipeline )
    , m_rootConversationID( rootConversationID )
{
    VERIFY_RTE( m_pPipeline );
}

network::Message JobConversation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    // base already inherits job interface
    return ExecutorRequestConversation::dispatchRequest( msg, yield_ctx );
}

network::PipelineResult JobConversation::JobStartTask( const mega::pipeline::TaskDescriptor& task,
                                                       boost::asio::yield_context&           yield_ctx )
{
    m_resultOpt.reset();
    m_pPipeline->execute( task, *this, *this, *this );
    VERIFY_RTE( m_resultOpt.has_value() );
    return m_resultOpt.value();
}

// pipeline::DependencyProvider
EG_PARSER_INTERFACE* JobConversation::getParser() { return m_executor.m_pParser.get(); }

// pipeline::Progress

void JobConversation::onStarted( const std::string& strMsg )
{
    getRootRequest< network::job::Request_Encoder >( *m_pYieldCtx ).JobProgress( strMsg );
}

void JobConversation::onProgress( const std::string& strMsg )
{
    getRootRequest< network::job::Request_Encoder >( *m_pYieldCtx ).JobProgress( strMsg );
}

void JobConversation::onFailed( const std::string& strMsg )
{
    m_resultOpt = network::PipelineResult( false, strMsg, {} );
}

void JobConversation::onCompleted( const std::string& strMsg )
{
    m_resultOpt = network::PipelineResult( true, strMsg, {} );
}

// pipeline::Stash
task::FileHash JobConversation::getBuildHashCode( const boost::filesystem::path& filePath )
{
    return getRootRequest< network::stash::Request_Encoder >( *m_pYieldCtx ).BuildGetHashCode( filePath );
}
void JobConversation::setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode )
{
    getRootRequest< network::stash::Request_Encoder >( *m_pYieldCtx ).BuildSetHashCode( filePath, hashCode );
}
void JobConversation::stash( const boost::filesystem::path& file, task::DeterminantHash code )
{
    getRootRequest< network::stash::Request_Encoder >( *m_pYieldCtx ).StashStash( file, code );
}
bool JobConversation::restore( const boost::filesystem::path& file, task::DeterminantHash code )
{
    return getRootRequest< network::stash::Request_Encoder >( *m_pYieldCtx ).StashRestore( file, code );
}

void JobConversation::run( boost::asio::yield_context& yield_ctx )
{
    m_pYieldCtx = &yield_ctx;
    getRootRequest< network::job::Request_Encoder >( yield_ctx ).JobReadyForWork( m_rootConversationID );
}

} // namespace mega::service
