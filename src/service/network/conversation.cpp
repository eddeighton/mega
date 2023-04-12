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

#include "jit/jit_exception.hpp"

#include <chrono>
#include <iostream>

namespace mega::network
{

Conversation::Conversation( ConversationManager& conversationManager, const ConversationID& conversationID,
                            std::optional< ConnectionID > originatingConnectionID )
    : m_conversationManager( conversationManager )
    , m_conversationID( conversationID )
    , m_originatingEndPoint( originatingConnectionID )
{
}

Conversation::~Conversation() = default;

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
        // SPDLOG_TRACE( "Conversation::requestCompleted: {}", getID() );
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

ReceivedMsg Conversation::receiveDeferred( boost::asio::yield_context& yield_ctx )
{
    ReceivedMsg msg;
    while( true )
    {
        if( m_bEnableQueueing && !m_unqueuedMessages.empty() )
        {
            msg = m_unqueuedMessages.back();
            m_unqueuedMessages.pop_back();
        }
        else
        {
            msg = receive( yield_ctx );
        }
        if( m_bEnableQueueing )
        {
            if( !queue( msg ) )
            {
                return msg;
            }
        }
        else
        {
            return msg;
        }
    }
}

bool Conversation::queue( const ReceivedMsg& msg )
{
    if( m_bQueueing )
    {
        m_deferedMessages.push_back( msg );
        return true;
    }
    else
    {
        return false;
    }
}

void Conversation::unqueue()
{
    if( m_bEnableQueueing )
    {
        m_bQueueing = false;
        VERIFY_RTE( m_unqueuedMessages.empty() );
        m_deferedMessages.swap( m_unqueuedMessages );
        std::reverse( m_unqueuedMessages.begin(), m_unqueuedMessages.end() );
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
    catch( mega::runtime::JITException& ex )
    {
        SPDLOG_WARN( "Conversation: {} exception: {}", getID(), ex.what() );
        m_conversationManager.conversationCompleted( shared_from_this() );
    }
}

void Conversation::run_one( boost::asio::yield_context& yield_ctx )
{
    // SPDLOG_TRACE( "Conversation::run_one" );
    unqueue();
    const ReceivedMsg msg = receiveDeferred( yield_ctx );
    dispatchRequestImpl( msg, yield_ctx );
}

Message Conversation::dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
{
    ReceivedMsg msg;
    while( true )
    {
        msg = receiveDeferred( yield_ctx );

        if( isRequest( msg.msg ) )
        {
            dispatchRequestImpl( msg, yield_ctx );

            // check if connection has disconnected
            if( !m_disconnections.empty() )
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

    // SPDLOG_TRACE( "Conversation::dispatchRequestsUntilResponse: returned {}", msg.msg );
    return msg.msg;
}

void Conversation::dispatchRequestImpl( const ReceivedMsg& msg, boost::asio::yield_context& yield_ctx )
{
    // SPDLOG_TRACE( "Conversation::dispatchRequestImpl: {}", msg.msg );

    ConversationBase::RequestStack stack( msg.msg.getName(), shared_from_this(), msg.connectionID );
    try
    {
        VERIFY_RTE_MSG( isRequest( msg.msg ), "Dispatch request got response: " << msg.msg );
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
    catch( mega::runtime::JITException& ex )
    {
        error( msg, ex.what(), yield_ctx );
    }
}

void Conversation::dispatchRemaining( boost::asio::yield_context& yield_ctx )
{
    bool bRemaining = true;
    while( bRemaining )
    {
        bRemaining = false;

        /* while( !m_deferedMessages.empty() || !m_unqueuedMessages.empty() )
         {
             run_one( yield_ctx );
             bRemaining = true;
         }*/

        // close out existing messages
        std::optional< network::ReceivedMsg > pendingMsgOpt = try_receive( yield_ctx );
        if( pendingMsgOpt.has_value() )
        {
            bRemaining = true;
            if( isRequest( pendingMsgOpt.value().msg ) )
            {
                SPDLOG_TRACE( "Conversation::dispatchRemaining {} got request: {}", getID(),
                              pendingMsgOpt.value().msg.getName() );
                dispatchRequestImpl( pendingMsgOpt.value(), yield_ctx );
            }
            else
            {
                SPDLOG_TRACE( "Conversation::dispatchRemaining {} got response: {}", getID(),
                              pendingMsgOpt.value().msg.getName() );
            }
        }
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

std::optional< ReceivedMsg > InThreadConversation::try_receive( boost::asio::yield_context& yield_ctx )
{
    std::optional< ReceivedMsg > result;

    m_channel.try_receive(
        [ &optMsg = result ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
        {
            if( !ec )
            {
                optMsg = msg;
            }
            else
            {
                THROW_TODO;
            }
        } );

    return result;
}

void InThreadConversation::send( const ReceivedMsg& msg )
{
    m_channel.async_send(
        boost::system::error_code(), msg,
        [ &msg ]( boost::system::error_code ec )
        {
            if( ec )
            {
                if( ec.value() == boost::asio::error::eof )
                {
                }
                else if( ec.value() == boost::asio::error::operation_aborted )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_closed )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_cancelled )
                {
                }
                else if( ec.failed() )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
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

std::optional< ReceivedMsg > ConcurrentConversation::try_receive( boost::asio::yield_context& yield_ctx )
{
    std::optional< ReceivedMsg > result;

    m_channel.try_receive(
        [ &optMsg = result ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
        {
            if( !ec )
            {
                optMsg = msg;
            }
            else
            {
                THROW_TODO;
            }
        } );

    return result;
}

void ConcurrentConversation::send( const ReceivedMsg& msg )
{
    m_channel.async_send(
        boost::system::error_code(), msg,
        [ &msg ]( boost::system::error_code ec )
        {
            if( ec )
            {
                if( ec.value() == boost::asio::error::eof )
                {
                }
                else if( ec.value() == boost::asio::error::operation_aborted )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_closed )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_cancelled )
                {
                }
                else if( ec.failed() )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
            }
        } );
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

ExternalConversation::ExternalConversation( ConversationManager&  conversationManager,
                                            const ConversationID& conversationID, boost::asio::io_context& ioContext )
    : m_conversationManager( conversationManager )
    , m_conversationID( conversationID )
    , m_ioContext( ioContext )
    , m_channel( m_ioContext )
{
}

ReceivedMsg ExternalConversation::receive()
{
    ReceivedMsg result;
    bool        bReceived = false;
    m_channel.async_receive(
        [ &bReceived, &result ]( boost::system::error_code ec, const ReceivedMsg& msg )
        {
            if( ec )
            {
                SPDLOG_ERROR( "Failed to receive msg with error: {}", ec.what() );
                THROW_RTE( "Failed to receive msg on channel: " << ec.what() );
            }
            else
            {
                result    = msg;
                bReceived = true;
            }
        } );
    while( !bReceived )
    {
        m_ioContext.run_one();
    }
    return result;
}

void ExternalConversation::send( const ReceivedMsg& msg )
{
    m_channel.async_send(
        boost::system::error_code(), msg,
        [ &msg ]( boost::system::error_code ec )
        {
            if( ec )
            {
                if( ec.value() == boost::asio::error::eof )
                {
                }
                else if( ec.value() == boost::asio::error::operation_aborted )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_closed )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_cancelled )
                {
                }
                else if( ec.failed() )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
            }
        } );
}
} // namespace mega::network
