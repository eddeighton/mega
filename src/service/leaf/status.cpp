
#include "request.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
network::Status LeafRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                    boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::GetVersion" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::ConversationID > conversations;
        for ( const auto& [ id, pCon ] : m_leaf.m_conversations )
        {
            conversations.push_back( id );
        }
        status.setConversationID( conversations );
        status.setMPO( m_leaf.m_mpo );
        status.setDescription( m_leaf.m_strProcessName );

    }

    return status;
}

} // namespace service
} // namespace mega
