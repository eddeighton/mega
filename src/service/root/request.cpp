#include "request.hpp"

#include "root.hpp"

namespace mega
{
namespace service
{

RootRequestConversation::RootRequestConversation( Root&                          root,
                                                  const network::ConversationID& conversationID,
                                                  const network::ConnectionID&   originatingConnectionID )
    : InThreadConversation( root, conversationID, originatingConnectionID )
    , m_root( root )
{
}

network::Message RootRequestConversation::dispatchRequest( const network::Message&     msg,
                                                           boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::dispatchRequest {}", network::getMsgNameFromID( network::getMsgID( msg ) ) );
    network::Message result;
    if ( result = network::daemon_root::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::project::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::enrole::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return result;
}

void RootRequestConversation::dispatchResponse( const network::ConnectionID& connectionID,
                                                const network::Message&      msg,
                                                boost::asio::yield_context&  yield_ctx )
{
    if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( connectionID ) )
    {
        pHostConnection->send( getID(), msg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Root cannot resolve connection: {} on response: {}", connectionID, msg );
    }
}

void RootRequestConversation::error( const network::ConnectionID& connectionID,
                                     const std::string&           strErrorMsg,
                                     boost::asio::yield_context&  yield_ctx )
{
    if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( connectionID ) )
    {
        pHostConnection->sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Root cannot resolve connection: {} on error: {}", connectionID, strErrorMsg );
    }
}
/*
network::root_daemon::Request_Sender
RootRequestConversation::getDaemonRequest( network::Server::Connection::Ptr pConnection,
                                           boost::asio::yield_context&      yield_ctx )
{
    return network::root_daemon::Request_Sender( *this, *pConnection, yield_ctx );
}*/

// network::daemon_root::Impl
network::Message RootRequestConversation::TermRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestConversation::ExeRoot( const network::Message&     request,
                                                   boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestConversation::ToolRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestConversation::LeafRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestConversation::DaemonRoot( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

// network::project::Impl
std::string RootRequestConversation::GetVersion( boost::asio::yield_context& yield_ctx ) { return "Ed was here"; }

// network::enrole::Impl
MPO RootRequestConversation::EnroleDaemon( boost::asio::yield_context& yield_ctx )
{
    const mega::MPO                  mpo         = m_root.m_mpoManager.newDaemon();
    network::Server::Connection::Ptr pConnection = m_root.m_server.getConnection( getOriginatingEndPointID().value() );
    pConnection->setMPO( mpo );
    pConnection->setDisconnectCallback( [ mpo, &root = m_root ]( const network::ConnectionID& connectionID )
                                        { root.onDaemonDisconnect( connectionID, mpo ); } );
    return mpo;
}

MPO RootRequestConversation::EnroleLeafWithRoot( const MPO& daemonMPO, boost::asio::yield_context& yield_ctx )
{
    return m_root.m_mpoManager.newLeaf( daemonMPO );
}

void RootRequestConversation::EnroleLeafDisconnect( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "EnroleLeafDisconnect {}", mpo );
    m_root.m_mpoManager.leafDisconnected( mpo );
}
} // namespace service
} // namespace mega