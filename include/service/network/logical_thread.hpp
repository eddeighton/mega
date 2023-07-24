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

#include "service/network/sender_factory.hpp"

#include "service/protocol/common/received_message.hpp"
#include "service/protocol/common/logical_thread_id.hpp"
#include "service/protocol/common/logical_thread_base.hpp"
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

class LogicalThreadManager;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class LogicalThread : public LogicalThreadBase
{
public:
    LogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID );

    inline std::shared_ptr< LogicalThread > shared_from_this()
    {
        return std::dynamic_pointer_cast< LogicalThread >( LogicalThreadBase::shared_from_this() );
    }

    // Sender
    // NOTE: sender implemented to enable logical thread to receive responses in inter-thread communication
    virtual boost::system::error_code send( const Message& responseMessage ) override;
    virtual boost::system::error_code send( const Message& responseMessage, boost::asio::yield_context& ) override;

    // LogicalThreadBase
    const LogicalThreadID&     getID() const override { return m_logicalthreadID; }
    virtual Message            dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override;
    virtual const std::string& getProcessName() const override;
    virtual U64                getStackSize() const override { return m_stack.size(); }
    virtual void               onDisconnect( Sender::Ptr pRequestResponseSender ) override;

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

    ReceivedMessage receiveDeferred( boost::asio::yield_context& yield_ctx );

protected:
    virtual void                             unqueue();
    virtual bool                             queue( const ReceivedMessage& msg );
    virtual ReceivedMessage                  receive( boost::asio::yield_context& yield_ctx )     = 0;
    virtual std::optional< ReceivedMessage > try_receive( boost::asio::yield_context& yield_ctx ) = 0;

protected:
    virtual void requestStarted( Sender::Ptr pRequestResponseSender ) override;
    virtual void requestCompleted() override;

protected:
    void run_one( boost::asio::yield_context& yield_ctx );

protected:
    friend class LogicalThreadManager;
    // this is called by LogicalThreadManager but can be overridden in initiating activities
    virtual void    run( boost::asio::yield_context& yield_ctx ) override;
    virtual Message dispatchRequest( const Message& msg, boost::asio::yield_context& yield_ctx ) = 0;

protected:
    void dispatchRequestImpl( const ReceivedMessage& msg, boost::asio::yield_context& yield_ctx );
    void dispatchRemaining( boost::asio::yield_context& yield_ctx );

protected:
    LogicalThreadManager& m_logicalthreadManager;
    LogicalThreadID       m_logicalthreadID;

    std::vector< Sender::Ptr >              m_stack;
    std::set< Sender::Ptr >                 m_disconnections;
    bool                                    m_bQueueing = false;
    std::vector< network::ReceivedMessage > m_deferedMessages;
    std::vector< network::ReceivedMessage > m_unqueuedMessages;
    bool                                    m_bEnableQueueing = false;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class InThreadLogicalThread : public LogicalThread
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
class ConcurrentLogicalThread : public LogicalThread
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

    virtual boost::system::error_code send( const Message& responseMessage ) override { THROW_TODO; }
    virtual boost::system::error_code send( const Message& responseMessage, boost::asio::yield_context& ) override
    {
        THROW_TODO;
    }

    // make this virtual so that windows can build...
    virtual ReceivedMessage receive();

    // LogicalThreadBase
    virtual const LogicalThreadID& getID() const override { return m_logicalthreadID; }
    // virtual void      send( const ReceivedMessage& msg );

    // NOT IMPLEMENTED - no stack or coroutine for external logicalthread
    virtual Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override { THROW_TODO; }
    virtual void    run( boost::asio::yield_context& yield_ctx ) override { THROW_TODO; }
    virtual const std::string& getProcessName() const override { THROW_TODO; }
    virtual U64                getStackSize() const override { THROW_TODO; }
    virtual void               onDisconnect( Sender::Ptr pRequestResponseSender ) override { THROW_TODO; }
    virtual void               requestStarted( Sender::Ptr pRequestResponseSender ) override { ; }
    virtual void               requestCompleted() override { ; }
    virtual void               receive( const ReceivedMessage& msg ) override;

protected:
    LogicalThreadManager&    m_logicalthreadManager;
    LogicalThreadID          m_logicalthreadID;
    boost::asio::io_context& m_ioContext;
    MessageChannel           m_channel;
};

} // namespace mega::network

#endif // LOGICALTHREAD_24_MAY_2022
