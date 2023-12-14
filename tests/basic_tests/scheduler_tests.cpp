
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

#ifndef GUARD_2023_August_01_scheduler_tests
#define GUARD_2023_August_01_scheduler_tests

#include "mega/scheduler.hpp"
#include "mega/coroutine.hpp"
#include "mega/resumption.hpp"

#include <gtest/gtest.h>

#include <functional>

template < typename ExecutionStateImpl >
class BasicSchedulerTestFixture : public ::testing::Test
{
public:
    struct ExecutionState
    {
        using EventVector = std::vector< Event >;

        ExecutionStateImpl&   m_impl;
        mega::Pointer       m_ref;
        mega::ActionCoroutine m_coroutine;
        EventVector           m_events;

        ExecutionState( ExecutionStateImpl& impl, const mega::Pointer& ref )
            : m_impl( impl )
            , m_ref( ref )
        {
        }

        const mega::Pointer& getRef() const { return m_ref; }

        void stop()
        {
            //
        }

        void execute()
        {
            if( !m_coroutine.started() || m_coroutine.done() )
            {
                m_coroutine = m_impl.start( m_ref );
                m_coroutine.resume();
            }
            else
            {
                m_coroutine.resume();
            }
        }

        void onEvent( const mega::Pointer& ev ) { m_events.push_back( ev ); }

        const mega::ReturnReason& getReturnReason() const { return m_coroutine.getReason(); }
    };

    using TestScheduler = mega::BasicScheduler< ExecutionState >;
    TestScheduler      scheduler;
    ExecutionStateImpl m_state;

    ExecutionState make_test( mega::SubType objectTypeID, std::size_t szHeapAddress = 0U )
    {
        mega::Pointer ref( mega::TypeInstance::make_object( mega::TypeID::make_object_from_objectID( objectTypeID ) ),
                             reinterpret_cast< mega::HeapAddress >( szHeapAddress ) );
        return ExecutionState( m_state, ref );
    }
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct TestActions
{
    int                   iRunCount = 0;
    mega::ActionCoroutine test()
    {
        ++iRunCount;
        co_return mega::sleep();
    }
    mega::ActionCoroutine start( const mega::Pointer& ref )
    {
        switch( ref.getType().getObjectID() )
        {
            case 0:
                return test();
            default:
                std::abort();
        }
    }
};

using BasicSchedulerCall = BasicSchedulerTestFixture< TestActions >;
TEST_F( BasicSchedulerCall, Call )
{
    ASSERT_EQ( m_state.iRunCount, 0 );
    ASSERT_FALSE( scheduler.active() );
    scheduler.cycle();
    ASSERT_FALSE( scheduler.active() );
    ASSERT_EQ( m_state.iRunCount, 0 );
    scheduler.call( make_test( 0, 1 ) );
    scheduler.call( make_test( 0, 2 ) );
    scheduler.call( make_test( 0, 3 ) );
    ASSERT_EQ( m_state.iRunCount, 0 );
    ASSERT_TRUE( scheduler.active() );
    scheduler.cycle();
    ASSERT_EQ( m_state.iRunCount, 3 );
    /*ASSERT_FALSE( scheduler.active() );
    scheduler.cycle();
    ASSERT_EQ( m_state.iRunCount, 3 );*/
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct BasicSchedulerWaitActions
{
    mega::Pointer    ref2, ref3;
    std::vector< int > states;

    mega::ActionCoroutine test_1()
    {
        states.push_back( 0 );
        co_yield mega::wait( ref2 );
        states.push_back( 4 );
        co_return mega::sleep();
    }

    mega::ActionCoroutine test_2()
    {
        states.push_back( 1 );
        co_yield mega::wait( ref3 );
        states.push_back( 3 );
        co_return mega::sleep();
    }

    mega::ActionCoroutine test_3()
    {
        states.push_back( 2 );
        co_return mega::sleep();
    }

    mega::ActionCoroutine start( const mega::Pointer& ref )
    {
        switch( ref.getType().getObjectID() )
        {
            case 1:
                return test_1();
            case 2:
                return test_2();
            case 3:
                return test_3();
            default:
                std::abort();
        }
    }
};

using BasicSchedulerWait = BasicSchedulerTestFixture< BasicSchedulerWaitActions >;
TEST_F( BasicSchedulerWait, Wait )
{
    ExecutionState ex1 = make_test( 1 );
    ExecutionState ex2 = make_test( 2 );
    ExecutionState ex3 = make_test( 3 );
    m_state.ref2       = ex2.getRef();
    m_state.ref3       = ex3.getRef();

    scheduler.call( std::move( ex1 ) );
    scheduler.call( std::move( ex2 ) );
    scheduler.call( std::move( ex3 ) );
    ASSERT_EQ( scheduler.size(), 3U );
    scheduler.cycle();
   /* ASSERT_EQ( scheduler.size(), 0U );
    std::vector< int > expected_ = { 0, 1, 2, 3, 4 };
    ASSERT_EQ( m_state.states, expected_ );*/
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct SleepActions
{
    int                   iRunCount = 0;
    mega::ActionCoroutine test()
    {
        ++iRunCount;
        co_return mega::sleep();
    }
    mega::ActionCoroutine start( const mega::Pointer& ref )
    {
        switch( ref.getType().getObjectID() )
        {
            case 0:
                return test();
            default:
                std::abort();
        }
    }
};

using BasicSchedulerSleep = BasicSchedulerTestFixture< SleepActions >;
TEST_F( BasicSchedulerSleep, CallAndStop )
{
    ASSERT_EQ( m_state.iRunCount, 0 );
    ASSERT_FALSE( scheduler.active() );
    scheduler.cycle();
    ASSERT_FALSE( scheduler.active() );
    ASSERT_EQ( m_state.iRunCount, 0 );

    ExecutionState  ex1  = make_test( 0, 1 );
    ExecutionState  ex2  = make_test( 0, 2 );
    ExecutionState  ex3  = make_test( 0, 3 );
    mega::Pointer ref1 = ex1.getRef();
    mega::Pointer ref2 = ex2.getRef();
    mega::Pointer ref3 = ex3.getRef();
    scheduler.call( std::move( ex1 ) );
    scheduler.call( std::move( ex2 ) );
    scheduler.call( std::move( ex3 ) );
    ASSERT_EQ( scheduler.size(), 3U );
    ASSERT_EQ( m_state.iRunCount, 0 );
    ASSERT_TRUE( scheduler.active() );
    scheduler.cycle();
    ASSERT_EQ( m_state.iRunCount, 3 );
    ASSERT_EQ( scheduler.size(), 3U );
    scheduler.cycle();
    ASSERT_EQ( m_state.iRunCount, 6 );
    ASSERT_EQ( scheduler.size(), 3U );
    scheduler.cycle();
    ASSERT_EQ( m_state.iRunCount, 9 );
    ASSERT_EQ( scheduler.size(), 3U );
    scheduler.stop( ref1 );
    ASSERT_EQ( scheduler.size(), 2U );
    scheduler.cycle();
    ASSERT_EQ( m_state.iRunCount, 11 );
    scheduler.stop( ref2 );
    scheduler.stop( ref3 );
    ASSERT_EQ( scheduler.size(), 0U );
    scheduler.cycle();
    ASSERT_EQ( m_state.iRunCount, 11 );
}

#endif // GUARD_2023_August_01_scheduler_tests
