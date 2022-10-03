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

#ifndef JOB_22_JUNE_2022
#define JOB_22_JUNE_2022

#include "request.hpp"

#include "service/network/conversation.hpp"

#include "pipeline/pipeline.hpp"

#include "service/protocol/model/job.hxx"

namespace mega::service
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

    std::optional< network::PipelineResult > m_resultOpt;

public:
    using Ptr = std::shared_ptr< JobConversation >;

    JobConversation( Executor& executor, const network::ConversationID& conversationID,
                     mega::pipeline::Pipeline::Ptr pPipeline, const network::ConversationID& rootConversationID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    // network::job::Impl,
    virtual network::PipelineResult JobStartTask( const mega::pipeline::TaskDescriptor& task,
                                                  boost::asio::yield_context&           yield_ctx ) override;

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

} // namespace mega::service

#endif // JOB_22_JUNE_2022
