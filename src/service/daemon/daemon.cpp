
#include "service/daemon.hpp"

#include "mega/common.hpp"
#include "service/network/conversation.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/daemon_leaf.hxx"
#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"

#include <iostream>

namespace mega
{
namespace service
{

class DaemonRequestConversation : public network::InThreadConversation,
                                  public network::leaf_daemon::Impl,
                                  public network::root_daemon::Impl
{
protected:
    Daemon& m_daemon;

public:
    DaemonRequestConversation( Daemon&                        daemon,
                               const network::ConversationID& conversationID,
                               const network::ConnectionID&   originatingConnectionID )
        : InThreadConversation( daemon, conversationID, originatingConnectionID )
        , m_daemon( daemon )
    {
    }

    virtual bool dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::leaf_daemon::Impl::dispatchRequest( msg, yield_ctx )
               || network::root_daemon::Impl::dispatchRequest( msg, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID,
                        const std::string&           strErrorMsg,
                        boost::asio::yield_context&  yield_ctx ) override
    {
        if ( m_daemon.m_rootClient.getConnectionID() == connectionID )
        {
            m_daemon.m_rootClient.sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else if ( network::Server::Connection::Ptr pLeafConnection
                  = m_daemon.m_leafServer.getConnection( connectionID ) )
        {
            pLeafConnection->sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Daemon: Critical error in error handler" );
        }
    }

    // helpers
    network::daemon_root::Request_Encode getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_root::Request_Encode( *this, m_daemon.m_rootClient, yield_ctx );
    }
    network::root_daemon::Response_Encode getRootResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::root_daemon::Response_Encode( *this, m_daemon.m_rootClient, yield_ctx );
    }

    network::leaf_daemon::Response_Encode getStackTopLeafResponse( boost::asio::yield_context& yield_ctx )
    {
        VERIFY_RTE( getStackConnectionID().has_value() );
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_leafServer.getConnection( getStackConnectionID().value() ) )
        {
            return network::leaf_daemon::Response_Encode( *this, *pConnection, yield_ctx );
        }
        THROW_RTE( "Daemon: Could not locate originating connection" );
    }

    std::optional< network::daemon_leaf::Request_Encode >
    leafRequestByCon( const mega::network::ConversationID& simulationID, boost::asio::yield_context& yield_ctx )
    {
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getConversations().count( simulationID ) )
            {
                return network::daemon_leaf::Request_Encode( *this, *pConnection, yield_ctx );
            }
        }
        return std::optional< network::daemon_leaf::Request_Encode >();
    }

    // leaf_daemon
    virtual void LeafEnrole( const mega::network::Node::Type& type, boost::asio::yield_context& yield_ctx ) override
    {
        network::Server::Connection::Ptr pConnection
            = m_daemon.m_leafServer.getConnection( getOriginatingEndPointID().value() );
        VERIFY_RTE( pConnection );
        pConnection->setType( type );
        SPDLOG_INFO( "Leaf {} enroled as {}", pConnection->getName(), network::Node::toStr( type ) );
        getStackTopLeafResponse( yield_ctx ).LeafEnrole();
    }

    virtual void TermListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermListNetworkNodes();
        SPDLOG_INFO( "Daemon got root response" );
        getStackTopLeafResponse( yield_ctx ).TermListNetworkNodes( result );
    }

    virtual void TermPipelineRun( const mega::pipeline::Configuration& configuration,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermPipelineRun( configuration );
        getStackTopLeafResponse( yield_ctx ).TermPipelineRun( result );
    }

    virtual void TermGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermGetMegastructureInstallation();
        getStackTopLeafResponse( yield_ctx ).TermGetMegastructureInstallation( result );
    }

    virtual void TermGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermGetProject();
        getStackTopLeafResponse( yield_ctx ).TermGetProject( result );
    }

    virtual void TermSetProject( const mega::network::Project& project, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermSetProject( project );
        getStackTopLeafResponse( yield_ctx ).TermSetProject( result );
    }

    virtual void TermNewInstallation( const mega::network::Project& project,
                                      boost::asio::yield_context&   yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermNewInstallation( project );
        getStackTopLeafResponse( yield_ctx ).TermNewInstallation( result );
    }

    virtual void TermSimNew( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermSimNew();
        getStackTopLeafResponse( yield_ctx ).TermSimNew( result );
    }

    virtual void TermSimList( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermSimList();
        getStackTopLeafResponse( yield_ctx ).TermSimList( result );
    }

    virtual void TermSimReadLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto leaf = leafRequestByCon( simulationID, yield_ctx );
        if ( leaf.has_value() )
            leaf->RootSimReadLock( simulationID );
        else
            getRootRequest( yield_ctx ).TermSimReadLock( simulationID );
        getStackTopLeafResponse( yield_ctx ).TermSimReadLock();
    }

    virtual void TermSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto leaf = leafRequestByCon( simulationID, yield_ctx );
        if ( leaf.has_value() )
            leaf->RootSimWriteLock( simulationID );
        else
            getRootRequest( yield_ctx ).TermSimWriteLock( simulationID );
        getStackTopLeafResponse( yield_ctx ).TermSimWriteLock();
    }

    virtual void ExePipelineReadyForWork( const network::ConversationID& rootConversationID,
                                          boost::asio::yield_context&    yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExePipelineReadyForWork( rootConversationID );
        getStackTopLeafResponse( yield_ctx ).ExePipelineReadyForWork();
    }

    virtual void ExePipelineWorkProgress( const std::string& message, boost::asio::yield_context& yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExePipelineWorkProgress( message );
        getStackTopLeafResponse( yield_ctx ).ExePipelineWorkProgress();
    }

    virtual void ExeGetBuildHashCode( const boost::filesystem::path& filePath,
                                      boost::asio::yield_context&    yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeGetBuildHashCode( filePath );
        getStackTopLeafResponse( yield_ctx ).ExeGetBuildHashCode( result );
    }

    virtual void ExeSetBuildHashCode( const boost::filesystem::path& filePath,
                                      const task::FileHash&          hashCode,
                                      boost::asio::yield_context&    yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExeSetBuildHashCode( filePath, hashCode );
        getStackTopLeafResponse( yield_ctx ).ExeSetBuildHashCode();
    }

    virtual void ExeStash( const boost::filesystem::path& filePath,
                           const task::DeterminantHash&   determinant,
                           boost::asio::yield_context&    yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExeStash( filePath, determinant );
        getStackTopLeafResponse( yield_ctx ).ExeStash();
    }

    virtual void ExeRestore( const boost::filesystem::path& filePath,
                             const task::DeterminantHash&   determinant,
                             boost::asio::yield_context&    yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeRestore( filePath, determinant );
        getStackTopLeafResponse( yield_ctx ).ExeRestore( result );
    }

    virtual void ExeGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeGetMegastructureInstallation();
        getStackTopLeafResponse( yield_ctx ).ExeGetMegastructureInstallation( result );
    }

    virtual void ExeGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeGetProject();
        getStackTopLeafResponse( yield_ctx ).ExeGetProject( result );
    }

    virtual void ExeSimReadLockReady( const mega::TimeStamp& timeStamp, boost::asio::yield_context& yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( getID(), yield_ctx );
        if ( leafRequest.has_value() )
        {
            leafRequest->RootSimReadLockReady( timeStamp );
        }
        else
        {
            getRootRequest( yield_ctx ).ExeSimReadLockReady( timeStamp );
        }
        getStackTopLeafResponse( yield_ctx ).ExeSimReadLockReady();
    }

    virtual void ExeSimWriteLockReady( const mega::TimeStamp&      timeStamp,
                                       boost::asio::yield_context& yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( getID(), yield_ctx );
        if ( leafRequest.has_value() )
        {
            leafRequest->RootSimWriteLockReady( timeStamp );
        }
        else
        {
            getRootRequest( yield_ctx ).ExeSimWriteLockReady( timeStamp );
        }
        getStackTopLeafResponse( yield_ctx ).ExeSimWriteLockReady();
    }

    // root_daemon
    virtual void RootListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        std::vector< std::string > result;
        result.push_back( getProcessName() );
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
            auto                                 r = rq.RootListNetworkNodes();
            std::copy( r.begin(), r.end(), std::back_inserter( result ) );
        }
        getRootResponse( yield_ctx ).RootListNetworkNodes( result );
    }

    virtual void RootPipelineStartJobs( const mega::pipeline::Configuration& configuration,
                                        const network::ConversationID&       rootConversationID,
                                        boost::asio::yield_context&          yield_ctx ) override
    {
        std::vector< network::ConversationID > allJobs;
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getTypeOpt().value() == network::Node::Executor )
            {
                network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
                auto jobs = rq.RootPipelineStartJobs( configuration, rootConversationID );
                std::copy( jobs.begin(), jobs.end(), std::back_inserter( allJobs ) );
            }
        }
        getRootResponse( yield_ctx ).RootPipelineStartJobs( allJobs );
    }

    virtual void RootPipelineStartTask( const mega::pipeline::TaskDescriptor& task,
                                        boost::asio::yield_context&           yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( getID(), yield_ctx );
        VERIFY_RTE( leafRequest.has_value() );
        auto response = leafRequest->RootPipelineStartTask( task );
        getRootResponse( yield_ctx ).RootPipelineStartTask( response );
    }

    virtual void RootProjectUpdated( const mega::network::Project& project,
                                     boost::asio::yield_context&   yield_ctx ) override
    {
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getTypeOpt().value() == network::Node::Executor )
            {
                network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
                rq.RootProjectUpdated( project );
            }
        }
        getRootResponse( yield_ctx ).RootProjectUpdated();
    }

    virtual void RootSimList( boost::asio::yield_context& yield_ctx ) override
    {
        std::vector< network::ConversationID > simulationIDs;

        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getTypeOpt().value() == network::Node::Executor )
            {
                network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
                auto                                 simIDs = rq.RootSimList();
                std::copy( simIDs.begin(), simIDs.end(), std::back_inserter( simulationIDs ) );
            }
        }

        getRootResponse( yield_ctx ).RootSimList( simulationIDs );
    }

    virtual void RootSimCreate( boost::asio::yield_context& yield_ctx ) override
    {
        network::ConversationID simID;
        {
            network::Server::Connection::Ptr pLowestLeaf;
            {
                std::size_t szLowest = std::numeric_limits< std::size_t >::max();
                for ( const auto& [ id, pLeaf ] : m_daemon.m_leafServer.getConnections() )
                {
                    if ( pLeaf->getTypeOpt().value() == network::Node::Executor )
                    {
                        network::daemon_leaf::Request_Encode rq( *this, *pLeaf, yield_ctx );
                        auto                                 simIDs = rq.RootSimList();
                        if ( simIDs.size() < szLowest )
                        {
                            szLowest    = simIDs.size();
                            pLowestLeaf = pLeaf;
                        }
                    }
                }
            }
            if ( pLowestLeaf )
            {
                network::daemon_leaf::Request_Encode rq( *this, *pLowestLeaf, yield_ctx );
                simID = rq.RootSimCreate();
            }
            else
            {
                THROW_RTE( "Daemon: Failed to locate executor to run simulation on" );
            }
        }

        getRootResponse( yield_ctx ).RootSimCreate( simID );
    }

    virtual void RootSimReadLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE_MSG( leafRequest.has_value(), "Failed to locate simulation: " << simulationID );
        leafRequest->RootSimReadLock( simulationID );
        getRootResponse( yield_ctx ).RootSimReadLock();
    }

    virtual void RootSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE_MSG( leafRequest.has_value(), "Failed to locate simulation: " << simulationID );
        leafRequest->RootSimWriteLock( simulationID );
        getRootResponse( yield_ctx ).RootSimWriteLock();
    }

    virtual void RootSimReadLockReady( const mega::TimeStamp&      timeStamp,
                                       boost::asio::yield_context& yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( getID(), yield_ctx );
        leafRequest->RootSimReadLockReady( timeStamp );
        getRootResponse( yield_ctx ).RootSimReadLockReady();
    }

    virtual void RootSimWriteLockReady( const mega::TimeStamp&      timeStamp,
                                        boost::asio::yield_context& yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( getID(), yield_ctx );
        leafRequest->RootSimWriteLockReady( timeStamp );
        getRootResponse( yield_ctx ).RootSimWriteLockReady();
    }

    virtual void ToolGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ToolGetMegastructureInstallation();
        getStackTopLeafResponse( yield_ctx ).ToolGetMegastructureInstallation( result );
    }
};

////////////////////////////////////////////////////////////////
// Daemon
Daemon::Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP )
    : network::ConversationManager( network::makeProcessName( network::Node::Daemon ), ioContext )
    , m_rootClient( ioContext, *this, strRootIP, mega::network::MegaRootServiceName() )
    , m_leafServer( ioContext, *this, network::MegaDaemonPort() )
{
    m_leafServer.waitForConnection();
}

Daemon::~Daemon() { SPDLOG_INFO( "Daemon shutdown" ); }

void Daemon::shutdown()
{
    m_rootClient.stop();
    m_leafServer.stop();
}

network::ConversationBase::Ptr Daemon::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                         const network::Header&       header,
                                                         const network::Message&      msg )
{
    /*switch ( msgHeader.getMessageID() )
    {
        default:
    }*/
    return network::ConversationBase::Ptr(
        new DaemonRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
}

void Daemon::conversationNew( const network::Header& header, const network::ReceivedMsg& msg )
{
    auto pCon = m_leafServer.getConnection( msg.connectionID );
    VERIFY_RTE( pCon );
    pCon->conversationNew( header.getConversationID() );
    boost::asio::spawn(
        m_ioContext,
        [ &m_rootClient = m_rootClient, header, message = msg.msg ]( boost::asio::yield_context yield_ctx )
        { m_rootClient.send( header.getConversationID(), message, yield_ctx ); } );
}

void Daemon::conversationEnd( const network::Header& header, const network::ReceivedMsg& msg )
{
    auto pCon = m_leafServer.getConnection( msg.connectionID );
    VERIFY_RTE( pCon );
    pCon->conversationEnd( header.getConversationID() );
    boost::asio::spawn(
        m_ioContext,
        [ &m_rootClient = m_rootClient, header, message = msg.msg ]( boost::asio::yield_context yield_ctx )
        { m_rootClient.send( header.getConversationID(), message, yield_ctx ); } );
}

} // namespace service
} // namespace mega