
#include "job.hpp"

#include "root.hpp"
#include "pipeline.hpp"

#include "spdlog/stopwatch.h"

namespace mega
{
namespace service
{

RootJobConversation::RootJobConversation( Root&                          root,
                                          const network::ConversationID& conversationID,
                                          const network::ConnectionID&   originatingConnectionID )
    : RootRequestConversation( root, conversationID, originatingConnectionID )
{
}

network::Message RootJobConversation::dispatchRequest( const network::Message&     msg,
                                                       boost::asio::yield_context& yield_ctx )
{
    return RootRequestConversation::dispatchRequest( msg, yield_ctx );
}

void RootJobConversation::JobReadyForWork( const network::ConversationID& rootConversationID,
                                           boost::asio::yield_context&    yield_ctx )
{
    std::shared_ptr< RootPipelineConversation > pCoordinator = std::dynamic_pointer_cast< RootPipelineConversation >(
        m_root.findExistingConversation( rootConversationID ) );
    VERIFY_RTE( pCoordinator );

    {
        auto exeRequest = getExeRequest< network::job::Request_Encoder >( yield_ctx );
        spdlog::stopwatch sw;
        while ( true )
        {
            const mega::pipeline::TaskDescriptor task = pCoordinator->getTask( yield_ctx );
            if ( task == mega::pipeline::TaskDescriptor() )
            {
                pCoordinator->completeTask( mega::pipeline::TaskDescriptor(), true, yield_ctx );
                break;
            }
            else
            {
                try
                {
                    sw.reset();
                    network::PipelineResult result = exeRequest.JobStartTask( task );
                    network::logLinesSuccessFail( result.getMessage(), result.getSuccess(),
                                                  std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
                    pCoordinator->completeTask( task, result.getSuccess(), yield_ctx );
                }
                catch ( std::exception& ex )
                {
                    network::logLinesWarn( task.getName(), ex.what() );
                    pCoordinator->completeTask( task, false, yield_ctx );
                }
            }
        }
    }
}

void RootJobConversation::JobProgress( const std::string& message, boost::asio::yield_context& yield_ctx )
{
    network::logLinesInfo( message );
}

} // namespace service
} // namespace mega
