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

#include "service/executor/request.hpp"

#include "service/network/logical_thread.hpp"

#include "pipeline/pipeline.hpp"

#include "service/protocol/model/job.hxx"

namespace mega::service
{

class Executor;

class JobLogicalThread : public ExecutorRequestLogicalThread,
                         public pipeline::Progress,
                         public pipeline::Stash,
                         public pipeline::DependencyProvider
{
    const network::LogicalThreadID m_rootLogicalThreadID;
    mega::pipeline::Pipeline::Ptr  m_pPipeline;
    boost::asio::yield_context*    m_pYieldCtx = nullptr;

    std::optional< pipeline::PipelineResult > m_resultOpt;
    std::optional< std::string >              m_lastMsg;

public:
    using Ptr = std::shared_ptr< JobLogicalThread >;

    JobLogicalThread( Executor& executor, const network::LogicalThreadID& logicalthreadID,
                      mega::pipeline::Pipeline::Ptr pPipeline, const network::LogicalThreadID& rootLogicalThreadID );

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;

    // network::job::Impl,
    virtual pipeline::PipelineResult JobStartTask( const mega::pipeline::TaskDescriptor& task,
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
    virtual mega::SymbolTable getSymbolTable() override;
    virtual mega::SymbolTable newSymbols( const mega::SymbolRequest& request ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;

    void run( boost::asio::yield_context& yield_ctx ) override;
};

} // namespace mega::service

#endif // JOB_22_JUNE_2022
