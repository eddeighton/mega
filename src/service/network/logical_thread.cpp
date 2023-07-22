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
#include "service/network/log.hpp"

#include "service/network/end_point.hpp"
#include "service/protocol/model/messages.hxx"

#include "service/protocol/common/context.hpp"

#include "jit/jit_exception.hpp"

#include <chrono>
#include <iostream>

namespace mega::network
{

LogicalThread::LogicalThread( LogicalThreadManager& logicalthreadManager, const LogicalThreadID& logicalthreadID,
                            std::optional< ConnectionID > originatingConnectionID )
    : m_logicalthreadManager( logicalthreadManager )
    , m_logicalthreadID( logicalthreadID )
    , m_originatingEndPoint( originatingConnectionID )
{
}

LogicalThread::~LogicalThread() = default;

void LogicalThread::requestStarted( const ConnectionID& connectionID )
{
    //
    m_stack.push_back( connectionID );
}

void LogicalThread::requestCompleted()
{
    VERIFY_RTE( !m_stack.empty() );
    m_stack.pop_back();
    if( m_stack.empty() )
    {
        // SPDLOG_TRACE( "LogicalThread::requestCompleted: {}", getID() );
        m_logicalthreadManager.logicalthreadCompleted( shared_from_this() );
    }
}
const std::string& LogicalThread::getProcessName() const
{
    return m_logicalthreadManager.getProcessName();
}

void LogicalThread::onDisconnect( const ConnectionID& connectionID )
{
    for( const ConnectionID& existing : m_stack )
    {
        if( existing == connectionID )
        {
            m_disconnections.insert( connectionID );
            break;
        }
    }

    if( !m_stack.empty() && m_stack.back() == connectionID )
    {
        SPDLOG_ERROR( "Generating disconnect on logicalthread: {} for connection: {}", getID(), connectionID );
        const ReceivedMsg rMsg{ connectionID, make_error_msg( getID(), "Disconnection" ) };
        send( rMsg );
    }
}

ReceivedMsg LogicalThread::receiveDeferred( boost::asio::yield_context& yield_ctx )
{
    ReceivedMsg msg;
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

bool LogicalThread::queue( const ReceivedMsg& msg )
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

void LogicalThread::unqueue()
{
    if( m_bEnableQueueing )
    {
        m_bQueueing = false;
        VERIFY_RTE( m_unqueuedMessages.empty() );
        m_deferedMessages.swap( m_unqueuedMessages );
        std::reverse( m_unqueuedMessages.begin(), m_unqueuedMessages.end() );
    }
}

void LogicalThread::run( boost::asio::yield_context& yield_ctx )
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
        m_logicalthreadManager.logicalthreadCompleted( shared_from_this() );
    }
    catch( mega::runtime::JITException& ex )
    {
        SPDLOG_WARN( "LogicalThread: {} exception: {}", getID(), ex.what() );
        m_logicalthreadManager.logicalthreadCompleted( shared_from_this() );
    }
}

void LogicalThread::run_one( boost::asio::yield_context& yield_ctx )
{
    // SPDLOG_TRACE( "LogicalThread::run_one" );
    unqueue();
    const ReceivedMsg msg = receiveDeferred( yield_ctx );
    dispatchRequestImpl( msg, yield_ctx );
}

Message LogicalThread::dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
{
    ReceivedMsg msg;
    while( true )
    {
        msg = receiveDeferred( yield_ctx );

        if( isRequest( msg.msg ) )
        {
            dispatchRequestImpl( msg, yield_ctx );

            // check if connection has disconnected
            if( !m_disconnections.empty() )
            {
                ASSERT( !m_stack.empty() );
                if( m_disconnections.count( m_stack.back() ) )
                {
                    SPDLOG_ERROR(
                        "Generating disconnect on logicalthread: {} for connection: {}", getID(), m_stack.back() );
                    const ReceivedMsg rMsg{
                        m_stack.back(), make_error_msg( msg.msg.getReceiverID(), "Disconnection" ) };
                    send( rMsg );
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

    // SPDLOG_TRACE( "LogicalThread::dispatchRequestsUntilResponse: returned {}", msg.msg );
    return msg.msg;
}

void LogicalThread::dispatchRequestImpl( const ReceivedMsg& msg, boost::asio::yield_context& yield_ctx )
{
    // SPDLOG_TRACE( "LogicalThread::dispatchRequestImpl: {}", msg.msg );

    LogicalThreadBase::RequestStack stack( msg.msg.getName(), shared_from_this(), msg.connectionID );
    try
    {
        VERIFY_RTE_MSG( isRequest( msg.msg ), "Dispatch request got response: " << msg.msg );
        network::Message result = dispatchRequest( msg.msg, yield_ctx );
        if( !result )
        {
            SPDLOG_ERROR( "Failed to dispatch request: {} on logicalthread: {}", msg.msg, getID() );
            THROW_RTE( "Failed to dispatch request message: " << msg.msg );
        }
        else
        {
            // send response
            dispatchResponse( msg.connectionID, result, yield_ctx );
        }
    }
    catch( std::exception& ex )
    {
        error( msg, ex.what(), yield_ctx );
    }
    catch( mega::runtime::JITException& ex )
    {
        error( msg, ex.what(), yield_ctx );
    }
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
        std::optional< network::ReceivedMsg > pendingMsgOpt = try_receive( yield_ctx );
        if( pendingMsgOpt.has_value() )
        {
            bRemaining = true;
            if( isRequest( pendingMsgOpt.value().msg ) )
            {
                SPDLOG_TRACE( "LogicalThread::dispatchRemaining {} got request: {}", getID(),
                              pendingMsgOpt.value().msg.getName() );
                dispatchRequestImpl( pendingMsgOpt.value(), yield_ctx );
            }
            else
            {
                SPDLOG_TRACE( "LogicalThread::dispatchRemaining {} got response: {}", getID(),
                              pendingMsgOpt.value().msg.getName() );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
InThreadLogicalThread::InThreadLogicalThread( LogicalThreadManager&  logicalthreadManager,
                                            const LogicalThreadID& logicalthreadID,
                                            std::optional< ConnectionID >
                                                originatingConnectionID /*= std::nullopt*/ )
    : LogicalThread( logicalthreadManager, logicalthreadID, originatingConnectionID )
    , m_channel( logicalthreadManager.getIOContext() )
{
}

ReceivedMsg InThreadLogicalThread::receive( boost::asio::yield_context& yield_ctx )
{
    mega::_MPOContextStack _mpoStack;
    return m_channel.async_receive( yield_ctx );
}

std::optional< ReceivedMsg > InThreadLogicalThread::try_receive( boost::asio::yield_context& yield_ctx )
{
    std::optional< ReceivedMsg > result;

    m_channel.try_receive(
        [ &optMsg = result ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
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

void InThreadLogicalThread::send( const ReceivedMsg& msg )
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
                                                const LogicalThreadID& logicalthreadID,
                                                std::optional< ConnectionID >
                                                    originatingConnectionID /*= std::nullopt*/ )
    : LogicalThread( logicalthreadManager, logicalthreadID, originatingConnectionID )
    , m_channel( logicalthreadManager.getIOContext() )
{
}

ReceivedMsg ConcurrentLogicalThread::receive( boost::asio::yield_context& yield_ctx )
{
    mega::_MPOContextStack _mpoStack;
    return m_channel.async_receive( yield_ctx );
}

std::optional< ReceivedMsg > ConcurrentLogicalThread::try_receive( boost::asio::yield_context& yield_ctx )
{
    std::optional< ReceivedMsg > result;

    m_channel.try_receive(
        [ &optMsg = result ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
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

void ConcurrentLogicalThread::send( const ReceivedMsg& msg )
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

ExternalLogicalThread::ExternalLogicalThread( LogicalThreadManager&  logicalthreadManager,
                                            const LogicalThreadID& logicalthreadID, boost::asio::io_context& ioContext )
    : m_logicalthreadManager( logicalthreadManager )
    , m_logicalthreadID( logicalthreadID )
    , m_ioContext( ioContext )
    , m_channel( m_ioContext )
{
}

ReceivedMsg ExternalLogicalThread::receive()
{
    ReceivedMsg result;
    bool        bReceived = false;
    m_channel.async_receive(
        [ &bReceived, &result ]( boost::system::error_code ec, const ReceivedMsg& msg )
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

void ExternalLogicalThread::send( const ReceivedMsg& msg )
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
