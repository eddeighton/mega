

#include "basic_scheduler.hpp"

#include "common.hpp"
#include "coroutine.hpp"

#include <list>
#include <map>
#include <unordered_map>
#include <optional>
#include <stdexcept>

#ifndef ERR
#define LOG( msg )
#define ERR( msg )
#endif

namespace mega
{
class Scheduler::SchedulerImpl
{
public:
    virtual ~SchedulerImpl() {}

    virtual void allocated( const reference& ref, StopperFunctionPtr pStopper )                   = 0;
    virtual void call( const reference& ref, StopperFunctionPtr pStopper, ActionOperator action ) = 0;
    virtual void signal( const reference& ref )                                                   = 0;
    virtual void stop( const reference& ref )                                                     = 0;
    virtual void pause( const reference& ref )                                                    = 0;
    virtual void unpause( const reference& ref )                                                  = 0;
    virtual void stopperStopped( const reference& ref )                                           = 0;
    virtual bool active()                                                                         = 0;
    virtual void cycle()                                                                          = 0;
};
} // namespace mega

namespace
{

class BasicScheduler : public mega::Scheduler::SchedulerImpl
{
private:
    struct ActiveAction;

    struct ReferenceHash
    {
        inline mega::U64 operator()( const mega::reference& ref ) const
        {
            return *reinterpret_cast< const mega::U64* >( &ref );
        }
    };

    using ActiveActionMap  = std::unordered_map< mega::reference, ActiveAction*, BasicScheduler::ReferenceHash >;
    using ActiveActionList = std::list< ActiveActionMap::iterator >;

    using Timeout      = std::chrono::steady_clock::time_point;
    using TimeoutQueue = std::multimap< Timeout, ActiveActionMap::iterator >;

    using EventRefMap          = std::multimap< mega::reference, ActiveActionMap::iterator >;
    using EventRefMapIterArray = std::vector< EventRefMap::iterator >;
    using EventVector          = std::vector< mega::Event >;

    struct ActiveAction
    {
        mega::reference                     ref;
        mega::Scheduler::StopperFunctionPtr pStopper;
        mega::Scheduler::ActionOperator     op;
        mega::ActionCoroutine               coroutine;

        ActiveActionList::iterator iter_one, iter_two, iter_three;
        TimeoutQueue::iterator     iter_timeout;
        ActiveActionList::iterator iter_pause;

        // event handling
        EventRefMapIterArray iter_event_ref;
        bool                 bWaitAny;

        EventVector events;

        ActiveAction()                      = delete;
        ActiveAction( const ActiveAction& ) = delete;

        ActiveAction( const mega::reference&              _ref,
                      mega::Scheduler::StopperFunctionPtr _pStopper,
                      mega::Scheduler::ActionOperator&    _op,
                      ActiveActionList::iterator          _iter_one,
                      ActiveActionList::iterator          _iter_two,
                      ActiveActionList::iterator          _iter_three,
                      ActiveActionList::iterator          _iter_pause )
            : ref( _ref )
            , pStopper( _pStopper )
            , op( _op )
            , iter_one( _iter_one )
            , iter_two( _iter_two )
            , iter_three( _iter_three )
            , iter_pause( _iter_pause )
            , bWaitAny( true )
        {
        }
    };

    ActiveActionMap m_actions;

    ActiveActionList m_listOne, m_listTwo, m_listThree;
    TimeoutQueue     m_timeouts;
    ActiveActionList m_paused;

    EventRefMap m_events_by_ref_sleep;
    EventRefMap m_events_by_ref_wait;

    ActiveAction* m_pCurrentAction = nullptr;

    enum class SleepSwapState
    {
        eState_A_W_S, // active, wait, sleep
        eState_A_S_W,
        eState_W_A_S,
        eState_W_S_A,
        eState_S_A_W,
        eState_S_W_A
    };
    SleepSwapState m_sleepState = SleepSwapState::eState_A_W_S;
    
    ActiveActionList::iterator& getActiveIter( ActiveAction& action )
    {
        switch ( m_sleepState )
        {
            case SleepSwapState::eState_A_W_S:
                return action.iter_one;
            case SleepSwapState::eState_A_S_W:
                return action.iter_one;
            case SleepSwapState::eState_W_A_S:
                return action.iter_two;
            case SleepSwapState::eState_W_S_A:
                return action.iter_three;
            case SleepSwapState::eState_S_A_W:
                return action.iter_two;
            case SleepSwapState::eState_S_W_A:
                return action.iter_three;
            default: __builtin_unreachable();
        }
    }
    ActiveActionList::iterator& getWaitIter( ActiveAction& action )
    {
        switch ( m_sleepState )
        {
            case SleepSwapState::eState_A_W_S:
                return action.iter_two;
            case SleepSwapState::eState_A_S_W:
                return action.iter_three;
            case SleepSwapState::eState_W_A_S:
                return action.iter_one;
            case SleepSwapState::eState_W_S_A:
                return action.iter_one;
            case SleepSwapState::eState_S_A_W:
                return action.iter_three;
            case SleepSwapState::eState_S_W_A:
                return action.iter_two;
            default: __builtin_unreachable();
        }
    }
    ActiveActionList::iterator& getSleepIter( ActiveAction& action )
    {
        switch ( m_sleepState )
        {
            case SleepSwapState::eState_A_W_S:
                return action.iter_three;
            case SleepSwapState::eState_A_S_W:
                return action.iter_two;
            case SleepSwapState::eState_W_A_S:
                return action.iter_three;
            case SleepSwapState::eState_W_S_A:
                return action.iter_two;
            case SleepSwapState::eState_S_A_W:
                return action.iter_one;
            case SleepSwapState::eState_S_W_A:
                return action.iter_one;
            default: __builtin_unreachable();
        }
    }

    void swapActiveSleep()
    {
        switch ( m_sleepState )
        {
            case SleepSwapState::eState_A_W_S:
                m_sleepState = SleepSwapState::eState_S_W_A;
                break;
            case SleepSwapState::eState_A_S_W:
                m_sleepState = SleepSwapState::eState_S_A_W;
                break;
            case SleepSwapState::eState_W_A_S:
                m_sleepState = SleepSwapState::eState_W_S_A;
                break;
            case SleepSwapState::eState_W_S_A:
                m_sleepState = SleepSwapState::eState_W_A_S;
                break;
            case SleepSwapState::eState_S_A_W:
                m_sleepState = SleepSwapState::eState_A_S_W;
                break;
            case SleepSwapState::eState_S_W_A:
                m_sleepState = SleepSwapState::eState_A_W_S;
                break;
            default: __builtin_unreachable();
        }
    }

    void swapActiveWait()
    {
        switch ( m_sleepState )
        {
            case SleepSwapState::eState_A_W_S:
                m_sleepState = SleepSwapState::eState_W_A_S;
                break;
            case SleepSwapState::eState_A_S_W:
                m_sleepState = SleepSwapState::eState_W_S_A;
                break;
            case SleepSwapState::eState_W_A_S:
                m_sleepState = SleepSwapState::eState_A_W_S;
                break;
            case SleepSwapState::eState_W_S_A:
                m_sleepState = SleepSwapState::eState_A_S_W;
                break;
            case SleepSwapState::eState_S_A_W:
                m_sleepState = SleepSwapState::eState_S_W_A;
                break;
            case SleepSwapState::eState_S_W_A:
                m_sleepState = SleepSwapState::eState_S_A_W;
                break;
            default: __builtin_unreachable();
        }
    }

    ActiveActionList& getActive()
    {
        switch ( m_sleepState )
        {
            case SleepSwapState::eState_A_W_S:
                return m_listOne;
            case SleepSwapState::eState_A_S_W:
                return m_listOne;
            case SleepSwapState::eState_W_A_S:
                return m_listTwo;
            case SleepSwapState::eState_W_S_A:
                return m_listThree;
            case SleepSwapState::eState_S_A_W:
                return m_listTwo;
            case SleepSwapState::eState_S_W_A:
                return m_listThree;
            default: __builtin_unreachable();
        }
    }

    ActiveActionList& getWaiting()
    {
        switch ( m_sleepState )
        {
            case SleepSwapState::eState_A_W_S:
                return m_listTwo;
            case SleepSwapState::eState_A_S_W:
                return m_listThree;
            case SleepSwapState::eState_W_A_S:
                return m_listOne;
            case SleepSwapState::eState_W_S_A:
                return m_listOne;
            case SleepSwapState::eState_S_A_W:
                return m_listThree;
            case SleepSwapState::eState_S_W_A:
                return m_listTwo;
            default: __builtin_unreachable();
        }
    }

    ActiveActionList& getSleeping()
    {
        switch ( m_sleepState )
        {
            case SleepSwapState::eState_A_W_S:
                return m_listThree;
            case SleepSwapState::eState_A_S_W:
                return m_listTwo;
            case SleepSwapState::eState_W_A_S:
                return m_listThree;
            case SleepSwapState::eState_W_S_A:
                return m_listTwo;
            case SleepSwapState::eState_S_A_W:
                return m_listOne;
            case SleepSwapState::eState_S_W_A:
                return m_listOne;
            default: __builtin_unreachable();
        }
    }

    void active_insert( ActiveActionMap::iterator iterAction )
    {
        ActiveActionList& list               = getActive();
        getActiveIter( *iterAction->second ) = list.insert( list.end(), iterAction );
    }
    void active_remove( ActiveActionMap::iterator iterAction )
    {
        ActiveActionList&           list = getActive();
        ActiveActionList::iterator& i    = getActiveIter( *iterAction->second );
        if ( i != list.end() )
        {
            list.erase( i );
            i = list.end();
        }
    }

    void wait_insert( ActiveActionMap::iterator iterAction )
    {
        ActiveActionList& list             = getWaiting();
        getWaitIter( *iterAction->second ) = list.insert( list.end(), iterAction );
    }
    void wait_remove( ActiveActionMap::iterator iterAction )
    {
        ActiveActionList&           list = getWaiting();
        ActiveActionList::iterator& i    = getWaitIter( *iterAction->second );
        if ( i != list.end() )
        {
            list.erase( i );
            i = list.end();
        }
    }

    void sleep_insert( ActiveActionMap::iterator iterAction )
    {
        ActiveActionList& list              = getSleeping();
        getSleepIter( *iterAction->second ) = list.insert( list.end(), iterAction );
    }
    void sleep_remove( ActiveActionMap::iterator iterAction )
    {
        ActiveActionList&           list = getSleeping();
        ActiveActionList::iterator& i    = getSleepIter( *iterAction->second );
        if ( i != list.end() )
        {
            list.erase( i );
            i = list.end();
        }
    }

    void pause_insert( ActiveActionMap::iterator iterAction )
    {
        iterAction->second->iter_pause = m_paused.insert( m_paused.end(), iterAction );
    }
    void pause_remove( ActiveActionMap::iterator iterAction )
    {
        if ( iterAction->second->iter_pause != m_paused.end() )
        {
            m_paused.erase( iterAction->second->iter_pause );
            iterAction->second->iter_pause = m_paused.end();
        }
    }

    void timeout_insert( ActiveActionMap::iterator iterAction, const Timeout& timeout )
    {
        TimeoutQueue::iterator insertResult = m_timeouts.insert( std::make_pair( timeout, iterAction ) );
        iterAction->second->iter_timeout    = insertResult;
    }
    void timeout_remove( ActiveActionMap::iterator iterAction )
    {
        if ( iterAction->second->iter_timeout != m_timeouts.end() )
        {
            m_timeouts.erase( iterAction->second->iter_timeout );
            iterAction->second->iter_timeout = m_timeouts.end();
        }
    }

    void event_insert( EventRefMap& eventMap, ActiveActionMap::iterator iterAction, const Event& ev )
    {
        EventRefMap::iterator i = eventMap.insert( std::make_pair( ev.data, iterAction ) );
        iterAction->second->iter_event_ref.push_back( i );
    }

    void event_remove( EventRefMap& eventMap, ActiveActionMap::iterator iterAction )
    {
        for ( EventRefMap::iterator i : iterAction->second->iter_event_ref )
        {
            eventMap.erase( i );
        }
        iterAction->second->iter_event_ref.clear();
    }

    void on_event( EventRefMap& eventMap, const mega::reference& ref )
    {
        EventRefMap::iterator iLower = eventMap.lower_bound( ref );
        EventRefMap::iterator iUpper = eventMap.upper_bound( ref );

        for ( EventRefMap::iterator iterEvent = iLower; iterEvent != iUpper; ++iterEvent )
        {
            ActiveActionMap::iterator iter    = iterEvent->second;
            ActiveAction*             pAction = iter->second;

            for ( EventRefMapIterArray::iterator i    = pAction->iter_event_ref.begin(),
                                                 iEnd = pAction->iter_event_ref.end();
                  i != iEnd; )
            {
                if ( *i == iterEvent )
                {
                    i = pAction->iter_event_ref.erase( i );
                    pAction->events.push_back( ref );

                    if ( pAction->iter_event_ref.empty() || pAction->bWaitAny )
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
    virtual void allocated( const mega::reference& ref, mega::Scheduler::StopperFunctionPtr pStopper )
    {
        mega::Scheduler::ActionOperator actionOperator;
        ActiveAction*                   pAction = new ActiveAction(
                              ref, pStopper, actionOperator, m_listOne.end(), m_listTwo.end(), m_listThree.end(), m_paused.end() );
        auto insertResult = m_actions.insert( std::make_pair( ref, pAction ) );
        if ( !insertResult.second )
        {
            ERR( "Could not allocation action" );
            delete pAction;
        }
    }

    virtual void stopperStopped( const mega::reference& ref )
    {
        ActiveActionMap::iterator iFind = m_actions.find( ref );
        if ( iFind != m_actions.end() )
        {
            ActiveAction* pAction = iFind->second;

            on_event( m_events_by_ref_wait, pAction->ref );
            on_event( m_events_by_ref_sleep, pAction->ref );

            active_remove( iFind );
            wait_remove( iFind );
            sleep_remove( iFind );
            event_remove( m_events_by_ref_wait, iFind );
            event_remove( m_events_by_ref_sleep, iFind );
            m_actions.erase( iFind );

            // DO NOT invoke the stopper

            if ( m_pCurrentAction == pAction )
                m_pCurrentAction = nullptr;

            delete pAction;
        }
        else
        {
            // TODO - need to analyse how stopperStopped is called...
            // ERR( "Scheduler::stopperStopped with no active action for type: " << ref.type << " instance: " <<
            // ref.instance << " address: " << ref.address );
        }
    }

    virtual void signal( const mega::reference& ref )
    {
        on_event( m_events_by_ref_wait, ref );
        on_event( m_events_by_ref_sleep, ref );
    }

    virtual void call( const mega::reference&              ref,
                       mega::Scheduler::StopperFunctionPtr pStopper,
                       mega::Scheduler::ActionOperator     actionOperator )
    {
        ActiveAction* pAction = new ActiveAction(
            ref, pStopper, actionOperator, m_listOne.end(), m_listTwo.end(), m_listThree.end(), m_paused.end() );

        auto insertResult = m_actions.insert( std::make_pair( ref, pAction ) );
        if ( insertResult.second )
        {
            active_insert( insertResult.first );
            // LOG( "Scheduler::call type: " << ref.type << " instance: " << ref.instance << " address: " << ref.address
            // );
        }
        else
        {
            ERR( "Scheduler::call failed type: " << ref.type << " instance: " << ref.instance
                                                 << " address: " << ref.address );
            delete pAction;
        }
    }

    virtual void stop( const mega::reference& ref )
    {
        ActiveActionMap::iterator iFind = m_actions.find( ref );
        if ( iFind != m_actions.end() )
        {
            ActiveAction* pAction = iFind->second;

            on_event( m_events_by_ref_wait, pAction->ref );
            on_event( m_events_by_ref_sleep, pAction->ref );

            active_remove( iFind );
            wait_remove( iFind );
            sleep_remove( iFind );
            event_remove( m_events_by_ref_wait, iFind );
            event_remove( m_events_by_ref_sleep, iFind );
            m_actions.erase( iFind );

            // invoke the stopper - after removing
            pAction->pStopper( pAction->ref.instance );

            if ( m_pCurrentAction == pAction )
                m_pCurrentAction = nullptr;

            delete pAction;
        }
        else
        {
            ERR( "Stopped inactive reference" );
        }
    }

    virtual void pause( const mega::reference& ref )
    {
        ActiveActionMap::iterator iFind = m_actions.find( ref );
        if ( iFind != m_actions.end() )
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

    virtual void unpause( const mega::reference& ref )
    {
        ActiveActionMap::iterator iFind = m_actions.find( ref );
        if ( iFind != m_actions.end() )
        {
            pause_remove( iFind );
            active_insert( iFind );
        }
        else
        {
            ERR( "Stopped inactive reference" );
        }
    }

    virtual bool active() { return !m_actions.empty(); }

    // run a cycle
    virtual void cycle()
    {
        using namespace mega;

        // timeouts
        while ( !m_timeouts.empty() )
        {
            TimeoutQueue::iterator iterTimeout = m_timeouts.begin();
            if ( iterTimeout->first <= std::chrono::steady_clock::now() )
            {
                ActiveActionMap::iterator iter = iterTimeout->second;
                timeout_remove( iter );
                active_insert( iter );
            }
            else
            {
                break;
            }
        }

        if ( getActive().empty() && getWaiting().empty() )
        {
            swapActiveSleep();
        }

        // run until everything is sleeping
        while ( !( getActive().empty() && getWaiting().empty() ) )
        {
            if ( getActive().empty() )
            {
                swapActiveWait();
            }

            ActiveActionMap::iterator iter = getActive().front();
            active_remove( iter );

            // invoke the action
            m_pCurrentAction = iter->second;
            {
                if ( !m_pCurrentAction->coroutine.started() || m_pCurrentAction->coroutine.done() )
                {
                    m_pCurrentAction->coroutine = m_pCurrentAction->op();
                    m_pCurrentAction->coroutine.resume();
                }
                else
                {
                    m_pCurrentAction->coroutine.resume();
                }
            }

            // pAction may have been deleted
            if ( m_pCurrentAction )
            {
                const mega::ReturnReason& reason = m_pCurrentAction->coroutine.getReason();
                switch ( reason.reason )
                {
                    case eReason_Wait:
                        wait_insert( iter );
                        break;
                    case eReason_Wait_All:
                        for ( mega::Event ev : reason.events )
                            event_insert( m_events_by_ref_wait, iter, ev );
                        iter->second->bWaitAny = false;
                        break;
                    case eReason_Wait_Any:
                        for ( mega::Event ev : reason.events )
                            event_insert( m_events_by_ref_wait, iter, ev );
                        iter->second->bWaitAny = true;
                        break;
                    case eReason_Sleep:
                        sleep_insert( iter );
                        break;
                    case eReason_Sleep_All:
                        for ( mega::Event ev : reason.events )
                            event_insert( m_events_by_ref_sleep, iter, ev );
                        iter->second->bWaitAny = false;
                        break;
                    case eReason_Sleep_Any:
                        for ( mega::Event ev : reason.events )
                            event_insert( m_events_by_ref_sleep, iter, ev );
                        iter->second->bWaitAny = true;
                        break;
                    case eReason_Timeout:
                        timeout_insert( iter, reason.timeout.value() );
                        break;
                    case eReason_Terminated:
                        m_actions.erase( iter );
                        on_event( m_events_by_ref_wait, m_pCurrentAction->ref );
                        on_event( m_events_by_ref_sleep, m_pCurrentAction->ref );
                        m_pCurrentAction->pStopper( m_pCurrentAction->ref.instance );
                        delete m_pCurrentAction;
                        break;
                    default:
                        throw std::runtime_error( "Unknown return reason" );
                }
                m_pCurrentAction = nullptr;
            }
        }

        while ( !m_events_by_ref_wait.empty() )
        {
            EventRefMap::iterator i       = m_events_by_ref_wait.begin();
            mega::reference       ref     = i->first;
            ActiveAction*         pAction = i->second->second;
            // error
            ERR( "Never got event: " << ref.instance << " " << ref.type << " " << ref.address
                                     << " for action: " << pAction->ref.instance << " " << pAction->ref.type << " "
                                     << pAction->ref.address );
            stop( pAction->ref );
        }
    }
};

} // namespace

namespace mega
{

Scheduler::Scheduler()
    : m_pPimpl( std::make_shared< BasicScheduler >() )
{
}
void Scheduler::allocated( const reference& ref, StopperFunctionPtr pStopper ) { m_pPimpl->allocated( ref, pStopper ); }
void Scheduler::call( const reference& ref, StopperFunctionPtr pStopper, ActionOperator action )
{
    m_pPimpl->call( ref, pStopper, action );
}
void Scheduler::signal( const reference& ref ) { m_pPimpl->signal( ref ); }
void Scheduler::stop( const reference& ref ) { m_pPimpl->stop( ref ); }
void Scheduler::pause( const reference& ref ) { m_pPimpl->pause( ref ); }
void Scheduler::unpause( const reference& ref ) { m_pPimpl->unpause( ref ); }
void Scheduler::stopperStopped( const reference& ref ) { m_pPimpl->stopperStopped( ref ); }
bool Scheduler::active() { return m_pPimpl->active(); }
void Scheduler::cycle() { m_pPimpl->cycle(); }

} // namespace mega
