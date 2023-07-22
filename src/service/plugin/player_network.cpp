
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

#include "service/plugin/player_network.hpp"

#include "service/executor/executor.hpp"

#include "service/protocol/model/sim.hxx"

#include "service/network/log.hpp"

namespace mega::service
{

PlayerNetwork::PlayerNetwork( Executor& executor, const network::LogicalThreadID& logicalthreadID,
                              network::LogicalThreadBase& plugin )
    : ExecutorRequestLogicalThread( executor, logicalthreadID, std::nullopt )
    , m_plugin( plugin )
{
}

network::Message PlayerNetwork::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::player_network::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return ExecutorRequestLogicalThread::dispatchRequest( msg, yield_ctx );
}

void PlayerNetwork::dispatchResponse( const network::ConnectionID& connectionID,
                                      const network::Message&      msg,
                                      boost::asio::yield_context&  yield_ctx )

{
    if( msg.getReceiverID() == m_plugin.getID() )
    {
        m_plugin.send( network::ReceivedMsg{ connectionID, msg } );
    }
    else
    {
        ExecutorRequestLogicalThread::dispatchResponse( connectionID, msg, yield_ctx );
    }
}

void PlayerNetwork::error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                           boost::asio::yield_context& yield_ctx )
{
    if( msg.msg.getSenderID() == m_plugin.getID() )
    {
        m_plugin.sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        ExecutorRequestLogicalThread::error( msg, strErrorMsg, yield_ctx );
    }
}

void PlayerNetwork::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "PlayerNetwork::run" );

    m_pYieldContext = &yield_ctx;

    network::player_network::Request_Sender rq( *this, m_plugin.getID(), m_plugin, yield_ctx );

    try
    {
        while( m_bRunning )
        {
            m_bRunning = rq.PlayerNetworkStatus( m_state );
        }
    }
    catch( std::exception& ex )
    {
        SPDLOG_ERROR( ex.what() );
    }

    dispatchRemaining( yield_ctx );
    
    SPDLOG_TRACE( "PlayerNetwork::run complete" );
}

bool PlayerNetwork::PlayerNetworkConnect( const mega::I64& networkID, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "PlayerNetwork::PlayerNetworkConnect" );
    m_state.m_currentNetwork = "Single Player";
    return true;
}

void PlayerNetwork::PlayerNetworkDisconnect( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "PlayerNetwork::PlayerNetworkDisconnect" );
    m_state.m_currentNetwork.clear();
}

void PlayerNetwork::PlayerNetworkCreatePlanet( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "PlayerNetwork::PlayerNetworkCreatePlanet" );
    m_state.m_currentPlanet = m_executor.createSimulation( *this, yield_ctx );
    m_pPlanet               = m_executor.getSimulation( m_state.m_currentPlanet.value() );
}

void PlayerNetwork::PlayerNetworkDestroyPlanet( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "PlayerNetwork::PlayerNetworkDestroyPlanet" );
    network::sim::Request_Sender rq( *this, *m_pPlanet, yield_ctx );
    rq.SimDestroy();
    m_pPlanet.reset();
    m_state.m_currentPlanet.reset();
}

} // namespace mega::service
