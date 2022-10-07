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

// ed was here! on deepthought
//
#include <gtest/gtest.h>

#include "service/state_machine.hpp"

using namespace std::literals;
static const mega::MPO id1( 1, 2, 3 );
static const mega::MPO id2( 1, 2, 4 );
static const mega::MPO id3( 1, 3, 3 );
static const mega::MPO id4( 2, 3, 4 );

using SM = mega::service::StateMachine;

SM::Msg makeMsg( const mega::MPO& id, const mega::network::Message& msg )
{
    std::ostringstream os;
    os << id;
    return mega::network::ReceivedMsg{ os.str(), msg };
}

SM::Msg makeClock( const mega::MPO& id )
{
    return makeMsg( id, SM::Clock::make( {}, SM::Clock{} ) );
}
SM::Msg makeRead( const mega::MPO& id )
{
    return makeMsg( id, SM::Read::make( {}, SM::Read{ id } ) );
}
SM::Msg makeWrite( const mega::MPO& id )
{
    return makeMsg( id, SM::Write::make( {}, SM::Write{ id } ) );
}
SM::Msg makeRelease( const mega::MPO& id )
{
    return makeMsg( id, SM::Release::make( {}, SM::Release{ id } ) );
}
SM::Msg makeDestroy( const mega::MPO& id )
{
    return makeMsg( id, SM::Destroy::make( {}, SM::Destroy{} ) );
}

TEST( SimStateMachine, WaitForClock )
{
    SM sm;
    ASSERT_EQ( sm.getState(), SM::SIM );

    // no messages means it should be waiting
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
    }
    {
        sm.onMsg( { makeClock( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
    }
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
    }
}

TEST( SimStateMachine, BasicRead )
{
    SM sm;
    ASSERT_TRUE( sm.acks().empty() );

    // read request puts into read state with single ack
    {
        sm.onMsg( { makeRead( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // remains in read state
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // once read is released remains in read
    {
        sm.onMsg( { makeRelease( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // no more acks
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // and clock tick causes next sim cycle
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now waits
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        ASSERT_TRUE( sm.acks().empty() );
    }
}

TEST( SimStateMachine, BasicReadBlocksClock )
{
    SM sm;
    ASSERT_TRUE( sm.acks().empty() );

    // read request puts into read state with single ack
    {
        sm.onMsg( { makeRead( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // remains in read state
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // once read is released returns to wait
    {
        sm.onMsg( { makeRelease( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // no more acks
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        ASSERT_TRUE( sm.acks().empty() );
    }
}

TEST( SimStateMachine, BasicWrite )
{
    SM sm;
    ASSERT_TRUE( sm.acks().empty() );

    // write request puts into write state with single ack
    {
        sm.onMsg( { makeWrite( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // remains in write state
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // remains in write state
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // once write is released returns to sim due to clock
    {
        sm.onMsg( { makeRelease( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // now waits
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        ASSERT_TRUE( sm.acks().empty() );
    }
}

TEST( SimStateMachine, WriteThenReadCancelsOut )
{
    SM sm;
    ASSERT_TRUE( sm.acks().empty() );

    // write request puts into write state with single ack
    {
        sm.onMsg( { makeWrite( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // remains in write state
    {
        sm.onMsg( { makeRead( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // remains in write state
    {
        sm.onMsg( { makeRelease( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // once write is released returns to sim due to clock
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now waits
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        ASSERT_TRUE( sm.acks().empty() );
    }
}

TEST( SimStateMachine, WriteBlocksRead )
{
    SM sm;
    ASSERT_TRUE( sm.acks().empty() );

    // write request puts into write state with single ack
    {
        sm.onMsg( { makeWrite( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // does not ack read
    {
        sm.onMsg( { makeRead( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // waits for write release
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // remains in write state - still no read ack
    {
        sm.onMsg( { makeRelease( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // clock breaks out of write
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now acks the read
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // now waits for read release
    {
        sm.onMsg( { makeRelease( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // clock breaks out of read
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now waits
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        ASSERT_TRUE( sm.acks().empty() );
    }
}

TEST( SimStateMachine, BasicTerm )
{
    SM sm;
    ASSERT_TRUE( sm.acks().empty() );

    // write request puts into write state with single ack
    {
        sm.onMsg( { makeDestroy( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::TERM );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // now waits in term
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::TERM );
        ASSERT_TRUE( sm.acks().empty() );
    }
}

TEST( SimStateMachine, BasicPromote )
{
    SM sm;
    ASSERT_TRUE( sm.acks().empty() );

    // read
    {
        sm.onMsg( { makeRead( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // clock has to wait
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now waits in read
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now promote to write
    {
        sm.onMsg( { makeWrite( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // now waits in write
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now release and get clock tick
    {
        sm.onMsg( { makeRelease( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // now waits
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        ASSERT_TRUE( sm.acks().empty() );
    }
}

TEST( SimStateMachine, IgnoreDuplicateReadsAndWrites )
{
    SM sm;
    ASSERT_TRUE( sm.acks().empty() );

    // read
    {
        sm.onMsg( { makeRead( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // clock has to wait
    {
        sm.onMsg( { makeClock( id2 ) } );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now waits in read
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::READ );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now promote to write
    {
        sm.onMsg( { makeWrite( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // ignors reads and writes 
    {
        sm.onMsg( { makeRead( id1 ), makeRead( id1 ), makeRead( id1 ), makeWrite( id1 ), makeWrite( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_EQ( sm.acks().size(), 5U );
    }
    // now waits in write
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WRITE );
        ASSERT_TRUE( sm.acks().empty() );
    }
    // now release and get clock tick
    {
        sm.onMsg( { makeRelease( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
        ASSERT_EQ( sm.acks().size(), 1U );
    }
    // now waits
    {
        sm.onMsg( {} );
        ASSERT_EQ( sm.getState(), SM::WAIT );
        ASSERT_TRUE( sm.acks().empty() );
    }
}
