#ifndef JOB_22_JUNE_2022
#define JOB_22_JUNE_2022

#include "request.hpp"

#include "service/network/conversation.hpp"

#include "pipeline/pipeline.hpp"

namespace mega
{
namespace service
{
class Executor;

class JobConversation : public ExecutorRequestConversation,
                        public pipeline::Progress,
                        public pipeline::Stash,
                        public pipeline::DependencyProvider
{
    const network::ConversationID m_rootConversationID;
    mega::pipeline::Pipeline::Ptr m_pPipeline;
    boost::asio::yield_context*   m_pYieldCtx = nullptr;

public:
    using Ptr = std::shared_ptr< JobConversation >;

    JobConversation( Executor& executor, const network::ConversationID& conversationID,
                     mega::pipeline::Pipeline::Ptr pPipeline, const network::ConversationID& rootConversationID );

    //virtual void RootPipelineStartTask( const mega::pipeline::TaskDescriptor& task,
    //                                    boost::asio::yield_context&           yield_ctx ) override;

    // pipeline::DependencyProvider
    virtual EG_PARSER_INTERFACE* getParser() override;

    // pipeline::Progress
    virtual void onStarted( const std::string& strMsg ) override;
    virtual void onProgress( const std::string& strMsg ) override;
    virtual void onFailed( const std::string& strMsg ) override;
    virtual void onCompleted( const std::string& strMsg ) override;

    // pipeline::Stash
    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath ) override;
    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) override;
    virtual void stash( const boost::filesystem::path& file, task::DeterminantHash code ) override;
    virtual bool restore( const boost::filesystem::path& file, task::DeterminantHash code ) override;

    void run( boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // JOB_22_JUNE_2022
