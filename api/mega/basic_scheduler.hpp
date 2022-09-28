


#ifndef BASIC_SCHEDULER_12_SEPT_2022
#define BASIC_SCHEDULER_12_SEPT_2022

#include "common.hpp"
#include "coroutine.hpp"

#include <functional>
#include <memory>

namespace mega
{

////////////////////////////////////////////////////////////////////////////
// the scheduler interface
class Scheduler
{
public:
    typedef void ( *StopperFunctionPtr )( mega::Instance );
    using ActionOperator = std::function< mega::ActionCoroutine() >;

    Scheduler();

    void allocated( const reference& ref, StopperFunctionPtr pStopper );
    void call( const reference& ref, StopperFunctionPtr pStopper, ActionOperator action );
    void signal( const reference& ref );
    void stop( const reference& ref );
    void pause( const reference& ref );
    void unpause( const reference& ref );

    template < typename T, typename... Args >
    void call( const T& staticRef, StopperFunctionPtr pStopper, Args... args )
    {
        using namespace std::placeholders;
        call( staticRef.data, pStopper, std::bind( &T::operator(), staticRef, args... ) );
    }

    // stopper callback
    void stopperStopped( const reference& ref );

    // are there any active actions
    bool active();

    // run a cycle
    void cycle();

    class SchedulerImpl;

private:
    std::shared_ptr< SchedulerImpl > m_pPimpl;
};

} // namespace mega

#endif // BASIC_SCHEDULER_12_SEPT_2022
