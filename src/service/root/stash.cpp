#include "request.hpp"

#include "root.hpp"

namespace mega
{
namespace service
{

// network::stash::Impl
void RootRequestConversation::StashClear( boost::asio::yield_context& yield_ctx ) { m_root.m_stash.clear(); }

void RootRequestConversation::StashStash( const boost::filesystem::path& filePath,
                                          const task::DeterminantHash&   determinant,
                                          boost::asio::yield_context&    yield_ctx )
{
    m_root.m_stash.stash( filePath, determinant );
}

bool RootRequestConversation::StashRestore( const boost::filesystem::path& filePath,
                                            const task::DeterminantHash&   determinant,
                                            boost::asio::yield_context&    yield_ctx )
{
    return m_root.m_stash.restore( filePath, determinant );
}

task::FileHash RootRequestConversation::BuildGetHashCode( const boost::filesystem::path& filePath,
                                                          boost::asio::yield_context&    yield_ctx )
{
    return m_root.m_buildHashCodes.get( filePath );
}

void RootRequestConversation::BuildSetHashCode( const boost::filesystem::path& filePath,
                                                const task::FileHash&          hashCode,
                                                boost::asio::yield_context&    yield_ctx )
{
    m_root.m_buildHashCodes.set( filePath, hashCode );
}

} // namespace service
} // namespace mega
