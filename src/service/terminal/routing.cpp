
#include "request.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace service
{

TerminalRequestConversation::TerminalRequestConversation( Terminal&                      terminal,
                                                          const network::ConversationID& conversationID,
                                                          const network::ConnectionID&   originatingConnectionID )
    : InThreadConversation( terminal, conversationID, originatingConnectionID )
    , m_terminal( terminal )
{
}

network::Message TerminalRequestConversation::dispatchRequest( const network::Message&     msg,
                                                               boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::leaf_term::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "TerminalRequestConversation::dispatchRequest failed" );
}
void TerminalRequestConversation::dispatchResponse( const network::ConnectionID& connectionID,
                                                    const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    if ( m_terminal.getLeafSender().getConnectionID() == connectionID )
    {
        m_terminal.getLeafSender().send( getID(), msg, yield_ctx );
    }
    else if ( m_terminal.m_receiverChannel.getSender()->getConnectionID() == connectionID )
    {
        m_terminal.getLeafSender().send( getID(), msg, yield_ctx );
    }
    else
    {
        // This can happen when initiating request has received exception - in which case
        SPDLOG_ERROR( "Terminal cannot resolve connection: {} on error: {}", connectionID, msg );
    }
}

void TerminalRequestConversation::error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                                         boost::asio::yield_context& yield_ctx )
{
    if ( m_terminal.getLeafSender().getConnectionID() == connection )
    {
        m_terminal.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else if ( m_terminal.m_receiverChannel.getSender()->getConnectionID() == connection )
    {
        m_terminal.m_receiverChannel.getSender()->sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else
    {
        // This can happen when initiating request has received exception - in which case
        SPDLOG_ERROR( "Terminal cannot resolve connection: {} on error: {}", connection, strErrorMsg );
    }
}
} // namespace service
} // namespace mega