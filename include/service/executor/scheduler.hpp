
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

#ifndef GUARD_2023_February_04_scheduler
#define GUARD_2023_February_04_scheduler

#include "jit/functions.hpp"

#include "service/protocol/common/jit_base.hpp"

#include "log/file_log.hpp"

#include "mega/coroutine.hpp"

#include <unordered_map>

namespace mega::service
{

class Scheduler
{
    struct ActionFunction
    {
        using FunctionType = mega::ActionCoroutine ( * )( mega::reference* );
        FunctionType                 functionPtr;
        runtime::JITBase::ActionInfo info;
    };
    using ActionFunctionMap = std::unordered_map< TypeID, ActionFunction, TypeID::Hash >;

    struct Activation
    {
        inline Activation( const mega::reference& ref, const ActionFunction& function )
            : m_ref( ref )
            , m_function( function )
            , m_routine( m_function.functionPtr( &m_ref ) )
        {
        }
        inline void run()
        {
            if( m_routine.done() )
            {
                m_routine = m_function.functionPtr( &m_ref );
            }
            m_routine.resume();
        }
        mega::reference       m_ref;
        const ActionFunction& m_function;
        mega::ActionCoroutine m_routine;
    };
    using ActivationMap = std::unordered_map< mega::reference, Activation, mega::reference::Hash >;

public:
    Scheduler( log::FileStorage& log );

    void cycle();

private:
    const ActionFunction& getActionFunction( TypeID concreteTypeID );

private:
    log::FileStorage&                          m_log;
    log::FileIterator< log::Scheduling::Read > m_schedulingIter;

    ActionFunctionMap m_actionFunctions;
    ActivationMap     m_activations;
};

} // namespace mega::service

#endif // GUARD_2023_February_04_scheduler
