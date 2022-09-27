
#include "service/network/conversation_manager.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/messages.hxx"

#include "boost/asio/this_coro.hpp"

#include <iostream>

namespace mega
{
namespace network
{

ConversationManager::ConversationManager( const std::string& strProcessName, boost::asio::io_context& ioContext )
    : m_strProcessName( strProcessName )
    , m_ioContext( ioContext )
{
}
ConversationManager::~ConversationManager() {}

boost::asio::io_context& ConversationManager::getIOContext() const { return m_ioContext; }

void ConversationManager::onDisconnect( const ConnectionID& connectionID )
{
    WriteLock lock( m_mutex );
    for ( const auto& [ id, pConversation ] : m_conversations )
    {
        pConversation->onDisconnect( connectionID );
    }
}

std::vector< ConversationID > ConversationManager::reportConversations() const
{
    std::vector< ConversationID > activities;
    {
        ReadLock lock( m_mutex );
        for ( const auto& [ id, pConversation ] : m_conversations )
        {
            activities.push_back( id );
        }
    }
    return activities;
}

ConversationID ConversationManager::createConversationID( const ConnectionID& connectionID ) const
{
    WriteLock lock( m_mutex );
    return ConversationID( ++m_nextConversationID, connectionID );
}

void ConversationManager::spawnInitiatedConversation( ConversationBase::Ptr pConversation, Sender& parentSender )
{
    // clang-format off
    boost::asio::spawn
    (
        m_ioContext, 
        [ pConversation, &parentSender ]( boost::asio::yield_context yield_ctx ) 
        { 
            ConversationBase::RequestStack stack( "Initiated", *pConversation, parentSender.getConnectionID() );
            pConversation->run( yield_ctx );
        }
/*#ifdef WIN32
    // segmented stacks do NOT work on windows
        ,boost::coroutines::attributes( 0xffffff ) //16,777,215₁₀
#endif*/
    );
    // clang-format on
    // SPDLOG_TRACE( "ConversationBase Started id: {}", pConversation->getID() );
}

void ConversationManager::conversationInitiated( ConversationBase::Ptr pConversation, Sender& parentSender )
{
    {
        WriteLock lock( m_mutex );
        m_conversations.insert( std::make_pair( pConversation->getID(), pConversation ) );
    }
    spawnInitiatedConversation( pConversation, parentSender );
}

void ConversationManager::conversationJoined( ConversationBase::Ptr pConversation )
{
    {
        WriteLock lock( m_mutex );
        m_conversations.insert( std::make_pair( pConversation->getID(), pConversation ) );
    }
    // clang-format off
    boost::asio::spawn
    (
        m_ioContext, 
        [ pConversation ]( boost::asio::yield_context yield_ctx ) 
        { 
            pConversation->run( yield_ctx ); 
        } 
/*#ifdef WIN32
    // segmented stacks do NOT work on windows
        ,boost::coroutines::attributes( 0xffffff ) //16,777,215₁₀
#endif*/
    );
    // clang-format on
    // SPDLOG_TRACE( "ConversationBase Started id: {}", pConversation->getID() );
}

void ConversationManager::conversationCompleted( ConversationBase::Ptr pConversation )
{
    {
        WriteLock lock( m_mutex );
        ConversationPtrMap::const_iterator iFind = m_conversations.find( pConversation->getID() );
        VERIFY_RTE( iFind != m_conversations.end() );
        {
            //SPDLOG_TRACE( "conversationCompleted {}", pConversation->getID() );
            m_conversations.erase( iFind );
        }
    }
    // SPDLOG_DEBUG( "ConversationBase Completed id: {}", pConversation->getID() );
}

ConversationBase::Ptr ConversationManager::findExistingConversation( const ConversationID& conversationID ) const
{
    ReadLock                           lock( m_mutex );
    ConversationPtrMap::const_iterator iFind = m_conversations.find( conversationID );
    if ( iFind != m_conversations.end() )
    {
        return iFind->second;
    }
    else
    {
        return ConversationBase::Ptr();
    }
}

void ConversationManager::dispatch( const ReceivedMsg& msg )
{
    ConversationBase::Ptr pConversation = findExistingConversation( getMsgReceiver( msg.msg ) );
    if ( !pConversation )
    {
        pConversation = joinConversation( msg.connectionID, msg.msg );
        conversationJoined( pConversation );
        // SPDLOG_TRACE( "Received msg {}. Started new conversation {}.",
        //               getMsgNameFromID( header.getMessageID() ),
        //               pConversation->getID() );
    }
    else
    {
        // SPDLOG_TRACE( "Received msg: {}. Resumed existing conversation: {}.",
        //               getMsgNameFromID( header.getMessageID() ),
        //               pConversation->getID() );
    }
    pConversation->send( msg );
}

} // namespace network
} // namespace mega