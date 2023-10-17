
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

namespace mega::service::report
{

ReportRequestLogicalThread::ReportRequestLogicalThread( Report& report, const network::LogicalThreadID& logicalthreadID )
    : InThreadLogicalThread( report, logicalthreadID )
    , m_report( report )
{
}
ReportRequestLogicalThread::~ReportRequestLogicalThread()
{
}

network::Message ReportRequestLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                             boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::leaf_report::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::report_leaf::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::mpo::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "ReportRequestLogicalThread::dispatchInBoundRequest failed: " << msg );
}

network::report_leaf::Request_Sender
ReportRequestLogicalThread::getReportRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_report.getLeafSender(), yield_ctx };
}

network::Message ReportRequestLogicalThread::RootAllBroadcast( const network::Message&     request,
                                                              boost::asio::yield_context& yield_ctx )
{

    // dispatch to the report MPO conversation
    SPDLOG_TRACE( "ReportRequestLogicalThread::RootAllBroadcast" );
    std::vector< network::Message > responses;
    {
        for( auto pThread : m_report.getLogicalThreads() )
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
                                "ReportRequestLogicalThread::RootAllBroadcast to logical thread: {}", pThread->getID() );
                            auto&                           msg = network::status::MSG_GetStatus_Request::get( request );
                            network::status::Request_Sender rq( *this, pThread, yield_ctx );
                            const network::Message          responseWrapper = network::status::MSG_GetStatus_Response::make(
                                request.getLogicalThreadID(),
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
        }
    }

    SPDLOG_TRACE( "ReportRequestLogicalThread::RootAllBroadcast got: {} responses", responses.size() );

    network::Message aggregateRequest = std::move( request );
    network::aggregate( aggregateRequest, responses );

    return dispatchInBoundRequest( aggregateRequest, yield_ctx );
}

network::Message ReportRequestLogicalThread::ReportRoot( const network::Message&     request,
                                                        boost::asio::yield_context& yield_ctx )
{
    return getReportRequest( yield_ctx ).ReportRoot( request );
}
network::Message ReportRequestLogicalThread::ReportDaemon( const network::Message&     request,
                                                          boost::asio::yield_context& yield_ctx )
{
    return getReportRequest( yield_ctx ).ReportDaemon( request );
}

network::Message ReportRequestLogicalThread::MPRoot( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender rq{ *this, m_report.getLeafSender(), yield_ctx };
    return rq.MPRoot( request, mp );
}

network::Message ReportRequestLogicalThread::MPUp( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender rq{ *this, m_report.getLeafSender(), yield_ctx };
    return rq.MPUp( request, mp );
}

network::Message ReportRequestLogicalThread::MPDown( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( m_report.getMP() == mp );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message ReportRequestLogicalThread::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                     boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( m_report.getMP() == MP( mpo ) );
    return dispatchInBoundRequest( request, yield_ctx );
}

} // namespace mega::service::report
