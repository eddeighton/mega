
#include "job.hpp"

#include "service/executor.hpp"

namespace mega
{
namespace service
{

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
    return ExecutorRequestConversation::dispatchRequest( msg, yield_ctx );
}

/*
void JobConversation::RootPipelineStartTask( const mega::pipeline::TaskDescriptor& task,
                                             boost::asio::yield_context&           yield_ctx )
{
    m_pYieldCtx = &yield_ctx;
    m_pPipeline->execute( task, *this, *this, *this );
}
*/

// pipeline::DependencyProvider
EG_PARSER_INTERFACE* JobConversation::getParser() { return m_executor.m_pParser.get(); }

// pipeline::Progress

void JobConversation::onStarted( const std::string& strMsg )
{
    // getLeafRequest( *m_pYieldCtx ).ExePipelineWorkProgress( strMsg );
}

void JobConversation::onProgress( const std::string& strMsg )
{
    // getLeafRequest( *m_pYieldCtx ).ExePipelineWorkProgress( strMsg );
}

void JobConversation::onFailed( const std::string& strMsg )
{
    // getLeafResponse( *m_pYieldCtx ).RootPipelineStartTask( network::PipelineResult( false, strMsg, {} ) );
}

void JobConversation::onCompleted( const std::string& strMsg )
{
    // getLeafResponse( *m_pYieldCtx ).RootPipelineStartTask( network::PipelineResult( true, strMsg, {} ) );
}

// pipeline::Stash
task::FileHash JobConversation::getBuildHashCode( const boost::filesystem::path& filePath )
{
    // return getLeafRequest( *m_pYieldCtx ).ExeGetBuildHashCode( filePath );
    THROW_RTE( "TODO" );
}
void JobConversation::setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode )
{
    // getLeafRequest( *m_pYieldCtx ).ExeSetBuildHashCode( filePath, hashCode );
    THROW_RTE( "TODO" );
}
void JobConversation::stash( const boost::filesystem::path& file, task::DeterminantHash code )
{
    // getLeafRequest( *m_pYieldCtx ).ExeStash( file, code );
    THROW_RTE( "TODO" );
}
bool JobConversation::restore( const boost::filesystem::path& file, task::DeterminantHash code )
{
    // return getLeafRequest( *m_pYieldCtx ).ExeRestore( file, code );
    THROW_RTE( "TODO" );
}

void JobConversation::run( boost::asio::yield_context& yield_ctx )
{
    // getLeafRequest( yield_ctx ).ExePipelineReadyForWork( m_rootConversationID );
    THROW_RTE( "TODO" );
}

} // namespace service
} // namespace mega