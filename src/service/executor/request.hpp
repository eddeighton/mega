
#ifndef EXECUTOR_REQUEST_22_JUNE_2022
#define EXECUTOR_REQUEST_22_JUNE_2022

#include "service/network/conversation.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/leaf_exe.hxx"
#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/job.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/status.hxx"

#include "runtime/runtime_api.hpp"

namespace mega
{
namespace service
{
class Executor;

class ExecutorRequestConversation : public network::ConcurrentConversation,
                                    public network::leaf_exe::Impl,
                                    public network::mpo::Impl,
                                    public network::job::Impl,
                                    public network::status::Impl,
                                    public network::sim::Impl
{
protected:
    Executor& m_executor;

public:
    ExecutorRequestConversation( Executor& executor, const network::ConversationID& conversationID,
                                 std::optional< network::ConnectionID > originatingConnectionID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;
    virtual void             dispatchResponse( const network::ConnectionID& connectionID,
                                               const network::Message&      msg,
                                               boost::asio::yield_context&  yield_ctx ) override;

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override;

    // helpers
    network::exe_leaf::Request_Sender getLeafRequest( boost::asio::yield_context& yield_ctx );
    network::mpo::Request_Sender getMPRequest( boost::asio::yield_context& yield_ctx );

    template < typename RequestEncoderType >
    RequestEncoderType getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getLeafRequest( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return rootRequest.ExeRoot( msg ); } );
    }

    template < typename RequestEncoderType >
    RequestEncoderType getDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getLeafRequest( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return rootRequest.ExeDaemon( msg ); } );
    }

    // network::leaf_exe::Impl - NOTE: RootSimRun note implemented here
    virtual network::Message RootExeBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExe( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;

    // network::mpo::Impl
    virtual network::Message
    MPODown( const network::Message& request, const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message
    MPDown( const network::Message& request, const mega::MP& mp, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message
    MPUp( const network::Message& request, const mega::MP& mp, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message
    MPOUp( const network::Message& request, const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string Ping( boost::asio::yield_context& yield_ctx ) override;

    // network::job::Impl - note also in JobConversation
    virtual std::vector< network::ConversationID >
    JobStart( const mega::utilities::ToolChain&                            toolChain,
              const mega::pipeline::Configuration&                         configuration,
              const network::ConversationID&                               rootConversationID,
              const std::vector< std::vector< network::ConversationID > >& jobs,
              boost::asio::yield_context&                                  yield_ctx ) override;

    // network::sim::Impl
    virtual mega::MPO SimCreate( boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // EXECUTOR_REQUEST_22_JUNE_2022