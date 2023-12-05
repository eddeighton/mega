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

ExecutorRequestLogicalThread::ExecutorRequestLogicalThread( Executor&                       executor,
                                                            const network::LogicalThreadID& logicalthreadID )
    : ConcurrentLogicalThread( executor, logicalthreadID )
    , m_executor( executor )
{
}
ExecutorRequestLogicalThread::~ExecutorRequestLogicalThread()
{
}

network::Message ExecutorRequestLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                                       boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::leaf_exe::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::job::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::mpo::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::sim::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::report::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::enrole::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::host::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "ExecutorRequestLogicalThread::dispatchInBoundRequest failed for: " << msg.getName() );
}

network::exe_leaf::Request_Sender ExecutorRequestLogicalThread::getLeafRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_executor.getLeafSender(), yield_ctx };
}
network::host::Request_Sender ExecutorRequestLogicalThread::getLeafHostRequest( boost::asio::yield_context& yield_ctx )
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
        for( Simulation::Ptr pThread : simulations ) // Not m_executor.getLogicalThreads()
        {
            if( pThread->getID() != getID() )
            {
                switch( request.getID() )
                {
                    case network::status::MSG_GetStatus_Request::ID:
                    {
                        SPDLOG_TRACE(
                            "ExecutorRequestLogicalThread::RootAllBroadcast to logical thread: {}", pThread->getID() );
                        auto&                           msg = network::status::MSG_GetStatus_Request::get( request );
                        network::status::Request_Sender rq( *this, pThread, yield_ctx );
                        const network::Message          responseWrapper = network::status::MSG_GetStatus_Response::make(
                            request.getLogicalThreadID(),
                            network::status::MSG_GetStatus_Response{ rq.GetStatus( msg.status ) } );
                        responses.push_back( responseWrapper );
                    }
                    break;
                    case network::report::MSG_GetReport_Request::ID:
                    {
                        SPDLOG_TRACE(
                            "ExecutorRequestLogicalThread::RootAllBroadcast to logical thread: {}", pThread->getID() );
                        auto&                           msg = network::report::MSG_GetReport_Request::get( request );
                        network::report::Request_Sender rq( *this, pThread, yield_ctx );
                        const network::Message          responseWrapper = network::report::MSG_GetReport_Response::make(
                            request.getLogicalThreadID(),
                            network::report::MSG_GetReport_Response{ rq.GetReport( msg.url, msg.report ) } );
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
    }

    SPDLOG_TRACE( "ExecutorRequestLogicalThread::RootAllBroadcast got: {} responses", responses.size() );

    network::Message aggregateRequest = std::move( request );
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchInBoundRequest( aggregateRequest, yield_ctx );
}

network::Message ExecutorRequestLogicalThread::RootExeBroadcast( const network::Message&     request,
                                                                 boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}
network::Message ExecutorRequestLogicalThread::RootExe( const network::Message&     request,
                                                        boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message ExecutorRequestLogicalThread::MPDown( const network::Message& request, const mega::MP& mp,
                                                       boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}

void ExecutorRequestLogicalThread::EnroleDestroy( boost::asio::yield_context& yield_ctx )
{
    m_executor.shutdown( yield_ctx );
}

} // namespace mega::service
