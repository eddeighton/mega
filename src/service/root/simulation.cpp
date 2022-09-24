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
                                mega::MPO                      leafMPO )
    : RootRequestConversation( root, conversationID, originatingConnectionID )
    , m_mpo( leafMPO )
{
}

network::Message RootSimulation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::sim::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return RootRequestConversation::dispatchRequest( msg, yield_ctx );
}

void RootSimulation::SimStart( boost::asio::yield_context& yield_ctx )
{
    m_mpo = m_root.m_mpoManager.newOwner( m_mpo, getID() );
    RootSimRun( m_mpo, yield_ctx );
}

} // namespace service
} // namespace mega
