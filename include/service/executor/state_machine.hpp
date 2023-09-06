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

#ifndef STATE_MACHINE_16_SEPT_2022
#define STATE_MACHINE_16_SEPT_2022

#include "service/executor/sim_move_manager.hpp"

#include "service/network/log.hpp"

#include "service/protocol/common/logical_thread_id.hpp"
#include "service/protocol/common/received_message.hpp"
#include "service/protocol/model/sim.hxx"

#include <set>
#include <vector>
#include <optional>
#include <utility>

namespace mega::service
{
class StateMachine
{
    using ID    = mega::MPO;
    using IDSet = std::set< ID >;

public:
    enum State
    {
        SIM,
        MOVE,
        READ,
        WRITE,
        TERM,
        WAIT
    };
    using Msg       = network::ReceivedMessage;
    using MsgVector = std::vector< Msg >;
    using AckVector = MsgVector;

    enum MsgResult
    {
        eNothing,
        eClockTick,
        eMoveComplete
    };

private:
    SimMoveManager&     m_moveManager;
    AckVector           m_acks;
    State               m_state = SIM;
    MsgVector           m_msgQueue;
    IDSet               m_activeReads;
    std::optional< ID > m_activeWrite;

public:
    inline StateMachine( SimMoveManager& moveManager )
        : m_moveManager( moveManager )
    {
    }

    inline State                     getState() const { return m_state; }
    inline bool                      isTerminating() const { return m_state == TERM; }
    inline bool                      isTerminated() const
    {
        return ( m_state == TERM ) && m_activeReads.empty() && ( !m_activeWrite.has_value() ) && m_acks.empty();
    }
    inline const AckVector&    acks() const { return m_acks; }
    inline const IDSet&        reads() const { return m_activeReads; }
    inline std::optional< ID > writer() const { return m_activeWrite; }

    using Read         = network::sim::MSG_SimLockRead_Request;
    using Write        = network::sim::MSG_SimLockWrite_Request;
    using Release      = network::sim::MSG_SimLockRelease_Request;
    using Destroy      = network::sim::MSG_SimDestroy_Request;
    using Block        = network::sim::MSG_SimDestroyBlocking_Request;
    using Clock        = network::sim::MSG_SimClock_Response;
    using MoveComplete = SimMoveManager::MoveComplete;
    using MoveRequest  = SimMoveManager::MoveRequest;
    using MoveResponse = SimMoveManager::MoveResponse;

    static inline network::MessageID getMsgID( const Msg& msg ) { return msg.msg.getID(); }

private:
    static inline const mega::MPO& getSimID( const Msg& msg )
    {
        switch( getMsgID( msg ) )
        {
            case Read::ID:
                return Read::get( msg.msg ).requestingMPO;
            case Write::ID:
                return Write::get( msg.msg ).requestingMPO;
            case Release::ID:
                return Release::get( msg.msg ).requestingMPO;
            case Destroy::ID:
            case Block::ID:
            case Clock::ID:
            default:
                THROW_RTE( "Unreachable" );
        }
    }

    inline MsgResult onMove()
    {
        MsgResult msgResult = eNothing;
        m_state             = MOVE;

        MsgVector moves;
        {
            MsgVector other;
            for( const Msg& msg : m_msgQueue )
            {
                switch( getMsgID( msg ) )
                {
                    case MoveRequest::ID:
                    case MoveResponse::ID:
                    case MoveComplete::ID:
                        moves.push_back( msg );
                        break;
                    default:
                        other.push_back( msg );
                        break;
                }
            }
            m_msgQueue = std::move( other );
        }

        // even if NO messages must call since may be NO moves
        switch( m_moveManager.onMsg( moves ) )
        {
            case SimMoveManager::MsgResult::eNothing:
            {
                // Do nothing...
            }
            break;
            case SimMoveManager::MsgResult::eMoveRequestsComplete:
            {
                msgResult = eMoveComplete;
            }
            break;
            case SimMoveManager::MsgResult::eProcessMoveComplete:
            {
                return onWait();
            }
            break;
            default:
            {
                THROW_RTE( "Unknown SimMoveManager msg result type" );
            }
        }

        return msgResult;
    }

    inline MsgResult onWait()
    {
        using namespace mega::network;

        MsgResult msgResult = eNothing;
        m_state             = WAIT;
        MsgVector reads;
        MsgVector other;
        for( const Msg& msg : m_msgQueue )
        {
            switch( getMsgID( msg ) )
            {
                case Read::ID:
                    reads.push_back( msg );
                    break;
                default:
                    other.push_back( msg );
                    break;
            }
        }

        if( !reads.empty() )
        {
            for( const auto& msg : reads )
            {
                const ID& id = getSimID( msg );
                m_acks.push_back( msg );
                m_activeReads.insert( id );
            }
            m_msgQueue = std::move( other );
            m_state    = READ;
        }
        else if( !other.empty() )
        {
            m_msgQueue.clear();
            for( const auto& msg : other )
            {
                switch( getMsgID( msg ) )
                {
                    case Read::ID:
                        THROW_RTE( "Unreachable" );
                        break;
                    case Write::ID:
                    {
                        if( m_state == WAIT )
                        {
                            const ID& id = getSimID( msg );
                            m_acks.push_back( msg );
                            m_activeWrite = id;
                            m_state       = WRITE;
                        }
                        else
                        {
                            m_msgQueue.push_back( msg );
                        }
                    }
                    break;
                    case Release::ID:
                    {
                        SPDLOG_ERROR( "SIM: Invalid release request: {}", msg.msg );
                        THROW_RTE( "SIM: Invalid release request" );
                    }
                    break;
                    case Clock::ID:
                    {
                        if( m_state == WAIT )
                        {
                            m_state     = SIM;
                            msgResult   = eClockTick;
                        }
                        else if( m_state != SIM )
                        {
                            m_msgQueue.push_back( msg );
                        }
                        else
                        {
                            // discard
                        }
                    }
                    break;
                    default:
                        SPDLOG_ERROR( "SIM: onWait Invalid message: {}", msg.msg );
                        THROW_RTE( "Unreachable" );
                }
            }
        }
        return msgResult;
    }

    inline MsgResult onRead()
    {
        using namespace mega::network;

        MsgResult msgResult = eNothing;

        // first process ALL release locks and determine if remaining read locks active
        MsgVector other;
        {
            // add new reads and process release
            for( const Msg& msg : m_msgQueue )
            {
                switch( getMsgID( msg ) )
                {
                    case Read::ID:
                    {
                        const ID& id = getSimID( msg );
                        m_acks.push_back( msg );
                        m_activeReads.insert( id );
                    }
                    break;
                    case Release::ID:
                    {
                        const ID& id    = getSimID( msg );
                        auto      iFind = m_activeReads.find( id );
                        if( iFind != m_activeReads.end() )
                        {
                            m_activeReads.erase( iFind );
                            m_acks.push_back( msg );
                        }
                        else
                        {
                            THROW_RTE( "Invalid release" );
                        }
                    }
                    break;
                    default:
                        other.push_back( msg );
                        break;
                }
            }
            m_msgQueue.clear();
        }

        // see if can promote read to write
        if( m_activeReads.size() == 1U )
        {
            MsgVector other2;
            std::swap( other2, other );
            for( const Msg& msg : other2 )
            {
                switch( getMsgID( msg ) )
                {
                    case Write::ID:
                    {
                        bool bPromoted = false;
                        if( m_state == READ )
                        {
                            const ID& idRead = *m_activeReads.begin();
                            const ID& id     = getSimID( msg );
                            if( idRead == id )
                            {
                                m_state = WRITE;
                                m_activeReads.clear();
                                m_activeWrite = id;
                                m_acks.push_back( msg );
                                bPromoted = true;
                            }
                        }
                        if( !bPromoted )
                        {
                            other.push_back( msg );
                        }
                    }
                    break;
                    default:
                        other.push_back( msg );
                        break;
                }
            }
        }
        else if( m_activeReads.empty() )
        {
            // direct transition to write?
        }

        for( const Msg& msg : other )
        {
            switch( getMsgID( msg ) )
            {
                case Read::ID:
                    THROW_RTE( "Unreachable" );
                    break;
                case Write::ID:
                    m_msgQueue.push_back( msg );
                    break;
                case Release::ID:
                    THROW_RTE( "Unreachable" );
                    break;
                case Clock::ID:
                    if( m_activeReads.empty() && m_state == READ )
                    {
                        m_state     = SIM;
                        msgResult   = eClockTick;
                    }
                    else
                    {
                        m_msgQueue.push_back( msg );
                    }
                    break;
                case Block::ID:
                case Destroy::ID:
                    THROW_RTE( "Unreachable" );
                default:
                    THROW_RTE( "Unreachable" );
                    break;
            }
        }
        return msgResult;
    }

    inline MsgResult onWrite()
    {
        using namespace mega::network;

        MsgResult msgResult = eNothing;

        // filter duplicate writes
        MsgVector other;
        {
            // add new reads and process release
            for( const Msg& msg : m_msgQueue )
            {
                switch( getMsgID( msg ) )
                {
                    case Write::ID:
                    {
                        const ID& id = getSimID( msg );
                        if( m_activeWrite.value() == id )
                        {
                            m_acks.push_back( msg );
                        }
                        else
                        {
                            other.push_back( msg );
                        }
                    }
                    break;
                    default:
                        other.push_back( msg );
                        break;
                }
            }
            m_msgQueue.clear();
        }

        // NOTE: only process one write at a time and queue all other messages
        // determine if active write lock is released
        {
            // add new reads and process release
            MsgVector other2;
            std::swap( other, other2 );
            for( const Msg& msg : other2 )
            {
                switch( getMsgID( msg ) )
                {
                    case Release::ID:
                    {
                        const ID& id = getSimID( msg );
                        if( m_activeWrite.value() == id )
                        {
                            m_activeWrite.reset();
                            m_acks.push_back( msg );
                        }
                        else
                        {
                            THROW_RTE( "Invalid release" );
                        }
                    }
                    break;
                    default:
                        other.push_back( msg );
                        break;
                }
            }
        }

        // stay in the WRITE state until clock tick
        for( const Msg& msg : other )
        {
            switch( getMsgID( msg ) )
            {
                case Read::ID:
                {
                    if( m_activeWrite.has_value() )
                    {
                        const ID& id = getSimID( msg );
                        if( !( m_activeWrite.value() == id ) )
                        {
                            m_msgQueue.push_back( msg );
                        }
                        else
                        {
                            // if read is for same SimID as write then ack it
                            m_acks.push_back( msg );
                        }
                    }
                    else
                    {
                        m_msgQueue.push_back( msg );
                    }
                }
                break;
                case Write::ID:
                    if( m_state == WRITE && !m_activeWrite.has_value() )
                    {
                        const ID& id = getSimID( msg );
                        m_acks.push_back( msg );
                        m_activeWrite = id;
                    }
                    else
                    {
                        m_msgQueue.push_back( msg );
                    }
                    break;
                case Release::ID:
                    THROW_RTE( "Unreachable" );
                    break;
                case Clock::ID:
                    if( !m_activeWrite.has_value() )
                    {
                        m_state     = SIM;
                        msgResult   = eClockTick;
                    }
                    else
                    {
                        m_msgQueue.push_back( msg );
                    }
                    break;
                case Block::ID:
                case Destroy::ID:
                    THROW_RTE( "Unreachable" );
                default:
                    THROW_RTE( "Unreachable" );
                    break;
            }
        }
        return msgResult;
    }

    inline MsgResult onTerm()
    {
        using namespace mega::network;

        MsgResult msgResult = eNothing;

        // stay in the WRITE state until clock tick
        for( const Msg& msg : m_msgQueue )
        {
            switch( getMsgID( msg ) )
            {
                case Read::ID:
                {
                    const ID& id = getSimID( msg );
                    m_acks.push_back( msg );
                }
                break;
                case Write::ID:
                {
                    const ID& id = getSimID( msg );
                    m_acks.push_back( msg );
                }
                break;
                case Release::ID:
                {
                    const ID& id    = getSimID( msg );
                    auto      iFind = m_activeReads.find( id );
                    if( iFind != m_activeReads.end() )
                    {
                        m_activeReads.erase( iFind );
                        m_acks.push_back( msg );
                    }
                    else if( m_activeWrite.has_value() && ( m_activeWrite.value() == id ) )
                    {
                        m_activeWrite.reset();
                        m_acks.push_back( msg );
                    }
                    else
                    {
                        THROW_RTE( "Invalid release" );
                    }
                }
                break;
                case Clock::ID:
                {
                    msgResult   = eClockTick;
                }
                break;
                case Block::ID:
                case Destroy::ID:
                {
                    m_acks.push_back( msg );
                }
                break;
                default:
                    THROW_RTE( "Unreachable" );
                    break;
            }
        }
        m_msgQueue.clear();
        return msgResult;
    }

    // actual modifying interface
public:
    inline void resetAcks() { m_acks.clear(); }

    inline void onCycle()
    {
        m_state = MOVE;
        //m_state = WAIT;
    }

    // returns true if clock ticked
    inline MsgResult onMsg( const MsgVector& msgs )
    {
        using namespace mega::network;

        for( const Msg& msg : msgs )
        {
            switch( getMsgID( msg ) )
            {
                case Block::ID:
                case Destroy::ID:
                    m_state = TERM;
                    break;
            }
        }

        resetAcks();
        std::copy( msgs.begin(), msgs.end(), std::back_inserter( m_msgQueue ) );

        switch( m_state )
        {
            case SIM:
                THROW_RTE( "Invalid state when receive message - in SIM state" );
                break;
            case MOVE:
                return onMove();
            case WAIT:
                return onWait();
            case READ:
                return onRead();
            case WRITE:
                return onWrite();
            case TERM:
                return onTerm();
            default:
                THROW_RTE( "Unknown state" );
        }
    }
};

} // namespace mega::service

#endif // STATE_MACHINE_16_SEPT_2022
