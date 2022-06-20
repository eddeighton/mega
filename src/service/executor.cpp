
#include "service/executor.hpp"

#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/worker_leaf.hxx"
#include "service/protocol/model/leaf_worker.hxx"

#include "runtime/runtime.hpp"

#include "pipeline/pipeline.hpp"
#include "pipeline/stash.hpp"

#include "common/requireSemicolon.hpp"

#include "boost/dll.hpp"

#include <boost/dll/shared_library_load_mode.hpp>
#include <optional>
#include <future>
#include <thread>

#include "parser/parser.hpp"

namespace mega
{
namespace service
{

class ExecutorRequestConversation : public network::ConcurrentConversation, public network::leaf_worker::Impl
{
protected:
    Executor& m_executor;

public:
    ExecutorRequestConversation( Executor& executor, const network::ConversationID& conversationID,
                                 const network::ConnectionID& originatingConnectionID )
        : ConcurrentConversation( executor, conversationID, originatingConnectionID )
        , m_executor( executor )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::leaf_worker::Impl::dispatchRequest( msg, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override
    {
        if ( m_executor.getLeafSender().getConnectionID() == connectionID )
        {
            m_executor.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Executor Critical error in error handler" );
        }
    }

    network::worker_leaf::Request_Encode getLeafRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::worker_leaf::Request_Encode( *this, m_executor.getLeafSender(), yield_ctx );
    }
    network::leaf_worker::Response_Encode getLeafResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_worker::Response_Encode( *this, m_executor.getLeafSender(), yield_ctx );
    }

    virtual void RootListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        std::vector< std::string > result = { m_executor.getProcessName() };
        getLeafResponse( yield_ctx ).RootListNetworkNodes( result );
    }

    virtual void RootPipelineStartJobs( const mega::pipeline::Configuration& configuration,
                                        const network::ConversationID&       rootConversationID,
                                        boost::asio::yield_context&          yield_ctx ) override;
};

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
                     mega::pipeline::Pipeline::Ptr pPipeline, const network::ConversationID& rootConversationID )
        : ExecutorRequestConversation( executor, conversationID, conversationID.getConnectionID() )
        , m_pPipeline( pPipeline )
        , m_rootConversationID( rootConversationID )
    {
        VERIFY_RTE( m_pPipeline );
    }

    virtual void RootPipelineStartTask( const mega::pipeline::TaskDescriptor& task,
                                        boost::asio::yield_context&           yield_ctx ) override
    {
        m_pYieldCtx = &yield_ctx;
        m_pPipeline->execute( task, *this, *this, *this );
    }

    // pipeline::DependencyProvider
    virtual EG_PARSER_INTERFACE* getParser() override { return m_executor.m_pParser.get(); }

    // pipeline::Progress
    virtual void onStarted( const std::string& strMsg ) override
    {
        getLeafRequest( *m_pYieldCtx ).ExePipelineWorkProgress( strMsg );
    }

    virtual void onProgress( const std::string& strMsg ) override
    {
        getLeafRequest( *m_pYieldCtx ).ExePipelineWorkProgress( strMsg );
    }

    virtual void onFailed( const std::string& strMsg ) override
    {
        getLeafResponse( *m_pYieldCtx ).RootPipelineStartTask( network::PipelineResult( false, strMsg ) );
    }

    virtual void onCompleted( const std::string& strMsg ) override
    {
        getLeafResponse( *m_pYieldCtx ).RootPipelineStartTask( network::PipelineResult( true, strMsg ) );
    }

    // pipeline::Stash
    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath ) override
    {
        return getLeafRequest( *m_pYieldCtx ).ExeGetBuildHashCode( filePath );
    }
    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) override
    {
        getLeafRequest( *m_pYieldCtx ).ExeSetBuildHashCode( filePath, hashCode );
    }
    virtual void stash( const boost::filesystem::path& file, task::DeterminantHash code ) override
    {
        getLeafRequest( *m_pYieldCtx ).ExeStash( file, code );
    }
    virtual bool restore( const boost::filesystem::path& file, task::DeterminantHash code ) override
    {
        return getLeafRequest( *m_pYieldCtx ).ExeRestore( file, code );
    }

    void run( boost::asio::yield_context& yield_ctx ) override
    {
        getLeafRequest( yield_ctx ).ExePipelineReadyForWork( m_rootConversationID );
    }
};

void ExecutorRequestConversation::RootPipelineStartJobs( const pipeline::Configuration& configuration,
                                                         const network::ConversationID& rootConversationID,
                                                         boost::asio::yield_context&    yield_ctx )
{
    mega::pipeline::Pipeline::Ptr pPipeline;
    {
        std::ostringstream osLog;
        pPipeline = pipeline::Registry::getPipeline( configuration, osLog );
        if ( !pPipeline )
        {
            SPDLOG_ERROR( "Failed to load pipeline: {}", configuration.getPipelineID() );
            THROW_RTE( "Failed to load pipeline: " << configuration.get() );
        }
        else
        {
            SPDLOG_INFO( "{}", osLog.str() );
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
    getLeafResponse( yield_ctx ).RootPipelineStartJobs( jobIDs );

    for ( JobConversation::Ptr pJob : jobs )
    {
        m_executor.conversationStarted( pJob );
    }
}

Executor::Executor( boost::asio::io_context& io_context, int numThreads )
    : network::ConversationManager( network::Node::toStr( network::Node::Executor ), io_context )
    , m_io_context( io_context )
    , m_numThreads( numThreads )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf( m_receiverChannel.getSender(), network::Node::Executor )
{
    m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
        "parser", "g_parserSymbol", boost::dll::load_mode::append_decorations );

    std::ostringstream os;
    os << network::Node::toStr( network::Node::Executor ) << "_" << std::this_thread::get_id();
    network::ConnectionID connectionID = os.str();
    m_receiverChannel.run( connectionID );
}

Executor::~Executor()
{
    m_receiverChannel.stop();
    SPDLOG_INFO( "Executor shutdown" );
}

void Executor::shutdown() {}

network::ConversationBase::Ptr Executor::joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                           const network::Header&         header,
                                                           const network::MessageVariant& msg )
{
    return network::ConversationBase::Ptr(
        new ExecutorRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
}
} // namespace service
} // namespace mega
