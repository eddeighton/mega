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
    LogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID,
                   std::optional< ConnectionID > originatingConnectionID = std::nullopt );
    virtual ~LogicalThread();

    const LogicalThreadID&               getID() const { return m_logicalthreadID; }
    const std::optional< ConnectionID >& getOriginatingEndPointID() const
    {
        ASSERT( !m_originatingEndPoint.has_value() || ( m_originatingEndPoint.value() == m_stack.front() ) );
        return m_originatingEndPoint;
    }
    std::optional< ConnectionID > getStackConnectionID() const
    {
        if( !m_stack.empty() )
            return m_stack.back();
        else
            return {};
    }

    ReceivedMsg receiveDeferred( boost::asio::yield_context& yield_ctx );

protected:
    virtual void                         unqueue();
    virtual bool                         queue( const ReceivedMsg& msg );
    virtual ReceivedMsg                  receive( boost::asio::yield_context& yield_ctx )     = 0;
    virtual std::optional< ReceivedMsg > try_receive( boost::asio::yield_context& yield_ctx ) = 0;

public:
    virtual void send( const ReceivedMsg& msg ) = 0;

    // Sender
    virtual ConnectionID getConnectionID() const
    {
        if( m_selfConnectionID.has_value() )
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
    virtual void sendErrorResponse( const ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        const ReceivedMsg rMsg{ getConnectionID(), make_error_msg( msg.msg.getReceiverID(), strErrorMsg ) };
        send( rMsg );
    }

protected:
    virtual void               requestStarted( const ConnectionID& connectionID );
    virtual void               requestCompleted();
    virtual const std::string& getProcessName() const;
    virtual U64                getStackSize() const { return m_stack.size(); }
    virtual void               onDisconnect( const ConnectionID& connectionID );

protected:
    void run_one( boost::asio::yield_context& yield_ctx );

protected:
    friend class LogicalThreadManager;
    // this is called by LogicalThreadManager but can be overridden in initiating activities
    virtual void    run( boost::asio::yield_context& yield_ctx );
    virtual Message dispatchRequest( const Message& msg, boost::asio::yield_context& yield_ctx ) = 0;
    virtual void    dispatchResponse( const ConnectionID& connectionID, const Message& msg,
                                      boost::asio::yield_context& yield_ctx )
        = 0;
    virtual void error( const ReceivedMsg& msg, const std::string& strErrorMsg, boost::asio::yield_context& yield_ctx )
        = 0;

public:
    virtual Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx );

protected:
    void dispatchRequestImpl( const ReceivedMsg& msg, boost::asio::yield_context& yield_ctx );
    void dispatchRemaining( boost::asio::yield_context& yield_ctx );

protected:
    LogicalThreadManager& m_logicalthreadManager;
    LogicalThreadID       m_logicalthreadID;

    std::optional< ConnectionID >         m_originatingEndPoint;
    std::vector< ConnectionID >           m_stack;
    std::set< ConnectionID >              m_disconnections;
    mutable std::optional< ConnectionID > m_selfConnectionID;
    bool                                  m_bQueueing = false;
    std::vector< network::ReceivedMsg >   m_deferedMessages;
    std::vector< network::ReceivedMsg >   m_unqueuedMessages;
    bool                                  m_bEnableQueueing = false;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class InThreadLogicalThread : public LogicalThread
{
    using MessageChannel = boost::asio::experimental::channel< void( boost::system::error_code, ReceivedMsg ) >;

public:
    InThreadLogicalThread( LogicalThreadManager&         logicalthreadManager,
                           const LogicalThreadID&        logicalthreadID,
                           std::optional< ConnectionID > originatingConnectionID = std::nullopt );

protected:
    virtual ReceivedMsg                  receive( boost::asio::yield_context& yield_ctx );
    virtual std::optional< ReceivedMsg > try_receive( boost::asio::yield_context& yield_ctx );
    virtual void                         send( const ReceivedMsg& msg );

private:
    MessageChannel m_channel;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class ConcurrentLogicalThread : public LogicalThread
{
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, ReceivedMsg ) >;

public:
    ConcurrentLogicalThread( LogicalThreadManager&         logicalthreadManager,
                             const LogicalThreadID&        logicalthreadID,
                             std::optional< ConnectionID > originatingConnectionID = std::nullopt );

protected:
    virtual ReceivedMsg                  receive( boost::asio::yield_context& yield_ctx );
    virtual std::optional< ReceivedMsg > try_receive( boost::asio::yield_context& yield_ctx );
    virtual void                         send( const ReceivedMsg& msg );

protected:
    MessageChannel m_channel;
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class ExternalLogicalThread : public LogicalThreadBase
{
    using MessageChannel = boost::asio::experimental::channel< void( boost::system::error_code, ReceivedMsg ) >;

public:
    using Ptr = std::shared_ptr< ExternalLogicalThread >;

    ExternalLogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID,
                           boost::asio::io_context& ioContext );

    // make this virtual so that windows can build...
    virtual ReceivedMsg receive();

    // Sender
    virtual ConnectionID getConnectionID() const
    {
        if( m_selfConnectionID.has_value() )
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

    void sendErrorResponse( const ReceivedMsg& msg, const std::string& strErrorMsg )
    {
        const ReceivedMsg rMsg{ getConnectionID(), make_error_msg( msg.msg.getReceiverID(), strErrorMsg ) };
        send( rMsg );
    }

    virtual void sendErrorResponse( const ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        sendErrorResponse( msg, strErrorMsg );
    }

    // LogicalThreadBase
    virtual const ID& getID() const { return m_logicalthreadID; }
    virtual void      send( const ReceivedMsg& msg );

    // NOT IMPLEMENTED - no stack or coroutine for external logicalthread
    virtual Message            dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) { THROW_TODO; }
    virtual void               run( boost::asio::yield_context& yield_ctx ) { THROW_TODO; }
    virtual const std::string& getProcessName() const { THROW_TODO; }
    virtual U64                getStackSize() const { THROW_TODO; }
    virtual void               onDisconnect( const ConnectionID& connectionID ) { THROW_TODO; }
    virtual void               requestStarted( const ConnectionID& connectionID ) { ; }
    virtual void               requestCompleted() { ; }

protected:
    LogicalThreadManager&                 m_logicalthreadManager;
    LogicalThreadID                       m_logicalthreadID;
    boost::asio::io_context&              m_ioContext;
    MessageChannel                        m_channel;
    mutable std::optional< ConnectionID > m_selfConnectionID;
};

} // namespace mega::network

#endif // LOGICALTHREAD_24_MAY_2022
