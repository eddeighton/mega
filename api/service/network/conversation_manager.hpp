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
    ConversationManager( const std::string& strProcessName, boost::asio::io_context& ioContext );
    virtual ~ConversationManager() = 0;

    const std::string&       getProcessName() const { return m_strProcessName; }
    boost::asio::io_context& getIOContext() const;

    std::vector< ConversationID > reportConversations() const;
    ConversationID                createConversationID( const ConnectionID& connectionID ) const;
    void                          conversationInitiated( ConversationBase::Ptr pConversation, Sender& parentSender );
    void                          conversationJoined( ConversationBase::Ptr pConversation );
    void                          conversationCompleted( ConversationBase::Ptr pConversation );

    ConversationBase::Ptr findExistingConversation( const network::ConversationID& conversationID ) const;

    virtual ConversationBase::Ptr joinConversation( const ConnectionID& originatingConnectionID, const Header& header,
                                                    const Message& msg )
        = 0;
    virtual void conversationNew( const Header& header, const ReceivedMsg& msg );
    virtual void conversationEnd( const Header& header, const ReceivedMsg& msg );

    virtual void dispatch( const Header& header, const ReceivedMsg& msg );

protected:
    boost::asio::io_context& m_ioContext;

private:
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
