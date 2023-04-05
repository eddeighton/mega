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

#include "service/executor/executor.hpp"

#include "service/executor/request.hpp"
#include "service/executor/simulation.hpp"

#include "service/network/log.hpp"

namespace mega::service
{

// network::project::Impl
void ExecutorRequestConversation::SetProject( const Project& project, boost::asio::yield_context& yield_ctx )
{
    {
        std::vector< Simulation::Ptr > simulations;
        m_executor.getSimulations( simulations );
        for( Simulation::Ptr pSim : simulations )
        {
            network::project::Request_Sender rq( *this, pSim->getID(), *pSim, yield_ctx );
            rq.SetProject( project );
        }
    }

    if( network::ConversationBase* pClock = m_executor.getClock() )
    {

        // fire and forget to the plugin
        using namespace network::project;
        const network::ReceivedMsg rMsg{
            pClock->getConnectionID(), MSG_SetProject_Request::make( getID(), MSG_SetProject_Request{ project } ) };
        pClock->send( rMsg );
    }
}

} // namespace mega::service
