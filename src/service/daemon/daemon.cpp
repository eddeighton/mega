
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
    network::leaf_daemon::Response_Encode getOriginatingLeafResponse( boost::asio::yield_context& yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_leafServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::leaf_daemon::Response_Encode( *this, *pConnection, yield_ctx );
        }
        THROW_RTE( "Daemon: Could not locate originating connection" );
    }
    network::daemon_leaf::Request_Encode getOriginatingLeafRequest( boost::asio::yield_context& yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_leafServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::daemon_leaf::Request_Encode( *this, *pConnection, yield_ctx );
        }
        THROW_RTE( "Daemon: Could not locate originating connection" );
    }

    // leaf_daemon
    virtual void LeafEnrole( const mega::network::Node::Type& type, boost::asio::yield_context& yield_ctx ) override
    {
        network::Server::Connection::Ptr pConnection
            = m_daemon.m_leafServer.getConnection( getOriginatingEndPointID().value() );
        VERIFY_RTE( pConnection );
        pConnection->setType( type );
        SPDLOG_INFO( "Leaf {} enroled as {}", pConnection->getName(), network::Node::toStr( type ) );
        getOriginatingLeafResponse( yield_ctx ).LeafEnrole();
    }

    virtual void TermListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermListNetworkNodes();
        getOriginatingLeafResponse( yield_ctx ).TermListNetworkNodes( result );
    }

    virtual void TermPipelineRun( const mega::pipeline::Configuration& configuration,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermPipelineRun( configuration );
        getOriginatingLeafResponse( yield_ctx ).TermPipelineRun( result );
    }

    virtual void TermGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermGetProject();
        getOriginatingLeafResponse( yield_ctx ).TermGetProject( result );
    }

    virtual void TermSetProject( const mega::network::Project& project, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermSetProject( project );
        getOriginatingLeafResponse( yield_ctx ).TermSetProject( result );
    }

    virtual void TermNewInstallation( const mega::network::Project& project,
                                      boost::asio::yield_context&   yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermNewInstallation( project );
        getOriginatingLeafResponse( yield_ctx ).TermNewInstallation( result );
    }

    virtual void TermSimNew( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermSimNew();
        getOriginatingLeafResponse( yield_ctx ).TermSimNew( result );
    }

    virtual void TermSimList( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermSimList();
        getOriginatingLeafResponse( yield_ctx ).TermSimList( result );
    }

    virtual void TermSimReadLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        std::optional< mega::TimeStamp > result;
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getSimulations().count( simulationID ) )
            {
                network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
                result = rq.RootSimReadLock( simulationID );
                break;
            }
        }
        if ( !result.has_value() )
        {
            result = getRootRequest( yield_ctx ).TermSimReadLock( simulationID );
        }
        getOriginatingLeafResponse( yield_ctx ).TermSimReadLock( result.value() );
    }

    virtual void TermSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        std::optional< mega::TimeStamp > result;
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getSimulations().count( simulationID ) )
            {
                network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
                result = rq.RootSimWriteLock( simulationID );
                break;
            }
        }
        if ( !result.has_value() )
        {
            result = getRootRequest( yield_ctx ).TermSimWriteLock( simulationID );
        }
        getOriginatingLeafResponse( yield_ctx ).TermSimWriteLock( result.value() );
    }

    virtual void ExePipelineReadyForWork( const network::ConversationID& rootConversationID,
                                          boost::asio::yield_context&    yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExePipelineReadyForWork( rootConversationID );
        getOriginatingLeafResponse( yield_ctx ).ExePipelineReadyForWork();
    }

    virtual void ExePipelineWorkProgress( const std::string& message, boost::asio::yield_context& yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExePipelineWorkProgress( message );
        getOriginatingLeafResponse( yield_ctx ).ExePipelineWorkProgress();
    }

    virtual void ExeGetBuildHashCode( const boost::filesystem::path& filePath,
                                      boost::asio::yield_context&    yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeGetBuildHashCode( filePath );
        getOriginatingLeafResponse( yield_ctx ).ExeGetBuildHashCode( result );
    }

    virtual void ExeSetBuildHashCode( const boost::filesystem::path& filePath,
                                      const task::FileHash&          hashCode,
                                      boost::asio::yield_context&    yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExeSetBuildHashCode( filePath, hashCode );
        getOriginatingLeafResponse( yield_ctx ).ExeSetBuildHashCode();
    }

    virtual void ExeStash( const boost::filesystem::path& filePath,
                           const task::DeterminantHash&   determinant,
                           boost::asio::yield_context&    yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExeStash( filePath, determinant );
        getOriginatingLeafResponse( yield_ctx ).ExeStash();
    }

    virtual void ExeRestore( const boost::filesystem::path& filePath,
                             const task::DeterminantHash&   determinant,
                             boost::asio::yield_context&    yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeRestore( filePath, determinant );
        getOriginatingLeafResponse( yield_ctx ).ExeRestore( result );
    }

    virtual void ExeGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeGetProject();
        getOriginatingLeafResponse( yield_ctx ).ExeGetProject( result );
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
        auto response = getOriginatingLeafRequest( yield_ctx ).RootPipelineStartTask( task );
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
                pLowestLeaf->addSimulation( simID );
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
        std::optional< mega::TimeStamp > result;
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getTypeOpt().value() == network::Node::Executor )
            {
                if ( pConnection->getSimulations().count( simulationID ) )
                {
                    network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
                    result = rq.RootSimReadLock( simulationID );
                    break;
                }
            }
        }
        VERIFY_RTE_MSG( result, "Failed to locate simulation: " << simulationID );
        getRootResponse( yield_ctx ).RootSimReadLock( result.value() );
    }

    virtual void RootSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        std::optional< mega::TimeStamp > result;
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getTypeOpt().value() == network::Node::Executor )
            {
                if ( pConnection->getSimulations().count( simulationID ) )
                {
                    network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
                    result = rq.RootSimWriteLock( simulationID );
                    break;
                }
            }
        }
        VERIFY_RTE_MSG( result, "Failed to locate simulation: " << simulationID );
        getRootResponse( yield_ctx ).RootSimWriteLock( result.value() );
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
} // namespace service
} // namespace mega