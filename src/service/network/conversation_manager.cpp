
#include "service/network/conversation_manager.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/messages.hxx"

#include <iostream>

namespace mega
{
namespace network
{

ConversationManager::ConversationManager( const char* pszProcessName, boost::asio::io_context& ioContext )
    : m_pszProcessName( pszProcessName )
    , m_ioContext( ioContext )
{
}
ConversationManager::~ConversationManager() {}

boost::asio::io_context& ConversationManager::getIOContext() const { return m_ioContext; }

std::vector< ConversationID > ConversationManager::reportActivities() const
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

void ConversationManager::conversationStarted( ConversationBase::Ptr pConversation )
{
    {
        WriteLock lock( m_mutex );
        m_conversations.insert( std::make_pair( pConversation->getID(), pConversation ) );
    }
    boost::asio::spawn(
        m_ioContext, [ pConversation ]( boost::asio::yield_context yield_ctx ) { pConversation->run( yield_ctx ); } );
    SPDLOG_TRACE( "ConversationBase Started id: {} end point: {}", pConversation->getID().getID(),
                  pConversation->getID().getConnectionID() );
}

void ConversationManager::conversationCompleted( ConversationBase::Ptr pConversation )
{
    {
        WriteLock                          lock( m_mutex );
        ConversationPtrMap::const_iterator iFind = m_conversations.find( pConversation->getID() );
        VERIFY_RTE( iFind != m_conversations.end() );
        m_conversations.erase( iFind );
    }
    SPDLOG_DEBUG( "ConversationBase Completed id: {} end point: {}", pConversation->getID().getID(),
                  pConversation->getID().getConnectionID() );
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

void ConversationManager::dispatch( const Header& header, const ReceivedMsg& msg )
{
    ConversationBase::Ptr pConversation = findExistingConversation( header.getConversationID() );
    if ( !pConversation )
    {
        pConversation = joinConversation( msg.connectionID, header, msg.msg );
        conversationStarted( pConversation );
        SPDLOG_TRACE( "Received msg {}. Started new conversation {}.",
                      getMsgNameFromID( header.getMessageID() ),
                      pConversation->getID().getID() );
    }
    else
    {
        SPDLOG_TRACE( "Received msg: {}. Resumed existing conversation: {}.",
                      getMsgNameFromID( header.getMessageID() ),
                      pConversation->getID().getID() );
    }

    pConversation->send( msg );
}

} // namespace network
} // namespace mega