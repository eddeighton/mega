#ifndef REQUEST_22_SEPT_2022
#define REQUEST_22_SEPT_2022

#include "service/network/conversation.hpp"
#include "service/network/server.hpp"

#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/stash.hxx"
#include "service/protocol/model/job.hxx"

namespace mega
{
namespace service
{
class Root;

class RootRequestConversation : public network::InThreadConversation,
                                public network::daemon_root::Impl,
                                public network::root_daemon::Impl,
                                public network::project::Impl,
                                public network::enrole::Impl,
                                public network::status::Impl,
                                public network::stash::Impl,
                                public network::job::Impl
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
    virtual void             error( const network::ConnectionID& connectionID,
                                    const std::string&           strErrorMsg,
                                    boost::asio::yield_context&  yield_ctx ) override;

    template < typename RequestEncoderType >
    RequestEncoderType getExeRequest( boost::asio::yield_context& yield_ctx )
    {
        RootRequestConversation* pThis = this;
        return RequestEncoderType( [ pThis, &yield_ctx ]( const network::Message& msg ) mutable
                                   { return pThis->RootExe( msg, yield_ctx ); } );
    }
    template < typename RequestEncoderType >
    RequestEncoderType getExeBroadcastRequest( boost::asio::yield_context& yield_ctx )
    {
        RootRequestConversation* pThis = this;
        return RequestEncoderType( [ pThis, &yield_ctx ]( const network::Message& msg ) mutable
                                   { return pThis->RootExeBroadcast( msg, yield_ctx ); } );
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
    virtual network::Message
    MPORoot( const network::Message& request, const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message
    MPOMPO( const network::Message& request, const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::root_daemon::Impl
    virtual network::Message RootLeafBroadcast( const network::Message&     request,
                                                boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExeBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExe( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual void RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::project::Impl
    virtual network::Project GetProject( boost::asio::yield_context& yield_ctx ) override;
    virtual void SetProject( const network::Project& project, boost::asio::yield_context& yield_ctx ) override;
    virtual mega::network::MegastructureInstallation
    GetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override;

    // network::enrole::Impl
    virtual MPO  EnroleDaemon( boost::asio::yield_context& yield_ctx ) override;
    virtual MPO  EnroleLeafWithRoot( const MPO& daemonMPO, boost::asio::yield_context& yield_ctx ) override;
    virtual void EnroleLeafDisconnect( const MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual mega::network::Status GetStatus( const std::vector< mega::network::Status >& status,
                                             boost::asio::yield_context&                 yield_ctx ) override;
    virtual mega::network::Status GetNetworkStatus( boost::asio::yield_context& yield_ctx ) override;

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

protected:
    Root& m_root;
};

} // namespace service
} // namespace mega

#endif // REQUEST_22_SEPT_2022