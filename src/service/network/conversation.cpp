#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/log.hpp"

#include "service/network/end_point.hpp"
#include "service/protocol/model/messages.hxx"

#include <chrono>
#include <iostream>

namespace mega
{
namespace network
{

ConversationBase::RequestStack::RequestStack( const char* pszMsg, ConversationBase& conversation,
                                              const ConnectionID& connectionID )
    : m_pszMsg( pszMsg )
    , m_startTime( std::chrono::steady_clock::now() )
    , conversation( conversation )
{
    conversation.requestStarted( connectionID );
}
ConversationBase::RequestStack::~RequestStack()
{
    const auto timeDelta = std::chrono::steady_clock::now() - m_startTime;
    SPDLOG_DEBUG( "{} {} {} {}", conversation.getProcessName(), conversation.getID().getID(), m_pszMsg, timeDelta );
    conversation.requestCompleted();
}

Conversation::Conversation( ConversationManager& conversationManager, const ConversationID& conversationID,
                            std::optional< ConnectionID > originatingConnectionID )
    : m_conversationManager( conversationManager )
    , m_conversationID( conversationID )
    , m_originatingEndPoint( originatingConnectionID )
{
}

Conversation::~Conversation() {}

void Conversation::requestStarted( const ConnectionID& connectionID )
{
    //
    m_stack.push_back( connectionID );
}

void Conversation::requestCompleted()
{
    VERIFY_RTE( !m_stack.empty() );
    m_stack.pop_back();
    if ( m_stack.empty() )
    {
        m_conversationManager.conversationCompleted( shared_from_this() );
    }
}
const char* Conversation::getProcessName() const { return m_conversationManager.getProcessName(); }

// run is ALWAYS call for each conversation after it is created
void Conversation::run( boost::asio::yield_context& yield_ctx )
{
    try
    {
        do
        {
            run_one( yield_ctx );
        } while ( !m_stack.empty() );
    }
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "Conversation: {} exception: {}", getID().getID(), ex.what() );
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
    while ( true )
    {
        msg = receive( yield_ctx );
        if ( isRequest( msg.msg ) )
        {
            dispatchRequestImpl( msg, yield_ctx );
        }
        else
        {
            break;
        }
    }
    if ( getMsgID( msg.msg ) == MSG_Error_Response::ID )
    {
        throw std::runtime_error( MSG_Error_Response::get( msg.msg ).what );
    }
    return msg.msg;
}

void Conversation::dispatchRequestImpl( const ReceivedMsg& msg, boost::asio::yield_context& yield_ctx )
{
    ConversationBase::RequestStack stack( getMsgName( msg.msg ), *this, msg.connectionID );
    try
    {
        ASSERT( isRequest( msg.msg ) );
        if ( !dispatchRequest( msg.msg, yield_ctx ) )
        {
            SPDLOG_ERROR( "Failed to dispatch request: {} on conversation: {}", msg.msg, getID().getID() );
            THROW_RTE( "Failed to dispatch request message: " << msg.msg );
        }
    }
    catch ( std::exception& ex )
    {
        error( m_stack.back(), ex.what(), yield_ctx );
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
                              if ( ec )
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
                              if ( ec )
                              {
                                  SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                                  THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                              }
                          } );
}

} // namespace network
} // namespace mega
