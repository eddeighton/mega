


#ifndef CONVERSATION_MANAGER_26_MAY_2022
#define CONVERSATION_MANAGER_26_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/network/conversation.hpp"

#include <shared_mutex>
#include <mutex>

namespace mega
{
namespace network
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
    ConversationID                createConversationID( const ConnectionID& connectionID ) const;
    void                          conversationInitiated( ConversationBase::Ptr pConversation, Sender& parentSender );
    void                          conversationJoined( ConversationBase::Ptr pConversation );
    virtual void                  conversationCompleted( ConversationBase::Ptr pConversation );

    ConversationBase::Ptr findExistingConversation( const network::ConversationID& conversationID ) const;

    virtual ConversationBase::Ptr joinConversation( const ConnectionID& originatingConnectionID, const Message& msg )
        = 0;
    virtual void dispatch( const ReceivedMsg& msg );

protected:
    void spawnInitiatedConversation( ConversationBase::Ptr pConversation, Sender& parentSender );

protected:
    boost::asio::io_context&  m_ioContext;
    std::string               m_strProcessName;
    ConversationPtrMap        m_conversations;
    mutable std::shared_mutex m_mutex;
    using WriteLock = std::unique_lock< std::shared_mutex >;
    using ReadLock  = std::shared_lock< std::shared_mutex >;

    mutable ConversationID::ID m_nextConversationID = 0U;
};

} // namespace network
} // namespace mega

#endif // CONVERSATION_MANAGER_26_MAY_2022
