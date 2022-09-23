#ifndef DAEMON_REQUEST_23_SEPT_2022
#define DAEMON_REQUEST_23_SEPT_2022

#include "daemon.hpp"

#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/daemon_leaf.hxx"
#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/project.hxx"

namespace mega
{
namespace service
{

class DaemonRequestConversation : public network::InThreadConversation,
                                  public network::leaf_daemon::Impl,
                                  public network::root_daemon::Impl,
                                  public network::enrole::Impl,
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
    virtual void             error( const network::ConnectionID& connectionID,
                                    const std::string&           strErrorMsg,
                                    boost::asio::yield_context&  yield_ctx ) override;

    // helpers
    network::daemon_root::Request_Sender getRootSender( boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_root::Request_Sender( *this, m_daemon.m_rootClient, yield_ctx );
    }

    template < typename RequestEncoderType >
    RequestEncoderType getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getRootSender( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return rootRequest.DaemonRoot( msg ); } );
    }

    // routing
    virtual network::Message TermRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ExeRoot( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ToolRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message LeafRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message LeafDaemon( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;

    // network::enrole::Impl
    virtual MPO EnroleLeafWithDaemon( const mega::network::Node::Type& type,
                                      boost::asio::yield_context&      yield_ctx ) override;

    // network::project::Impl
    virtual std::string GetVersion( boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // DAEMON_REQUEST_23_SEPT_2022
