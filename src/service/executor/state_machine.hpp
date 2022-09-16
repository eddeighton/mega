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
    using Ack       = std::pair< network::ChannelMsg, std::optional< ID > >;
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
    State                m_state = SIM;
    MsgVector            m_msgQueue;
    IDSet                m_activeReads;
    std::optional< ID >  m_activeWrite;
    std::optional< Ack > m_termination;

public:
    State            getState() const { return m_state; }
    const AckVector& acks() const { return m_acks; }

    using Read    = mega::network::exe_sim::MSG_ExeSimReadLockAcquire_Request;
    using Write   = mega::network::exe_sim::MSG_ExeSimWriteLockAcquire_Request;
    using Release = mega::network::exe_sim::MSG_ExeSimLockRelease_Request;
    using Destroy = mega::network::exe_sim::MSG_ExeSimDestroy_Request;
    using Clock   = mega::network::exe_sim::MSG_ExeExeClock_Request;

    void onMsg( const MsgVector& msgs )
    {
        using namespace mega::network;
        std::optional< ID > requestID;
        m_acks.clear();

        std::copy( msgs.begin(), msgs.end(), std::back_inserter( m_msgQueue ) );

        switch ( m_state )
        {
            default:
                THROW_RTE( "Unknown state" );
            case WAIT:
            case SIM:
            {
                m_state = WAIT;
                MsgVector reads;
                MsgVector other;
                for ( const network::ChannelMsg& msg : m_msgQueue )
                {
                    switch ( getMsgID( msg.msg ) )
                    {
                        case Read::ID:
                            if ( !m_termination.has_value() )
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
                        requestID = Read::get( msg.msg ).requestID;
                        VERIFY_RTE( requestID.has_value() );
                        m_acks.push_back( { msg, requestID } );
                        m_activeReads.insert( requestID.value() );
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
                                __builtin_unreachable();
                                break;
                            case Write::ID:
                            {
                                // if( !m_termination.has_value() ) ????
                                if ( m_state == WAIT )
                                {
                                    requestID = Write::get( msg.msg ).requestID;
                                    m_acks.push_back( { msg, requestID } );
                                    m_activeWrite = requestID.value();
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
                            }
                            break;
                            case Destroy::ID:
                            {
                                if ( m_state == WAIT )
                                {
                                    requestID = Destroy::get( msg.msg ).requestID;
                                    VERIFY_RTE( requestID.has_value() );
                                    m_termination = { msg, requestID };
                                    m_state = TERM;
                                }
                                else
                                {
                                    m_msgQueue.push_back( msg );
                                }
                            }
                            break;
                            case Clock::ID:
                            {
                                if ( m_state == WAIT )
                                {
                                    m_state = SIM;
                                }
                                else
                                {
                                    m_msgQueue.push_back( msg );
                                }
                            }
                            break;
                            default:
                                __builtin_unreachable();
                        }
                    }
                }
                else
                {
                    m_state = WAIT;
                }
            }
            break;
            case READ:
            {
                State nextState = WAIT;
                for ( const network::ChannelMsg& msg : m_msgQueue )
                {
                    switch ( getMsgID( msg.msg ) )
                    {
                        case Read::ID:
                        {
                            requestID = Read::get( msg.msg ).requestID;
                            VERIFY_RTE( requestID.has_value() );
                            m_acks.push_back( { msg, requestID } );
                            m_activeReads.insert( requestID.value() );
                        }
                        break;
                        case Write::ID:
                        {
                            requestID = Read::get( msg.msg ).requestID;
                            VERIFY_RTE( requestID.has_value() );
                            auto iFind = m_activeReads.find( requestID.value() );
                            if ( iFind != m_activeReads.end() )
                            {
                                m_activeReads.erase( iFind );
                            }
                            m_msgQueue.push_back( msg );
                            if ( nextState == WAIT )
                            {
                                nextState = WRITE;
                            }
                        }
                        break;
                        case Release::ID:
                        {
                            requestID = Release::get( msg.msg ).requestID;
                            VERIFY_RTE( requestID.has_value() );
                            auto iFind = m_activeReads.find( requestID.value() );
                            if ( iFind != m_activeReads.end() )
                            {
                                m_activeReads.erase( iFind );
                            }
                            else
                            {
                                SPDLOG_ERROR( "SIM: Invalid release request: {}", msg.header );
                            }
                        }
                        break;
                        case Destroy::ID:
                        {
                            m_msgQueue.push_back( msg );
                            // requestID = Release::get( msg.msg ).requestID;
                            // VERIFY_RTE( requestID.has_value() );
                            // m_acks.push_back( { msg, requestID } );
                            // nextState = TERM;
                        }
                        break;
                        case Clock::ID:
                        {
                            m_msgQueue.push_back( msg );
                            // if ( nextState == WAIT )
                            //{
                            //     nextState = SIM;
                            // }
                            // else
                            //{
                            //     m_msgQueue.push_back( msg );
                            // }
                        }
                        break;
                        default:
                            __builtin_unreachable();
                            break;
                    }
                }
                if ( m_activeReads.empty() )
                {
                    m_state = nextState;
                }
            }
            break;
            case WRITE:
            {
                State nextState      = WRITE;
                bool  bPendingWrites = false;

                for ( const network::ChannelMsg& msg : m_msgQueue )
                {
                    switch ( getMsgID( msg.msg ) )
                    {
                        case Read::ID:
                        {
                            m_msgQueue.push_back( msg );
                        }
                        break;
                        case Write::ID:
                        {
                            // only if have not encountered a Destroy or Clock are there
                            // further writes to complete
                            if ( nextState == WRITE )
                            {
                                bPendingWrites = true;
                            }
                            requestID = Write::get( msg.msg ).requestID;
                            VERIFY_RTE( requestID.has_value() );
                            if ( !m_activeWrite.has_value() )
                            {
                                m_acks.push_back( { msg, requestID } );
                                m_activeWrite = requestID.value();
                            }
                            else
                            {
                                m_msgQueue.push_back( msg );
                            }
                        }
                        break;
                        case Release::ID:
                        {
                            requestID = Release::get( msg.msg ).requestID;
                            if ( m_activeWrite.has_value() )
                            {
                                if ( m_activeWrite.value() == requestID.value() )
                                {
                                    m_activeWrite.reset();
                                }
                                else
                                {
                                    m_msgQueue.push_back( msg );
                                }
                            }
                            else
                            {
                                SPDLOG_ERROR( "SIM: Invalid release request: {}", msg.header );
                            }
                        }
                        break;
                        case Destroy::ID:
                        {
                            m_msgQueue.push_back( msg );
                            // requestID = Release::get( msg.msg ).requestID;
                            // VERIFY_RTE( requestID.has_value() );
                            // m_acks.push_back( { msg, requestID } );
                            // nextState = TERM;
                        }
                        break;
                        case Clock::ID:
                        {
                            m_msgQueue.push_back( msg );
                            // if ( nextState == WAIT )
                            //{
                            //     nextState = SIM;
                            // }
                            // else
                            //{
                            //     m_msgQueue.push_back( msg );
                            // }
                        }
                        break;
                        default:
                            __builtin_unreachable();
                            break;
                    }
                }
                if ( !bPendingWrites )
                {
                    m_state = nextState;
                }
            }
            break;
            case TERM:
            {
                __builtin_unreachable();
            }
            break;
        }
    }
};

} // namespace service
} // namespace mega

#endif // STATE_MACHINE_16_SEPT_2022
