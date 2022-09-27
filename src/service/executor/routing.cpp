
#include "request.hpp"
#include "simulation.hpp"

#include "service/executor.hpp"

namespace mega
{
namespace service
{

ExecutorRequestConversation::ExecutorRequestConversation( Executor&                      executor,
                                                          const network::ConversationID& conversationID,
                                                          std::optional< network::ConnectionID >
                                                              originatingConnectionID )
    : ConcurrentConversation( executor, conversationID, originatingConnectionID )
    , m_executor( executor )
{
}

network::Message ExecutorRequestConversation::dispatchRequest( const network::Message&     msg,
                                                               boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::leaf_exe::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::job::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::mpo::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::sim::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "ExecutorRequestConversation::dispatchRequest failed" );
}

void ExecutorRequestConversation::dispatchResponse( const network::ConnectionID& connectionID,
                                                    const network::Message&      msg,
                                                    boost::asio::yield_context&  yield_ctx )
{
    if ( ( m_executor.getLeafSender().getConnectionID() == connectionID ) )
    {
        m_executor.getLeafSender().send( msg, yield_ctx );
    }
    else if ( m_executor.m_receiverChannel.getSender()->getConnectionID() == connectionID )
    {
        m_executor.getLeafSender().send( msg, yield_ctx );
    }
    else if ( network::ConversationBase::Ptr pConversation = m_executor.findExistingConversation( msg.receiver ) )
    {
        SPDLOG_TRACE( "Found response conversation within executor" );
        pConversation->send( network::ReceivedMsg{ connectionID, msg } );
    }
    else
    {
        SPDLOG_ERROR( "ExecutorRequestConversation: Cannot resolve connection for response: {}", connectionID );
        THROW_RTE( "ExecutorRequestConversation: Executor Critical error in response handler: " << connectionID );
    }
}

void ExecutorRequestConversation::error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                         boost::asio::yield_context& yield_ctx )
{
    if ( ( m_executor.getLeafSender().getConnectionID() == msg.connectionID )
         || ( m_executor.m_receiverChannel.getSender()->getConnectionID() == msg.connectionID ) )
    {
        m_executor.getLeafSender().sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else if ( m_executor.m_receiverChannel.getSender()->getConnectionID() == msg.connectionID )
    {
        m_executor.m_receiverChannel.getSender()->sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "ExecutorRequestConversation: Cannot resolve connection in error handler: {} for error: {}",
                      msg.connectionID, strErrorMsg );
        THROW_RTE( "ExecutorRequestConversation: Executor Critical error in error handler: " << msg.connectionID
                                                                                             << " : " << strErrorMsg );
    }
}

network::exe_leaf::Request_Sender ExecutorRequestConversation::getLeafRequest( boost::asio::yield_context& yield_ctx )
{
    return network::exe_leaf::Request_Sender( *this, m_executor.getLeafSender(), yield_ctx );
}
network::mpo::Request_Sender ExecutorRequestConversation::getMPRequest( boost::asio::yield_context& yield_ctx )
{
    return network::mpo::Request_Sender( *this, m_executor.getLeafSender(), yield_ctx );
}

network::Message ExecutorRequestConversation::RootExeBroadcast( const network::Message&     request,
                                                                boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}
network::Message ExecutorRequestConversation::RootExe( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message ExecutorRequestConversation::MPDown( const network::Message& request, const mega::MP& mp,
                                                      boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}
network::Message ExecutorRequestConversation::MPUp( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender leaf( *this, m_executor.getLeafSender(), yield_ctx );
    return leaf.MPUp( request, mp );
}

network::Message ExecutorRequestConversation::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                       boost::asio::yield_context& yield_ctx )
{
    THROW_RTE( "TODO" );
}
network::Message ExecutorRequestConversation::MPOUp( const network::Message& request, const mega::MPO& mpo,
                                                     boost::asio::yield_context& yield_ctx )
{
    THROW_RTE( "TODO" );
}
/*
network::Message ExecutorRequestConversation::RootMPO( const network::Message& request, const mega::MPO& mpo,
                                                       boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestConversation::RootMPO: {}", mpo );
    return MPODown( request, mpo, yield_ctx );
}
*/

/*
network::Message ExecutorRequestConversation::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                          boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestConversation::MPODown: {}", mpo );
    if ( request.index == network::sim::MSG_SimCreate_Request::ID )
    {
        return dispatchRequest( request, yield_ctx );
    }
    else
    {
        if ( Simulation::Ptr pSim = m_executor.getSimulation( mpo ) )
        {
            return pSim->dispatchRequest( request, yield_ctx );
        }
        else
        {
            // network::exe_leaf::Request_Encoder request(
            //     [ rootRequest = getMPORequest( yield_ctx ) ]( const network::Message& msg ) mutable
            //     { return rootRequest.MPORoot( msg ); } );
            // request.MPOUp( request );

            THROW_RTE( "Failed to locate simulation" );
        }
    }
}
*/

} // namespace service
} // namespace mega
