
#include "service/executor.hpp"

#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

/*
#include "service/protocol/model/executor_daemon.hxx"
#include "service/protocol/model/daemon_executor.hxx"
*/

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
/*
class ExecutorRequestConversation : public network::Conversation, public network::daemon_executor::Impl
{
protected:
    Executor& m_executor;

public:
    ExecutorRequestConversation( Executor& executor, const network::ConversationID& conversationID,
                           const network::ConnectionID& originatingConnectionID )
        : Conversation( executor.m_conversationManager, conversationID, originatingConnectionID )
        , m_executor( executor )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_executor::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
    {
        if ( network::getConnectionID( m_executor.m_client.getSocket() ) == connectionID )
        {
            network::sendErrorResponse( getConversationID(), m_executor.m_client.getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Executor Critical error in error handler" );
        }
    }

    network::executor_daemon::Request_Encode getDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::executor_daemon::Request_Encode( *this, m_executor.m_client.getSocket(), yield_ctx );
    }
    network::daemon_executor::Response_Encode getDaemonResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_executor::Response_Encode( *this, m_executor.m_client.getSocket(), yield_ctx );
    }

    virtual void ReportActivities( boost::asio::yield_context& yield_ctx )
    {
        std::vector< network::ConversationID > activities;

        for ( const auto& id : m_conversationManager.reportActivities() )
        {
            activities.push_back( id );
        }

        getDaemonResponse( yield_ctx ).ReportActivities( activities );
    }

    virtual void ListThreads( boost::asio::yield_context& yield_ctx )
    {
        getDaemonResponse( yield_ctx ).ListThreads( m_executor.getNumThreads() );
    }

    virtual void ExecuteShutdown( boost::asio::yield_context& yield_ctx )
    {
        getDaemonResponse( yield_ctx ).ExecuteShutdown();

        boost::asio::post( [ &executor = m_executor ]() { executor.shutdown(); } );
    }

    virtual void PipelineStartJobs( const pipeline::Configuration& configuration,
                                    const network::ConversationID&     rootConversationID,
                                    boost::asio::yield_context&    yield_ctx );
};

class JobConversation : public ExecutorRequestConversation,
                    public pipeline::Progress,
                    public pipeline::Stash,
                    public pipeline::DependencyProvider
{
    const network::ConversationID     m_rootConversationID;
    mega::pipeline::Pipeline::Ptr m_pPipeline;
    boost::asio::yield_context*   m_pYieldCtx = nullptr;

public:
    using Ptr = std::shared_ptr< JobConversation >;
    JobConversation( Executor& executor, const network::ConversationID& conversationID, mega::pipeline::Pipeline::Ptr
pPipeline, const network::ConversationID& rootConversationID ) : ExecutorRequestConversation( executor, conversationID,
conversationID.getConnectionID() ) , m_pPipeline( pPipeline ) , m_rootConversationID( rootConversationID )
    {
        VERIFY_RTE( m_pPipeline );
    }

    virtual void PipelineStartTask( const mega::pipeline::TaskDescriptor& task,
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
        getDaemonRequest( *m_pYieldCtx ).PipelineWorkProgress( strMsg );
    }

    virtual void onProgress( const std::string& strMsg ) override
    {
        getDaemonRequest( *m_pYieldCtx ).PipelineWorkProgress( strMsg );
    }

    virtual void onFailed( const std::string& strMsg ) override
    {
        getDaemonResponse( *m_pYieldCtx ).PipelineStartTask( network::PipelineResult( false, strMsg ) );
    }

    virtual void onCompleted( const std::string& strMsg ) override
    {
        getDaemonResponse( *m_pYieldCtx ).PipelineStartTask( network::PipelineResult( true, strMsg ) );
    }

    // pipeline::Stash
    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath ) override
    {
        return getDaemonRequest( *m_pYieldCtx ).getBuildHashCode( filePath );
    }
    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) override
    {
        getDaemonRequest( *m_pYieldCtx ).setBuildHashCode( filePath, hashCode );
    }
    virtual void stash( const boost::filesystem::path& file, task::DeterminantHash code ) override
    {
        getDaemonRequest( *m_pYieldCtx ).stash( file, code );
    }
    virtual bool restore( const boost::filesystem::path& file, task::DeterminantHash code ) override
    {
        return getDaemonRequest( *m_pYieldCtx ).restore( file, code );
    }

    void run( boost::asio::yield_context& yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).PipelineReadyForWork( m_rootConversationID );
    }
};

void ExecutorRequestConversation::PipelineStartJobs( const pipeline::Configuration& configuration,
                                               const network::ConversationID&     rootConversationID,
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
            m_executor,
            m_executor.m_conversationManager.createConversationID( network::getConnectionID(
m_executor.m_client.getSocket() ) ), pPipeline, rootConversationID ); jobIDs.push_back( pJob->getConversationID() );
        jobs.push_back( pJob );
    }
    getDaemonResponse( yield_ctx ).PipelineStartJobs( jobIDs );

    for ( JobConversation::Ptr pJob : jobs )
    {
        m_executor.m_conversationManager.conversationStarted( pJob );
    }
}*/

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

Executor::~Executor() { SPDLOG_INFO( "Executor shutdown" ); }

void Executor::shutdown() {  }

network::ConversationBase::Ptr Executor::joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                           const network::Header&         header,
                                                           const network::MessageVariant& msg )
{
    // return network::Conversation::Ptr(
    //     new ExecutorRequestConversation( m_executor, msgHeader.getConversationID(), originatingConnectionID ) );

    return network::ConversationBase::Ptr();
}
} // namespace service
} // namespace mega
