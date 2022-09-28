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

#include "simulation.hpp"

#include "root.hpp"
#include "mpo_manager.hpp"

namespace mega
{
namespace service
{

RootSimulation::RootSimulation( Root&                          root,
                                const network::ConversationID& conversationID,
                                const network::ConnectionID&   originatingConnectionID,
                                mega::MP                       leafMP )
    : RootRequestConversation( root, conversationID, originatingConnectionID )
    , m_leafMP( leafMP )
{
}

network::Message RootSimulation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::sim::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return RootRequestConversation::dispatchRequest( msg, yield_ctx );
}

struct Deferred
{
    MPOManager&                          mpoManager;
    const mega::network::ConversationID& id;
    Deferred( MPOManager& mpoManager, const mega::network::ConversationID& id )
        : mpoManager( mpoManager )
        , id( id )
    {
    }
    ~Deferred() { mpoManager.release( id ); }
};

void RootSimulation::SimStart( boost::asio::yield_context& yield_ctx )
{
    mega::MPO simulationMPO = m_root.m_mpoManager.newOwner( m_leafMP, getID() );
    SPDLOG_TRACE( "RootSimulation::SimStart: {}", simulationMPO );

    Deferred defered( m_root.m_mpoManager, getID() );

    auto stackCon = getOriginatingEndPointID();
    VERIFY_RTE( stackCon.has_value() );
    auto pConnection = m_root.m_server.getConnection( stackCon.value() );
    VERIFY_RTE( pConnection );

    // simulation runs entirely on the stack in this scope!
    {
        network::Server::MPOConnection       mpoConnection( m_root.m_server, simulationMPO, pConnection );
        network::root_daemon::Request_Sender sender( *this, *pConnection, yield_ctx );
        SPDLOG_TRACE( "RootSimulation::SimStart: sending RootSimRun for {}", simulationMPO );
        sender.RootSimRun( simulationMPO );
    }
}

} // namespace service
} // namespace mega
