
#include "request.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace service
{

ToolRequestConversation::ToolRequestConversation( Tool& tool, const network::ConversationID& conversationID,
                                                  const network::ConnectionID& originatingConnectionID )
    : InThreadConversation( tool, conversationID, originatingConnectionID )
    , m_tool( tool )
{
}

network::Message ToolRequestConversation::dispatchRequest( const network::Message&     msg,
                                                           boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::leaf_tool::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::mpo::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "ToolRequestConversation::dispatchRequest failed" );
}
void ToolRequestConversation::dispatchResponse( const network::ConnectionID& connectionID, const network::Message& msg,
                                                boost::asio::yield_context& yield_ctx )
{
    if ( m_tool.getLeafSender().getConnectionID() == connectionID )
    {
        m_tool.getLeafSender().send( msg, yield_ctx );
    }
    else if ( m_tool.m_receiverChannel.getSender()->getConnectionID() == connectionID )
    {
        m_tool.getLeafSender().send( msg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Terminal cannot resolve connection: {} on error: {}", connectionID, msg );
    }
}

void ToolRequestConversation::error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                     boost::asio::yield_context& yield_ctx )
{
    if ( m_tool.getLeafSender().getConnectionID() == msg.connectionID )
    {
        m_tool.getLeafSender().sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else if ( m_tool.m_receiverChannel.getSender()->getConnectionID() == msg.connectionID )
    {
        m_tool.m_receiverChannel.getSender()->sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        // This can happen when initiating request has received exception - in which case
        SPDLOG_ERROR( "Tool cannot resolve connection: {} on error: {}", msg.connectionID, strErrorMsg );
    }
}

network::tool_leaf::Request_Sender ToolRequestConversation::getToolRequest( boost::asio::yield_context& yield_ctx )
{
    return network::tool_leaf::Request_Sender( *this, m_tool.getLeafSender(), yield_ctx );
}

network::Message ToolRequestConversation::MPDown( const network::Message& request, const mega::MP& mp,
                                                  boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}
network::Message ToolRequestConversation::MPUp( const network::Message& request, const mega::MP& mp,
                                                boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender leaf( *this, m_tool.getLeafSender(), yield_ctx );
    return leaf.MPUp( request, mp );
}

network::Message ToolRequestConversation::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                   boost::asio::yield_context& yield_ctx )
{
    THROW_RTE( "TODO" );
}
network::Message ToolRequestConversation::MPOUp( const network::Message& request, const mega::MPO& mpo,
                                                 boost::asio::yield_context& yield_ctx )
{
    THROW_RTE( "TODO" );
}

} // namespace service
} // namespace mega