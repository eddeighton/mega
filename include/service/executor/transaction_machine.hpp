
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

#ifndef GUARD_2023_September_12_transaction_machine
#define GUARD_2023_September_12_transaction_machine

#include "service/executor/message_traits.hpp"

#include "service/protocol/model/sim.hxx"

#include "service/protocol/common/received_message.hpp"
#include "mega/values/service/status.hpp"

#include <optional>

namespace mega::service
{

struct TransactionMsgTraits : public MsgTraits
{
    using Read    = network::sim::MSG_SimLockRead_Request;
    using Write   = network::sim::MSG_SimLockWrite_Request;
    using Release = network::sim::MSG_SimLockRelease_Request;

    static inline bool isRead( const Msg& msg ) { return getMsgID( msg ) == Read::ID; }
    static inline bool isWrite( const Msg& msg ) { return getMsgID( msg ) == Write::ID; }
    static inline bool isRelease( const Msg& msg ) { return getMsgID( msg ) == Release::ID; }

    static inline bool isMsg( const Msg& msg )
    {
        switch( getMsgID( msg ) )
        {
            case Read::ID:
            case Write::ID:
            case Release::ID:
                return true;
            default:
                return false;
        }
    }

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
            default:
                THROW_RTE( "Unreachable" );
        }
    }
};

template < typename Simulation >
class TransactionMachine : public TransactionMsgTraits
{
    using ID    = mega::MPO;
    using IDSet = std::set< ID >;

    MsgVector           m_queue;
    Simulation&         m_sim;
    AckVector&          m_ackVector;
    IDSet               m_activeReads;
    std::optional< ID > m_activeWrite;

    void processMessages()
    {
        // if no active write then process reads
        if( !m_activeWrite.has_value() )
        {
            // reading
            MsgVector temp;
            for( const Msg& msg : m_queue )
            {
                switch( getMsgID( msg ) )
                {
                    case Read::ID:
                    {
                        m_ackVector.push_back( msg );
                        m_activeReads.insert( getSimID( msg ) );
                    }
                    break;
                    case Release::ID:
                    {
                        const ID& id    = getSimID( msg );
                        auto      iFind = m_activeReads.find( id );
                        if( iFind != m_activeReads.end() )
                        {
                            m_activeReads.erase( iFind );
                            m_ackVector.push_back( msg );
                        }
                        else
                        {
                            THROW_RTE( "Invalid release: " << id );
                        }
                    }
                    break;
                    case Write::ID:
                    {
                        temp.push_back( msg );
                    }
                    break;
                }
            }
            m_queue = std::move( temp );
        }

        // if no reads then process write
        if( m_activeReads.empty() )
        {
            MsgVector temp;
            for( const Msg& msg : m_queue )
            {
                switch( getMsgID( msg ) )
                {
                    case Read::ID:
                    {
                        temp.push_back( msg );
                    }
                    break;
                    case Release::ID:
                    {
                        if( m_activeWrite.has_value() )
                        {
                            if( m_activeWrite.value() == getSimID( msg ) )
                            {
                                m_ackVector.push_back( msg );
                                m_activeWrite.reset();
                            }
                            else
                            {
                                temp.push_back( msg );
                            }
                        }
                        else
                        {
                            temp.push_back( msg );
                        }
                    }
                    break;
                    case Write::ID:
                    {
                        if( m_activeWrite.has_value() )
                        {
                            temp.push_back( msg );
                        }
                        else
                        {
                            m_activeWrite = getSimID( msg );
                            m_ackVector.push_back( msg );
                        }
                    }
                    break;
                }
            }
            m_queue = std::move( temp );
        }
        else if( m_activeReads.size() == 1U )
        {
            // attempt to promote read to write
            MsgVector temp;
            std::swap( temp, m_queue );
            for( const Msg& msg : temp )
            {
                switch( getMsgID( msg ) )
                {
                    case Write::ID:
                    {
                        const ID& id = getSimID( msg );
                        if( *m_activeReads.begin() == id )
                        {
                            m_activeReads.clear();
                            m_activeWrite = id;
                            m_ackVector.push_back( msg );
                        }
                        else
                        {
                            m_queue.push_back( msg );
                        }
                    }
                    break;
                    default:
                        m_queue.push_back( msg );
                        break;
                }
            }
        }
    }

    inline bool isLock() const { return !m_activeReads.empty() || m_activeWrite.has_value(); }

public:
    inline const IDSet&        reads() const { return m_activeReads; }
    inline std::optional< ID > writer() const { return m_activeWrite; }

    TransactionMachine( Simulation& sim, AckVector& ackVector )
        : m_sim( sim )
        , m_ackVector( ackVector )
    {
    }

    inline void status( network::Status& status )
    {
        if( const auto& readers = m_activeReads; !readers.empty() )
            status.setReaders( std::vector< MPO >{ readers.begin(), readers.end() } );
        if( const auto& writer = m_activeWrite; writer.has_value() )
            status.setWriter( writer.value() );
    }

    inline void queue( const Msg& msg ) { m_queue.push_back( msg ); }

    // return if should enter transaction processing
    inline bool onNext()
    {
        ASSERT( !isLock() );

        processMessages();

        if( !m_activeReads.empty() || m_activeWrite.has_value() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    inline void onTerminate()
    {
        for( const Msg& msg : m_queue )
        {
            m_ackVector.push_back( msg );
        }
        m_activeReads.clear();
        m_activeWrite.reset();
        m_queue.clear();
    }

    // return if should enter transaction processing
    inline bool onSleep( const Msg& msg )
    {
        m_queue.push_back( msg );

        if( m_sim.unrequestClock() )
        {
            processMessages();
            return true;
        }
        else
        {
            return false;
        }
    }

    // return if transaction processing complete
    inline bool onMessage( const Msg& msg )
    {
        ASSERT( isMsg( msg ) );

        m_queue.push_back( msg );

        processMessages();

        if( isLock() )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
};

} // namespace mega::service

#endif // GUARD_2023_September_12_transaction_machine
