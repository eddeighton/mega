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

#ifndef REQUEST_22_SEPT_2022
#define REQUEST_22_SEPT_2022

#include "service/network/conversation.hpp"

#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"
#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/stash.hxx"
#include "service/protocol/model/job.hxx"
#include "service/protocol/model/address.hxx"
#include "service/protocol/model/sim.hxx"

namespace mega::service
{
class Root;

class RootRequestConversation : public network::InThreadConversation,
                                public network::daemon_root::Impl,
                                public network::mpo::Impl,
                                public network::project::Impl,
                                public network::enrole::Impl,
                                public network::status::Impl,
                                public network::stash::Impl,
                                public network::job::Impl,
                                public network::address::Impl,
                                public network::sim::Impl
{
public:
    RootRequestConversation( Root&                          root,
                             const network::ConversationID& conversationID,
                             const network::ConnectionID&   originatingConnectionID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;
    virtual void             dispatchResponse( const network::ConnectionID& connectionID,
                                               const network::Message&      msg,
                                               boost::asio::yield_context&  yield_ctx ) override;
    virtual void             error( const network::ReceivedMsg& msg,
                                    const std::string&          strErrorMsg,
                                    boost::asio::yield_context& yield_ctx ) override;

    // helpers
    network::root_daemon::Request_Sender getDaemonSender( boost::asio::yield_context& yield_ctx );
    network::Message broadcastAll( const network::Message& msg, boost::asio::yield_context& yield_ctx );
    network::Message broadcastExe( const network::Message& msg, boost::asio::yield_context& yield_ctx );

    template < typename RequestEncoderType >
    RequestEncoderType getExeRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ sender = getDaemonSender( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return sender.RootExe( msg ); },
                                   getID() );
    }
    template < typename RequestEncoderType >
    RequestEncoderType getExeBroadcastRequest( boost::asio::yield_context& yield_ctx )
    {
        RootRequestConversation* pThis = this;
        return RequestEncoderType( [ pThis, &yield_ctx ]( const network::Message& msg ) mutable
                                   { return pThis->broadcastExe( msg, yield_ctx ); },
                                   getID() );
    }
    template < typename RequestEncoderType >
    RequestEncoderType getAllBroadcastRequest( boost::asio::yield_context& yield_ctx )
    {
        RootRequestConversation* pThis = this;
        return RequestEncoderType( [ pThis, &yield_ctx ]( const network::Message& msg ) mutable
                                   { return pThis->broadcastAll( msg, yield_ctx ); },
                                   getID() );
    }

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

    // network::mpo::Impl
    virtual network::Message MPRoot( const network::Message& request, const MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPODown( const network::Message& request, const MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPDown( const network::Message& request, const MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPUp( const network::Message& request, const MP& mp,
                                   boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPOUp( const network::Message& request, const MPO& mpo,
                                    boost::asio::yield_context& yield_ctx ) override;

    // network::project::Impl
    virtual network::Project GetProject( boost::asio::yield_context& yield_ctx ) override;
    virtual void SetProject( const network::Project& project, boost::asio::yield_context& yield_ctx ) override;
    virtual network::MegastructureInstallation
    GetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override;

    // network::enrole::Impl
    virtual MP   EnroleDaemon( boost::asio::yield_context& yield_ctx ) override;
    virtual MP   EnroleLeafWithRoot( const MP& daemonMP, boost::asio::yield_context& yield_ctx ) override;
    virtual void EnroleLeafDisconnect( const MP& mp, boost::asio::yield_context& yield_ctx ) override;
    virtual std::vector< MachineID > EnroleGetDaemons( boost::asio::yield_context& yield_ctx ) override;
    virtual std::vector< MP >        EnroleGetProcesses( const MachineID&            machineID,
                                                         boost::asio::yield_context& yield_ctx ) override;
    virtual std::vector< MPO > EnroleGetMPO( const MP& machineProcess, boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual network::Status GetNetworkStatus( boost::asio::yield_context& yield_ctx ) override;

    // network::address::Impl
    virtual MPO            GetNetworkAddressMPO( const NetworkAddress&       networkAddress,
                                                 boost::asio::yield_context& yield_ctx ) override;
    virtual NetworkAddress GetRootNetworkAddress( const MPO& mpo, boost::asio::yield_context& yield_ctx ) override;
    virtual NetworkAddress AllocateNetworkAddress( const MPO&                  mpo,
                                                   const TypeID&               objectTypeID,
                                                   boost::asio::yield_context& yield_ctx ) override;
    virtual void           DeAllocateNetworkAddress( const MPO&                  mpo,
                                                     const NetworkAddress&       networkAddress,
                                                     boost::asio::yield_context& yield_ctx ) override;

    // network::stash::Impl
    virtual void           StashClear( boost::asio::yield_context& yield_ctx ) override;
    virtual void           StashStash( const boost::filesystem::path& filePath,
                                       const task::DeterminantHash&   determinant,
                                       boost::asio::yield_context&    yield_ctx ) override;
    virtual bool           StashRestore( const boost::filesystem::path& filePath,
                                         const task::DeterminantHash&   determinant,
                                         boost::asio::yield_context&    yield_ctx ) override;
    virtual task::FileHash BuildGetHashCode( const boost::filesystem::path& filePath,
                                             boost::asio::yield_context&    yield_ctx ) override;
    virtual void           BuildSetHashCode( const boost::filesystem::path& filePath,
                                             const task::FileHash&          hashCode,
                                             boost::asio::yield_context&    yield_ctx ) override;

    // network::job::Impl
    virtual std::vector< network::ConversationID >
    JobStart( const utilities::ToolChain&                                  toolChain,
              const pipeline::Configuration&                               configuration,
              const network::ConversationID&                               rootConversationID,
              const std::vector< std::vector< network::ConversationID > >& jobs,
              boost::asio::yield_context&                                  yield_ctx ) override
    {
        std::vector< network::ConversationID > result;
        for ( const auto& j : jobs )
        {
            for ( const auto& k : j )
                result.push_back( k );
        }
        return result;
    }

    // network::sim::Impl
    virtual Snapshot SimLockRead( const MPO& requestingMPO, const MPO& targetMPO,
                                  boost::asio::yield_context& yield_ctx ) override;
    virtual Snapshot SimLockWrite( const MPO& requestingMPO, const MPO& targetMPO,
                                   boost::asio::yield_context& yield_ctx ) override;
    virtual void     SimLockRelease( const MPO&                  requestingMPO,
                                     const MPO&                  targetMPO,
                                     const network::Transaction& transaction,
                                     boost::asio::yield_context& yield_ctx ) override;

protected:
    Root& m_root;
};

} // namespace mega::service

#endif // REQUEST_22_SEPT_2022