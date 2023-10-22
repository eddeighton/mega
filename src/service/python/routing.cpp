
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

#include "mpo_logical_thread.hpp"

#include "service/network/log.hpp"

namespace mega::service::python
{

PythonRequestLogicalThread::PythonRequestLogicalThread( Python& python, const network::LogicalThreadID& logicalthreadID )
    : InThreadLogicalThread( python, logicalthreadID )
    , m_python( python )
{
}
PythonRequestLogicalThread::~PythonRequestLogicalThread()
{
}

network::Message PythonRequestLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                             boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::leaf_python::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::python_leaf::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::mpo::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::report::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "PythonRequestLogicalThread::dispatchInBoundRequest failed: " << msg );
}

network::python_leaf::Request_Sender
PythonRequestLogicalThread::getPythonRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_python.getLeafSender(), yield_ctx };
}

network::Message PythonRequestLogicalThread::RootAllBroadcast( const network::Message&     request,
                                                              boost::asio::yield_context& yield_ctx )
{

    // dispatch to the python MPO conversation
    SPDLOG_TRACE( "PythonRequestLogicalThread::RootAllBroadcast" );
    std::vector< network::Message > responses;
    {
        for( auto pThread : m_python.getLogicalThreads() )
        {
            if( std::dynamic_pointer_cast< MPOLogicalThread >( pThread ) )
            {
                if( pThread->getID() != getID() )
                {
                    switch( request.getID() )
                    {
                        case network::status::MSG_GetStatus_Request::ID:
                        {
                            SPDLOG_TRACE(
                                "PythonRequestLogicalThread::RootAllBroadcast to logical thread: {}", pThread->getID() );
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
                                "PythonRequestLogicalThread::RootAllBroadcast to logical thread: {}", pThread->getID() );
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
    }

    SPDLOG_TRACE( "PythonRequestLogicalThread::RootAllBroadcast got: {} responses", responses.size() );

    network::Message aggregateRequest = std::move( request );
    network::aggregate( aggregateRequest, responses );

    return dispatchInBoundRequest( aggregateRequest, yield_ctx );
}

network::Message PythonRequestLogicalThread::PythonRoot( const network::Message&     request,
                                                        boost::asio::yield_context& yield_ctx )
{
    return getPythonRequest( yield_ctx ).PythonRoot( request );
}
network::Message PythonRequestLogicalThread::PythonDaemon( const network::Message&     request,
                                                          boost::asio::yield_context& yield_ctx )
{
    return getPythonRequest( yield_ctx ).PythonDaemon( request );
}

network::Message PythonRequestLogicalThread::MPRoot( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender rq{ *this, m_python.getLeafSender(), yield_ctx };
    return rq.MPRoot( request, mp );
}

network::Message PythonRequestLogicalThread::MPUp( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender rq{ *this, m_python.getLeafSender(), yield_ctx };
    return rq.MPUp( request, mp );
}

network::Message PythonRequestLogicalThread::MPDown( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( mega::MP( m_python.getMPO() ) == mp );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message PythonRequestLogicalThread::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                     boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( mega::MPO( m_python.getMPO() ) == mpo );
    return dispatchInBoundRequest( request, yield_ctx );
}

} // namespace mega::service::python
