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

#include "service/executor/sim_move_machine.hpp"
#include "service/executor/transaction_machine.hpp"
#include "service/executor/state_machine.hpp"

#include "mega/values/service/project.hpp"

using namespace std::literals;
static const mega::MPO id1( 1, 2, 3 );
static const mega::MPO id2( 1, 2, 4 );
static const mega::MPO id3( 1, 3, 3 );
static const mega::MPO id4( 2, 3, 4 );
/*
using MM = mega::service::SimMoveMachine;
using SM = mega::service::StateMachine;
SM::Msg makeMsg( const mega::MPO& id, const mega::network::Message& msg )
{
    return mega::network::ReceivedMessage{ mega::network::Sender::Ptr{}, msg };
}

SM::Msg makeClock( const mega::MPO& id )
{
    return makeMsg( id, SM::Clock::make( {}, SM::Clock{} ) );
}
SM::Msg makeMoveComplete( const mega::MPO& id )
{
    return makeMsg( id, SM::MoveComplete::make( {}, SM::MoveComplete{} ) );
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
*/
/*
TEST( SimStateMachine, BasicCycle )
{
    MM mm;
    SM sm( mm );
    ASSERT_EQ( sm.getState(), SM::SIM );

    for( int i = 0; i != 4; ++i )
    {
        // move requests always sent after cycle completes
        mm.sendMoveRequests();

        // since there are no Moves sm will return eMoveComplete straight away
        ASSERT_EQ( SM::eMoveComplete, sm.onMsg( {} ) );
        // stays in MOVE state waiting for process wide MoveComplete response
        ASSERT_EQ( sm.getState(), SM::MOVE );

        // continues waiting
        ASSERT_EQ( SM::eNothing, sm.onMsg( {} ) );
        ASSERT_EQ( sm.getState(), SM::MOVE );

        // once received will transition to WAIT since no transaction
        ASSERT_EQ( SM::eNothing, sm.onMsg( { makeMoveComplete( id1 ) } ) );
        ASSERT_EQ( sm.getState(), SM::WAIT );

        // once receive clock tick the cycle repeats
        sm.onMsg( { makeClock( id1 ) } );
        ASSERT_EQ( sm.getState(), SM::SIM );
    }
}
*/
/*
TEST( SimStateMachine, BasicRead )
{
    MM mm;
    SM sm( mm );
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
    MM mm;
    SM sm( mm );
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
    MM mm;
    SM sm( mm );
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
    MM mm;
    SM sm( mm );
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
    MM mm;
    SM sm( mm );
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
    MM mm;
    SM sm( mm );
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
    MM mm;
    SM sm( mm );
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
    MM mm;
    SM sm( mm );
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
*/