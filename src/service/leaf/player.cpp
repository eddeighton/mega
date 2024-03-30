
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

#include "service/player.hpp"

#include "service/leaf/request_player.hpp"

namespace mega::service
{

Player::Player( network::Log log, network::Sender::Ptr pSender, network::Node nodeType, short daemonPortNumber,
                ProcessClock& processClock )
    : Host( std::move( log ), pSender, nodeType, daemonPortNumber )
    , m_processClock( processClock )
{
}

// network::LogicalThreadManager
network::LogicalThreadBase::Ptr Player::joinLogicalThread( const network::Message& msg )
{
    switch( m_nodeType )
    {
        case network::Node::Leaf:
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Python:
        case network::Node::Report:
        case network::Node::Executor:
        case network::Node::Plugin:
            return network::LogicalThreadBase::Ptr( new PlayerRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
            break;
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::TOTAL_NODE_TYPES:
        default:
            THROW_RTE( "Leaf: Unknown leaf type" );
            break;
    }
    UNREACHABLE;
}

Player::RuntimeLock::RuntimeLock( Player& player, network::LogicalThread& logicalThread,
                                  boost::asio::yield_context& yield_context )
    : m_player( player )
    , m_logicalThread( logicalThread )
    , m_yield_context( yield_context )
{
}

Player::RuntimeLock::~RuntimeLock()
{
    m_player.releaseRuntimeLock( m_logicalThread, m_yield_context );
}

Player::RuntimeLock Player::acquireRuntimeLock( network::LogicalThread&     logicalThread,
                                                boost::asio::yield_context& yield_ctx )
{
    m_processClock.runtimeLock( &logicalThread );

    auto response = logicalThread.dispatchInBoundRequestsUntilResponse( yield_ctx );

    VERIFY_RTE_MSG( response.getID() == network::sim::MSG_SimLockRuntimeAcquire_Response::ID,
                    "Received incorrect response type of: " << response );
    SPDLOG_TRACE( "Player::acquireRuntimeLock got MSG_SimLockRuntimeAcquire_Response" );

    return RuntimeLock{ *this, logicalThread, yield_ctx };
}

void Player::releaseRuntimeLock( network::LogicalThread& logicalThread, boost::asio::yield_context& yield_ctx )
{
    m_processClock.runtimeUnLock( &logicalThread );

    auto response = logicalThread.dispatchInBoundRequestsUntilResponse( yield_ctx );

    VERIFY_RTE_MSG( response.getID() == network::sim::MSG_SimLockRuntimeRelease_Response::ID,
                    "Received incorrect response type of: " << response );

    SPDLOG_TRACE( "Player::releaseRuntimeLock got MSG_SimLockRuntimeRelease_Response" );
}

} // namespace mega::service
