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

#include "service/network/logical_thread_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/logical_thread_id.hpp"
#include "service/protocol/model/messages.hxx"

#include <boost/asio/this_coro.hpp>

#include <iostream>

namespace mega::network
{

LogicalThreadManager::LogicalThreadManager( const std::string& strProcessName, boost::asio::io_context& ioContext )
    : m_strProcessName( strProcessName )
    , m_ioContext( ioContext )
{
}

LogicalThreadManager::~LogicalThreadManager() = default;

boost::asio::io_context& LogicalThreadManager::getIOContext() const
{
    return m_ioContext;
}

void LogicalThreadManager::onDisconnect( network::Sender::Ptr pConnectionSender )
{
    // send error message to ALL active logical threads with the connection sender
    WriteLock lock( m_mutex );
    for( const auto& [ id, pLogicalThread ] : m_logicalthreads )
    {
        ReceivedMessage receivedMessage{ pConnectionSender, network::make_disconnect_error_msg( id, "Disconnection" ) };
        pLogicalThread->receive( receivedMessage );
    }
}

std::vector< LogicalThreadID > LogicalThreadManager::reportLogicalThreads() const
{
    std::vector< LogicalThreadID > activities;
    {
        ReadLock lock( m_mutex );
        for( const auto& [ id, pLogicalThread ] : m_logicalthreads )
        {
            activities.push_back( id );
        }
    }
    return activities;
}

LogicalThreadID LogicalThreadManager::createLogicalThreadID() const
{
    return {};
}

void LogicalThreadManager::spawnInitiatedLogicalThread( LogicalThreadBase::Ptr pLogicalThread )
{
    // clang-format off
    boost::asio::spawn
    (
        m_ioContext, 
        [ pLogicalThread ](boost::asio::yield_context yield_ctx)
        {
            LogicalThreadBase::RequestStack stack("spawnInitiatedLogicalThread", pLogicalThread, Sender::Ptr{} );
            pLogicalThread->run(yield_ctx);
        }
        // segmented stacks do NOT work on windows
#ifndef BOOST_USE_SEGMENTED_STACKS
        , boost::coroutines::attributes(NON_SEGMENTED_STACK_SIZE)
#endif
    );
    // clang-format on
    // SPDLOG_TRACE( "LogicalThreadBase Started id: {}", pLogicalThread->getID() );
}

void LogicalThreadManager::externalLogicalThreadInitiated( ExternalLogicalThread::Ptr pLogicalThread )
{
    WriteLock lock( m_mutex );
    VERIFY_RTE_MSG( !m_pExternalLogicalThread, "Existing external logicalthread" );
    m_logicalthreads.insert( std::make_pair( pLogicalThread->getID(), pLogicalThread ) );
    m_pExternalLogicalThread = pLogicalThread;
}

void LogicalThreadManager::logicalthreadInitiated( LogicalThreadBase::Ptr pLogicalThread )
{
    SPDLOG_TRACE( "LogicalThreadManager::logicalthreadInitiated: {} {}", m_strProcessName, pLogicalThread->getID() );
    WriteLock lock( m_mutex );
    m_logicalthreads.insert( std::make_pair( pLogicalThread->getID(), pLogicalThread ) );
    spawnInitiatedLogicalThread( pLogicalThread );
}

void LogicalThreadManager::logicalthreadJoined( LogicalThreadBase::Ptr pLogicalThread )
{
    // SPDLOG_TRACE( "LogicalThreadManager::logicalthreadJoined: {} {}", m_strProcessName, pLogicalThread->getID() );
    {
        WriteLock lock( m_mutex );
        m_logicalthreads.insert( std::make_pair( pLogicalThread->getID(), pLogicalThread ) );
    }
    // clang-format off
    boost::asio::spawn
    (
        m_ioContext, 
        [ pLogicalThread ]( boost::asio::yield_context yield_ctx ) 
        { 
            pLogicalThread->run( yield_ctx ); 
        } 
        // segmented stacks do NOT work on windows
#ifndef BOOST_USE_SEGMENTED_STACKS
        , boost::coroutines::attributes(NON_SEGMENTED_STACK_SIZE)
#endif
    );
    // clang-format on
    // SPDLOG_TRACE( "LogicalThreadBase Started id: {}", pLogicalThread->getID() );
}

void LogicalThreadManager::logicalthreadCompleted( LogicalThreadBase::Ptr pLogicalThread )
{
    WriteLock lock( m_mutex );
    auto      iFind = m_logicalthreads.find( pLogicalThread->getID() );
    if( iFind != m_logicalthreads.end() )
    {
        m_logicalthreads.erase( iFind );
    }
    if( m_pExternalLogicalThread == pLogicalThread )
    {
        m_pExternalLogicalThread.reset();
    }
}

LogicalThreadBase::Ptr LogicalThreadManager::findExistingLogicalThread( const LogicalThreadID& logicalthreadID ) const
{
    ReadLock lock( m_mutex );
    auto     iFind = m_logicalthreads.find( logicalthreadID );
    if( iFind != m_logicalthreads.end() )
    {
        return iFind->second;
    }
    else
    {
        return {};
    }
}

ExternalLogicalThread::Ptr LogicalThreadManager::getExternalLogicalThread() const
{
    ReadLock lock( m_mutex );
    return m_pExternalLogicalThread;
}

void LogicalThreadManager::dispatch( const ReceivedMessage& msg )
{
    LogicalThreadBase::Ptr pLogicalThread = findExistingLogicalThread( msg.msg.getLogicalThreadID() );
    if( !pLogicalThread )
    {
        pLogicalThread = joinLogicalThread( msg.msg );
        logicalthreadJoined( pLogicalThread );
    }
    pLogicalThread->receive( msg );
}

} // namespace mega::network
