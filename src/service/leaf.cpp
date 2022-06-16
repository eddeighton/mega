
#include "service/leaf.hpp"

#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/term_leaf.hxx"
#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/daemon_leaf.hxx"
#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/worker_leaf.hxx"
#include "service/protocol/model/leaf_worker.hxx"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/leaf_tool.hxx"

#include "common/requireSemicolon.hpp"

#include "boost/system/detail/error_code.hpp"

namespace mega
{
namespace service
{

class LeafRequestConversation : public network::Conversation,
                                public network::term_leaf::Impl,
                                public network::daemon_leaf::Impl,
                                public network::worker_leaf::Impl,
                                public network::tool_leaf::Impl
{
protected:
    Leaf& m_leaf;

public:
    LeafRequestConversation( Leaf& leaf, const network::ConversationID& conversationID,
                             const network::ConnectionID& originatingConnectionID )
        : Conversation( leaf, conversationID, originatingConnectionID )
        , m_leaf( leaf )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::term_leaf::Impl::dispatchRequest( msg, yield_ctx )
               || network::daemon_leaf::Impl::dispatchRequest( msg, yield_ctx )
               || network::worker_leaf::Impl::dispatchRequest( msg, yield_ctx )
               || network::tool_leaf::Impl::dispatchRequest( msg, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override
    {
        if ( m_leaf.getTerminalSender().getConnectionID() == connection )
        {
            m_leaf.getTerminalSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else if ( m_leaf.getDaemonSender().getConnectionID() == connection )
        {
            m_leaf.getDaemonSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Leaf Critical error in error handler" );
        }
    }
    network::daemon_leaf::Response_Encode getDaemonResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_leaf::Response_Encode( *this, m_leaf.getDaemonSender(), yield_ctx );
    }
    network::leaf_daemon::Request_Encode getDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_daemon::Request_Encode( *this, m_leaf.getDaemonSender(), yield_ctx );
    }

    network::term_leaf::Response_Encode getTermResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::term_leaf::Response_Encode( *this, m_leaf.getTerminalSender(), yield_ctx );
    }
    network::leaf_term::Request_Encode getTermRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_term::Request_Encode( *this, m_leaf.getTerminalSender(), yield_ctx );
    }
    network::worker_leaf::Response_Encode getExeResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::worker_leaf::Response_Encode( *this, m_leaf.getTerminalSender(), yield_ctx );
    }
    network::leaf_worker::Request_Encode getExeRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_worker::Request_Encode( *this, m_leaf.getTerminalSender(), yield_ctx );
    }

    // term_leaf
    virtual void TermListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermListNetworkNodes();
        getTermResponse( yield_ctx ).TermListNetworkNodes( result );
    }

    virtual void TermPipelineRun( const mega::pipeline::Configuration& configuration,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermPipelineRun( configuration );
        getTermResponse( yield_ctx ).TermPipelineRun( result );
    }

    // daemon_leaf
    virtual void RootListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getTermRequest( yield_ctx ).RootListNetworkNodes();
        result.push_back( getProcessName() );
        getDaemonResponse( yield_ctx ).RootListNetworkNodes( result );
    }

    virtual void RootPipelineStartJobs( const mega::pipeline::Configuration& configuration,
                                        const network::ConversationID&       rootConversationID,
                                        boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getExeRequest( yield_ctx ).RootPipelineStartJobs( configuration, rootConversationID );
        getDaemonResponse( yield_ctx ).RootPipelineStartJobs( result );
    }

    virtual void RootPipelineStartTask( const mega::pipeline::TaskDescriptor& task,
                                        boost::asio::yield_context&           yield_ctx ) override
    {
        auto result = getExeRequest( yield_ctx ).RootPipelineStartTask( task );
        getDaemonResponse( yield_ctx ).RootPipelineStartTask( result );
    }

    // network::worker_leaf::Impl
    virtual void ExePipelineReadyForWork( const network::ConversationID& rootConversationID,
                                          boost::asio::yield_context&    yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ExePipelineReadyForWork( rootConversationID );
        getExeResponse( yield_ctx ).ExePipelineReadyForWork();
    }

    virtual void ExePipelineWorkProgress( const std::string& message, boost::asio::yield_context& yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ExePipelineWorkProgress( message );
        getExeResponse( yield_ctx ).ExePipelineWorkProgress();
    }

    virtual void ExeGetBuildHashCode( const boost::filesystem::path& filePath, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeGetBuildHashCode( filePath );
        getExeResponse( yield_ctx ).ExeGetBuildHashCode( result );
    }

    virtual void ExeSetBuildHashCode( const boost::filesystem::path& filePath,
                                      const task::FileHash&          hashCode,
                                      boost::asio::yield_context&    yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ExeSetBuildHashCode( filePath, hashCode );
        getExeResponse( yield_ctx ).ExeSetBuildHashCode();
    }

    virtual void ExeStash( const boost::filesystem::path& filePath,
                           const task::DeterminantHash&   determinant,
                           boost::asio::yield_context&    yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ExeStash( filePath, determinant );
        getExeResponse( yield_ctx ).ExeStash();
    }

    virtual void ExeRestore( const boost::filesystem::path& filePath,
                             const task::DeterminantHash&   determinant,
                             boost::asio::yield_context&    yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeRestore( filePath, determinant );
        getExeResponse( yield_ctx ).ExeRestore( result );
    }
};

class LeafEnrole : public LeafRequestConversation
{
public:
    LeafEnrole( Leaf& leaf, const network::ConnectionID& originatingConnectionID )
        : LeafRequestConversation( leaf, leaf.createConversationID( originatingConnectionID ), originatingConnectionID )
    {
    }

    void run( boost::asio::yield_context& yield_ctx )
    {
        Conversation::RequestStack stack( "LeafEnrole", *this, m_leaf.getDaemonSender().getConnectionID() );
        getDaemonRequest( yield_ctx ).LeafEnrole( m_leaf.getType() );
    }
};

Leaf::Leaf( network::Sender::Ptr pSender, network::Node::Type nodeType )
    : network::ConversationManager( "leaf", m_io_context )
    , m_pSender( std::move( pSender ) )
    , m_nodeType( nodeType )
    , m_receiverChannel( m_io_context, *this )
    , m_client( m_io_context, *this, "localhost", mega::network::MegaDaemonServiceName() )
    , m_work_guard( m_io_context.get_executor() )
    , m_io_thread( [ &io_context = m_io_context ]() { io_context.run(); } )
{
    std::ostringstream os;
    os << "leaf_" << std::this_thread::get_id();
    network::ConnectionID connectionID = os.str();
    m_receiverChannel.run( connectionID );

    m_pSelfSender = m_receiverChannel.getSender();

    // immediately enrole with node type
    conversationStarted(
        network::ConversationBase::Ptr( new LeafEnrole( *this, getDaemonSender().getConnectionID() ) ) );
}

Leaf::~Leaf()
{
    m_client.stop();
    m_receiverChannel.stop();
    m_work_guard.reset();
    m_io_thread.join();
}
/*
void Leaf::shutdown()
{
    m_client.stop();
    m_receiverChannel.stop();
}
*/
// network::ConversationManager
network::ConversationBase::Ptr Leaf::joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                       const network::Header&         header,
                                                       const network::MessageVariant& msg )
{
    switch ( m_nodeType )
    {
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Executor:
            return network::Conversation::Ptr(
                new LeafRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
            break;
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::TOTAL_NODE_TYPES:
        default:
            THROW_RTE( "Unknown leaf type" );
            break;
    }
}

} // namespace service
} // namespace mega
