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

#include "service/executor/request.hpp"

#include "simulation.hpp"

#include "service/executor/executor.hpp"

#include "service/protocol/common/conversation_id.hpp"

#include "service/network/log.hpp"

#include "common/requireSemicolon.hpp"

namespace mega::service
{

MPO ExecutorRequestConversation::SimCreate( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestConversation::SimCreate {}", getID() );
    return m_executor.createSimulation( *this, yield_ctx );
}

#define FORWARDED_MSG_ERROR( code ) \
DO_STUFF_AND_REQUIRE_SEMI_COLON( code )

// TODO: check ording of receiverID senderID is correct way round! - they are being swapped in response
#define FORWARD_SIM_MSG_NO_ARG_NO_RETURN( namespace_, name )                                                 \
    case network::namespace_::MSG_##name##_Request::ID:                                                      \
    {                   \
            auto&                               msg = network::namespace_::MSG_##name##_Request::get( request ); \
            network::namespace_::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );                    \
            rq.name();                                                                                          \
            return network::namespace_::MSG_##name##_Response::make(                                             \
                request.getReceiverID(), request.getSenderID(), network::namespace_::MSG_##name##_Response{} );  \
    } \
    break

#define FORWARD_SIM_MSG_NO_ARG_ONE_RETURN( namespace_, name )                                                          \
    case network::namespace_::MSG_##name##_Request::ID:                                                                \
    {                                                                                                                  \
            auto&                               msg = network::namespace_::MSG_##name##_Request::get( request );           \
            network::namespace_::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );                              \
            return network::namespace_::MSG_##name##_Response::make(                                                       \
                request.getReceiverID(), request.getSenderID(), network::namespace_::MSG_##name##_Response{ rq.name() } ); \
    }                                                                                                        \
    break

#define FORWARD_SIM_MSG_ONE_ARG_ONE_RETURN( namespace_, name, arg1 )                                         \
    case network::namespace_::MSG_##name##_Request::ID:                                                      \
    {                                                                                                        \
            auto&                               msg = network::namespace_::MSG_##name##_Request::get( request ); \
            network::namespace_::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );                    \
            return network::namespace_::MSG_##name##_Response::make(                                             \
                request.getReceiverID(), request.getSenderID(),                                                  \
                network::namespace_::MSG_##name##_Response{ rq.name( msg.arg1 ) } );                             \
    }                                                                                                        \
    break

#define FORWARD_SIM_MSG_TWO_ARG_ONE_RETURN( namespace_, name, arg1, arg2 )                                   \
    case network::namespace_::MSG_##name##_Request::ID:                                                      \
    {                                                                                                        \
            auto&                               msg = network::namespace_::MSG_##name##_Request::get( request ); \
            network::namespace_::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );                    \
            return network::namespace_::MSG_##name##_Response::make(                                             \
                request.getReceiverID(), request.getSenderID(),                                                  \
                network::namespace_::MSG_##name##_Response{ rq.name( msg.arg1, msg.arg2 ) } );                   \
    }                                                                                                        \
    break

#define FORWARD_SIM_MSG_THREE_ARG_NO_RETURN( namespace_, name, arg1, arg2, arg3 )                            \
    case network::namespace_::MSG_##name##_Request::ID:                                                      \
    {                                                                                                        \
            auto&                               msg = network::namespace_::MSG_##name##_Request::get( request ); \
            network::namespace_::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );                    \
            rq.name( msg.arg1, msg.arg2, msg.arg3 );                                                             \
            return network::namespace_::MSG_##name##_Response::make(                                             \
                request.getReceiverID(), request.getSenderID(), network::namespace_::MSG_##name##_Response{} );  \
    }                                                                                                        \
    break

network::Message ExecutorRequestConversation::MPODown( const network::Message& request, const MPO& mpo,
                                                       boost::asio::yield_context& yield_ctx )
{
    using ::operator<<;
    if( Simulation::Ptr pSim = m_executor.getSimulation( mpo ) )
    {
        switch( request.getID() )
        {
            FORWARD_SIM_MSG_NO_ARG_NO_RETURN( sim, SimErrorCheck );
            FORWARD_SIM_MSG_ONE_ARG_ONE_RETURN( status, GetStatus, status );
            FORWARD_SIM_MSG_ONE_ARG_ONE_RETURN( status, Ping, msg );
            FORWARD_SIM_MSG_ONE_ARG_ONE_RETURN( sim, SimObjectSnapshot, object );
            FORWARD_SIM_MSG_ONE_ARG_ONE_RETURN( sim, SimAllocate, objectTypeID );
            FORWARD_SIM_MSG_TWO_ARG_ONE_RETURN( sim, SimLockRead, requestingMPO, targetMPO );
            FORWARD_SIM_MSG_TWO_ARG_ONE_RETURN( sim, SimLockWrite, requestingMPO, targetMPO );
            FORWARD_SIM_MSG_THREE_ARG_NO_RETURN( sim, SimLockRelease, requestingMPO, targetMPO, transaction );
            FORWARD_SIM_MSG_NO_ARG_NO_RETURN( sim, SimDestroy );

            default:
                THROW_RTE( "Unsupported MPO request type for mpo: " << mpo << " request: " << request.getName() );
        }
    }
    else
    {
        THROW_RTE( "Failed to resolve simulation: " << mpo );
    }
}

} // namespace mega::service
