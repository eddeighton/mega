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

#ifndef PIPELINE_22_SEPT_2022
#define PIPELINE_22_SEPT_2022

#include "request.hpp"

#include "service/network/log.hpp"

#include "service/protocol/model/pipeline.hxx"

namespace mega::service
{
class Root;

class RootPipelineLogicalThread : public RootRequestLogicalThread,
                                  public network::pipeline::Impl,
                                  public pipeline::Progress,
                                  public pipeline::Stash
{
    std::set< network::LogicalThreadID > m_jobs;

    using TaskChannel = boost::asio::experimental::concurrent_channel< void(
        boost::system::error_code, mega::pipeline::TaskDescriptor ) >;
    TaskChannel m_taskReady;

    struct TaskCompletion
    {
        network::LogicalThreadID       jobID;
        mega::pipeline::TaskDescriptor task;
        bool                           bSuccess;
    };
    using TaskCompletionChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, TaskCompletion ) >;
    TaskCompletionChannel m_taskComplete;

    static constexpr mega::U32 CHANNEL_SIZE = 256;

public:
    RootPipelineLogicalThread( Root& root, const network::LogicalThreadID& logicalthreadID );

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
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
    virtual mega::SymbolTable getSymbolTable() override { THROW_RTE( "Root: Unreachable" ); }
    virtual mega::SymbolTable newSymbols( const mega::SymbolRequest& request ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }

    // pipeline::Progress
    virtual void onStarted( const std::string& strMsg ) override { onProgress( strMsg ); }
    virtual void onProgress( const std::string& strMsg ) override { network::logLinesInfo( strMsg ); }
    virtual void onFailed( const std::string& strMsg ) override { onProgress( strMsg ); }
    virtual void onCompleted( const std::string& strMsg ) override { onProgress( strMsg ); }

    // network::pipeline::Impl
    virtual mega::pipeline::PipelineResult PipelineRun( const mega::pipeline::Configuration& configuration,
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
} // namespace mega::service

#endif // PIPELINE_22_SEPT_2022