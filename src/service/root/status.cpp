#include "request.hpp"

#include "root.hpp"

namespace mega
{
namespace service
{

// network::status::Impl
network::Status RootRequestConversation::GetNetworkStatus( boost::asio::yield_context& yield_ctx )
{
    return getLeafBroadcastRequest< network::status::Request_Encoder >( yield_ctx ).GetStatus( {} );
}

network::Status RootRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                    boost::asio::yield_context&           yield_ctx )
{
    //SPDLOG_TRACE( "RootRequestConversation::GetVersion" );
    network::Status status{ childNodeStatus };
    {
        std::vector< network::ConversationID > conversations;
        for ( const auto& [ id, pCon ] : m_root.m_conversations )
        {
            conversations.push_back( id );
        }
        status.setConversationID( conversations );
        status.setDescription( m_root.m_strProcessName );
    }

    return status;
}

} // namespace service
} // namespace mega
