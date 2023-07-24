
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

#ifndef GUARD_2023_March_21_state_machine
#define GUARD_2023_March_21_state_machine

#include "downstream.hpp"

#include "service/protocol/model/sim.hxx"

#include "service/protocol/common/logical_thread_base.hpp"

#include <optional>
#include <unordered_map>

namespace mega::service
{

template < typename Sender >
class PluginStateMachine
{
    struct State
    {
        enum Type
        {
            eWaitingForClock,
            eWaitingForDownstream,
            eWaitingForUpstream,
            TOTAL_STATES
        };
        Type                       m_type;
        network::LogicalThreadBase* m_pSender;
        Downstream                 m_downstream;
    };
    using MPOTable = std::unordered_map< MPO, State, MPO::Hash >;

public:
    PluginStateMachine( Sender& sender )
        : m_sender( sender )
    {
    }

    Downstream* getDownstream()
    {
        bool bRemaining = true;
        while( bRemaining )
        {
            bRemaining = false;
            for( auto& [ mpo, state ] : m_sims )
            {
                switch( state.m_type )
                {
                    case State::eWaitingForDownstream:
                    {
                        // return it now
                        state.m_type = State::eWaitingForUpstream;
                        return &state.m_downstream;
                    }
                    break;
                    case State::eWaitingForClock:
                    {
                        bRemaining = true;
                    }
                    break;
                    case State::eWaitingForUpstream:
                    case State::TOTAL_STATES:
                        break;
                }
            }

            // invoke blocking wait
            if( bRemaining )
            {
                m_sender.runOne();
            }
        }
        return nullptr;
    }

    void sendUpstream( const network::ClockTick& clockTick )
    {
        for( auto& [ mpo, state ] : m_sims )
        {
            ASSERT( state.m_type == State::eWaitingForUpstream );
            THROW_TODO;
            // m_sender.send( *state.m_pSender, network::sim::MSG_SimClock_Response{ clockTick } );
            state.m_type = State::eWaitingForClock;
        }
    }

    void simRegister( const network::sim::MSG_SimRegister_Request& msg )
    {
        // start in the eWaitingForDownstream state such that
        // sim will send register initially and wait for clock response
        // then it will start sending clock request
        m_sims.insert( { msg.senderRef.m_mpo,
                         { State::eWaitingForDownstream, msg.senderRef.m_pSender,
                           Downstream{ msg.senderRef.m_mpo, log::Range{}, msg.senderRef.m_allocators } } } );
    }

    void simUnregister( const network::sim::MSG_SimUnregister_Request& msg ) { m_sims.erase( msg.mpo ); }

    void simClock( const network::sim::MSG_SimClock_Request& msg )
    {
        auto iFind = m_sims.find( msg.mpo );
        ASSERT( iFind != m_sims.end() );
        State& state = iFind->second;

        state.m_downstream.m_range = msg.range;
        state.m_type               = State::eWaitingForDownstream;
    }

private:
    Sender&  m_sender;
    MPOTable m_sims;
};
} // namespace mega::service

#endif // GUARD_2023_March_21_state_machine
