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

#include "service/network/logical_thread.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "log/log.hpp"

#include "service/network/end_point.hpp"
#include "service/protocol/model/messages.hxx"

#include "runtime/exception.hpp"
#include "runtime/context.hpp"

#include <chrono>
#include <iostream>

namespace mega::network
{

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
LogicalThread::LogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID )
    : LogicalThreadBase( logicalthreadManager, logicalthreadID )
{
}

ReceivedMessage LogicalThread::receiveDeferred( boost::asio::yield_context& yield_ctx )
{
    return receive( yield_ctx );
}

Message LogicalThread::dispatchInBoundRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
{
    ReceivedMessage msg;
    while( true )
    {
        msg = receiveDeferred( yield_ctx );

        if( isRequest( msg.msg ) )
        {
            acknowledgeInboundRequest( msg, yield_ctx );
        }
        else if( msg.msg.getID() == MSG_Error_Disconnect::ID )
        {
            throw std::runtime_error( MSG_Error_Disconnect::get( msg.msg ).what );
        }
        else
        {
            break;
        }
    }
    if( msg.msg.getID() == MSG_Error_Response::ID )
    {
        throw std::runtime_error( MSG_Error_Response::get( msg.msg ).what );
    }
    return msg.msg;
}

void LogicalThread::dispatchRemaining( boost::asio::yield_context& yield_ctx )
{
    bool bRemaining = true;
    while( bRemaining )
    {
        bRemaining = false;

        /* while( !m_deferedMessages.empty() || !m_unqueuedMessages.empty() )
         {
             run_one( yield_ctx );
             bRemaining = true;
         }*/

        // close out existing messages
        std::optional< network::ReceivedMessage > pendingMsgOpt = try_receive( yield_ctx );
        if( pendingMsgOpt.has_value() )
        {
            bRemaining = true;
            if( isRequest( pendingMsgOpt.value().msg ) )
            {
                SPDLOG_TRACE( "LogicalThread::dispatchRemaining {} got request: {}", getID(),
                              pendingMsgOpt.value().msg.getName() );
                acknowledgeInboundRequest( pendingMsgOpt.value(), yield_ctx );
            }
            else
            {
                SPDLOG_TRACE( "LogicalThread::dispatchRemaining {} got response: {}", getID(),
                              pendingMsgOpt.value().msg.getName() );
            }
        }
    }
}

void LogicalThread::acknowledgeInboundRequest( const ReceivedMessage& msg, boost::asio::yield_context& yield_ctx )
{
    // handling in-coming request
    LogicalThreadBase::InBoundRequestStack stack( shared_from_this(), msg.pResponseSender );
    try
    {
        VERIFY_RTE_MSG( isRequest( msg.msg ), "Dispatch request got response: " << msg.msg );
        network::Message response = dispatchInBoundRequest( msg.msg, yield_ctx );
        if( !response )
        {
            SPDLOG_ERROR( "Failed to dispatch request: {} on logicalthread: {}", msg.msg, getID() );
            THROW_RTE( "Failed to dispatch request message: " << msg.msg );
        }
        else
        {
            ASSERT( msg.pResponseSender );
            msg.pResponseSender->send( response, yield_ctx );
        }
    }
    catch( std::exception& ex )
    {
        msg.pResponseSender->send( make_response_error_msg( getID(), ex.what() ), yield_ctx );
    }
    catch( mega::runtime::RuntimeException& ex )
    {
        msg.pResponseSender->send( make_response_error_msg( getID(), ex.what() ), yield_ctx );
    }
}


void LogicalThread::run( boost::asio::yield_context& yield_ctx )
{
    try
    {
        run_one( yield_ctx );
    }
    catch( std::exception& ex )
    {
        SPDLOG_WARN( "LogicalThread: {} exception: {}", getID(), ex.what() );
        getThreadManager().logicalthreadCompleted( shared_from_this() );
    }
    catch( mega::runtime::RuntimeException& ex )
    {
        SPDLOG_WARN( "LogicalThread: {} exception: {}", getID(), ex.what() );
        getThreadManager().logicalthreadCompleted( shared_from_this() );
    }
}

void LogicalThread::run_one( boost::asio::yield_context& yield_ctx )
{
    const ReceivedMessage msg = receiveDeferred( yield_ctx );
    acknowledgeInboundRequest( msg, yield_ctx );
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
StackfulLogicalThread::StackfulLogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID )
:   LogicalThread( logicalthreadManager, logicalthreadID )
{

}

void StackfulLogicalThread::requestStarted( Sender::Ptr pRequestResponseSender )
{
    //
    m_stack.push_back( pRequestResponseSender );
}

void StackfulLogicalThread::requestCompleted()
{
    VERIFY_RTE( !m_stack.empty() );
    m_stack.pop_back();
    if( m_stack.empty() )
    {
        getThreadManager().logicalthreadCompleted( shared_from_this() );
    }
}

ReceivedMessage StackfulLogicalThread::receiveDeferred( boost::asio::yield_context& yield_ctx )
{
    ReceivedMessage msg;
    while( true )
    {
        if( m_bEnableQueueing && !m_unqueuedMessages.empty() )
        {
            msg = m_unqueuedMessages.back();
            m_unqueuedMessages.pop_back();
        }
        else
        {
            msg = receive( yield_ctx );
        }
        if( m_bEnableQueueing )
        {
            if( !queue( msg ) )
            {
                return msg;
            }
        }
        else
        {
            return msg;
        }
    }
}

Message StackfulLogicalThread::dispatchInBoundRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
{
    ReceivedMessage msg;
    while( true )
    {
        msg = receiveDeferred( yield_ctx );

        if( isRequest( msg.msg ) )
        {
            acknowledgeInboundRequest( msg, yield_ctx );
        }
        else if( msg.msg.getID() == MSG_Error_Disconnect::ID )
        {
            // ignor the disconnect unless for active sender
            ASSERT( msg.pResponseSender );

            for( auto pActiveSender : m_stack )
            {
                if( pActiveSender == msg.pResponseSender )
                {
                    throw std::runtime_error( MSG_Error_Disconnect::get( msg.msg ).what );
                }
            }
        }
        else
        {
            break;
        }
    }
    if( msg.msg.getID() == MSG_Error_Response::ID )
    {
        throw std::runtime_error( MSG_Error_Response::get( msg.msg ).what );
    }
    return msg.msg;
}

bool StackfulLogicalThread::queue( const ReceivedMessage& msg )
{
    if( m_bQueueing )
    {
        m_deferedMessages.push_back( msg );
        return true;
    }
    else
    {
        return false;
    }
}

void StackfulLogicalThread::unqueue()
{
    if( m_bEnableQueueing )
    {
        m_bQueueing = false;
        VERIFY_RTE( m_unqueuedMessages.empty() );
        m_deferedMessages.swap( m_unqueuedMessages );
        std::reverse( m_unqueuedMessages.begin(), m_unqueuedMessages.end() );
    }
}


void StackfulLogicalThread::run( boost::asio::yield_context& yield_ctx )
{
    try
    {
        do
        {
            run_one( yield_ctx );
        } while( !m_stack.empty() );
    }
    catch( std::exception& ex )
    {
        SPDLOG_WARN( "LogicalThread: {} exception: {}", getID(), ex.what() );
        getThreadManager().logicalthreadCompleted( shared_from_this() );
    }
    catch( mega::runtime::RuntimeException& ex )
    {
        SPDLOG_WARN( "LogicalThread: {} exception: {}", getID(), ex.what() );
        getThreadManager().logicalthreadCompleted( shared_from_this() );
    }
}

void StackfulLogicalThread::run_one( boost::asio::yield_context& yield_ctx )
{
    unqueue();
    const ReceivedMessage msg = receiveDeferred( yield_ctx );
    acknowledgeInboundRequest( msg, yield_ctx );
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
InThreadLogicalThread::InThreadLogicalThread( LogicalThreadManager&  logicalthreadManager,
                                              const LogicalThreadID& logicalthreadID )
    : StackfulLogicalThread( logicalthreadManager, logicalthreadID )
    , m_channel( logicalthreadManager.getIOContext() )
{
}

ReceivedMessage InThreadLogicalThread::receive( boost::asio::yield_context& yield_ctx )
{
    mega::runtime::_MPOContextStack _mpoStack;
    return m_channel.async_receive( yield_ctx );
}

std::optional< ReceivedMessage > InThreadLogicalThread::try_receive( boost::asio::yield_context& yield_ctx )
{
    std::optional< ReceivedMessage > result;

    m_channel.try_receive(
        [ &optMsg = result ]( boost::system::error_code ec, const network::ReceivedMessage& msg )
        {
            if( !ec )
            {
                optMsg = msg;
            }
            else
            {
                THROW_TODO;
            }
        } );

    return result;
}

void InThreadLogicalThread::receive( const ReceivedMessage& msg )
{
    m_channel.async_send(
        boost::system::error_code(), msg,
        [ &msg ]( boost::system::error_code ec )
        {
            if( ec )
            {
                if( ec.value() == boost::asio::error::eof )
                {
                }
                else if( ec.value() == boost::asio::error::operation_aborted )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_closed )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_cancelled )
                {
                }
                else if( ec.failed() )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
            }
        } );
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
ConcurrentLogicalThread::ConcurrentLogicalThread( LogicalThreadManager&  logicalthreadManager,
                                                  const LogicalThreadID& logicalthreadID )
    : StackfulLogicalThread( logicalthreadManager, logicalthreadID )
    , m_channel( logicalthreadManager.getIOContext() )
{
}

ReceivedMessage ConcurrentLogicalThread::receive( boost::asio::yield_context& yield_ctx )
{
    mega::runtime::_MPOContextStack _mpoStack;
    return m_channel.async_receive( yield_ctx );
}

std::optional< ReceivedMessage > ConcurrentLogicalThread::try_receive( boost::asio::yield_context& yield_ctx )
{
    std::optional< ReceivedMessage > result;

    m_channel.try_receive(
        [ &optMsg = result ]( boost::system::error_code ec, const network::ReceivedMessage& msg )
        {
            if( !ec )
            {
                optMsg = msg;
            }
            else
            {
                THROW_TODO;
            }
        } );

    return result;
}

void ConcurrentLogicalThread::receive( const ReceivedMessage& msg )
{
    m_channel.async_send(
        boost::system::error_code(), msg,
        [ &msg ]( boost::system::error_code ec )
        {
            if( ec )
            {
                if( ec.value() == boost::asio::error::eof )
                {
                }
                else if( ec.value() == boost::asio::error::operation_aborted )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_closed )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_cancelled )
                {
                }
                else if( ec.failed() )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
            }
        } );
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

ExternalLogicalThread::ExternalLogicalThread( LogicalThreadManager&    logicalThreadManager,
                                              const LogicalThreadID&   logicalThreadID,
                                              boost::asio::io_context& ioContext )
    : LogicalThreadBase( logicalThreadManager, logicalThreadID )
    , m_ioContext( ioContext )
    , m_channel( m_ioContext )
{
}

ReceivedMessage ExternalLogicalThread::receive()
{
    ReceivedMessage result;
    bool            bReceived = false;
    m_channel.async_receive(
        [ &bReceived, &result ]( boost::system::error_code ec, const ReceivedMessage& msg )
        {
            if( ec )
            {
                SPDLOG_ERROR( "Failed to receive msg with error: {}", ec.what() );
                THROW_RTE( "Failed to receive msg on channel: " << ec.what() );
            }
            else
            {
                result    = msg;
                bReceived = true;
            }
        } );
    while( !bReceived )
    {
        m_ioContext.run_one();
    }
    return result;
}

void ExternalLogicalThread::receive( const ReceivedMessage& msg )
{
    m_channel.async_send(
        boost::system::error_code(), msg,
        [ &msg ]( boost::system::error_code ec )
        {
            if( ec )
            {
                if( ec.value() == boost::asio::error::eof )
                {
                }
                else if( ec.value() == boost::asio::error::operation_aborted )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_closed )
                {
                }
                else if( ec.value() == boost::asio::experimental::error::channel_cancelled )
                {
                }
                else if( ec.failed() )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
            }
        } );
}
} // namespace mega::network
