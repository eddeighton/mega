
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

#ifndef GUARD_2023_September_12_state_machine
#define GUARD_2023_September_12_state_machine

#include "service/executor/message_traits.hpp"
#include "service/executor/sim_move_machine.hpp"
#include "service/executor/transaction_machine.hpp"

#include "service/protocol/model/sim.hxx"

#include "service/protocol/common/logical_thread_id.hpp"
#include "service/protocol/common/status.hpp"

#include "common/unreachable.hpp"

namespace mega::service
{

struct StateMachineMsgTraits : public MsgTraits
{
    using Read    = TransactionMsgTraits::Read;
    using Write   = TransactionMsgTraits::Write;
    using Release = TransactionMsgTraits::Release;

    using MoveComplete = MoveMsgTraits::MoveComplete;
    using MoveRequest  = MoveMsgTraits::MoveRequest;
    using MoveResponse = MoveMsgTraits::MoveResponse;

    using Destroy = network::sim::MSG_SimDestroy_Request;
    using Block   = network::sim::MSG_SimDestroyBlocking_Request;
    using Clock   = network::sim::MSG_SimClock_Response;

    static inline bool isBlock( const Msg& msg ) { return getMsgID( msg ) == Block::ID; }
    static inline bool isDestroy( const Msg& msg ) { return getMsgID( msg ) == Destroy::ID; }
    static inline bool isClock( const Msg& msg ) { return getMsgID( msg ) == Clock::ID; }

    static inline bool isMove( const Msg& msg ) { return MoveMsgTraits::isMsg( msg ); }
    static inline bool isTransaction( const Msg& msg ) { return TransactionMsgTraits::isMsg( msg ); }

    static inline bool isMsg( const Msg& msg )
    {
        switch( getMsgID( msg ) )
        {
            case Read::ID:
            case Write::ID:
            case Release::ID:
            case Block::ID:
            case Destroy::ID:
            case Clock::ID:
            case MoveComplete::ID:
            case MoveRequest::ID:
            case MoveResponse::ID:
                return true;
            default:
                return false;
        }
    }
};

/*
struct SimulationConcept
{
    bool unrequestClock();
};
*/

template < typename Simulation >
class StateMachine : public StateMachineMsgTraits
{
    using TM = TransactionMachine< Simulation >;
    using MM = SimMoveMachine< Simulation >;

    Simulation&          m_sim;
    TM&                  m_transactionMachine;
    MM&                  m_moveMachine;
    AckVector&           m_acks;
    std::optional< Msg > m_clockResponse;
    std::optional< Msg > m_destroy;

    enum State
    {
        SLEEP,
        TRANSACTION,
        SIM,
        MOVE,
        TERMINATED
    };
    State m_state = SLEEP;

public:
    inline StateMachine( Simulation& sim, TM& transactionMachine, MM& moveMachine, AckVector& ackVector )
        : m_sim( sim )
        , m_transactionMachine( transactionMachine )
        , m_moveMachine( moveMachine )
        , m_acks( ackVector )
    {
    }

    inline bool isTerminated() const { return m_state == TERMINATED; }

    inline void status( network::Status& status ) 
    {
        m_transactionMachine.status( status );
    }

    enum MsgResult
    {
        eSendClockRequest,
        eSendMoveComplete,
        eRunCycle,
        eUnregister,
        eRecognised,
        eIgnored
    };

    inline MsgResult onNext()
    {
        if( m_destroy.has_value() )
        {
            // ensure all pending transactions are acked
            m_transactionMachine.onTerminate();
            m_state = TERMINATED;
            m_acks.push_back( m_destroy.value() );
            return eUnregister;
        }
        else if( m_transactionMachine.onNext() )
        {
            m_state = TRANSACTION;
            return eRecognised;
        }
        else if( m_clockResponse.has_value() )
        {
            m_clockResponse.reset();
            m_state = SIM;
            return eRunCycle;
        }
        else
        {
            m_state = SLEEP;
            return eSendClockRequest;
        }
    }

    // return true is should send MoveCompleteRequest
    inline bool onCycle()
    {
        ASSERT( m_state == SIM );
        m_state = MOVE;
        return m_moveMachine.sendMoveRequests();
    }

    inline MsgResult onMessage( const Msg& msg )
    {
        if( !isMsg( msg ) )
        {
            return eIgnored;
        }

        switch( m_state )
        {
            case SLEEP:
            {
                if( isTransaction( msg ) )
                {
                    // can transaction processing start - must unrequest clock
                    if( m_transactionMachine.onSleep( msg ) )
                    {
                        m_state = TRANSACTION;
                    }
                    return eRecognised;
                }
                else if( isMove( msg ) )
                {
                    m_moveMachine.queue( msg );
                    return eRecognised;
                }
                else if( isClock( msg ) )
                {
                    m_state = SIM;
                    return eRunCycle;
                }
                else if( isBlock( msg ) || isDestroy( msg ) )
                {
                    m_destroy = msg;
                    return eRecognised;
                }
                else
                {
                    UNREACHABLE;
                }
            }
            break;
            case TRANSACTION:
            {
                if( isTransaction( msg ) )
                {
                    if( m_transactionMachine.onMessage( msg ) )
                    {
                        return onNext();
                    }
                    else
                    {
                        return eRecognised;
                    }
                }
                else if( isMove( msg ) )
                {
                    m_moveMachine.queue( msg );
                    return eRecognised;
                }
                else if( isClock( msg ) )
                {
                    ASSERT( !m_clockResponse.has_value() );
                    m_clockResponse = msg;
                    return eRecognised;
                }
                else if( isBlock( msg ) || isDestroy( msg ) )
                {
                    m_destroy = msg;
                    return eRecognised;
                }
                else
                {
                    UNREACHABLE;
                }
            }
            break;
            case SIM:
            {
                UNREACHABLE;
            }
            break;
            case MOVE:
            {
                if( isTransaction( msg ) )
                {
                    m_transactionMachine.queue( msg );
                    return eRecognised;
                }
                else if( isMove( msg ) )
                {
                    switch( m_moveMachine.onMessage( msg ) )
                    {
                        case MM::eNothing:
                        {
                            return eRecognised;
                        }
                        break;
                        case MM::eMoveRequestsComplete:
                        {
                            return eSendMoveComplete;
                        }
                        break;
                        case MM::eProcessMoveComplete:
                        {
                            return onNext();
                        }
                        break;
                    }
                }
                else if( isClock( msg ) )
                {
                    ASSERT( !m_clockResponse.has_value() );
                    m_clockResponse = msg;
                    return eRecognised;
                }
                else if( isBlock( msg ) || isDestroy( msg ) )
                {
                    m_destroy = msg;
                    return eRecognised;
                }
                else
                {
                    UNREACHABLE;
                }
            }
            break;
            case TERMINATED:
            {
                UNREACHABLE;
            }
            break;
        }
        return eRecognised;
    }
};

} // namespace mega::service

#endif // GUARD_2023_September_12_state_machine
