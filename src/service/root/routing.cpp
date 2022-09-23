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
    THROW_RTE( "RootRequestConversation::dispatchRequest failed" );
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
    SPDLOG_TRACE( "RootRequestConversation::TermRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestConversation::ExeRoot( const network::Message&     request,
                                                   boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::ExeRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestConversation::ToolRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::ToolRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestConversation::LeafRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::LeafRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestConversation::DaemonRoot( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::DaemonRoot" );
    return dispatchRequest( request, yield_ctx );
}


// network::root_daemon::Impl
network::Message RootRequestConversation::RootLeafBroadcast( const network::Message& request, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::RootLeafBroadcast" );
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
        {
            network::root_daemon::Request_Sender sender( *this, *pConnection, yield_ctx );
            const network::Message response = sender.RootLeafBroadcast( request );
            responses.push_back( response );
        }
    }

    network::Message aggregateRequest = request;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );
}



} // namespace service
} // namespace mega