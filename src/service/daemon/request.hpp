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

#ifndef DAEMON_REQUEST_23_SEPT_2022
#define DAEMON_REQUEST_23_SEPT_2022

#include "daemon.hpp"

#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/daemon_leaf.hxx"
#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"
#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/job.hxx"
#include "service/protocol/model/memory.hxx"

namespace mega::service
{

class DaemonRequestConversation : public network::InThreadConversation,
                                  public network::leaf_daemon::Impl,
                                  public network::root_daemon::Impl,
                                  public network::mpo::Impl,
                                  public network::enrole::Impl,
                                  public network::status::Impl,
                                  public network::job::Impl,
                                  public network::memory::Impl,
                                  public network::project::Impl
{
protected:
    Daemon& m_daemon;

public:
    DaemonRequestConversation( Daemon&                        daemon,
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
    network::daemon_root::Request_Sender getRootSender( boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_root::Request_Sender( *this, m_daemon.m_rootClient, yield_ctx );
    }

    template < typename RequestEncoderType >
    RequestEncoderType getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getRootSender( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return rootRequest.DaemonRoot( msg ); },
                                   getID() );
    }

    // network::leaf_daemon::Impl
    virtual network::Message TermRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ExeRoot( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ToolRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ToolDaemon( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message LeafRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message LeafDaemon( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ExeDaemon( const network::Message&     request,
                                        boost::asio::yield_context& yield_ctx ) override;

    // network::root_daemon::Impl
    virtual network::Message RootAllBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExeBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExe( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual void             RootSimRun( const mega::MPO& mpo, const mega::NetworkAddress& networkAddress,
                                         boost::asio::yield_context& yield_ctx ) override;

    // network::mpo::Impl
    virtual network::Message MPRoot( const network::Message& request, const mega::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPDown( const network::Message& request, const mega::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPUp( const network::Message& request, const mega::MP& mp,
                                   boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPODown( const network::Message& request, const mega::MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPOUp( const network::Message& request, const mega::MPO& mpo,
                                    boost::asio::yield_context& yield_ctx ) override;

    // network::enrole::Impl
    virtual MP EnroleLeafWithDaemon( const mega::network::Node::Type& type,
                                     boost::asio::yield_context&      yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( boost::asio::yield_context& yield_ctx ) override;

    // network::memory::Impl
    virtual void        MPODestroyed( const MPO& mpo, const bool& bDeleteShared,
                                      boost::asio::yield_context& yield_ctx ) override;
    virtual reference   Allocate( const MPO& mpo, const TypeID& objectTypeID,
                                  boost::asio::yield_context& yield_ctx ) override;
    virtual std::string Acquire( const MPO& mpo, boost::asio::yield_context& yield_ctx ) override;
    virtual reference NetworkToMachine( const reference& ref, boost::asio::yield_context& yield_ctx ) override;

    // network::project::Impl
    virtual void SetProject( const mega::network::Project& project, boost::asio::yield_context& yield_ctx ) override;

    // network::job::Impl
    virtual std::vector< network::ConversationID >
    JobStart( const mega::utilities::ToolChain&                            toolChain,
              const mega::pipeline::Configuration&                         configuration,
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
};

} // namespace mega::service

#endif // DAEMON_REQUEST_23_SEPT_2022
