#ifndef STATE_MACHINE_16_SEPT_2022
#define STATE_MACHINE_16_SEPT_2022

#include "mega/common.hpp"

#include "service/network/sender.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/exe_sim.hxx"

#include <set>
#include <vector>
#include <optional>
#include <utility>

namespace mega
{
namespace service
{

class SimulationStateMachine
{
    using ID        = mega::network::ConversationID;
    using IDSet     = std::set< ID >;
    using Ack       = std::pair< network::ChannelMsg, ID >;
    using AckVector = std::vector< Ack >;

    AckVector m_acks;

public:
    enum State
    {
        SIM,
        READ,
        WRITE,
        TERM,
        WAIT
    };
    using MsgVector = std::vector< network::ChannelMsg >;

private:
    State               m_state = SIM;
    MsgVector           m_msgQueue;
    IDSet               m_activeReads;
    std::optional< ID > m_activeWrite;

public:
    State getState() const { return m_state; }
    bool  isTerminating() const { return m_state == TERM; }
    bool  isTerminated() const
    {
        return ( m_state == TERM ) && m_activeReads.empty() && ( !m_activeWrite.has_value() ) && m_acks.empty();
    }
    const AckVector& acks() const { return m_acks; }
    void             resetAcks() { m_acks.clear(); }

    using Read    = mega::network::exe_sim::MSG_ExeSimReadLockAcquire_Request;
    using Write   = mega::network::exe_sim::MSG_ExeSimWriteLockAcquire_Request;
    using Release = mega::network::exe_sim::MSG_ExeSimLockRelease_Request;
    using Destroy = mega::network::exe_sim::MSG_ExeSimDestroy_Request;
    using Clock   = mega::network::exe_sim::MSG_ExeExeClock_Request;

    const network::ConversationID& getSimID( const mega::network::ChannelMsg& msg ) const
    {
        switch ( getMsgID( msg.msg ) )
        {
            case Read::ID:
                return Read::get( msg.msg ).requestID;
            case Write::ID:
                return Write::get( msg.msg ).requestID;
            case Release::ID:
                return Release::get( msg.msg ).requestID;
            case Destroy::ID:
                return Destroy::get( msg.msg ).requestID;
            case Clock::ID:
            default:
                THROW_RTE( "Unreachable" );
        }
    }

    bool onWait()
    {
        using namespace mega::network;

        bool bClockTicked = false;
        m_state           = WAIT;
        MsgVector reads;
        MsgVector other;
        for ( const ChannelMsg& msg : m_msgQueue )
        {
            switch ( getMsgID( msg.msg ) )
            {
                case Read::ID:
                    reads.push_back( msg );
                    break;
                default:
                    other.push_back( msg );
                    break;
            }
        }

        if ( !reads.empty() )
        {
            for ( const auto& msg : reads )
            {
                const ID& id = getSimID( msg );
                m_acks.push_back( { msg, id } );
                m_activeReads.insert( id );
            }
            m_msgQueue = std::move( other );
            m_state    = READ;
        }
        else if ( !other.empty() )
        {
            m_msgQueue.clear();
            for ( const auto& msg : other )
            {
                switch ( getMsgID( msg.msg ) )
                {
                    case Read::ID:
                        THROW_RTE( "Unreachable" );
                        break;
                    case Write::ID:
                    {
                        if ( m_state == WAIT && m_state != TERM )
                        {
                            const ID& id = getSimID( msg );
                            m_acks.push_back( { msg, id } );
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
                        SPDLOG_ERROR( "SIM: Invalid release request: {}", msg.header );
                        THROW_RTE( "SIM: Invalid release request" );
                    }
                    break;
                    case Clock::ID:
                    {
                        if ( m_state == WAIT )
                        {
                            m_state      = SIM;
                            bClockTicked = true;
                        }
                        else if ( m_state != SIM )
                        {
                            m_msgQueue.push_back( msg );
                        }
                        else
                        {
                            // discard
                        }
                    }
                    break;
                    case Destroy::ID:
                        THROW_RTE( "Unreachable" );
                    default:
                        THROW_RTE( "Unreachable" );
                }
            }
        }
        else
        {
            m_state = WAIT;
        }
        return bClockTicked;
    }

    bool onRead()
    {
        using namespace mega::network;

        bool bClockTicked = false;

        // first process ALL release locks and determine if remaining read locks active
        MsgVector other;
        {
            // add new reads and process release
            for ( const ChannelMsg& msg : m_msgQueue )
            {
                switch ( getMsgID( msg.msg ) )
                {
                    case Read::ID:
                    {
                        const ID& id = getSimID( msg );
                        m_acks.push_back( { msg, id } );
                        m_activeReads.insert( id );
                    }
                    break;
                    case Release::ID:
                    {
                        const ID& id    = getSimID( msg );
                        auto      iFind = m_activeReads.find( id );
                        if ( iFind != m_activeReads.end() )
                        {
                            m_activeReads.erase( iFind );
                            m_acks.push_back( { msg, id } );
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

        for ( const network::ChannelMsg& msg : other )
        {
            switch ( getMsgID( msg.msg ) )
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
                    if ( m_activeReads.empty() && m_state != TERM )
                    {
                        m_state      = SIM;
                        bClockTicked = true;
                    }
                    else
                    {
                        m_msgQueue.push_back( msg );
                    }
                    break;
                case Destroy::ID:
                    THROW_RTE( "Unreachable" );
                default:
                    THROW_RTE( "Unreachable" );
                    break;
            }
        }
        return bClockTicked;
    }

    bool onWrite()
    {
        using namespace mega::network;

        bool bClockTicked = false;
        // NOTE: only process one write at a time and queue all other messages
        // determine if active write lock is released
        MsgVector other;
        {
            // add new reads and process release
            for ( const ChannelMsg& msg : m_msgQueue )
            {
                switch ( getMsgID( msg.msg ) )
                {
                    case Release::ID:
                    {
                        const ID& id = getSimID( msg );
                        if ( m_activeWrite.value() == id )
                        {
                            m_activeWrite.reset();
                            m_acks.push_back( { msg, id } );
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

        // stay in the WRITE state until clock tick
        for ( const network::ChannelMsg& msg : other )
        {
            switch ( getMsgID( msg.msg ) )
            {
                case Read::ID:
                {
                    if ( m_activeWrite.has_value() )
                    {
                        const ID& id = getSimID( msg );
                        if ( !( m_activeWrite.value() == id ) )
                        {
                            m_msgQueue.push_back( msg );
                        }
                        else
                        {
                            // if read is for same SimID as write then ack it
                            m_acks.push_back( { msg, id } );
                        }
                    }
                    else
                    {
                        m_msgQueue.push_back( msg );
                    }
                }
                break;
                case Write::ID:
                    if ( m_state == WRITE && !m_activeWrite.has_value() )
                    {
                        const ID& id = getSimID( msg );
                        m_acks.push_back( { msg, id } );
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
                    if ( !m_activeWrite.has_value() )
                    {
                        m_state      = SIM;
                        bClockTicked = true;
                    }
                    else
                    {
                        m_msgQueue.push_back( msg );
                    }
                    break;
                case Destroy::ID:
                    THROW_RTE( "Unreachable" );
                default:
                    THROW_RTE( "Unreachable" );
                    break;
            }
        }
        return bClockTicked;
    }

    bool onTerm()
    {
        using namespace mega::network;

        bool bClockTicked = false;

        // stay in the WRITE state until clock tick
        for ( const network::ChannelMsg& msg : m_msgQueue )
        {
            switch ( getMsgID( msg.msg ) )
            {
                case Read::ID:
                {
                    const ID& id = getSimID( msg );
                    m_acks.push_back( { msg, id } );
                }
                break;
                case Write::ID:
                {
                    const ID& id = getSimID( msg );
                    m_acks.push_back( { msg, id } );
                }
                break;
                case Release::ID:
                {
                    const ID& id    = getSimID( msg );
                    auto      iFind = m_activeReads.find( id );
                    if ( iFind != m_activeReads.end() )
                    {
                        m_activeReads.erase( iFind );
                        m_acks.push_back( { msg, id } );
                    }
                    else if ( m_activeWrite.has_value() && ( m_activeWrite.value() == id ) )
                    {
                        m_activeWrite.reset();
                        m_acks.push_back( { msg, id } );
                    }
                    else
                    {
                        THROW_RTE( "Invalid release" );
                    }
                }
                break;
                case Clock::ID:
                {
                    bClockTicked = true;
                }
                break;
                case Destroy::ID:
                {
                    const ID& id = getSimID( msg );
                    m_acks.push_back( { msg, id } );
                }
                break;
                default:
                    THROW_RTE( "Unreachable" );
                    break;
            }
        }
        m_msgQueue.clear();
        return bClockTicked;
    }

    // returns true if clock ticked
    bool onMsg( const MsgVector& msgs )
    {
        using namespace mega::network;

        for ( const ChannelMsg& msg : msgs )
        {
            switch ( getMsgID( msg.msg ) )
            {
                case Destroy::ID:
                    m_state = TERM;
                    break;
            }
        }

        resetAcks();
        std::copy( msgs.begin(), msgs.end(), std::back_inserter( m_msgQueue ) );

        switch ( m_state )
        {
            case SIM:
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

} // namespace service
} // namespace mega

#endif // STATE_MACHINE_16_SEPT_2022
