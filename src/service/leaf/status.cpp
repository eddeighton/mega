
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
        status.setMP( m_leaf.m_mp );
        std::ostringstream os;
        os << mega::network::Node::toStr( m_leaf.m_nodeType ) << ": " << m_leaf.getProcessName();
        status.setDescription( os.str() );
    }

    return status;
}

std::string LeafRequestConversation::Ping( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping from " << mega::network::Node::toStr( m_leaf.m_nodeType ) << ": " << m_leaf.getProcessName();
    return os.str();
}

} // namespace service
} // namespace mega
