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

#include "service/network/conversation_manager.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/conversation_id.hpp"
#include "service/protocol/model/messages.hxx"

#include <boost/asio/this_coro.hpp>

#include <iostream>

namespace mega::network
{

ConversationManager::ConversationManager( const std::string& strProcessName, boost::asio::io_context& ioContext )
    : m_strProcessName( strProcessName )
    , m_ioContext( ioContext )
{
}
ConversationManager::~ConversationManager()
{
}

boost::asio::io_context& ConversationManager::getIOContext() const
{
    return m_ioContext;
}

void ConversationManager::onDisconnect( const ConnectionID& connectionID )
{
    WriteLock lock( m_mutex );
    for( const auto& [ id, pConversation ] : m_conversations )
    {
        pConversation->onDisconnect( connectionID );
    }
}

std::vector< ConversationID > ConversationManager::reportConversations() const
{
    std::vector< ConversationID > activities;
    {
        ReadLock lock( m_mutex );
        for( const auto& [ id, pConversation ] : m_conversations )
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

// static const mega::U64 NON_SEGMENTED_STACK_SIZE = 0x0FFFFF; // 1M bytes
// static const mega::U64 NON_SEGMENTED_STACK_SIZE = 0xAFFFFF; // 11,534,335
static const mega::U64 NON_SEGMENTED_STACK_SIZE = 0x400000;
// static const mega::U64 NON_SEGMENTED_STACK_SIZE = 0x200000; // 2,097,152

void ConversationManager::spawnInitiatedConversation( ConversationBase::Ptr pConversation, Sender& parentSender )
{
    // clang-format off
    boost::asio::spawn
    (
        m_ioContext, 
        [pConversation, &parentSender](boost::asio::yield_context yield_ctx)
        {
            ConversationBase::RequestStack stack("spawnInitiatedConversation", pConversation, parentSender.getConnectionID());
            pConversation->run(yield_ctx);
        }
        // segmented stacks do NOT work on windows
#ifndef BOOST_USE_SEGMENTED_STACKS
        , boost::coroutines::attributes(NON_SEGMENTED_STACK_SIZE)
#endif
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
        // segmented stacks do NOT work on windows
#ifndef BOOST_USE_SEGMENTED_STACKS
        , boost::coroutines::attributes(NON_SEGMENTED_STACK_SIZE)
#endif
    );
    // clang-format on
    // SPDLOG_TRACE( "ConversationBase Started id: {}", pConversation->getID() );
}

void ConversationManager::conversationCompleted( ConversationBase::Ptr pConversation )
{
    {
        WriteLock                          lock( m_mutex );
        ConversationPtrMap::const_iterator iFind = m_conversations.find( pConversation->getID() );
        if( iFind != m_conversations.end() )
            ;
        {
            // SPDLOG_TRACE( "conversationCompleted {}", pConversation->getID() );
            m_conversations.erase( iFind );
        }
    }
    // SPDLOG_DEBUG( "ConversationBase Completed id: {}", pConversation->getID() );
}

ConversationBase::Ptr ConversationManager::findExistingConversation( const ConversationID& conversationID ) const
{
    ReadLock                           lock( m_mutex );
    ConversationPtrMap::const_iterator iFind = m_conversations.find( conversationID );
    if( iFind != m_conversations.end() )
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
    ConversationBase::Ptr pConversation = findExistingConversation( msg.msg.getReceiverID() );
    if( !pConversation )
    {
        pConversation = joinConversation( msg.connectionID, msg.msg );
        conversationJoined( pConversation );
    }
    else
    {
        //
    }
    pConversation->send( msg );
}

} // namespace mega::network
