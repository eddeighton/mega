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

#include "request.hpp"
#include "simulation.hpp"

#include "service/executor.hpp"

#include "service/protocol/common/header.hpp"

#include "service/network/log.hpp"

namespace mega::service
{

MPO ExecutorRequestConversation::SimCreate( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestConversation::SimCreate {}", getID() );
    return m_executor.createSimulation( *this, yield_ctx );
}

TimeStamp ExecutorRequestConversation::SimLockReadMPO( const MPO& requestingMPO, const MPO& targetMPO,
                                                       boost::asio::yield_context& yield_ctx )
{
    if ( Simulation::Ptr pSim = m_executor.getSimulation( targetMPO ) )
    {
        network::sim::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );
        return rq.SimLockReadMPO( requestingMPO, targetMPO );
    }
    else
    {
        THROW_RTE( "Failed to resolve simulation: " << requestingMPO << " : " << targetMPO );
    }
}

TimeStamp ExecutorRequestConversation::SimLockWriteMPO( const MPO& requestingMPO, const MPO& targetMPO,
                                                        boost::asio::yield_context& yield_ctx )
{
    if ( Simulation::Ptr pSim = m_executor.getSimulation( targetMPO ) )
    {
        network::sim::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );
        return rq.SimLockWriteMPO( requestingMPO, targetMPO );
    }
    else
    {
        THROW_RTE( "Failed to resolve simulation: " << requestingMPO << " : " << targetMPO );
    }
}

void ExecutorRequestConversation::SimLockRelease( const MPO&                  requestingMPO,
                                                  const MPO&                  targetMPO,
                                                  const network::Transaction& transaction,
                                                  boost::asio::yield_context& yield_ctx )
{
    if ( Simulation::Ptr pSim = m_executor.getSimulation( targetMPO ) )
    {
        network::sim::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );
        rq.SimLockRelease( requestingMPO, targetMPO, transaction );
    }
    else
    {
        THROW_RTE( "Failed to resolve simulation: " << requestingMPO << " : " << targetMPO );
    }
}

network::Message ExecutorRequestConversation::MPODown( const network::Message& request, const MPO& mpo,
                                                       boost::asio::yield_context& yield_ctx )
{
    if ( Simulation::Ptr pSim = m_executor.getSimulation( mpo ) )
    {
        switch ( request.getID() )
        {
            case network::sim::MSG_SimDestroy_Request::ID:
            {
                auto&                        msg = network::sim::MSG_SimDestroy_Request::get( request );
                network::sim::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );
                rq.SimDestroy();
                return network::sim::MSG_SimDestroy_Response::make(
                    request.getReceiverID(), request.getSenderID(), network::sim::MSG_SimDestroy_Response{} );
            }
            break;

            case network::status::MSG_GetStatus_Request::ID:
            {
                auto&                           msg = network::status::MSG_GetStatus_Request::get( request );
                network::status::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );
                return network::status::MSG_GetStatus_Response::make(
                    request.getReceiverID(), request.getSenderID(),
                    network::status::MSG_GetStatus_Response{ rq.GetStatus( msg.status ) } );
            }
            break;
            case network::status::MSG_Ping_Request::ID:
            {
                auto&                           msg = network::status::MSG_Ping_Request::get( request );
                network::status::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );
                return network::status::MSG_Ping_Response::make(
                    request.getReceiverID(), request.getSenderID(),
                    network::status::MSG_Ping_Response{ rq.Ping( msg.msg ) } );
            }
            break;
            default:
                THROW_RTE( "Unsupported MPO request type" );
        }
    }
    else
    {
        THROW_RTE( "Failed to resolve simulation: " << mpo );
    }
}

} // namespace mega::service
