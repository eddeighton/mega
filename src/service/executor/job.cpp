
#include "job.hpp"

#include "service/executor.hpp"

namespace mega
{
namespace service
{

JobConversation::JobConversation( Executor& executor, const network::ConversationID& conversationID,
                                  mega::pipeline::Pipeline::Ptr  pPipeline,
                                  const network::ConversationID& rootConversationID )
    : ExecutorRequestConversation( executor, conversationID, conversationID.getConnectionID() )
    , m_pPipeline( pPipeline )
    , m_rootConversationID( rootConversationID )
{
    VERIFY_RTE( m_pPipeline );
}

void JobConversation::RootPipelineStartTask( const mega::pipeline::TaskDescriptor& task,
                                             boost::asio::yield_context&           yield_ctx )
{
    m_pYieldCtx = &yield_ctx;
    m_pPipeline->execute( task, *this, *this, *this );
}

// pipeline::DependencyProvider
EG_PARSER_INTERFACE* JobConversation::getParser() { return m_executor.m_pParser.get(); }

// pipeline::Progress
void JobConversation::onStarted( const std::string& strMsg )
{
    getLeafRequest( *m_pYieldCtx ).ExePipelineWorkProgress( strMsg );
}

void JobConversation::onProgress( const std::string& strMsg )
{
    getLeafRequest( *m_pYieldCtx ).ExePipelineWorkProgress( strMsg );
}

void JobConversation::onFailed( const std::string& strMsg )
{
    getLeafResponse( *m_pYieldCtx ).RootPipelineStartTask( network::PipelineResult( false, strMsg ) );
}

void JobConversation::onCompleted( const std::string& strMsg )
{
    getLeafResponse( *m_pYieldCtx ).RootPipelineStartTask( network::PipelineResult( true, strMsg ) );
}

// pipeline::Stash
task::FileHash JobConversation::getBuildHashCode( const boost::filesystem::path& filePath )
{
    return getLeafRequest( *m_pYieldCtx ).ExeGetBuildHashCode( filePath );
}
void JobConversation::setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode )
{
    getLeafRequest( *m_pYieldCtx ).ExeSetBuildHashCode( filePath, hashCode );
}
void JobConversation::stash( const boost::filesystem::path& file, task::DeterminantHash code )
{
    getLeafRequest( *m_pYieldCtx ).ExeStash( file, code );
}
bool JobConversation::restore( const boost::filesystem::path& file, task::DeterminantHash code )
{
    return getLeafRequest( *m_pYieldCtx ).ExeRestore( file, code );
}

void JobConversation::run( boost::asio::yield_context& yield_ctx )
{
    getLeafRequest( yield_ctx ).ExePipelineReadyForWork( m_rootConversationID );
}

} // namespace service
} // namespace mega