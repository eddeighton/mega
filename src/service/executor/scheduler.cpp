
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

#include "scheduler.hpp"

#include "mega/reference_io.hpp"

#include "service/protocol/common/context.hpp"

#include "jit/object_functions.hxx"

#include <iostream>

namespace mega::service
{

namespace
{
runtime::JITBase::ActionInfo getActionFPtr( TypeID typeID, void** pFunction )
{
    runtime::JITBase::ActionInfo actionInfo;
    mega::runtime::JITFunctor    functor(
        [ typeID, pFunction, &actionInfo = actionInfo ]( mega::runtime::JITBase& jit, void* )
        { jit.getActionFunction( typeID, pFunction, actionInfo ); } );
    // NOTE: call to jit MAY return with THIS coroutine resumed in DIFFERENT thread!
    mega::Context::get()->jit( functor );
    return actionInfo;
}
} // namespace

Scheduler::Scheduler( log::Storage& log )
    : m_log( log )
    , m_schedulingIter( m_log.begin< log::Scheduling::Read >() )
{
}

const Scheduler::ActionFunction& Scheduler::getActionFunction( TypeID typeID )
{
    auto iFind = m_actionFunctions.find( typeID );
    if( iFind != m_actionFunctions.end() )
    {
        if( iFind->second.functionPtr != nullptr )
        {
            return iFind->second;
        }
    }

    Scheduler::ActionFunction actionFunction;
    actionFunction.info = getActionFPtr( typeID, ( void** )&actionFunction.functionPtr );
    auto [ iter, _ ]    = m_actionFunctions.insert( { typeID, std::move( actionFunction ) } );
    return iter->second;
}

void Scheduler::cycle()
{
    const log::Offset& schedulingTrackEnd = m_log.getTrackEnd< log::Scheduling::Read >();

    for( ; m_schedulingIter.position() != schedulingTrackEnd; ++m_schedulingIter )
    {
        const log::Scheduling::Read read = *m_schedulingIter;

        switch( read.getType() )
        {
            case log::Scheduling::eStart:
            {
                reference             ref      = read.getRef();
                const ActionFunction& function = getActionFunction( ref.getType() );
                m_activations.insert( { ref, Activation( ref, function ) } );
            }
            break;
            case log::Scheduling::eStop:
            {
                m_activations.erase( read.getRef() );
            }
            break;
            default:
                THROW_RTE( "Unknown scheduling event type" );
                break;
        }

        std::cout << "Scheduler got scheduling event: " << log::Scheduling::toString( read.getType() ) << " : "
                  << read.getRef() << std::endl;
    }

    for( auto& [ _, activation ] : m_activations )
    {
        activation.run();
    }
}

} // namespace mega::service
