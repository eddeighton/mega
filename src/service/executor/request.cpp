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

#include "service/executor/request.hpp"
#include "service/executor/executor.hpp"
#include "service/executor/simulation.hpp"

#include "service/protocol/common/logical_thread_id.hpp"

#include "service/network/log.hpp"

#include "common/requireSemicolon.hpp"

namespace mega::service
{

MPO ExecutorRequestLogicalThread::SimCreate( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestLogicalThread::SimCreate {}", getID() );
    return m_executor.createSimulation( *this, yield_ctx );
}

network::Message ExecutorRequestLogicalThread::MPODown( const network::Message& request, const MPO& mpo,
                                                        boost::asio::yield_context& yield_ctx )
{
    using ::operator<<;
    if( Simulation::Ptr pSim = m_executor.getSimulation( mpo ) )
    {
        network::ReceivedMessage receivedMsg{ shared_from_this(), request };
        pSim->receive( receivedMsg );
        LogicalThreadBase::OutBoundRequestStack stack( shared_from_this() );
        return dispatchInBoundRequestsUntilResponse( yield_ctx );
    }
    THROW_RTE( "Failed to resolve simulation: " << mpo );
    return {}; // prevent warning
}

} // namespace mega::service
