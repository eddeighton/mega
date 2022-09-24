
#ifndef REQUEST_LEAF_23_SEPT_2022
#define REQUEST_LEAF_23_SEPT_2022

#include "service/leaf.hpp"

#include "service/protocol/model/term_leaf.hxx"
#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/mpo_leaf.hxx"
#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/daemon_leaf.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/job.hxx"

#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/leaf_exe.hxx"
#include "service/protocol/model/leaf_tool.hxx"
#include "service/protocol/model/leaf_term.hxx"

namespace mega
{
namespace service
{

class LeafRequestConversation : public network::InThreadConversation,
                                public network::term_leaf::Impl,
                                public network::exe_leaf::Impl,
                                public network::mpo_leaf::Impl,
                                public network::tool_leaf::Impl,
                                public network::daemon_leaf::Impl,
                                public network::status::Impl,
                                public network::job::Impl
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
    network::leaf_exe::Request_Sender    getExeSender( boost::asio::yield_context& yield_ctx );
    network::leaf_tool::Request_Sender   getToolSender( boost::asio::yield_context& yield_ctx );
    network::leaf_term::Request_Sender   getTermSender( boost::asio::yield_context& yield_ctx );

    // network::term_leaf::Impl
    virtual network::Message TermRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;

    // network::exe_leaf::Impl
    virtual network::Message ExeRoot( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;

    // network::tool_leaf::Impl
    virtual network::Message ToolRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;

    // network::mpo_leaf::Impl
    virtual network::Message MPORoot( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPOMPOUp( const network::Message& request, const mega::MPO& mpo,
                                     boost::asio::yield_context& yield_ctx ) override;

    // network::daemon_leaf::Impl
    virtual network::Message RootLeafBroadcast( const network::Message&     request,
                                                boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExeBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExe( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual void             RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootMPO( const network::Message& request, const mega::MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPOMPODown( const network::Message& request, const mega::MPO& mpo,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message DaemonLeafBroadcast( const network::Message&     request,
                                                  boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;

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

} // namespace service
} // namespace mega

#endif // REQUEST_LEAF_23_SEPT_2022
