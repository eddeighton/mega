
#include "service/leaf/leaf.hpp"

#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/term_leaf.hxx"
#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/daemon_leaf.hxx"
#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/leaf_exe.hxx"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/leaf_tool.hxx"

#include "common/requireSemicolon.hpp"

#include "boost/system/detail/error_code.hpp"

namespace mega
{
namespace service
{

class LeafRequestConversation : public network::InThreadConversation,
                                public network::term_leaf::Impl,
                                public network::daemon_leaf::Impl,
                                public network::exe_leaf::Impl,
                                public network::tool_leaf::Impl
{
protected:
    Leaf& m_leaf;

public:
    LeafRequestConversation( Leaf& leaf, const network::ConversationID& conversationID,
                             const network::ConnectionID& originatingConnectionID )
        : InThreadConversation( leaf, conversationID, originatingConnectionID )
        , m_leaf( leaf )
    {
    }

    virtual bool dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::term_leaf::Impl::dispatchRequest( msg, yield_ctx )
               || network::daemon_leaf::Impl::dispatchRequest( msg, yield_ctx )
               || network::exe_leaf::Impl::dispatchRequest( msg, yield_ctx )
               || network::tool_leaf::Impl::dispatchRequest( msg, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override
    {
        if ( ( m_leaf.getTerminalSender().getConnectionID() == connection )
             || ( m_leaf.m_pSelfSender->getConnectionID() == connection ) )
        {
            m_leaf.getTerminalSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else if ( m_leaf.getDaemonSender().getConnectionID() == connection )
        {
            m_leaf.getDaemonSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR(
                "Leaf: Cannot resolve connection: {} in error handler for error: {}", connection, strErrorMsg );
            THROW_RTE( "Leaf: Critical error in error handler" );
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
    network::exe_leaf::Response_Encode getExeResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::exe_leaf::Response_Encode( *this, m_leaf.getTerminalSender(), yield_ctx );
    }
    network::leaf_exe::Request_Encode getExeRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_exe::Request_Encode( *this, m_leaf.getTerminalSender(), yield_ctx );
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

    virtual void TermGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermGetProject();
        getTermResponse( yield_ctx ).TermGetProject( result );
    }

    virtual void TermSetProject( const mega::network::Project& project, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermSetProject( project );
        getTermResponse( yield_ctx ).TermSetProject( result );
    }

    virtual void TermNewInstallation( const mega::network::Project& project,
                                      boost::asio::yield_context&   yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermNewInstallation( project );
        getTermResponse( yield_ctx ).TermNewInstallation( result );
    }

    virtual void TermSimNew( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermSimNew();
        getTermResponse( yield_ctx ).TermSimNew( result );
    }

    virtual void TermSimList( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermSimList();
        getTermResponse( yield_ctx ).TermSimList( result );
    }

    virtual void TermSimReadLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).TermSimReadLock( simulationID );
        getTermResponse( yield_ctx ).TermSimReadLock();
    }

    virtual void TermSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).TermSimWriteLock( simulationID );
        getTermResponse( yield_ctx ).TermSimWriteLock();
    }

    // daemon_leaf
    virtual void RootListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        switch ( m_leaf.m_nodeType )
        {
            case network::Node::Terminal:
            {
                auto result = getTermRequest( yield_ctx ).RootListNetworkNodes();
                result.push_back( getProcessName() );
                getDaemonResponse( yield_ctx ).RootListNetworkNodes( result );
            }
            break;
            case network::Node::Tool:
            {
                // auto result = getToolRequest( yield_ctx ).RootListNetworkNodes();
                std::vector< std::string > result;
                result.push_back( getProcessName() );
                getDaemonResponse( yield_ctx ).RootListNetworkNodes( result );
            }
            break;
            case network::Node::Executor:
            {
                auto result = getExeRequest( yield_ctx ).RootListNetworkNodes();
                result.push_back( getProcessName() );
                getDaemonResponse( yield_ctx ).RootListNetworkNodes( result );
            }
            break;
            case network::Node::Daemon:
            case network::Node::Root:
            case network::Node::TOTAL_NODE_TYPES:
            default:
                THROW_RTE( "Leaf: Invalid leaf type" );
        }
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

    virtual void RootProjectUpdated( const mega::network::Project& project,
                                     boost::asio::yield_context&   yield_ctx ) override
    {
        getExeRequest( yield_ctx ).RootProjectUpdated( project );
        getDaemonResponse( yield_ctx ).RootProjectUpdated();
    }
    virtual void RootSimList( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getExeRequest( yield_ctx ).RootSimList();
        getDaemonResponse( yield_ctx ).RootSimList( result );
    }

    virtual void RootSimCreate( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getExeRequest( yield_ctx ).RootSimCreate();
        getDaemonResponse( yield_ctx ).RootSimCreate( result );
    }

    virtual void RootSimReadLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        getExeRequest( yield_ctx ).RootSimReadLock( simulationID );
        getDaemonResponse( yield_ctx ).RootSimReadLock();
    }

    virtual void RootSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        getExeRequest( yield_ctx ).RootSimWriteLock( simulationID );
        getDaemonResponse( yield_ctx ).RootSimWriteLock();
    }

    virtual void RootSimReadLockReady( const mega::TimeStamp&      timeStamp,
                                       boost::asio::yield_context& yield_ctx ) override
    {
        switch ( m_leaf.m_nodeType )
        {
            case network::Node::Terminal:
            {
                getTermRequest( yield_ctx ).RootSimReadLockReady( timeStamp );
                getDaemonResponse( yield_ctx ).RootSimReadLockReady();
            }
            break;
            case network::Node::Tool:
            {
                THROW_RTE( "Not implemented" );
            }
            break;
            case network::Node::Executor:
            {
                getExeRequest( yield_ctx ).RootSimReadLockReady( timeStamp );
                getDaemonResponse( yield_ctx ).RootSimReadLockReady();
            }
            break;
            case network::Node::Daemon:
            case network::Node::Root:
            case network::Node::TOTAL_NODE_TYPES:
            default:
                THROW_RTE( "Leaf: Invalid leaf type" );
        }
    }

    virtual void RootSimWriteLockReady( const mega::TimeStamp&      timeStamp,
                                        boost::asio::yield_context& yield_ctx ) override
    {
        switch ( m_leaf.m_nodeType )
        {
            case network::Node::Terminal:
            {
                getTermRequest( yield_ctx ).RootSimWriteLockReady( timeStamp );
                getDaemonResponse( yield_ctx ).RootSimWriteLockReady();
            }
            break;
            case network::Node::Tool:
            {
                THROW_RTE( "Not implemented" );
            }
            break;
            case network::Node::Executor:
            {
                getExeRequest( yield_ctx ).RootSimWriteLockReady( timeStamp );
                getDaemonResponse( yield_ctx ).RootSimWriteLockReady();
            }
            break;
            case network::Node::Daemon:
            case network::Node::Root:
            case network::Node::TOTAL_NODE_TYPES:
            default:
                THROW_RTE( "Leaf: Invalid leaf type" );
        }
    }

    // network::exe_leaf::Impl
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

    virtual void ExeGetBuildHashCode( const boost::filesystem::path& filePath,
                                      boost::asio::yield_context&    yield_ctx ) override
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

    virtual void ExeGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeGetProject();
        getExeResponse( yield_ctx ).ExeGetProject( result );
    }

    virtual void ExeSimReadLockReady( const mega::TimeStamp& timeStamp, boost::asio::yield_context& yield_ctx ) override
    {
        // is request initiated on this leaf?
        if ( m_leaf.m_conversationIDs.count( getID() ) )
        {
            // getExeRequest( yield_ctx ).ExeSimReadLockReady( timeStamp );
            THROW_RTE( "Not implemented" );
        }
        else
        {
            getDaemonRequest( yield_ctx ).ExeSimReadLockReady( timeStamp );
            getExeResponse( yield_ctx ).ExeSimReadLockReady();
        }
    }

    virtual void ExeSimWriteLockReady( const mega::TimeStamp&      timeStamp,
                                       boost::asio::yield_context& yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ExeSimWriteLockReady( timeStamp );
        getExeResponse( yield_ctx ).ExeSimWriteLockReady();
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
        ConversationBase::RequestStack stack( "LeafEnrole", *this, m_leaf.getDaemonSender().getConnectionID() );
        getDaemonRequest( yield_ctx ).LeafEnrole( m_leaf.getType() );
    }
};

Leaf::Leaf( network::Sender::Ptr pSender, network::Node::Type nodeType )
    : network::ConversationManager( network::makeProcessName( network::Node::Leaf ), m_io_context )
    , m_pSender( std::move( pSender ) )
    , m_nodeType( nodeType )
    , m_receiverChannel( m_io_context, *this )
    , m_client( m_io_context, *this, "localhost", mega::network::MegaDaemonServiceName() )
    , m_work_guard( m_io_context.get_executor() )
    , m_io_thread( [ &io_context = m_io_context ]() { io_context.run(); } )
{
    m_receiverChannel.run( network::makeProcessName( network::Node::Leaf ) );

    m_pSelfSender = m_receiverChannel.getSender();

    // immediately enrole with node type
    conversationInitiated(
        network::ConversationBase::Ptr( new LeafEnrole( *this, getDaemonSender().getConnectionID() ) ),
        getDaemonSender() );
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
network::ConversationBase::Ptr Leaf::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Header&       header,
                                                       const network::Message&      msg )
{
    switch ( m_nodeType )
    {
        case network::Node::Leaf:
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Executor:
            return network::ConversationBase::Ptr(
                new LeafRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
            break;
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::TOTAL_NODE_TYPES:
        default:
            THROW_RTE( "Leaf: Unknown leaf type" );
            break;
    }
}

void Leaf::conversationNew( const network::Header& header, const network::ReceivedMsg& msg )
{
    m_conversationIDs.insert( header.getConversationID() );
    boost::asio::spawn( m_ioContext,
                        [ &m_client = m_client, header, message = msg.msg ]( boost::asio::yield_context yield_ctx )
                        { m_client.send( header.getConversationID(), message, yield_ctx ); } );
}

void Leaf::conversationEnd( const network::Header& header, const network::ReceivedMsg& msg )
{
    m_conversationIDs.erase( header.getConversationID() );
    boost::asio::spawn( m_ioContext,
                        [ &m_client = m_client, header, message = msg.msg ]( boost::asio::yield_context yield_ctx )
                        { m_client.send( header.getConversationID(), message, yield_ctx ); } );
}

} // namespace service
} // namespace mega
