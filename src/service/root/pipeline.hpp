#ifndef PIPELINE_22_SEPT_2022
#define PIPELINE_22_SEPT_2022

#include "request.hpp"

#include "service/network/log.hpp"

#include "service/protocol/model/pipeline.hxx"

namespace mega
{
namespace service
{
class Root;

class RootPipelineConversation : public RootRequestConversation,
                                 public network::pipeline::Impl,
                                 public pipeline::Progress,
                                 public pipeline::Stash
{
    std::set< network::ConversationID > m_jobs;

    using TaskChannel = boost::asio::experimental::concurrent_channel< void(
        boost::system::error_code, mega::pipeline::TaskDescriptor ) >;
    TaskChannel m_taskReady;

    struct TaskCompletion
    {
        network::ConversationID        jobID;
        mega::pipeline::TaskDescriptor task;
        bool                           bSuccess;
    };
    using TaskCompletionChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, TaskCompletion ) >;
    TaskCompletionChannel m_taskComplete;

    static constexpr mega::U32 CHANNEL_SIZE = 256;

public:
    RootPipelineConversation( Root&                          root,
                              const network::ConversationID& conversationID,
                              const network::ConnectionID&   originatingConnectionID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    // pipeline::Stash implement pipeline::Stash so that can create schedule - not actually used
    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }
    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }
    virtual void stash( const boost::filesystem::path& filePath, task::DeterminantHash determinant ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }
    virtual bool restore( const boost::filesystem::path& filePath, task::DeterminantHash determinant ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }

    // pipeline::Progress
    virtual void onStarted( const std::string& strMsg ) override { onProgress( strMsg ); }
    virtual void onProgress( const std::string& strMsg ) override { network::logLinesInfo( strMsg ); }
    virtual void onFailed( const std::string& strMsg ) override { onProgress( strMsg ); }
    virtual void onCompleted( const std::string& strMsg ) override { onProgress( strMsg ); }

    // network::pipeline::Impl
    virtual mega::network::PipelineResult PipelineRun( const mega::pipeline::Configuration& configuration,
                                                       boost::asio::yield_context&          yield_ctx ) override;

    
    mega::pipeline::TaskDescriptor getTask( boost::asio::yield_context& yield_ctx )
    {
        return m_taskReady.async_receive( yield_ctx );
    }

    void completeTask( const mega::pipeline::TaskDescriptor& task, bool bSuccess,
                       boost::asio::yield_context& yield_ctx )
    {
        m_taskComplete.async_send( boost::system::error_code(), TaskCompletion{ getID(), task, bSuccess }, yield_ctx );
    }

};

} // namespace service
} // namespace mega

#endif // PIPELINE_22_SEPT_2022