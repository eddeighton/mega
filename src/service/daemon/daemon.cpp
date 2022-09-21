
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
        // SPDLOG_TRACE( "Leaf {} enroled as {}", pConnection->getName(), network::Node::toStr( type ) );

        const mega::MPO leafMPO = getRootRequest( yield_ctx ).DaemonLeafEnrole( m_daemon.m_mpo );

        pConnection->setMPO( leafMPO );
        pConnection->setDisconnectCallback( [ leafMPO, &daemon = m_daemon ]( const network::ConnectionID& connectionID )
                                            { daemon.onLeafDisconnect( connectionID, leafMPO ); } );

        getStackTopLeafResponse( yield_ctx ).LeafEnrole( leafMPO );
    }

    virtual void TermListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermListNetworkNodes();
        // SPDLOG_TRACE( "Daemon got root response" );
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

    virtual void TermSimDestroy( const mega::network::ConversationID& simulationID,
                                 boost::asio::yield_context&          yield_ctx ) override
    {
        getRootRequest( yield_ctx ).TermSimDestroy( simulationID );
        getStackTopLeafResponse( yield_ctx ).TermSimDestroy();
    }

    virtual void TermSimList( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermSimList();
        getStackTopLeafResponse( yield_ctx ).TermSimList( result );
    }

    virtual void TermSimReadLock( const mega::network::ConversationID& owningID,
                                  const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto leaf = leafRequestByCon( simulationID, yield_ctx );
        bool bResult;
        if ( leaf.has_value() )
            bResult = leaf->RootSimReadLock( owningID, simulationID );
        else
            bResult = getRootRequest( yield_ctx ).TermSimReadLock( owningID, simulationID );
        getStackTopLeafResponse( yield_ctx ).TermSimReadLock( bResult );
    }

    virtual void TermSimWriteLock( const mega::network::ConversationID& owningID,
                                   const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto leaf = leafRequestByCon( simulationID, yield_ctx );
        bool bResult;
        if ( leaf.has_value() )
            bResult = leaf->RootSimWriteLock( owningID, simulationID );
        else
            bResult = getRootRequest( yield_ctx ).TermSimWriteLock( owningID, simulationID );
        getStackTopLeafResponse( yield_ctx ).TermSimWriteLock( bResult );
    }

    virtual void TermSimReleaseLock( const mega::network::ConversationID& owningID,
                                     const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
    {
        auto leaf = leafRequestByCon( simulationID, yield_ctx );
        if ( leaf.has_value() )
            leaf->RootSimReleaseLock( owningID, simulationID );
        else
            getRootRequest( yield_ctx ).TermSimReleaseLock( owningID, simulationID );
        getStackTopLeafResponse( yield_ctx ).TermSimReleaseLock();
    }

    virtual void TermClearStash( boost::asio::yield_context& yield_ctx ) override
    {
        getRootRequest( yield_ctx ).TermClearStash();
        getStackTopLeafResponse( yield_ctx ).TermClearStash();
    }

    virtual void TermCapacity( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).TermCapacity();
        getStackTopLeafResponse( yield_ctx ).TermCapacity( result );
    }

    virtual void TermShutdown( boost::asio::yield_context& yield_ctx ) override
    {
        getRootRequest( yield_ctx ).TermShutdown();
        getStackTopLeafResponse( yield_ctx ).TermShutdown();
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

    virtual void ExeCreateMPO( const mega::MPO& leafMPO, boost::asio::yield_context& yield_ctx ) override
    {
        const mega::MPO daemonLeafMPO( m_daemon.m_mpo.getMachineID(), leafMPO.getProcessID(), 0U );
        auto            result = getRootRequest( yield_ctx ).ExeCreateMPO( daemonLeafMPO );
        getStackTopLeafResponse( yield_ctx ).ExeCreateMPO( result );
    }

    virtual void ExeAcquireMemory( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        const network::ConversationID conversationID = getRootRequest( yield_ctx ).DaemonGetMPOContextID( mpo );
        const std::string             strMemory      = m_daemon.m_sharedMemoryManager.acquire( conversationID );
        getStackTopLeafResponse( yield_ctx ).ExeAcquireMemory( strMemory );
    }

    virtual void ExeGetRootNetworkAddress( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeGetRootNetworkAddress( mpo );
        getStackTopLeafResponse( yield_ctx ).ExeGetRootNetworkAddress( result );
    }
    virtual void ExeGetNetworkAddressMPO( const mega::AddressStorage& networkAddress,
                                          boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeGetNetworkAddressMPO( networkAddress );
        getStackTopLeafResponse( yield_ctx ).ExeGetNetworkAddressMPO( result );
    }
    virtual void ExeAllocateNetworkAddress( const mega::MPO&            mpo,
                                            const mega::TypeID&         objectTypeID,
                                            boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ExeAllocateNetworkAddress( mpo, objectTypeID );
        getStackTopLeafResponse( yield_ctx ).ExeAllocateNetworkAddress( result );
    }

    virtual void ExeDeAllocateNetworkAddress( const mega::MPO&            mpo,
                                              const mega::AddressStorage& networkAddress,
                                              boost::asio::yield_context& yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ExeDeAllocateNetworkAddress( mpo, networkAddress );
        getStackTopLeafResponse( yield_ctx ).ExeDeAllocateNetworkAddress();
    }

    virtual void ExeGetMPOContextID( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).DaemonGetMPOContextID( mpo );
        getStackTopLeafResponse( yield_ctx ).ExeGetMPOContextID( result );
    }

    virtual void ExeSimReadLock( const mega::network::ConversationID& simulationID,
                                 boost::asio::yield_context&          yield_ctx ) override
    {
        auto leaf = leafRequestByCon( simulationID, yield_ctx );
        bool bResult;
        if ( leaf.has_value() )
            bResult = leaf->RootSimReadLock( getID(), simulationID );
        else
            bResult = getRootRequest( yield_ctx ).ExeSimReadLock( simulationID );
        getStackTopLeafResponse( yield_ctx ).ExeSimReadLock( bResult );
    }

    virtual void ExeSimWriteLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto leaf = leafRequestByCon( simulationID, yield_ctx );
        bool bResult;
        if ( leaf.has_value() )
            bResult = leaf->RootSimWriteLock( getID(), simulationID );
        else
            bResult = getRootRequest( yield_ctx ).ExeSimWriteLock( simulationID );
        getStackTopLeafResponse( yield_ctx ).ExeSimWriteLock( bResult );
    }

    virtual void ExeSimReleaseLock( const mega::network::ConversationID& simulationID,
                                    boost::asio::yield_context&          yield_ctx ) override
    {
        auto leaf = leafRequestByCon( simulationID, yield_ctx );
        if ( leaf.has_value() )
            leaf->RootSimReleaseLock( getID(), simulationID );
        else
            getRootRequest( yield_ctx ).ExeSimReleaseLock( simulationID );
        getStackTopLeafResponse( yield_ctx ).ExeSimReleaseLock();
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

    virtual void RootPipelineStartJobs( const mega::utilities::ToolChain&    toolChain,
                                        const mega::pipeline::Configuration& configuration,
                                        const network::ConversationID&       rootConversationID,
                                        boost::asio::yield_context&          yield_ctx ) override
    {
        std::vector< network::ConversationID > allJobs;
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            if ( pConnection->getTypeOpt().value() == network::Node::Executor )
            {
                network::daemon_leaf::Request_Encode rq( *this, *pConnection, yield_ctx );
                auto jobs = rq.RootPipelineStartJobs( toolChain, configuration, rootConversationID );
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
                mega::U64 szLowest = std::numeric_limits< mega::U64 >::max();
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

    virtual void RootSimDestroy( const mega::network::ConversationID& simulationID,
                                 boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE_MSG( leafRequest.has_value(), "Failed to locate simulation: " << simulationID );
        leafRequest->RootSimDestroy( simulationID );
        getRootResponse( yield_ctx ).RootSimDestroy();
    }

    virtual void RootSimReadLock( const mega::network::ConversationID& owningID,
                                  const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE_MSG( leafRequest.has_value(), "Failed to locate simulation: " << simulationID );
        const bool bResult = leafRequest->RootSimReadLock( owningID, simulationID );
        getRootResponse( yield_ctx ).RootSimReadLock( bResult );
    }

    virtual void RootSimWriteLock( const mega::network::ConversationID& owningID,
                                   const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE_MSG( leafRequest.has_value(), "Failed to locate simulation: " << simulationID );
        const bool bResult = leafRequest->RootSimWriteLock( owningID, simulationID );
        getRootResponse( yield_ctx ).RootSimWriteLock( bResult );
    }

    virtual void RootSimReleaseLock( const mega::network::ConversationID& owningID,
                                     const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( getID(), yield_ctx );
        leafRequest->RootSimReleaseLock( owningID, simulationID );
        getRootResponse( yield_ctx ).RootSimReleaseLock();
    }

    virtual void RootShutdown( boost::asio::yield_context& yield_ctx ) override
    {
        for ( const auto& [ id, pLeaf ] : m_daemon.m_leafServer.getConnections() )
        {
            network::daemon_leaf::Request_Encode rq( *this, *pLeaf, yield_ctx );
            rq.RootShutdown();
        }
        getRootResponse( yield_ctx ).RootShutdown();

        boost::asio::post( [ &daemon = m_daemon ]() { daemon.shutdown(); } );
    }

    virtual void ToolGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ToolGetMegastructureInstallation();
        getStackTopLeafResponse( yield_ctx ).ToolGetMegastructureInstallation( result );
    }
    virtual void ToolCreateMPO( const mega::MPO& leafMPO, boost::asio::yield_context& yield_ctx ) override
    {
        const mega::MPO daemonLeafMPO( m_daemon.m_mpo.getMachineID(), leafMPO.getProcessID(), 0U );
        auto            result = getRootRequest( yield_ctx ).ToolCreateMPO( daemonLeafMPO );
        getStackTopLeafResponse( yield_ctx ).ToolCreateMPO( result );
    }

    virtual void ToolAcquireMemory( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        const network::ConversationID conversationID = getRootRequest( yield_ctx ).DaemonGetMPOContextID( mpo );
        const std::string             strMemory      = m_daemon.m_sharedMemoryManager.acquire( conversationID );
        getStackTopLeafResponse( yield_ctx ).ToolAcquireMemory( strMemory );
    }
    virtual void ToolGetRootNetworkAddress( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ToolGetRootNetworkAddress( mpo );
        getStackTopLeafResponse( yield_ctx ).ToolGetRootNetworkAddress( result );
    }
    virtual void ToolGetNetworkAddressMPO( const mega::AddressStorage& networkAddress,
                                           boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ToolGetNetworkAddressMPO( networkAddress );
        getStackTopLeafResponse( yield_ctx ).ToolGetNetworkAddressMPO( result );
    }
    virtual void ToolAllocateNetworkAddress( const mega::MPO&            mpo,
                                             const mega::TypeID&         objectTypeID,
                                             boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ToolAllocateNetworkAddress( mpo, objectTypeID );
        getStackTopLeafResponse( yield_ctx ).ToolAllocateNetworkAddress( result );
    }

    virtual void ToolDeAllocateNetworkAddress( const mega::MPO&            mpo,
                                               const mega::AddressStorage& networkAddress,
                                               boost::asio::yield_context& yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ToolDeAllocateNetworkAddress( mpo, networkAddress );
        getStackTopLeafResponse( yield_ctx ).ToolDeAllocateNetworkAddress();
    }
    virtual void ToolStash( const boost::filesystem::path& filePath,
                            const task::DeterminantHash&   determinant,
                            boost::asio::yield_context&    yield_ctx ) override
    {
        getRootRequest( yield_ctx ).ToolStash( filePath, determinant );
        getStackTopLeafResponse( yield_ctx ).ToolStash();
    }
    virtual void ToolRestore( const boost::filesystem::path& filePath,
                              const task::DeterminantHash&   determinant,
                              boost::asio::yield_context&    yield_ctx ) override
    {
        const bool bRestored = getRootRequest( yield_ctx ).ToolRestore( filePath, determinant );
        getStackTopLeafResponse( yield_ctx ).ToolRestore( bRestored );
    }

    virtual void ToolGetMPOContextID( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).DaemonGetMPOContextID( mpo );
        getStackTopLeafResponse( yield_ctx ).ToolGetMPOContextID( result );
    }
    virtual void ToolGetMPO( const mega::network::ConversationID& conversationID,
                             boost::asio::yield_context&          yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).DaemonGetMPO( conversationID );
        getStackTopLeafResponse( yield_ctx ).ToolGetMPO( result );
    }
    virtual void ToolSimReadLock( const mega::network::ConversationID& owningID,
                                  const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( simulationID, yield_ctx );
        bool bResult;
        if ( leafRequest.has_value() )
        {
            bResult = leafRequest->RootSimReadLock( owningID, simulationID );
        }
        else
        {
            bResult = getRootRequest( yield_ctx ).ToolSimReadLock( owningID, simulationID );
        }
        getStackTopLeafResponse( yield_ctx ).ToolSimReadLock( bResult );
    }
    virtual void ToolSimWriteLock( const mega::network::ConversationID& owningID,
                                   const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( simulationID, yield_ctx );
        bool bResult;
        if ( leafRequest.has_value() )
        {
            bResult = leafRequest->RootSimWriteLock( owningID, simulationID );
        }
        else
        {
            bResult = getRootRequest( yield_ctx ).ToolSimWriteLock( owningID, simulationID );
        }
        getStackTopLeafResponse( yield_ctx ).ToolSimWriteLock( bResult );
    }
    virtual void ToolSimReleaseLock( const mega::network::ConversationID& owningID,
                                     const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
    {
        auto leafRequest = leafRequestByCon( simulationID, yield_ctx );
        if ( leafRequest.has_value() )
        {
            leafRequest->RootSimReleaseLock( owningID, simulationID );
        }
        else
        {
            getRootRequest( yield_ctx ).ToolSimReleaseLock( owningID, simulationID );
        }
        getStackTopLeafResponse( yield_ctx ).ToolSimReleaseLock();
    }
    virtual void ToolSimList( boost::asio::yield_context& yield_ctx ) override
    {
        auto result = getRootRequest( yield_ctx ).ToolSimList();
        getStackTopLeafResponse( yield_ctx ).ToolSimList( result );
    }
};

////////////////////////////////////////////////////////////////
class DaemonEnrole : public DaemonRequestConversation
{
    std::promise< void >& m_promise;

public:
    DaemonEnrole( Daemon& daemon, const network::ConnectionID& originatingConnectionID, std::promise< void >& promise )
        : DaemonRequestConversation(
            daemon, daemon.createConversationID( originatingConnectionID ), originatingConnectionID )
        , m_promise( promise )
    {
    }

    void run( boost::asio::yield_context& yield_ctx )
    {
        // SPDLOG_TRACE( "DaemonEnrole" );
        m_daemon.m_mpo = getRootRequest( yield_ctx ).DaemonEnrole();
        // SPDLOG_TRACE( "Daemon enroled with mpo: {}", m_daemon.m_mpo );
        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
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

    // immediately enrole
    {
        std::promise< void >            promise;
        std::future< void >             future = promise.get_future();
        std::shared_ptr< DaemonEnrole > pEnrole
            = std::make_shared< DaemonEnrole >( *this, m_rootClient.getConnectionID(), promise );
        conversationInitiated( pEnrole, m_rootClient );

        using namespace std::chrono_literals;
        while ( std::future_status::timeout == future.wait_for( 0s ) )
        {
            ioContext.run_one();
        }
    }
}

Daemon::~Daemon()
{
    //
    // SPDLOG_TRACE( "Daemon shutdown" );
}

void Daemon::onLeafDisconnect( const network::ConnectionID& connectionID, mega::MPO mpo )
{
    onDisconnect( connectionID );

    class DaemonLeafDisconnect : public DaemonRequestConversation
    {
        mega::MPO m_leafMPO;

    public:
        DaemonLeafDisconnect( Daemon& daemon, const network::ConnectionID& originatingConnectionID, mega::MPO leafMPO )
            : DaemonRequestConversation(
                daemon, daemon.createConversationID( originatingConnectionID ), originatingConnectionID )
            , m_leafMPO( leafMPO )
        {
        }
        void run( boost::asio::yield_context& yield_ctx )
        {
            getRootRequest( yield_ctx ).DaemonLeafDisconnect( m_leafMPO );
        }
    };
    conversationInitiated(
        std::make_shared< DaemonLeafDisconnect >( *this, m_rootClient.getConnectionID(), mpo ), m_rootClient );
}

void Daemon::shutdown()
{
    m_rootClient.stop();
    m_leafServer.stop();
}

network::ConversationBase::Ptr Daemon::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                         const network::Header&       header,
                                                         const network::Message&      msg )
{
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

    m_sharedMemoryManager.release( header.getConversationID() );

    boost::asio::spawn(
        m_ioContext,
        [ &m_rootClient = m_rootClient, header, message = msg.msg ]( boost::asio::yield_context yield_ctx )
        { m_rootClient.send( header.getConversationID(), message, yield_ctx ); } );
}

} // namespace service
} // namespace mega