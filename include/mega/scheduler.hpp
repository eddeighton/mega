
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

#ifndef GUARD_2023_August_01_BasicScheduler
#define GUARD_2023_August_01_BasicScheduler

#include "mega/values/runtime/reference.hpp"
#include "mega/return_reason.hpp"

#include "common/unreachable.hpp"

#include <list>
#include <map>
#include <unordered_map>
#include <optional>
#include <chrono>

#ifndef ERR
#define ERR( msg )
#endif

namespace mega
{

template < typename ExecutionState >
class BasicScheduler
{
private:
    struct ActiveAction;

    using ActiveActionMap      = std::unordered_map< reference, ActiveAction*, reference::HashHeap >;
    using ActiveActionMapIter  = typename ActiveActionMap::iterator;
    using ActiveActionList     = std::list< typename ActiveActionMap::iterator >;
    using ActiveActionIter     = typename ActiveActionList::iterator;
    using Timeout              = std::chrono::steady_clock::time_point;
    using TimeoutQueue         = std::multimap< Timeout, ActiveActionMapIter >;
    using EventRefMap          = std::multimap< reference, ActiveActionMapIter >;
    using EventRefMapIterArray = std::vector< typename EventRefMap::iterator >;

    enum SleepSwapState
    {
        eState_A_W_S, // active, wait, sleep
        eState_A_S_W,
        eState_W_A_S,
        eState_W_S_A,
        eState_S_A_W,
        eState_S_W_A
    };

    class ActiveAction
    {
        ExecutionState                  m_executionState;
        ActiveActionIter                iter_one, iter_two, iter_three;
        ActiveActionIter                iter_pause;
        bool                            bWaitAny;
        typename TimeoutQueue::iterator iter_timeout;
        EventRefMapIterArray            iter_event_ref;

    public:
        ActiveAction()                           = delete;
        ActiveAction( const ActiveAction& )      = delete;
        ActiveAction& operator=( ActiveAction& ) = delete;

        ActiveAction( ExecutionState&& _executionState,
                      ActiveActionIter _iter_one,
                      ActiveActionIter _iter_two,
                      ActiveActionIter _iter_three,
                      ActiveActionIter _iter_pause )
            : m_executionState( std::move( _executionState ) )
            , iter_one( _iter_one )
            , iter_two( _iter_two )
            , iter_three( _iter_three )
            , iter_pause( _iter_pause )
            , bWaitAny( true )

        {
        }

        // execution state interaction
        void                stop() { m_executionState.stop(); }
        void                run() { m_executionState.execute(); }
        const ReturnReason& getReturnReason() const { return m_executionState.getReturnReason(); }
        void                onEvent( const reference& event ) { m_executionState.onEvent( event ); }

        inline const reference& getRef() const { return m_executionState.getRef(); }
        bool                    isWaitAny() const { return bWaitAny; }
        void                    setWaitAny( bool _bWaitAny ) { bWaitAny = _bWaitAny; }

        inline ActiveActionIter& getActiveIter( SleepSwapState state )
        {
            switch( state )
            {
                case eState_A_W_S:
                    return iter_one;
                case eState_A_S_W:
                    return iter_one;
                case eState_W_A_S:
                    return iter_two;
                case eState_W_S_A:
                    return iter_three;
                case eState_S_A_W:
                    return iter_two;
                case eState_S_W_A:
                    return iter_three;
                default:
                    UNREACHABLE;
                    break;
            }
        }
        inline ActiveActionIter& getWaitIter( SleepSwapState state )
        {
            switch( state )
            {
                case eState_A_W_S:
                    return iter_two;
                case eState_A_S_W:
                    return iter_three;
                case eState_W_A_S:
                    return iter_one;
                case eState_W_S_A:
                    return iter_one;
                case eState_S_A_W:
                    return iter_three;
                case eState_S_W_A:
                    return iter_two;
                default:
                    UNREACHABLE;
                    break;
            }
        }
        inline ActiveActionIter& getSleepIter( SleepSwapState state )
        {
            switch( state )
            {
                case eState_A_W_S:
                    return iter_three;
                case eState_A_S_W:
                    return iter_two;
                case eState_W_A_S:
                    return iter_three;
                case eState_W_S_A:
                    return iter_two;
                case eState_S_A_W:
                    return iter_one;
                case eState_S_W_A:
                    return iter_one;
                default:
                    UNREACHABLE;
                    break;
            }
        }

        inline ActiveActionIter&                getPauseIter() { return iter_pause; }
        inline typename TimeoutQueue::iterator& getTimeoutIter() { return iter_timeout; }
        inline EventRefMapIterArray&            getEventIterArray() { return iter_event_ref; }
    };

    ActiveActionMap  m_actions;
    ActiveActionList m_listOne, m_listTwo, m_listThree;
    TimeoutQueue     m_timeouts;
    ActiveActionList m_paused;
    EventRefMap      m_events_by_ref_sleep;
    EventRefMap      m_events_by_ref_wait;
    ActiveAction*    m_pCurrentAction = nullptr;
    SleepSwapState   m_sleepState     = eState_A_W_S;

    void swapActiveSleep()
    {
        switch( m_sleepState )
        {
            case eState_A_W_S:
                m_sleepState = eState_S_W_A;
                break;
            case eState_A_S_W:
                m_sleepState = eState_S_A_W;
                break;
            case eState_W_A_S:
                m_sleepState = eState_W_S_A;
                break;
            case eState_W_S_A:
                m_sleepState = eState_W_A_S;
                break;
            case eState_S_A_W:
                m_sleepState = eState_A_S_W;
                break;
            case eState_S_W_A:
                m_sleepState = eState_A_W_S;
                break;
            default:
                UNREACHABLE;
                break;
        }
    }

    void swapActiveWait()
    {
        switch( m_sleepState )
        {
            case eState_A_W_S:
                m_sleepState = eState_W_A_S;
                break;
            case eState_A_S_W:
                m_sleepState = eState_W_S_A;
                break;
            case eState_W_A_S:
                m_sleepState = eState_A_W_S;
                break;
            case eState_W_S_A:
                m_sleepState = eState_A_S_W;
                break;
            case eState_S_A_W:
                m_sleepState = eState_S_W_A;
                break;
            case eState_S_W_A:
                m_sleepState = eState_S_A_W;
                break;
            default:
                UNREACHABLE;
                break;
        }
    }

    ActiveActionList& getActive()
    {
        switch( m_sleepState )
        {
            case eState_A_W_S:
                return m_listOne;
            case eState_A_S_W:
                return m_listOne;
            case eState_W_A_S:
                return m_listTwo;
            case eState_W_S_A:
                return m_listThree;
            case eState_S_A_W:
                return m_listTwo;
            case eState_S_W_A:
                return m_listThree;
            default:
                UNREACHABLE;
                break;
        }
    }

    ActiveActionList& getWaiting()
    {
        switch( m_sleepState )
        {
            case eState_A_W_S:
                return m_listTwo;
            case eState_A_S_W:
                return m_listThree;
            case eState_W_A_S:
                return m_listOne;
            case eState_W_S_A:
                return m_listOne;
            case eState_S_A_W:
                return m_listThree;
            case eState_S_W_A:
                return m_listTwo;
            default:
                UNREACHABLE;
                break;
        }
    }

    ActiveActionList& getSleeping()
    {
        switch( m_sleepState )
        {
            case eState_A_W_S:
                return m_listThree;
            case eState_A_S_W:
                return m_listTwo;
            case eState_W_A_S:
                return m_listThree;
            case eState_W_S_A:
                return m_listTwo;
            case eState_S_A_W:
                return m_listOne;
            case eState_S_W_A:
                return m_listOne;
            default:
                UNREACHABLE;
                break;
        }
    }

    void active_insert( ActiveActionMapIter iterAction )
    {
        ActiveActionList& list                            = getActive();
        iterAction->second->getActiveIter( m_sleepState ) = list.insert( list.end(), iterAction );
    }

    void active_remove( ActiveActionMapIter iterAction )
    {
        ActiveActionList& list = getActive();
        ActiveActionIter& i    = iterAction->second->getActiveIter( m_sleepState );
        if( i != list.end() )
        {
            list.erase( i );
            i = list.end();
        }
    }

    void wait_insert( ActiveActionMapIter iterAction )
    {
        ActiveActionList& list                          = getWaiting();
        iterAction->second->getWaitIter( m_sleepState ) = list.insert( list.end(), iterAction );
    }
    void wait_remove( ActiveActionMapIter iterAction )
    {
        ActiveActionList& list = getWaiting();
        ActiveActionIter& i    = iterAction->second->getWaitIter( m_sleepState );
        if( i != list.end() )
        {
            list.erase( i );
            i = list.end();
        }
    }

    void sleep_insert( ActiveActionMapIter iterAction )
    {
        ActiveActionList& list                           = getSleeping();
        iterAction->second->getSleepIter( m_sleepState ) = list.insert( list.end(), iterAction );
    }
    void sleep_remove( ActiveActionMapIter iterAction )
    {
        ActiveActionList& list = getSleeping();
        ActiveActionIter& i    = iterAction->second->getSleepIter( m_sleepState );
        if( i != list.end() )
        {
            list.erase( i );
            i = list.end();
        }
    }

    void pause_insert( ActiveActionMapIter iterAction )
    {
        iterAction->second->getPauseIter() = m_paused.insert( m_paused.end(), iterAction );
    }
    void pause_remove( ActiveActionMapIter iterAction )
    {
        if( iterAction->second->getPauseIter() != m_paused.end() )
        {
            m_paused.erase( iterAction->second->getPauseIter() );
            iterAction->second->getPauseIter() = m_paused.end();
        }
    }

    void timeout_insert( ActiveActionMapIter iterAction, const Timeout& timeout )
    {
        typename TimeoutQueue::iterator insertResult = m_timeouts.insert( std::make_pair( timeout, iterAction ) );
        iterAction->second->getTimeoutIter()         = insertResult;
    }
    void timeout_remove( ActiveActionMapIter iterAction )
    {
        if( iterAction->second->getTimeoutIter() != m_timeouts.end() )
        {
            m_timeouts.erase( iterAction->second->getTimeoutIter() );
            iterAction->second->getTimeoutIter() = m_timeouts.end();
        }
    }

    void event_insert( EventRefMap& eventMap, ActiveActionMapIter iterAction, const Event& ev )
    {
        typename EventRefMap::iterator i = eventMap.insert( std::make_pair( ev, iterAction ) );
        iterAction->second->getEventIterArray().push_back( i );
    }

    void event_remove( EventRefMap& eventMap, ActiveActionMapIter iterAction )
    {
        for( typename EventRefMap::iterator i : iterAction->second->getEventIterArray() )
        {
            eventMap.erase( i );
        }
        iterAction->second->getEventIterArray().clear();
    }

    void on_event( EventRefMap& eventMap, const reference& ref )
    {
        typename EventRefMap::iterator iLower = eventMap.lower_bound( ref );
        typename EventRefMap::iterator iUpper = eventMap.upper_bound( ref );

        for( typename EventRefMap::iterator iterEvent = iLower; iterEvent != iUpper; ++iterEvent )
        {
            ActiveActionMapIter iter    = iterEvent->second;
            ActiveAction*       pAction = iter->second;

            for( typename EventRefMapIterArray::iterator i    = pAction->getEventIterArray().begin(),
                                                         iEnd = pAction->getEventIterArray().end();
                 i != iEnd; )
            {
                if( *i == iterEvent )
                {
                    i = pAction->getEventIterArray().erase( i );
                    pAction->onEvent( ref );

                    if( pAction->getEventIterArray().empty() || pAction->isWaitAny() )
                    {
                        // activate the action
                        event_remove( eventMap, iter );
                        active_insert( iter );
                    }
                    break;
                }
                else
                {
                    ++i;
                }
            }
        }
        eventMap.erase( iLower, iUpper );
    }

public:
    ~BasicScheduler()
    {
        for( auto& [ _, pAction ] : m_actions )
        {
            delete pAction;
        }
    }

    void call( ExecutionState&& executionState )
    {
        const mega::reference ref = executionState.getRef();

        ActiveAction* pAction = new ActiveAction(
            std::move( executionState ), m_listOne.end(), m_listTwo.end(), m_listThree.end(), m_paused.end() );

        auto insertResult = m_actions.insert( std::make_pair( ref, pAction ) );
        if( insertResult.second )
        {
            active_insert( insertResult.first );
        }
        else
        {
            ERR( "Scheduler::call failed type: " << ref );
            delete pAction;
        }
    }

    void stop( const reference& ref )
    {
        ActiveActionMapIter iFind = m_actions.find( ref );
        if( iFind != m_actions.end() )
        {
            ActiveAction* pAction = iFind->second;

            on_event( m_events_by_ref_wait, pAction->getRef() );
            on_event( m_events_by_ref_sleep, pAction->getRef() );

            active_remove( iFind );
            wait_remove( iFind );
            sleep_remove( iFind );
            event_remove( m_events_by_ref_wait, iFind );
            event_remove( m_events_by_ref_sleep, iFind );
            m_actions.erase( iFind );

            // invoke the stopper - after removing
            pAction->stop();

            if( m_pCurrentAction == pAction )
                m_pCurrentAction = nullptr;

            delete pAction;
        }
        else
        {
            ERR( "Stopped inactive reference" );
        }
    }

    void signal( const reference& ref )
    {
        on_event( m_events_by_ref_wait, ref );
        on_event( m_events_by_ref_sleep, ref );
    }

    bool active() const { return !m_actions.empty(); }
    U64  size() const { return m_actions.size(); }

    // run a cycle
    void cycle()
    {
        // timeouts
        while( !m_timeouts.empty() )
        {
            typename TimeoutQueue::iterator iterTimeout = m_timeouts.begin();
            if( iterTimeout->first <= std::chrono::steady_clock::now() )
            {
                ActiveActionMapIter iter = iterTimeout->second;
                timeout_remove( iter );
                active_insert( iter );
            }
            else
            {
                break;
            }
        }

        if( getActive().empty() && getWaiting().empty() )
        {
            swapActiveSleep();
        }

        // run until everything is sleeping
        while( !( getActive().empty() && getWaiting().empty() ) )
        {
            if( getActive().empty() )
            {
                swapActiveWait();
            }

            ActiveActionMapIter iter = getActive().front();
            active_remove( iter );

            // invoke the action
            m_pCurrentAction = iter->second;

            m_pCurrentAction->run();

            // pAction may have been deleted
            if( m_pCurrentAction )
            {
                const ReturnReason& reason = m_pCurrentAction->getReturnReason();
                switch( reason.reason )
                {
                    case eReason_Wait:
                        wait_insert( iter );
                        break;
                    case eReason_Wait_All:
                        for( Event ev : reason.events )
                            event_insert( m_events_by_ref_wait, iter, ev );
                        iter->second->setWaitAny( false );
                        break;
                    case eReason_Wait_Any:
                        for( Event ev : reason.events )
                            event_insert( m_events_by_ref_wait, iter, ev );
                        iter->second->setWaitAny( true );
                        break;
                    case eReason_Sleep:
                        sleep_insert( iter );
                        break;
                    case eReason_Sleep_All:
                        for( Event ev : reason.events )
                            event_insert( m_events_by_ref_sleep, iter, ev );
                        iter->second->setWaitAny( false );
                        break;
                    case eReason_Sleep_Any:
                        for( Event ev : reason.events )
                            event_insert( m_events_by_ref_sleep, iter, ev );
                        iter->second->setWaitAny( true );
                        break;
                    case eReason_Timeout:
                        timeout_insert( iter, reason.timeout.value() );
                        break;
                    case eReason_Terminated:
                        m_actions.erase( iter );
                        on_event( m_events_by_ref_wait, m_pCurrentAction->getRef() );
                        on_event( m_events_by_ref_sleep, m_pCurrentAction->getRef() );
                        m_pCurrentAction->stop();
                        delete m_pCurrentAction;
                        break;
                    default:
                        ERR( "Unknown return reason" );
                        break;
                }
                m_pCurrentAction = nullptr;
            }
        }

        while( !m_events_by_ref_wait.empty() )
        {
            typename EventRefMap::iterator i       = m_events_by_ref_wait.begin();
            reference                      ref     = i->first;
            ActiveAction*                  pAction = i->second->second;
            // error
            ERR( "Never got event: " << ref << " for action: " << pAction->getRef() );
            stop( pAction->getRef() );
        }
    }

    /*void allocated( const reference& ref )
    {
        ActiveAction* pAction
            = new ActiveAction( ref, m_listOne.end(), m_listTwo.end(), m_listThree.end(), m_paused.end() );
        auto insertResult = m_actions.insert( std::make_pair( ref, pAction ) );
        if( !insertResult.second )
        {
            ERR( "Could not allocation action" );
            delete pAction;
        }
    }*/
    /*void stopperStopped( const reference& ref )
    {
        ActiveActionMapIter iFind = m_actions.find( ref );
        if( iFind != m_actions.end() )
        {
            ActiveAction* pAction = iFind->second;

            on_event( m_events_by_ref_wait, pAction->getRef() );
            on_event( m_events_by_ref_sleep, pAction->getRef() );

            active_remove( iFind );
            wait_remove( iFind );
            sleep_remove( iFind );
            event_remove( m_events_by_ref_wait, iFind );
            event_remove( m_events_by_ref_sleep, iFind );
            m_actions.erase( iFind );

            // DO NOT invoke the stopper

            if( m_pCurrentAction == pAction )
                m_pCurrentAction = nullptr;

            delete pAction;
        }
        else
        {
            // TODO - need to analyse how stopperStopped is called...
            // ERR( "Scheduler::stopperStopped with no active action for type: " << ref.type << " instance: " <<
            // ref.instance << " timestamp: " << ref.timestamp );
        }
    }*/
    /*void pause( const reference& ref )
    {
        ActiveActionMapIter iFind = m_actions.find( ref );
        if( iFind != m_actions.end() )
        {
            active_remove( iFind );
            wait_remove( iFind );
            sleep_remove( iFind );
            event_remove( m_events_by_ref_wait, iFind );
            event_remove( m_events_by_ref_sleep, iFind );
            pause_insert( iFind );
        }
        else
        {
            ERR( "Stopped inactive reference" );
        }
    }

    void unpause( const reference& ref )
    {
        ActiveActionMapIter iFind = m_actions.find( ref );
        if( iFind != m_actions.end() )
        {
            pause_remove( iFind );
            active_insert( iFind );
        }
        else
        {
            ERR( "Stopped inactive reference" );
        }
    }*/
};

} // namespace mega

#endif // GUARD_2023_August_01_BasicScheduler-1
