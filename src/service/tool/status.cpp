
#include "request.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
network::Status ToolRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                    boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "ToolRequestConversation::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::ConversationID > conversations;
        /*for ( const auto& [ id, pCon ] : m_tool.m_conversations )
        {
            conversations.push_back( id );
        }*/
        status.setConversationID( conversations );
        // status.setMP( m_tool.m_mp );
        status.setDescription( m_tool.m_strProcessName );
    }

    return status;
}

std::string ToolRequestConversation::Ping( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping from Tool: " << m_tool.m_strProcessName;
    return os.str();
}

} // namespace service
} // namespace mega
