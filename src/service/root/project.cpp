#include "request.hpp"

#include "root.hpp"

namespace mega
{
namespace service
{

// network::project::Impl

network::Project RootRequestConversation::GetProject( boost::asio::yield_context& yield_ctx )
{
    return m_root.getProject();
}

void RootRequestConversation::SetProject( const network::Project& project, boost::asio::yield_context& yield_ctx )
{
    m_root.setProject( project );
}

} // namespace service
} // namespace mega
