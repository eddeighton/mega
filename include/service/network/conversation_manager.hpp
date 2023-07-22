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

#ifndef CONVERSATION_MANAGER_26_MAY_2022
#define CONVERSATION_MANAGER_26_MAY_2022

#include "service/protocol/common/conversation_id.hpp"

#include "service/network/conversation.hpp"

#include <shared_mutex>
#include <mutex>

namespace mega::network
{

class ConversationManager
{
    using ConversationPtrMap = std::map< ConversationID, ConversationBase::Ptr >;
    using ConversationIDSet  = std::set< ConversationID >;

public:
    ConversationManager( const std::string& strProcessName, boost::asio::io_context& ioContext );
    virtual ~ConversationManager() = 0;

    const std::string&       getProcessName() const { return m_strProcessName; }
    boost::asio::io_context& getIOContext() const;

    void onDisconnect( const ConnectionID& connectionID );

    std::vector< ConversationID > reportConversations() const;
    ConversationID                createConversationID() const;
    void                          externalConversationInitiated( ExternalConversation::Ptr pConversation );
    void                          conversationInitiated( ConversationBase::Ptr pConversation, Sender& parentSender );
    void                          conversationJoined( ConversationBase::Ptr pConversation );
    virtual void                  conversationCompleted( ConversationBase::Ptr pConversation );

    ConversationBase::Ptr findExistingConversation( const network::ConversationID& conversationID ) const;
    ExternalConversation::Ptr getExternalConversation() const;

    virtual ConversationBase::Ptr joinConversation( const ConnectionID& originatingConnectionID, const Message& msg )
        = 0;
    virtual void dispatch( const ReceivedMsg& msg );

protected:
    void spawnInitiatedConversation( ConversationBase::Ptr pConversation, Sender& parentSender );

protected:
    boost::asio::io_context&  m_ioContext;
    std::string               m_strProcessName;
    ConversationPtrMap        m_conversations;
    ExternalConversation::Ptr m_pExternalConversation;
    mutable std::shared_mutex m_mutex;
    using WriteLock = std::unique_lock< std::shared_mutex >;
    using ReadLock  = std::shared_lock< std::shared_mutex >;
};

} // namespace mega::network

#endif // CONVERSATION_MANAGER_26_MAY_2022
