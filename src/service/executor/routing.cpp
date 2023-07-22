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

#include "service/executor/request.hpp"
#include "service/executor/simulation.hpp"
#include "service/executor/executor.hpp"

namespace mega::service
{

ExecutorRequestLogicalThread::ExecutorRequestLogicalThread( Executor&                      executor,
                                                          const network::LogicalThreadID& logicalthreadID,
                                                          std::optional< network::ConnectionID >
                                                              originatingConnectionID )
    : ConcurrentLogicalThread( executor, logicalthreadID, originatingConnectionID )
    , m_executor( executor )
{
}

network::Message ExecutorRequestLogicalThread::dispatchRequest( const network::Message&     msg,
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
    if ( result = network::project::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::enrole::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "ExecutorRequestLogicalThread::dispatchRequest failed for: " << msg.getName() );
}

void ExecutorRequestLogicalThread::dispatchResponse( const network::ConnectionID& connectionID,
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
    else if ( network::LogicalThreadBase::Ptr pLogicalThread
              = m_executor.findExistingLogicalThread( msg.getReceiverID() ) )
    {
        if( network::sim::MSG_SimClock_Response::ID == msg.getID() )
        {
            THROW_RTE( "SimClock response being routed back into simulation" );
        }
        pLogicalThread->send( network::ReceivedMsg{ connectionID, msg } );
    }
    else
    {
        SPDLOG_ERROR( "ExecutorRequestLogicalThread::dispatchResponse Cannot resolve connection for response: {}", connectionID );
        THROW_RTE( "ExecutorRequestLogicalThread::dispatchResponse Executor Critical error in response handler: " << connectionID );
    }
}

void ExecutorRequestLogicalThread::error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                         boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestLogicalThread::error conid:{} msg:{} err:{}", msg.connectionID, msg.msg, strErrorMsg );
    if ( ( m_executor.getLeafSender().getConnectionID() == msg.connectionID )
         || ( m_executor.m_receiverChannel.getSender()->getConnectionID() == msg.connectionID )  )
    {
        m_executor.getLeafSender().sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else if ( network::LogicalThreadBase::Ptr pLogicalThread
              = m_executor.findExistingLogicalThread( msg.msg.getSenderID() ) )
    {
        pLogicalThread->sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "ExecutorRequestLogicalThread: Cannot resolve connection in error handler: {} msg:{} for error: {}",
                      msg.connectionID, msg.msg, strErrorMsg );
        THROW_RTE( "ExecutorRequestLogicalThread: Executor Critical error in error handler: " << msg.connectionID
                                                                                             << " : " << strErrorMsg );
    }
}

network::exe_leaf::Request_Sender ExecutorRequestLogicalThread::getLeafRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_executor.getLeafSender(), yield_ctx };
}
network::mpo::Request_Sender ExecutorRequestLogicalThread::getMPRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_executor.getLeafSender(), yield_ctx };
}

network::Message ExecutorRequestLogicalThread::RootAllBroadcast( const network::Message&     request,
                                                                boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestLogicalThread::RootAllBroadcast" );
    std::vector< network::Message > responses;
    {
        std::vector< Simulation::Ptr > simulations;
        m_executor.getSimulations( simulations );
        for ( Simulation::Ptr pSimulation : simulations )
        {
            switch ( request.getID() )
            {
                case network::status::MSG_GetStatus_Request::ID:
                {
                    SPDLOG_TRACE( "ExecutorRequestLogicalThread::RootAllBroadcast to sim: {}", pSimulation->getID() );
                    auto&                           msg = network::status::MSG_GetStatus_Request::get( request );
                    network::status::Request_Sender rq( *this, pSimulation->getID(), *pSimulation, yield_ctx );
                    const network::Message          responseWrapper = network::status::MSG_GetStatus_Response::make(
                        request.getReceiverID(), request.getSenderID(),
                        network::status::MSG_GetStatus_Response{ rq.GetStatus( msg.status ) } );
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

    SPDLOG_TRACE( "ExecutorRequestLogicalThread::RootAllBroadcast got: {} responses", responses.size() );

    network::Message aggregateRequest = std::move( request );
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );
}

network::Message ExecutorRequestLogicalThread::RootExeBroadcast( const network::Message&     request,
                                                                boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}
network::Message ExecutorRequestLogicalThread::RootExe( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message ExecutorRequestLogicalThread::MPDown( const network::Message& request, const mega::MP& mp,
                                                      boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

void ExecutorRequestLogicalThread::EnroleDestroy( boost::asio::yield_context& )
{
    m_executor.shutdown();
}

} // namespace mega::service
