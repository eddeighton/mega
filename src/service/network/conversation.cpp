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

#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/log.hpp"

#include "service/network/end_point.hpp"
#include "service/protocol/model/messages.hxx"

#include <chrono>
#include <iostream>

namespace mega::network
{

ConversationBase::RequestStack::RequestStack( const char* pszMsg, ConversationBase::Ptr pConversation,
                                              const ConnectionID& connectionID )
    : m_pszMsg( pszMsg )
    //, m_startTime( std::chrono::steady_clock::now() )
    , pConversation( pConversation )
{
    pConversation->requestStarted( connectionID );
}
ConversationBase::RequestStack::~RequestStack()
{
    // const auto timeDelta = std::chrono::steady_clock::now() - m_startTime;
    // SPDLOG_DEBUG( "{} {} {} {}", conversation.getProcessName(), conversation.getID(), m_pszMsg, timeDelta );
    pConversation->requestCompleted();
}

Conversation::Conversation( ConversationManager& conversationManager, const ConversationID& conversationID,
                            std::optional< ConnectionID > originatingConnectionID )
    : m_conversationManager( conversationManager )
    , m_conversationID( conversationID )
    , m_originatingEndPoint( originatingConnectionID )
{
}

Conversation::~Conversation()
{
}

void Conversation::requestStarted( const ConnectionID& connectionID )
{
    //
    m_stack.push_back( connectionID );
}

void Conversation::requestCompleted()
{
    VERIFY_RTE( !m_stack.empty() );
    m_stack.pop_back();
    if( m_stack.empty() )
    {
        m_conversationManager.conversationCompleted( shared_from_this() );
    }
}
const std::string& Conversation::getProcessName() const
{
    return m_conversationManager.getProcessName();
}

void Conversation::onDisconnect( const ConnectionID& connectionID )
{
    for( const ConnectionID& existing : m_stack )
    {
        if( existing == connectionID )
        {
            m_disconnections.insert( connectionID );
            break;
        }
    }

    if( !m_stack.empty() && m_stack.back() == connectionID )
    {
        SPDLOG_ERROR( "Generating disconnect on conversation: {} for connection: {}", getID(), connectionID );
        const ReceivedMsg rMsg{ connectionID, make_error_msg( getID(), "Disconnection" ) };
        send( rMsg );
    }
}

void Conversation::run( boost::asio::yield_context& yield_ctx )
{
    try
    {
        do
        {
            run_one( yield_ctx );
        } while( !m_stack.empty() );
    }
    catch( std::exception& ex )
    {
        SPDLOG_WARN( "Conversation: {} exception: {}", getID(), ex.what() );
        m_conversationManager.conversationCompleted( shared_from_this() );
    }
}

void Conversation::run_one( boost::asio::yield_context& yield_ctx )
{
    const ReceivedMsg msg = receive( yield_ctx );
    dispatchRequestImpl( msg, yield_ctx );
}

Message Conversation::dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
{
    ReceivedMsg msg;
    while( true )
    {
        msg = receive( yield_ctx );
        if( isRequest( msg.msg ) )
        {
            dispatchRequestImpl( msg, yield_ctx );

            // check if connection has disconnected
            if( m_disconnections.empty() )
            {
                ASSERT( !m_stack.empty() );
                if( m_disconnections.count( m_stack.back() ) )
                {
                    SPDLOG_ERROR(
                        "Generating disconnect on conversation: {} for connection: {}", getID(), m_stack.back() );
                    const ReceivedMsg rMsg{
                        m_stack.back(), make_error_msg( msg.msg.getReceiverID(), "Disconnection" ) };
                    send( rMsg );
                }
            }
        }
        else
        {
            break;
        }
    }
    if( msg.msg.getID() == MSG_Error_Response::ID )
    {
        throw std::runtime_error( MSG_Error_Response::get( msg.msg ).what );
    }
    return msg.msg;
}

void Conversation::dispatchRequestImpl( const ReceivedMsg& msg, boost::asio::yield_context& yield_ctx )
{
    ConversationBase::RequestStack stack( msg.msg.getName(), shared_from_this(), msg.connectionID );
    try
    {
        ASSERT( isRequest( msg.msg ) );
        network::Message result = dispatchRequest( msg.msg, yield_ctx );
        if( !result )
        {
            SPDLOG_ERROR( "Failed to dispatch request: {} on conversation: {}", msg.msg, getID() );
            THROW_RTE( "Failed to dispatch request message: " << msg.msg );
        }
        else
        {
            // send response
            dispatchResponse( msg.connectionID, result, yield_ctx );
        }
    }
    catch( std::exception& ex )
    {
        error( msg, ex.what(), yield_ctx );
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
InThreadConversation::InThreadConversation( ConversationManager&  conversationManager,
                                            const ConversationID& conversationID,
                                            std::optional< ConnectionID >
                                                originatingConnectionID /*= std::nullopt*/ )
    : Conversation( conversationManager, conversationID, originatingConnectionID )
    , m_channel( conversationManager.getIOContext() )
{
}

ReceivedMsg InThreadConversation::receive( boost::asio::yield_context& yield_ctx )
{
    return m_channel.async_receive( yield_ctx );
}

void InThreadConversation::send( const ReceivedMsg& msg )
{
    m_channel.async_send( boost::system::error_code(), msg,
                          [ &msg ]( boost::system::error_code ec )
                          {
                              if( ec )
                              {
                                  SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                                  THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                              }
                          } );
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
ConcurrentConversation::ConcurrentConversation( ConversationManager&  conversationManager,
                                                const ConversationID& conversationID,
                                                std::optional< ConnectionID >
                                                    originatingConnectionID /*= std::nullopt*/ )
    : Conversation( conversationManager, conversationID, originatingConnectionID )
    , m_channel( conversationManager.getIOContext() )
{
}

ReceivedMsg ConcurrentConversation::receive( boost::asio::yield_context& yield_ctx )
{
    return m_channel.async_receive( yield_ctx );
}

void ConcurrentConversation::send( const ReceivedMsg& msg )
{
    m_channel.async_send( boost::system::error_code(), msg,
                          [ &msg ]( boost::system::error_code ec )
                          {
                              if( ec )
                              {
                                  SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                                  THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                              }
                          } );
}

} // namespace mega::network
