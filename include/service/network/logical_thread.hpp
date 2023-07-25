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

#ifndef LOGICALTHREAD_24_MAY_2022
#define LOGICALTHREAD_24_MAY_2022

#include "service/protocol/common/received_message.hpp"
#include "service/protocol/common/logical_thread_id.hpp"
#include "service/protocol/common/logical_thread_base.hpp"
#include "service/protocol/model/messages.hxx"

#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>
#include <boost/asio/experimental/channel.hpp>

#include <cstddef>
#include <memory>

namespace mega::network
{

class LogicalThreadManager;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class LogicalThread : public LogicalThreadBase
{
public:
    using Ptr = std::shared_ptr< LogicalThread >;

    LogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID );

    inline std::shared_ptr< LogicalThread > shared_from_this()
    {
        return std::dynamic_pointer_cast< LogicalThread >( LogicalThreadBase::shared_from_this() );
    }

    virtual Message         dispatchInBoundRequestsUntilResponse( boost::asio::yield_context& yield_ctx );
    virtual ReceivedMessage receiveDeferred( boost::asio::yield_context& yield_ctx );
    virtual void            run( boost::asio::yield_context& yield_ctx );

protected:
    virtual void run_one( boost::asio::yield_context& yield_ctx );
    void         acknowledgeInboundRequest( const ReceivedMessage& msg, boost::asio::yield_context& yield_ctx );
    void         dispatchRemaining( boost::asio::yield_context& yield_ctx );

    virtual ReceivedMessage                  receive( boost::asio::yield_context& yield_ctx )           = 0;
    virtual std::optional< ReceivedMessage > try_receive( boost::asio::yield_context& yield_ctx )       = 0;
    virtual Message dispatchInBoundRequest( const Message& msg, boost::asio::yield_context& yield_ctx ) = 0;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class StackfulLogicalThread : public LogicalThread
{
public:
    StackfulLogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID );
    inline Sender::Ptr getStackResponseSender() const
    {
        if( !m_stack.empty() )
            return m_stack.back();
        else
            return {};
    }
    inline Sender::Ptr getOriginatingStackResponseSender() const
    {
        if( !m_stack.empty() )
            return m_stack.front();
        else
            return {};
    }

protected:
    virtual void requestStarted( Sender::Ptr pRequestResponseSender ) override;
    virtual void requestCompleted() override;

protected:
    virtual Message         dispatchInBoundRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override;
    virtual ReceivedMessage receiveDeferred( boost::asio::yield_context& yield_ctx ) override;
    virtual void            unqueue();
    virtual bool            queue( const ReceivedMessage& msg );
    virtual void            run( boost::asio::yield_context& yield_ctx ) override;
    virtual void            run_one( boost::asio::yield_context& yield_ctx ) override;

protected:
    std::vector< Sender::Ptr >              m_stack;
    std::vector< network::ReceivedMessage > m_deferedMessages;
    std::vector< network::ReceivedMessage > m_unqueuedMessages;
    bool                                    m_bQueueing       = false;
    bool                                    m_bEnableQueueing = false;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class InThreadLogicalThread : public StackfulLogicalThread
{
    using MessageChannel = boost::asio::experimental::channel< void( boost::system::error_code, ReceivedMessage ) >;

public:
    InThreadLogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID );

protected:
    virtual ReceivedMessage                  receive( boost::asio::yield_context& yield_ctx ) override;
    virtual std::optional< ReceivedMessage > try_receive( boost::asio::yield_context& yield_ctx ) override;
    virtual void                             receive( const ReceivedMessage& msg ) override;

private:
    MessageChannel m_channel;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class ConcurrentLogicalThread : public StackfulLogicalThread
{
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, ReceivedMessage ) >;

public:
    ConcurrentLogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID );

protected:
    virtual ReceivedMessage                  receive( boost::asio::yield_context& yield_ctx ) override;
    virtual std::optional< ReceivedMessage > try_receive( boost::asio::yield_context& yield_ctx ) override;
    virtual void                             receive( const ReceivedMessage& msg ) override;

protected:
    MessageChannel m_channel;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class ExternalLogicalThread : public LogicalThreadBase
{
    using MessageChannel = boost::asio::experimental::channel< void( boost::system::error_code, ReceivedMessage ) >;

public:
    using Ptr = std::shared_ptr< ExternalLogicalThread >;

    ExternalLogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID,
                           boost::asio::io_context& ioContext );

    // make this virtual so that windows can build...
    virtual ReceivedMessage receive();
    virtual void            receive( const ReceivedMessage& msg ) override;

protected:
    boost::asio::io_context& m_ioContext;
    MessageChannel           m_channel;
};

} // namespace mega::network

#endif // LOGICALTHREAD_24_MAY_2022
