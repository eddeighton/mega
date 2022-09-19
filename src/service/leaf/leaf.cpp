
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

#include "mega/common.hpp"

#include "runtime/mpo_context.hpp"

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
        if ( ( m_leaf.getNodeChannelSender().getConnectionID() == connection )
             || ( m_leaf.m_pSelfSender->getConnectionID() == connection ) )
        {
            m_leaf.getNodeChannelSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
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
        return network::term_leaf::Response_Encode( *this, m_leaf.getNodeChannelSender(), yield_ctx );
    }
    network::leaf_term::Request_Encode getTermRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_term::Request_Encode( *this, m_leaf.getNodeChannelSender(), yield_ctx );
    }
    network::tool_leaf::Response_Encode getToolResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::tool_leaf::Response_Encode( *this, m_leaf.getNodeChannelSender(), yield_ctx );
    }
    network::leaf_tool::Request_Encode getToolRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_tool::Request_Encode( *this, m_leaf.getNodeChannelSender(), yield_ctx );
    }
    network::exe_leaf::Response_Encode getExeResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::exe_leaf::Response_Encode( *this, m_leaf.getNodeChannelSender(), yield_ctx );
    }
    network::leaf_exe::Request_Encode getExeRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_exe::Request_Encode( *this, m_leaf.getNodeChannelSender(), yield_ctx );
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

    virtual void TermGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermGetMegastructureInstallation();
        getTermResponse( yield_ctx ).TermGetMegastructureInstallation( result );
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

    virtual void TermSimDestroy( const mega::network::ConversationID& simulationID,
                                 boost::asio::yield_context&          yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).TermSimDestroy( simulationID );
        getTermResponse( yield_ctx ).TermSimDestroy();
    }

    virtual void TermSimList( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermSimList();
        getTermResponse( yield_ctx ).TermSimList( result );
    }

    virtual void TermSimReadLock( const mega::network::ConversationID& owningID,
                                  const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermSimReadLock( owningID, simulationID );
        getTermResponse( yield_ctx ).TermSimReadLock( result );
    }

    virtual void TermSimWriteLock( const mega::network::ConversationID& owningID,
                                   const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermSimWriteLock( owningID, simulationID );
        getTermResponse( yield_ctx ).TermSimWriteLock( result );
    }

    virtual void TermSimReleaseLock( const mega::network::ConversationID& owningID,
                                     const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).TermSimReleaseLock( owningID, simulationID );
        getTermResponse( yield_ctx ).TermSimReleaseLock();
    }

    virtual void TermClearStash( boost::asio::yield_context& yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).TermClearStash();
        getTermResponse( yield_ctx ).TermClearStash();
    }

    virtual void TermCapacity( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).TermCapacity();
        getTermResponse( yield_ctx ).TermCapacity( result );
    }

    virtual void TermShutdown( boost::asio::yield_context& yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).TermShutdown();
        getTermResponse( yield_ctx ).TermShutdown();
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
                auto result = getToolRequest( yield_ctx ).RootListNetworkNodes();
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
        SPDLOG_TRACE( "Leaf: RootListNetworkNodes end" );
    }

    virtual void RootPipelineStartJobs( const mega::utilities::ToolChain&    toolChain,
                                        const mega::pipeline::Configuration& configuration,
                                        const network::ConversationID&       rootConversationID,
                                        boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getExeRequest( yield_ctx ).RootPipelineStartJobs( toolChain, configuration, rootConversationID );
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
        VERIFY_RTE_MSG( m_leaf.m_nodeType == network::Node::Executor, "RootSimList on non-executor" );
        auto result = getExeRequest( yield_ctx ).RootSimList();
        getDaemonResponse( yield_ctx ).RootSimList( result );
    }

    virtual void RootSimCreate( boost::asio::yield_context& yield_ctx ) override
    {
        VERIFY_RTE_MSG( m_leaf.m_nodeType == network::Node::Executor, "RootSimCreate on non-executor" );
        auto result = getExeRequest( yield_ctx ).RootSimCreate();
        getDaemonResponse( yield_ctx ).RootSimCreate( result );
    }

    virtual void RootSimDestroy( const mega::network::ConversationID& simulationID,
                                 boost::asio::yield_context&          yield_ctx ) override
    {
        VERIFY_RTE_MSG( m_leaf.m_nodeType == network::Node::Executor, "RootSimDestroy on non-executor" );
        getExeRequest( yield_ctx ).RootSimDestroy( simulationID );
        getDaemonResponse( yield_ctx ).RootSimDestroy();
    }

    virtual void RootSimReadLock( const mega::network::ConversationID& owningID,
                                  const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        VERIFY_RTE_MSG( m_leaf.m_nodeType == network::Node::Executor, "RootSimReadLock on non-executor" );
        auto result = getExeRequest( yield_ctx ).RootSimReadLock( owningID, simulationID );
        getDaemonResponse( yield_ctx ).RootSimReadLock( result );
    }

    virtual void RootSimWriteLock( const mega::network::ConversationID& owningID,
                                   const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        VERIFY_RTE_MSG( m_leaf.m_nodeType == network::Node::Executor, "RootSimWriteLock on non-executor" );
        auto result = getExeRequest( yield_ctx ).RootSimWriteLock( owningID, simulationID );
        getDaemonResponse( yield_ctx ).RootSimWriteLock( result );
    }

    virtual void RootSimReleaseLock( const mega::network::ConversationID& owningID,
                                     const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
    {
        VERIFY_RTE_MSG( m_leaf.m_nodeType == network::Node::Executor, "RootSimReleaseLock on non-executor" );
        getExeRequest( yield_ctx ).RootSimReleaseLock( owningID, simulationID );
        getDaemonResponse( yield_ctx ).RootSimReleaseLock();
    }

    virtual void RootShutdown( boost::asio::yield_context& yield_ctx ) override
    {
        switch ( m_leaf.m_nodeType )
        {
            case network::Node::Terminal:
            {
                getTermRequest( yield_ctx ).RootShutdown();
                getDaemonResponse( yield_ctx ).RootShutdown();
            }
            break;
            case network::Node::Tool:
            {
                getToolRequest( yield_ctx ).RootShutdown();
                getDaemonResponse( yield_ctx ).RootShutdown();
            }
            break;
            case network::Node::Executor:
            {
                getExeRequest( yield_ctx ).RootShutdown();
                getDaemonResponse( yield_ctx ).RootShutdown();
            }
            break;
            case network::Node::Daemon:
            case network::Node::Root:
            case network::Node::TOTAL_NODE_TYPES:
            default:
                THROW_RTE( "Leaf: Invalid leaf type" );
        }

        // boost::asio::post( [ &leaf = m_leaf ]() { leaf.shutdown(); } );
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

    virtual void ExeGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeGetMegastructureInstallation();
        getExeResponse( yield_ctx ).ExeGetMegastructureInstallation( result );
    }

    virtual void ExeGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeGetProject();
        getExeResponse( yield_ctx ).ExeGetProject( result );
    }

    virtual void ExeCreateMPO( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeCreateMPO( m_leaf.m_mpo );
        getExeResponse( yield_ctx ).ExeCreateMPO( result );
    }

    virtual void ExeAcquireMemory( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeAcquireMemory( mpo );
        getExeResponse( yield_ctx ).ExeAcquireMemory( result );
    }
    virtual void ExeAllocateNetworkAddress( const mega::MPO&            mpo,
                                            const mega::TypeID&         objectTypeID,
                                            boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeAllocateNetworkAddress( mpo, objectTypeID );
        getExeResponse( yield_ctx ).ExeAllocateNetworkAddress( result );
    }

    virtual void ExeDeAllocateNetworkAddress( const mega::MPO&            mpo,
                                              const mega::AddressStorage& networkAddress,
                                              boost::asio::yield_context& yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ExeDeAllocateNetworkAddress( mpo, networkAddress );
        getExeResponse( yield_ctx ).ExeDeAllocateNetworkAddress();
    }

    virtual void ExeGetMPOContextID( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeGetMPOContextID( mpo );
        getExeResponse( yield_ctx ).ExeGetMPOContextID( result );
    }

    virtual void ExeSimReadLock( const mega::network::ConversationID& simulationID,
                                 boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeSimReadLock( simulationID );
        getExeResponse( yield_ctx ).ExeSimReadLock( result );
    }

    virtual void ExeSimWriteLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ExeSimWriteLock( simulationID );
        getExeResponse( yield_ctx ).ExeSimWriteLock( result );
    }

    virtual void ExeSimReleaseLock( const mega::network::ConversationID& simulationID,
                                    boost::asio::yield_context&          yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ExeSimReleaseLock( simulationID );
        getExeResponse( yield_ctx ).ExeSimReleaseLock();
    }

    virtual void ToolGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolGetMegastructureInstallation();
        getToolResponse( yield_ctx ).ToolGetMegastructureInstallation( result );
    }

    virtual void ToolCreateMPO( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolCreateMPO( m_leaf.m_mpo );
        getToolResponse( yield_ctx ).ToolCreateMPO( result );
    }

    virtual void ToolAcquireMemory( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolAcquireMemory( mpo );
        getToolResponse( yield_ctx ).ToolAcquireMemory( result );
    }
    virtual void ToolAllocateNetworkAddress( const mega::MPO&            mpo,
                                             const mega::TypeID&         objectTypeID,
                                             boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolAllocateNetworkAddress( mpo, objectTypeID );
        getToolResponse( yield_ctx ).ToolAllocateNetworkAddress( result );
    }

    virtual void ToolDeAllocateNetworkAddress( const mega::MPO&            mpo,
                                               const mega::AddressStorage& networkAddress,
                                               boost::asio::yield_context& yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ToolDeAllocateNetworkAddress( mpo, networkAddress );
        getToolResponse( yield_ctx ).ToolDeAllocateNetworkAddress();
    }
    virtual void ToolStash( const boost::filesystem::path& filePath,
                            const task::DeterminantHash&   determinant,
                            boost::asio::yield_context&    yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ToolStash( filePath, determinant );
        getToolResponse( yield_ctx ).ToolStash();
    }
    virtual void ToolRestore( const boost::filesystem::path& filePath,
                              const task::DeterminantHash&   determinant,
                              boost::asio::yield_context&    yield_ctx ) override
    {
        const bool bRestored = getDaemonRequest( yield_ctx ).ToolRestore( filePath, determinant );
        getToolResponse( yield_ctx ).ToolRestore( bRestored );
    }

    virtual void ToolGetMPOContextID( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolGetMPOContextID( mpo );
        getToolResponse( yield_ctx ).ToolGetMPOContextID( result );
    }
    virtual void ToolGetMPO( const mega::network::ConversationID& conversationID,
                             boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolGetMPO( conversationID );
        getToolResponse( yield_ctx ).ToolGetMPO( result );
    }
    virtual void ToolSimReadLock( const mega::network::ConversationID& owningID,
                                  const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolSimReadLock( owningID, simulationID );
        getToolResponse( yield_ctx ).ToolSimReadLock( result );
    }

    virtual void ToolSimWriteLock( const mega::network::ConversationID& owningID,
                                   const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolSimWriteLock( owningID, simulationID );
        getToolResponse( yield_ctx ).ToolSimWriteLock( result );
    }

    virtual void ToolSimReleaseLock( const mega::network::ConversationID& owningID,
                                     const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).ToolSimReleaseLock( owningID, simulationID );
        getToolResponse( yield_ctx ).ToolSimReleaseLock();
    }
    virtual void ToolSimList( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getDaemonRequest( yield_ctx ).ToolSimList();
        getToolResponse( yield_ctx ).ToolSimList( result );
    }
};

class LeafEnrole : public LeafRequestConversation
{
    std::promise< void >& m_promise;

public:
    LeafEnrole( Leaf& leaf, const network::ConnectionID& originatingConnectionID, std::promise< void >& promise )
        : LeafRequestConversation( leaf, leaf.createConversationID( originatingConnectionID ), originatingConnectionID )
        , m_promise( promise )
    {
    }

    void run( boost::asio::yield_context& yield_ctx )
    {
        m_leaf.m_mpo = getDaemonRequest( yield_ctx ).LeafEnrole( m_leaf.getType() );
        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
    }
};

Leaf::Leaf( network::Sender::Ptr pSender, network::Node::Type nodeType )
    : network::ConversationManager( network::makeProcessName( network::Node::Leaf ), m_io_context )
    , m_pSender( std::move( pSender ) )
    , m_nodeType( nodeType )
    , m_io_context( 1 ) // single threaded concurrency hint
    , m_receiverChannel( m_io_context, *this )
    , m_client( m_io_context, *this, "localhost", mega::network::MegaDaemonServiceName() )
    , m_work_guard( m_io_context.get_executor() )
    , m_io_thread( [ &io_context = m_io_context ]() { io_context.run(); } )
{
    m_receiverChannel.run( network::makeProcessName( network::Node::Leaf ) );

    m_pSelfSender = m_receiverChannel.getSender();

    // immediately enrole with node type
    {
        std::promise< void >          promise;
        std::future< void >           future = promise.get_future();
        std::shared_ptr< LeafEnrole > pEnrole
            = std::make_shared< LeafEnrole >( *this, getDaemonSender().getConnectionID(), promise );
        conversationInitiated( pEnrole, getDaemonSender() );
        future.wait();
    }
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
