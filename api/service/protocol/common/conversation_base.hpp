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

#ifndef CONVERSATION_BASE_5_JAN_2023
#define CONVERSATION_BASE_5_JAN_2023

#include "service/protocol/common/sender.hpp"
#include "service/protocol/common/conversation_id.hpp"
#include "service/protocol/model/messages.hxx"

#include "common/assert_verify.hpp"

#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>
#include <boost/asio/experimental/channel.hpp>

#include <cstddef>
#include <memory>

namespace mega::network
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
        const char* m_pszMsg;
        // boost::asio::steady_timer::time_point m_startTime;
        ConversationBase::Ptr pConversation;
        RequestStack( RequestStack& )            = delete;
        RequestStack& operator=( RequestStack& ) = delete;

    public:
        RequestStack( const char* pszMsg, ConversationBase::Ptr pConversation, const ConnectionID& connectionID );
        ~RequestStack();
    };
    friend class ConversationBase::RequestStack;
};

}

#endif //CONVERSATION_BASE_5_JAN_2023
