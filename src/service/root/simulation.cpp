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

struct Deferred
{
    MPOManager&      mpoManager;
    const mega::MPO& mpo;
    Deferred( MPOManager& mpoManager, const mega::MPO& mpo )
        : mpoManager( mpoManager )
        , mpo( mpo )
    {
    }
    ~Deferred() { mpoManager.leafDisconnected( mpo ); }
};

void RootSimulation::SimStart( boost::asio::yield_context& yield_ctx )
{
    m_mpo = m_root.m_mpoManager.newOwner( m_mpo, getID() );
    Deferred defered( m_root.m_mpoManager, m_mpo );

    auto stackCon = getOriginatingEndPointID();
    VERIFY_RTE( stackCon.has_value() );
    auto pConnection = m_root.m_server.getConnection( stackCon.value() );
    VERIFY_RTE( pConnection );

    // simulation runs entirely on the stack in this scope!
    {
        network::Server::MPOMapping          mpoMapping( m_root.m_server, m_mpo, pConnection );
        network::root_daemon::Request_Sender sender( *this, *pConnection, yield_ctx );
        sender.RootSimRun( m_mpo );
    }
}

} // namespace service
} // namespace mega
