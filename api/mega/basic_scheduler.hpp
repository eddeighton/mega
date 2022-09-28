
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
