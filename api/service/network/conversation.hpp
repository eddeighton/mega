#ifndef CONVERSATION_24_MAY_2022
#define CONVERSATION_24_MAY_2022

#include "service/network/sender.hpp"

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
    ConnectionID connectionID;
    Message      msg;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class ConversationBase : public std::enable_shared_from_this< ConversationBase >, public Sender
{
public:
    virtual ~ConversationBase() {}

    using Ptr = std::shared_ptr< ConversationBase >;
    using ID  = ConversationID;

    virtual const ID&          getID() const                                                          = 0;
    virtual void               send( const ReceivedMsg& msg )                                         = 0;
    virtual Message            dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) = 0;
    virtual void               run( boost::asio::yield_context& yield_ctx )                           = 0;
    virtual const std::string& getProcessName() const                                                 = 0;

protected:
    virtual void requestStarted( const ConnectionID& connectionID ) = 0;
    virtual void requestCompleted()                                 = 0;

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
    const std::optional< ConnectionID >& getOriginatingEndPointID() const
    {
        ASSERT( !m_originatingEndPoint.has_value() || ( m_originatingEndPoint.value() == m_stack.front() ) );
        return m_originatingEndPoint;
    }
    std::optional< ConnectionID > getStackConnectionID() const
    {
        if ( !m_stack.empty() )
            return m_stack.back();
        else
            return std::optional< ConnectionID >();
    }

protected:
    virtual ReceivedMsg receive( boost::asio::yield_context& yield_ctx ) = 0;

public:
    virtual void send( const ReceivedMsg& msg ) = 0;

    // Sender
    virtual ConnectionID getConnectionID() const
    {
        if ( m_selfConnectionID.has_value() )
            return m_selfConnectionID.value();
        // synthesize a connectionID value
        std::ostringstream os;
        os << "self_" << getID();
        m_selfConnectionID = os.str();
        return m_selfConnectionID.value();
    }
    virtual boost::system::error_code send( const ConversationID& conversationID, const Message& msg,
                                            boost::asio::yield_context& yield_ctx )
    {
        const ReceivedMsg rMsg{ getConnectionID(), msg };
        send( rMsg );
        return boost::system::error_code{};
    }
    virtual void sendErrorResponse( const ConversationID& conversationID, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        const ReceivedMsg rMsg{ getConnectionID(), make_error_msg( strErrorMsg ) };
        send( rMsg );
    }

protected:
    virtual void               requestStarted( const ConnectionID& connectionID );
    virtual void               requestCompleted();
    virtual const std::string& getProcessName() const;

protected:
    void run_one( boost::asio::yield_context& yield_ctx );

protected:
    friend class ConversationManager;
    // this is called by ConversationManager but can be overridden in initiating activities
    virtual void run( boost::asio::yield_context& yield_ctx );
    virtual bool dispatchRequest( const Message& msg, boost::asio::yield_context& yield_ctx ) = 0;
    virtual void error( const ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
        = 0;

public:
    // this is used by the Request_Encode generated classes
    virtual Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx );

protected:
    void dispatchRequestImpl( const ReceivedMsg& msg, boost::asio::yield_context& yield_ctx );

protected:
    ConversationManager&                  m_conversationManager;
    ConversationID                        m_conversationID;
    std::optional< ConnectionID >         m_originatingEndPoint;
    std::vector< ConnectionID >           m_stack;
    mutable std::optional< ConnectionID > m_selfConnectionID;
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
