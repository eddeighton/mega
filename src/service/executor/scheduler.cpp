
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

#include "service/executor/scheduler.hpp"



#include <iostream>

namespace mega::service
{

namespace
{
} // namespace
/*
Scheduler::Scheduler( log::FileStorage& log )
    : m_log( log )
    , m_schedulingIter( m_log.begin< log::Scheduling::Read >() )
{
}
*/
/*
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
                Pointer             ref      = read.getRef();
                const ActionFunction& function = getActionFunction( ref.getType() );
                m_activations.insert( { ref, Activation( ref, function ) } );
            }
            break;
            case log::Scheduling::eSignal:
            {
                //
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
*/
} // namespace mega::service
