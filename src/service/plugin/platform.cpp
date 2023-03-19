
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

#include "platform.hpp"

#include "service/executor/executor.hpp"

#include "service/network/log.hpp"

namespace mega::service
{

Platform::Platform( Executor& executor, const network::ConversationID& conversationID,
                    network::ConversationBase& plugin )
    : ExecutorRequestConversation( executor, conversationID, std::nullopt )
    , m_plugin( plugin )
    , m_timer( executor.m_io_context )
{
}

network::Message Platform::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::platform::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return ExecutorRequestConversation::dispatchRequest( msg, yield_ctx );
}

void Platform::dispatchResponse( const network::ConnectionID& connectionID,
                                 const network::Message&      msg,
                                 boost::asio::yield_context&  yield_ctx )

{
    if( msg.getReceiverID() == m_plugin.getID() )
    {
        m_plugin.send( network::ReceivedMsg{ connectionID, msg } );
    }
    else
    {
        ExecutorRequestConversation::dispatchResponse( connectionID, msg, yield_ctx );
    }
}

void Platform::error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                      boost::asio::yield_context& yield_ctx )
{
    if( msg.msg.getSenderID() == m_plugin.getID() )
    {
        m_plugin.sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        error( msg, strErrorMsg, yield_ctx );
    }
}

void Platform::statusUpdate()
{
    SPDLOG_TRACE( "Platform::statusUpdate" );

    auto pThis = this;
    using namespace std::chrono_literals;

    if( m_bRunning )
    {
        using namespace network::platform;
        send( network::ReceivedMsg{
            getConnectionID(), MSG_PlatformClock_Request::make( getID(), MSG_PlatformClock_Request{} ) } );

        m_timer.expires_from_now( 1s );
        m_timer.async_wait( [ pThis ]( boost::system::error_code ec ) { pThis->statusUpdate(); } );
    }
}

void Platform::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "Platform::run" );

    m_pYieldContext = &yield_ctx;

    statusUpdate();

    while( m_bRunning )
    {
        run_one( yield_ctx );
    }

    dispatchRemaining( yield_ctx );
}

void Platform::PlatformDestroy( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "Platform::PlatformDestroy" );
    m_bRunning = false;
}

void Platform::PlatformClock( boost::asio::yield_context& yield_ctx )
{
    // send status update to plugin
    network::platform::Request_Sender rq( *this, m_plugin, yield_ctx );
    rq.PlatformStatus( m_state );
}
} // namespace mega::service
