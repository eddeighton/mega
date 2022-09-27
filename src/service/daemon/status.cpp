
#include "request.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
network::Status DaemonRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                      boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::ConversationID > conversations;
        for ( const auto& [ id, pCon ] : m_daemon.m_conversations )
        {
            conversations.push_back( id );
        }
        status.setConversationID( conversations );
        status.setMP( m_daemon.m_mp );
        status.setDescription( m_daemon.m_strProcessName );
    }

    return status;
}

std::string DaemonRequestConversation::Ping( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping from Daemon: " << m_daemon.getProcessName();
    return os.str();
}

} // namespace service
} // namespace mega
