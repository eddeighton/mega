
#ifndef REQUEST_LEAF_23_SEPT_2022
#define REQUEST_LEAF_23_SEPT_2022

#include "service/leaf.hpp"

#include "service/protocol/model/term_leaf.hxx"
#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/daemon_leaf.hxx"
#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/leaf_exe.hxx"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/leaf_tool.hxx"
#include "service/protocol/model/project.hxx"

namespace mega
{
namespace service
{

class LeafRequestConversation : public network::InThreadConversation,
                                public network::term_leaf::Impl,
                                public network::daemon_leaf::Impl,
                                public network::exe_leaf::Impl,
                                public network::tool_leaf::Impl,
                                public network::project::Impl
{
protected:
    Leaf& m_leaf;

public:
    LeafRequestConversation( Leaf& leaf, const network::ConversationID& conversationID,
                             const network::ConnectionID& originatingConnectionID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;
    virtual void             dispatchResponse( const network::ConnectionID& connectionID,
                                               const network::Message&      msg,
                                               boost::asio::yield_context&  yield_ctx ) override;
    virtual void             error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx ) override;

    network::leaf_daemon::Request_Sender getDaemonSender( boost::asio::yield_context& yield_ctx );

    // network::term_leaf::Impl
    virtual network::Message TermRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;

    // network::exe_leaf::Impl
    virtual network::Message ExeRoot( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;

    // network::tool_leaf::Impl
    virtual network::Message ToolRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;

    // network::project::Impl
    virtual std::string GetVersion( boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // REQUEST_LEAF_23_SEPT_2022
