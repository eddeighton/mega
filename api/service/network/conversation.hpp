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
    virtual void               onDisconnect( const ConnectionID& connectionID )                       = 0;

protected:
    virtual void requestStarted( const ConnectionID& connectionID ) = 0;
    virtual void requestCompleted()                                 = 0;

public:
    class RequestStack
    {
        const char*                           m_pszMsg;
        boost::asio::steady_timer::time_point m_startTime;
        ConversationBase&                     conversation;
        RequestStack( RequestStack& )            = delete;
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
    virtual boost::system::error_code send( const Message& msg, boost::asio::yield_context& yield_ctx )
    {
        const ReceivedMsg rMsg{ getConnectionID(), msg };
        send( rMsg );
        return boost::system::error_code{};
    }
    virtual void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        const ReceivedMsg rMsg{ getConnectionID(), make_error_msg( msg.msg.receiver, strErrorMsg ) };
        send( rMsg );
    }

protected:
    virtual void               requestStarted( const ConnectionID& connectionID );
    virtual void               requestCompleted();
    virtual const std::string& getProcessName() const;
    virtual void               onDisconnect( const ConnectionID& connectionID );

protected:
    void run_one( boost::asio::yield_context& yield_ctx );

protected:
    friend class ConversationManager;
    // this is called by ConversationManager but can be overridden in initiating activities
    virtual void    run( boost::asio::yield_context& yield_ctx );
    virtual Message dispatchRequest( const Message& msg, boost::asio::yield_context& yield_ctx ) = 0;
    virtual void    dispatchResponse( const network::ConnectionID& connectionID, const Message& msg,
                                      boost::asio::yield_context& yield_ctx )
        = 0;
    virtual void error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
        = 0;

public:
    virtual Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx );

protected:
    void dispatchRequestImpl( const ReceivedMsg& msg, boost::asio::yield_context& yield_ctx );

protected:
    ConversationManager& m_conversationManager;
    ConversationID       m_conversationID;

    std::optional< ConnectionID >         m_originatingEndPoint;
    std::vector< ConnectionID >           m_stack;
    std::set< ConnectionID >              m_disconnections;
    mutable std::optional< ConnectionID > m_selfConnectionID;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class InThreadConversation : public Conversation
{
    using MessageChannel = boost::asio::experimental::channel< void( boost::system::error_code, ReceivedMsg ) >;

public:
    InThreadConversation( ConversationManager&          conversationManager,
                          const ConversationID&         conversationID,
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
    ConcurrentConversation( ConversationManager&          conversationManager,
                            const ConversationID&         conversationID,
                            std::optional< ConnectionID > originatingConnectionID = std::nullopt );

protected:
    virtual ReceivedMsg receive( boost::asio::yield_context& yield_ctx );
    virtual void        send( const ReceivedMsg& msg );

protected:
    MessageChannel m_channel;
};

} // namespace network
} // namespace mega

#endif // CONVERSATION_24_MAY_2022
