
#include "request.hpp"

namespace mega
{
namespace service
{

// network::memory::Impl
std::string DaemonRequestConversation::AcquireSharedMemory( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    return m_daemon.m_sharedMemoryManager.acquire( mpo );
}
void DaemonRequestConversation::ReleaseSharedMemory( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    return m_daemon.m_sharedMemoryManager.release( mpo );
}

} // namespace service
} // namespace mega