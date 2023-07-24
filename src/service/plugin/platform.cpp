
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

#include "service/plugin/platform.hpp"

#include "service/executor/executor.hpp"

#include "service/network/log.hpp"

namespace mega::service
{

Platform::Platform( Executor& executor, const network::LogicalThreadID& logicalthreadID,
                    network::LogicalThreadBase& plugin )
    : ExecutorRequestLogicalThread( executor, logicalthreadID )
    , m_plugin( plugin )
{
}

network::Message Platform::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::platform::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return ExecutorRequestLogicalThread::dispatchRequest( msg, yield_ctx );
}

void Platform::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "Platform::run" );

    m_pYieldContext = &yield_ctx;

    THROW_TODO;
    //network::platform::Request_Sender rq( *this, m_plugin.getID(), m_plugin.shared_from_this(), yield_ctx );
//
    //try
    //{
    //    while( m_bRunning )
    //    {
    //        // update available networks
    //        m_state.m_availableNetworks = { "Single Player" };
//
    //        // send status update to plugin
    //        m_bRunning = rq.PlatformStatus( m_state );
    //    }
    //}
    //catch( std::exception& ex )
    //{
    //    SPDLOG_ERROR( ex.what() );
    //}
//
    //dispatchRemaining( yield_ctx );
    //
    //SPDLOG_TRACE( "Platform::run complete" );
}

} // namespace mega::service
