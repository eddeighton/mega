
#include "request.hpp"

#include "service/network/log.hpp"

#include "service/executor.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
network::Status ExecutorRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                        boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "ExecutorRequestConversation::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::ConversationID > conversations;
        /*for ( const auto& [ id, pCon ] : m_executor.m_conversations )
        {
            conversations.push_back( id );
        }*/
        status.setConversationID( conversations );
        // status.setMP( m_executor.m_mp );
        //status.setDescription( m_executor.m_strProcessName );
    }

    return status;
}

std::string ExecutorRequestConversation::Ping( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping from Executor";
    return os.str();
}

} // namespace service
} // namespace mega
