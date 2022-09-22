#ifndef REQUEST_22_SEPT_2022
#define REQUEST_22_SEPT_2022

#include "service/network/conversation.hpp"
#include "service/network/server.hpp"

#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/enrole.hxx"

namespace mega
{
namespace service
{
class Root;

class RootRequestConversation : public network::InThreadConversation,
                                public network::daemon_root::Impl,
                                public network::project::Impl,
                                public network::enrole::Impl
{
protected:
    Root& m_root;

public:
    RootRequestConversation( Root&                          root,
                             const network::ConversationID& conversationID,
                             const network::ConnectionID&   originatingConnectionID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;
    virtual void             dispatchResponse( const network::ConnectionID& connectionID,
                                               const network::Message&      msg,
                                               boost::asio::yield_context&  yield_ctx ) override;
    virtual void             error( const network::ConnectionID& connectionID,
                                    const std::string&           strErrorMsg,
                                    boost::asio::yield_context&  yield_ctx ) override;

    //network::root_daemon::Request_Sender getDaemonRequest( network::Server::Connection::Ptr pConnection,
    //                                                       boost::asio::yield_context&      yield_ctx );

    // network::daemon_root::Impl
    virtual network::Message TermRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ExeRoot( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ToolRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message LeafRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message DaemonRoot( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;

    // network::project::Impl
    virtual std::string GetVersion( boost::asio::yield_context& yield_ctx ) override;

    // network::enrole::Impl
    virtual MPO  EnroleDaemon( boost::asio::yield_context& yield_ctx ) override;
    virtual MPO  EnroleLeafWithRoot( const MPO& daemonMPO, boost::asio::yield_context& yield_ctx ) override;
    virtual void EnroleLeafDisconnect( const MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::daemon_root::Impl
    /*
        virtual void DaemonGetMPOContextID( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
        {
            const network::ConversationID& conversationID = m_root.m_mpoManager.get( mpo );
            getStackTopDaemonResponse( yield_ctx ).DaemonGetMPOContextID( conversationID );
        }
        virtual void DaemonGetMPO( const mega::network::ConversationID& conversationID,
                                   boost::asio::yield_context&          yield_ctx ) override
        {
            const mega::MPO mpo = m_root.m_mpoManager.get( conversationID );
            getStackTopDaemonResponse( yield_ctx ).DaemonGetMPO( mpo );
        }

        virtual void TermListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
        {
            std::vector< std::string > nodes;
            nodes.push_back( getProcessName() );
            {
                for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
                {
                    network::root_daemon::Request_Encode rq( *this, *pConnection, yield_ctx );
                    auto                                 r = rq.RootListNetworkNodes();
                    std::copy( r.begin(), r.end(), std::back_inserter( nodes ) );
                }
            }
            getStackTopDaemonResponse( yield_ctx ).TermListNetworkNodes( nodes );
        }

        virtual void TermGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
        {
            getStackTopDaemonResponse( yield_ctx )
                .TermGetMegastructureInstallation( m_root.getMegastructureInstallation() );
        }

        virtual void TermGetProject( boost::asio::yield_context& yield_ctx ) override
        {
            getStackTopDaemonResponse( yield_ctx ).TermGetProject( m_root.getProject() );
        }

        virtual void TermSetProject( const mega::network::Project& project, boost::asio::yield_context& yield_ctx )
       override
        {
            auto daemon = getStackTopDaemonResponse( yield_ctx );

            if ( project.getProjectInstallPath() != m_root.getProject().getProjectInstallPath() )
            {
                m_root.setProject( project );
                m_root.saveConfig();

                // notify all executors of the project change
                for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
                {
                    network::root_daemon::Request_Encode rq( *this, *pConnection, yield_ctx );
                    rq.RootProjectUpdated( project );
                }

                daemon.TermSetProject( true );
            }
            else
            {
                daemon.TermSetProject( false );
            }
        }

        virtual void TermNewInstallation( const mega::network::Project& project,
                                          boost::asio::yield_context&   yield_ctx ) override
        {
            bool bSuccess = true;
            try
            {
                // notify all executors of the project change
                for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
                {
                    network::root_daemon::Request_Encode rq( *this, *pConnection, yield_ctx );
                    rq.RootProjectUpdated( project );
                }
            }
            catch ( std::exception& ex )
            {
                SPDLOG_ERROR( "Exception attempting to roll out project installation: {} : {}",
                              project.getProjectInstallPath().string(), ex.what() );
                bSuccess = false;
            }

            getStackTopDaemonResponse( yield_ctx ).TermNewInstallation( bSuccess );
        }

        virtual void TermSimNew( boost::asio::yield_context& yield_ctx ) override
        {
            network::ConversationID simID;
            {
                network::Server::Connection::Ptr pLowestDaemon;
                {
                    mega::U64 szLowest = std::numeric_limits< mega::U64 >::max();
                    for ( const auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
                    {
                        auto simIDs = getDaemonRequest( pDaemon, yield_ctx ).RootSimList();
                        if ( simIDs.size() < szLowest )
                        {
                            szLowest      = simIDs.size();
                            pLowestDaemon = pDaemon;
                        }
                    }
                }
                if ( pLowestDaemon )
                {
                    simID = getDaemonRequest( pLowestDaemon, yield_ctx ).RootSimCreate();
                }
                else
                {
                    THROW_RTE( "Root: Failed to locate daemon to run simulation on" );
                }
            }

            getStackTopDaemonResponse( yield_ctx ).TermSimNew( simID );
        }

        virtual void TermSimDestroy( const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
        {
            const mega::MPO mpo = m_root.m_mpoManager.get( simulationID );

            network::Server::Connection::Ptr pOwningDaemon;
            {
                for ( const auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
                {
                    if ( pDaemon->getMPOOpt().has_value() )
                    {
                        if ( pDaemon->getMPOOpt().value().getMachineID() == mpo.getMachineID() )
                        {
                            pOwningDaemon = pDaemon;
                            break;
                        }
                    }
                }
            }
            VERIFY_RTE_MSG( pOwningDaemon, "Failed to locate daemon owning simulationID: " << simulationID );

            getDaemonRequest( pOwningDaemon, yield_ctx ).RootSimDestroy( simulationID );

            getStackTopDaemonResponse( yield_ctx ).TermSimDestroy();
        }

        virtual void TermSimList( boost::asio::yield_context& yield_ctx ) override
        {
            auto result = getSimulationIDs( yield_ctx );
            getStackTopDaemonResponse( yield_ctx ).TermSimList( result );
        }

        virtual void TermSimReadLock( const mega::network::ConversationID& owningID,
                                      const mega::network::ConversationID& simulationID,
                                      boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            auto result = pDaemon->RootSimReadLock( owningID, simulationID );
            getStackTopDaemonResponse( yield_ctx ).TermSimReadLock( result );
        }

        virtual void TermSimWriteLock( const mega::network::ConversationID& owningID,
                                       const mega::network::ConversationID& simulationID,
                                       boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            auto result = pDaemon->RootSimWriteLock( owningID, simulationID );
            getStackTopDaemonResponse( yield_ctx ).TermSimWriteLock( result );
        }

        virtual void TermSimReleaseLock( const mega::network::ConversationID& owningID,
                                         const mega::network::ConversationID& simulationID,
                                         boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            pDaemon->RootSimReleaseLock( owningID, simulationID );
            getStackTopDaemonResponse( yield_ctx ).TermSimReleaseLock();
        }

        virtual void TermClearStash( boost::asio::yield_context& yield_ctx ) override
        {
            m_root.m_stash.clear();
            getStackTopDaemonResponse( yield_ctx ).TermClearStash();
        }

        virtual void TermCapacity( boost::asio::yield_context& yield_ctx ) override
        {
            auto capacity = m_root.m_logicalAddressSpace.getCapacity();
            getStackTopDaemonResponse( yield_ctx ).TermCapacity( capacity );
        }

        virtual void TermShutdown( boost::asio::yield_context& yield_ctx ) override
        {
            getStackTopDaemonResponse( yield_ctx ).TermShutdown();

            for ( const auto& [ id, pDeamon ] : m_root.m_server.getConnections() )
            {
                getDaemonRequest( pDeamon, yield_ctx ).RootShutdown();
            }

            boost::asio::post( [ &root = m_root ]() { root.shutdown(); } );
        }

        // pipeline::Stash
        virtual void ExeGetBuildHashCode( const boost::filesystem::path& filePath,
                                          boost::asio::yield_context&    yield_ctx ) override
        {
            auto daemon = getStackTopDaemonResponse( yield_ctx );
            daemon.ExeGetBuildHashCode( m_root.m_buildHashCodes.get( filePath ) );
        }

        virtual void ExeSetBuildHashCode( const boost::filesystem::path& filePath, const task::FileHash& hashCode,
                                          boost::asio::yield_context& yield_ctx ) override
        {
            m_root.m_buildHashCodes.set( filePath, hashCode );
            auto daemon = getStackTopDaemonResponse( yield_ctx );
            daemon.ExeSetBuildHashCode();
        }

        virtual void ExeStash( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                               boost::asio::yield_context& yield_ctx ) override
        {
            m_root.m_stash.stash( filePath, determinant );
            auto daemon = getStackTopDaemonResponse( yield_ctx );
            daemon.ExeStash();
        }

        virtual void ExeRestore( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                                 boost::asio::yield_context& yield_ctx ) override
        {
            const bool bRestored = m_root.m_stash.restore( filePath, determinant );
            auto       daemon    = getStackTopDaemonResponse( yield_ctx );
            daemon.ExeRestore( bRestored );
        }

        virtual void ExeGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
        {
            auto daemon = getStackTopDaemonResponse( yield_ctx );
            daemon.ExeGetMegastructureInstallation( m_root.getMegastructureInstallation() );
        }

        virtual void ExeGetProject( boost::asio::yield_context& yield_ctx ) override
        {
            auto daemon = getStackTopDaemonResponse( yield_ctx );
            daemon.ExeGetProject( m_root.getProject() );
        }

        virtual void ExeCreateMPO( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
        {
            auto result = m_root.m_mpoManager.newOwner( mpo, getID() );
            getStackTopDaemonResponse( yield_ctx ).ExeCreateMPO( result );
        }

        virtual void ToolGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
        {
            auto daemon = getStackTopDaemonResponse( yield_ctx );
            daemon.ToolGetMegastructureInstallation( m_root.getMegastructureInstallation() );
        }
        virtual void ExeGetRootNetworkAddress( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
        {
            const auto result = m_root.m_logicalAddressSpace.getRootNetworkAddress( mpo );
            getStackTopDaemonResponse( yield_ctx ).ExeAllocateNetworkAddress( result );
        }
        virtual void ExeGetNetworkAddressMPO( const mega::AddressStorage& networkAddress,
                                              boost::asio::yield_context& yield_ctx ) override
        {
            const auto result = m_root.m_logicalAddressSpace.getNetworkAddressMPO( networkAddress );
            getStackTopDaemonResponse( yield_ctx ).ExeGetNetworkAddressMPO( result );
        }
        virtual void ExeAllocateNetworkAddress( const mega::MPO&            mpo,
                                                const mega::TypeID&         objectTypeID,
                                                boost::asio::yield_context& yield_ctx ) override
        {
            const NetworkAddress result = m_root.m_logicalAddressSpace.allocateNetworkAddress( mpo, objectTypeID );
            getStackTopDaemonResponse( yield_ctx ).ExeAllocateNetworkAddress( result );
        }

        virtual void ExeDeAllocateNetworkAddress( const mega::MPO&            mpo,
                                                  const mega::AddressStorage& networkAddress,
                                                  boost::asio::yield_context& yield_ctx ) override
        {
            m_root.m_logicalAddressSpace.deAllocateNetworkAddress( mpo, networkAddress );
            getStackTopDaemonResponse( yield_ctx ).ExeDeAllocateNetworkAddress();
        }
        virtual void ExeSimReadLock( const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            auto result = pDaemon->RootSimReadLock( getID(), simulationID );
            getStackTopDaemonResponse( yield_ctx ).ExeSimReadLock( result );
        }

        virtual void ExeSimWriteLock( const mega::network::ConversationID& simulationID,
                                      boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            auto result = pDaemon->RootSimWriteLock( getID(), simulationID );
            getStackTopDaemonResponse( yield_ctx ).ExeSimWriteLock( result );
        }

        virtual void ExeSimReleaseLock( const mega::network::ConversationID& simulationID,
                                        boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            pDaemon->RootSimReleaseLock( getID(), simulationID );
            getStackTopDaemonResponse( yield_ctx ).ExeSimReleaseLock();
        }

        virtual void ToolCreateMPO( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
        {
            auto result = m_root.m_mpoManager.newOwner( mpo, getID() );
            getStackTopDaemonResponse( yield_ctx ).ToolCreateMPO( result );
        }

        virtual void ToolGetRootNetworkAddress( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
        {
            auto result = m_root.m_logicalAddressSpace.getRootNetworkAddress( mpo );
            getStackTopDaemonResponse( yield_ctx ).ToolGetRootNetworkAddress( result );
        }
        virtual void ToolGetNetworkAddressMPO( const mega::AddressStorage& networkAddress,
                                               boost::asio::yield_context& yield_ctx ) override
        {
            auto result = m_root.m_logicalAddressSpace.getNetworkAddressMPO( networkAddress );
            getStackTopDaemonResponse( yield_ctx ).ToolGetNetworkAddressMPO( result );
        }
        virtual void ToolAllocateNetworkAddress( const mega::MPO&            mpo,
                                                 const mega::TypeID&         objectTypeID,
                                                 boost::asio::yield_context& yield_ctx ) override
        {
            const NetworkAddress result = m_root.m_logicalAddressSpace.allocateNetworkAddress( mpo, objectTypeID );
            getStackTopDaemonResponse( yield_ctx ).ToolAllocateNetworkAddress( result );
        }

        virtual void ToolDeAllocateNetworkAddress( const mega::MPO&            mpo,
                                                   const mega::AddressStorage& networkAddress,
                                                   boost::asio::yield_context& yield_ctx ) override
        {
            m_root.m_logicalAddressSpace.deAllocateNetworkAddress( mpo, networkAddress );
            getStackTopDaemonResponse( yield_ctx ).ToolDeAllocateNetworkAddress();
        }
        virtual void ToolStash( const boost::filesystem::path& filePath,
                                const task::DeterminantHash&   determinant,
                                boost::asio::yield_context&    yield_ctx ) override
        {
            m_root.m_stash.stash( filePath, determinant );
            getStackTopDaemonResponse( yield_ctx ).ToolStash();
        }
        virtual void ToolRestore( const boost::filesystem::path& filePath,
                                  const task::DeterminantHash&   determinant,
                                  boost::asio::yield_context&    yield_ctx ) override
        {
            const bool bRestored = m_root.m_stash.restore( filePath, determinant );
            getStackTopDaemonResponse( yield_ctx ).ToolRestore( bRestored );
        }
        virtual void ToolSimReadLock( const mega::network::ConversationID& owningID,
                                      const mega::network::ConversationID& simulationID,
                                      boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            auto result = pDaemon->RootSimReadLock( owningID, simulationID );
            getStackTopDaemonResponse( yield_ctx ).ToolSimReadLock( result );
        }

        virtual void ToolSimWriteLock( const mega::network::ConversationID& owningID,
                                       const mega::network::ConversationID& simulationID,
                                       boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            auto result = pDaemon->RootSimWriteLock( owningID, simulationID );
            getStackTopDaemonResponse( yield_ctx ).ToolSimWriteLock( result );
        }

        virtual void ToolSimReleaseLock( const mega::network::ConversationID& owningID,
                                         const mega::network::ConversationID& simulationID,
                                         boost::asio::yield_context&          yield_ctx ) override
        {
            auto pDaemon = getDaemonRequestByInitiatedCon( simulationID, yield_ctx );
            VERIFY_RTE( pDaemon.has_value() );
            pDaemon->RootSimReleaseLock( owningID, simulationID );
            getStackTopDaemonResponse( yield_ctx ).ToolSimReleaseLock();
        }
        virtual void ToolSimList( boost::asio::yield_context& yield_ctx ) override
        {
            auto result = getSimulationIDs( yield_ctx );
            getStackTopDaemonResponse( yield_ctx ).ToolSimList( result );
        }*/
};

} // namespace service
} // namespace mega

#endif // REQUEST_22_SEPT_2022