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

public:
    ConversationManager( const char* pszProcessName, boost::asio::io_context& ioContext );
    virtual ~ConversationManager() = 0;

    const char*              getProcessName() const { return m_pszProcessName; }
    boost::asio::io_context& getIOContext() const;

    std::vector< ConversationID > reportActivities() const;
    ConversationID                createConversationID( const ConnectionID& connectionID ) const;
    void                          conversationStarted( ConversationBase::Ptr pConversation );
    void                          conversationCompleted( ConversationBase::Ptr pConversation );

    ConversationBase::Ptr findExistingConversation( const network::ConversationID& conversationID ) const;

    virtual ConversationBase::Ptr joinConversation( const ConnectionID& originatingConnectionID, const Header& header,
                                                    const Message& msg )
        = 0;
    virtual void dispatch( const Header& header, const ReceivedMsg& msg );

protected:
    boost::asio::io_context& m_ioContext;

private:
    const char*               m_pszProcessName;
    ConversationPtrMap        m_conversations;
    mutable std::shared_mutex m_mutex;
    using WriteLock = std::unique_lock< std::shared_mutex >;
    using ReadLock  = std::shared_lock< std::shared_mutex >;

    mutable ConversationID::ID m_nextConversationID = 0U;
};

} // namespace network
} // namespace mega

#endif // CONVERSATION_MANAGER_26_MAY_2022
