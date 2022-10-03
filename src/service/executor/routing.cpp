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

#include "request.hpp"
#include "simulation.hpp"

#include "service/executor.hpp"

namespace mega::service
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

network::Message ExecutorRequestConversation::RootAllBroadcast( const network::Message&     request,
                                                                boost::asio::yield_context& yield_ctx )
{
    std::vector< network::Message > responses;
    {
        std::vector< Simulation::Ptr > simulations;
        m_executor.getSimulations( simulations );
        for ( Simulation::Ptr pSimulation : simulations )
        {
            switch ( getMsgID( request ) )
            {
                case network::status::MSG_GetStatus_Request::ID:
                {
                    auto&                           msg = network::status::MSG_GetStatus_Request::get( request );
                    network::status::Request_Sender rq( *this, pSimulation->getID(), *pSimulation, yield_ctx );
                    auto response = network::status::MSG_GetStatus_Response{ rq.GetStatus( msg.status ) };
                    const network::Message responseWrapper
                        = network::status::MSG_GetStatus_Response::make( request.receiver, request.sender, response );
                    responses.push_back( responseWrapper );
                }
                break;
                default:
                {
                    THROW_RTE( "Unsupported RootAllBroadcast request type" );
                }
            }
        }
    }

    network::Message aggregateRequest = request;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );
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

} // namespace mega::service
