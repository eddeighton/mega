
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

#include "steamWorks.hpp"

#include "service/network/log.hpp"

namespace mega::service
{

SteamWorks::SteamWorks( Executor& executor, const network::ConversationID& conversationID )
    : ExecutorRequestConversation( executor, conversationID, std::nullopt )
{
}

network::Message SteamWorks::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::steam::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return ExecutorRequestConversation::dispatchRequest( msg, yield_ctx );
}

void SteamWorks::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SteamWorks::run" );

    m_pYieldContext = &yield_ctx;

    while( m_bRunning )
    {
        // process a message
        {
            {
                const network::ReceivedMsg msg = receive( yield_ctx );
                m_messageQueue.push_back( msg );
            }
            while( m_channel.try_receive(
                [ &msgs = m_messageQueue ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
                {
                    if( !ec )
                    {
                        msgs.push_back( msg );
                    }
                    else
                    {
                        THROW_TODO;
                    }
                } ) )
                ;
        }

        // process messages
        {
            for( const auto& msg : m_messageQueue )
            {
                SPDLOG_TRACE( "SteamWorks::run msg" );
                dispatchRequestImpl( msg, yield_ctx );
            }
            m_messageQueue.clear();
        }
    }
}

void SteamWorks::SteamDestroy( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SteamWorks::SteamDestroy" );
    m_bRunning = false;
}

} // namespace mega::service
