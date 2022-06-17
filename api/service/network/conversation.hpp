#ifndef CONVERSATION_24_MAY_2022
#define CONVERSATION_24_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/messages.hxx"

#include "common/assert_verify.hpp"

#include "boost/asio/spawn.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/experimental/concurrent_channel.hpp"
#include "boost/asio/experimental/channel.hpp"

#include <cstddef>
#include <memory>

namespace mega
{
namespace network
{

class ConversationManager;

struct ReceivedMsg
{
    ConnectionID   connectionID;
    MessageVariant msg;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class ConversationBase : public std::enable_shared_from_this< ConversationBase >
{
public:
    virtual ~ConversationBase() {}

    using Ptr = std::shared_ptr< ConversationBase >;
    using ID  = ConversationID;

    virtual const ID&      getID() const                                                          = 0;
    virtual void           send( const ReceivedMsg& msg )                                         = 0;
    virtual MessageVariant dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) = 0;
    virtual void           run( boost::asio::yield_context& yield_ctx )                           = 0;
    virtual void           requestStarted( const ConnectionID& connectionID )                     = 0;
    virtual void           requestCompleted()                                                     = 0;
    virtual const char*    getProcessName() const                                                 = 0;

public:
    class RequestStack
    {
        const char*                           m_pszMsg;
        boost::asio::steady_timer::time_point m_startTime;
        ConversationBase&                     conversation;
        RequestStack( RequestStack& ) = delete;
        RequestStack& operator=( RequestStack& ) = delete;

    public:
        RequestStack( const char* pszMsg, ConversationBase& conversation, const ConnectionID& connectionID );
        ~RequestStack();
    };
    friend class ConversationBase::RequestStack;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class Conversation : public ConversationBase
{
public:
    Conversation( ConversationManager& conversationManager, const ConversationID& conversationID,
                  std::optional< ConnectionID > originatingConnectionID = std::nullopt );
    virtual ~Conversation();

    const ConversationID&                getID() const { return m_conversationID; }
    const std::optional< ConnectionID >& getOriginatingEndPointID() const { return m_originatingEndPoint; }

protected:
    virtual ReceivedMsg receive( boost::asio::yield_context& yield_ctx ) = 0;

public:
    virtual void send( const ReceivedMsg& msg ) = 0;

protected:
    virtual void        requestStarted( const ConnectionID& connectionID );
    virtual void        requestCompleted();
    virtual const char* getProcessName() const;

protected:
    void run_one( boost::asio::yield_context& yield_ctx );

protected:
    friend class ConversationManager;
    // this is called by ConversationManager but can be overridden in initiating activities
    virtual void run( boost::asio::yield_context& yield_ctx );
    virtual bool dispatchRequest( const MessageVariant& msg, boost::asio::yield_context& yield_ctx ) = 0;
    virtual void error( const ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
        = 0;

public:
    // this is used by the Request_Encode generated classes
    virtual MessageVariant dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx );

protected:
    void dispatchRequestImpl( const ReceivedMsg& msg, boost::asio::yield_context& yield_ctx );

protected:
    ConversationManager&          m_conversationManager;
    ConversationID                m_conversationID;
    std::optional< ConnectionID > m_originatingEndPoint;
    std::vector< ConnectionID >   m_stack;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class InThreadConversation : public Conversation
{
    using MessageChannel = boost::asio::experimental::channel< void( boost::system::error_code, ReceivedMsg ) >;

public:
    InThreadConversation( ConversationManager& conversationManager, const ConversationID& conversationID,
                          std::optional< ConnectionID > originatingConnectionID = std::nullopt );

protected:
    virtual ReceivedMsg receive( boost::asio::yield_context& yield_ctx );
    virtual void        send( const ReceivedMsg& msg );

private:
    MessageChannel m_channel;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class ConcurrentConversation : public Conversation
{
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, ReceivedMsg ) >;

public:
    ConcurrentConversation( ConversationManager& conversationManager, const ConversationID& conversationID,
                            std::optional< ConnectionID > originatingConnectionID = std::nullopt );

protected:
    virtual ReceivedMsg receive( boost::asio::yield_context& yield_ctx );
    virtual void        send( const ReceivedMsg& msg );

private:
    MessageChannel m_channel;
};

} // namespace network
} // namespace mega

#endif // CONVERSATION_24_MAY_2022
