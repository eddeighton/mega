
#include "request.hpp"

namespace mega
{
namespace service
{

DaemonRequestConversation::DaemonRequestConversation( Daemon&                        daemon,
                                                      const network::ConversationID& conversationID,
                                                      const network::ConnectionID&   originatingConnectionID )
    : InThreadConversation( daemon, conversationID, originatingConnectionID )
    , m_daemon( daemon )
{
}

network::Message DaemonRequestConversation::dispatchRequest( const network::Message&     msg,
                                                             boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::mpo::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::leaf_daemon::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::root_daemon::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::enrole::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::job::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "DaemonRequestConversation::dispatchRequest failed: " << network::getMsgName( msg ) );
}

void DaemonRequestConversation::dispatchResponse( const network::ConnectionID& connectionID,
                                                  const network::Message&      msg,
                                                  boost::asio::yield_context&  yield_ctx )
{
    if ( m_daemon.m_rootClient.getConnectionID() == connectionID )
    {
        m_daemon.m_rootClient.send( msg, yield_ctx );
    }
    else if ( network::Server::Connection::Ptr pLeafConnection = m_daemon.m_server.getConnection( connectionID ) )
    {
        pLeafConnection->send( msg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Daemon cannot resolve response connection: {}", connectionID );
    }
}

void DaemonRequestConversation::error( const network::ReceivedMsg& msg,
                                       const std::string&          strErrorMsg,
                                       boost::asio::yield_context& yield_ctx )
{
    if ( m_daemon.m_rootClient.getConnectionID() == msg.connectionID )
    {
        m_daemon.m_rootClient.sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else if ( network::Server::Connection::Ptr pLeafConnection = m_daemon.m_server.getConnection( msg.connectionID ) )
    {
        pLeafConnection->sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Daemon cannot resolve connection: {} on error: {}", msg.connectionID, strErrorMsg );
    }
}

network::Message DaemonRequestConversation::TermRoot( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::TermRoot" );
    return getRootSender( yield_ctx ).TermRoot( request );
}

network::Message DaemonRequestConversation::ExeRoot( const network::Message&     request,
                                                     boost::asio::yield_context& yield_ctx )
{
    return getRootSender( yield_ctx ).ExeRoot( request );
}

network::Message DaemonRequestConversation::ToolRoot( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    return getRootSender( yield_ctx ).ToolRoot( request );
}

network::Message DaemonRequestConversation::LeafRoot( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    return getRootSender( yield_ctx ).LeafRoot( request );
}

network::Message DaemonRequestConversation::LeafDaemon( const network::Message&     request,
                                                        boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message DaemonRequestConversation::ExeDaemon( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

// network::root_daemon::Impl
network::Message DaemonRequestConversation::RootLeafBroadcast( const network::Message&     request,
                                                               boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::RootLeafBroadcast" );
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for ( auto& [ id, pConnection ] : m_daemon.m_server.getConnections() )
        {
            network::daemon_leaf::Request_Sender sender( *this, *pConnection, yield_ctx );
            const network::Message               response = sender.RootLeafBroadcast( request );
            responses.push_back( response );
        }
    }

    network::Message aggregateRequest = request;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );
}

network::Message DaemonRequestConversation::RootExeBroadcast( const network::Message&     request,
                                                              boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::RootExeBroadcast" );
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for ( auto& [ id, pConnection ] : m_daemon.m_server.getConnections() )
        {
            if ( pConnection->getTypeOpt().value() == network::Node::Executor )
            {
                network::daemon_leaf::Request_Sender sender( *this, *pConnection, yield_ctx );
                const network::Message               response = sender.RootExeBroadcast( request );
                responses.push_back( response );
            }
        }
    }

    network::Message aggregateRequest = request;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );
}

network::Message DaemonRequestConversation::RootExe( const network::Message&     request,
                                                     boost::asio::yield_context& yield_ctx )
{
    auto stackCon = getOriginatingEndPointID();
    VERIFY_RTE( stackCon.has_value() );
    auto pConnection = m_daemon.m_server.getConnection( stackCon.value() );
    VERIFY_RTE( pConnection );
    VERIFY_RTE( pConnection->getTypeOpt().value() == network::Node::Executor );
    network::daemon_leaf::Request_Sender sender( *this, *pConnection, yield_ctx );
    return sender.RootExe( request );
}

void DaemonRequestConversation::RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::RootSimRun: {}", mpo );

    auto stackCon = getOriginatingEndPointID();
    VERIFY_RTE( stackCon.has_value() );
    auto pConnection = m_daemon.m_server.getConnection( stackCon.value() );
    VERIFY_RTE( pConnection );
    VERIFY_RTE( pConnection->getTypeOpt().value() == network::Node::Executor );

    {
        network::Server::MPOConnection       mpoConnection( m_daemon.m_server, mpo, pConnection );
        network::daemon_leaf::Request_Sender sender( *this, *pConnection, yield_ctx );
        sender.RootSimRun( mpo );
    }
}

// network::mpo::Impl
network::Message DaemonRequestConversation::MPRoot( const network::Message&     request,
                                                    const mega::MP&             mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender sender( *this, m_daemon.m_rootClient, yield_ctx );
    return sender.MPRoot( request, mp );
}

network::Message DaemonRequestConversation::MPDown( const network::Message&     request,
                                                    const mega::MP&             mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    if ( m_daemon.m_mp == mp )
    {
        return dispatchRequest( request, yield_ctx );
    }
    else
    {
        network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( mp );
        VERIFY_RTE_MSG( pConnection, "Failed to locate connection for mp: " << mp );
        network::mpo::Request_Sender sender( *this, *pConnection, yield_ctx );
        return sender.MPDown( request, mp );
    }
}

network::Message DaemonRequestConversation::MPUp( const network::Message&     request,
                                                  const mega::MP&             mp,
                                                  boost::asio::yield_context& yield_ctx )
{
    if ( m_daemon.m_mp == mp )
    {
        return dispatchRequest( request, yield_ctx );
    }
    else
    {
        if ( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( mp ) )
        {
            network::mpo::Request_Sender sender( *this, *pConnection, yield_ctx );
            return sender.MPDown( request, mp );
        }
        else
        {
            network::mpo::Request_Sender sender( *this, m_daemon.m_rootClient, yield_ctx );
            return sender.MPUp( request, mp );
        }
    }
}

network::Message DaemonRequestConversation::MPODown( const network::Message&     request,
                                                     const mega::MPO&            mpo,
                                                     boost::asio::yield_context& yield_ctx )
{
    THROW_RTE( "TODO" );
}
network::Message DaemonRequestConversation::MPOUp( const network::Message&     request,
                                                   const mega::MPO&            mpo,
                                                   boost::asio::yield_context& yield_ctx )
{
    THROW_RTE( "TODO" );
}

} // namespace service
} // namespace mega