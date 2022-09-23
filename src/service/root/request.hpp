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
                                public network::root_daemon::Impl,
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

    // network::root_daemon::Request_Sender getDaemonRequest( network::Server::Connection::Ptr pConnection,
    //                                                        boost::asio::yield_context&      yield_ctx );

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

    // network::root_daemon::Impl
    virtual network::Message RootLeafBroadcast( const network::Message&     request,
                                                boost::asio::yield_context& yield_ctx ) override;

    // network::project::Impl
    virtual std::string CollateVersions( boost::asio::yield_context& yield_ctx ) override;
    virtual std::string GetVersion( const std::vector< std::string >& version,
                                    boost::asio::yield_context&       yield_ctx ) override;

    // network::enrole::Impl
    virtual MPO  EnroleDaemon( boost::asio::yield_context& yield_ctx ) override;
    virtual MPO  EnroleLeafWithRoot( const MPO& daemonMPO, boost::asio::yield_context& yield_ctx ) override;
    virtual void EnroleLeafDisconnect( const MPO& mpo, boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // REQUEST_22_SEPT_2022