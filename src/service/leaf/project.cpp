
#include "request.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
std::string LeafRequestConversation::GetVersion( const std::vector< std::string >& version,
                                                 boost::asio::yield_context&       yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::GetVersion" );
    std::ostringstream os;
    const std::size_t  szMachineID = m_leaf.m_mpo.getMachineID();
    const std::size_t  szProcessID = m_leaf.m_mpo.getProcessID();
    os << "        LEAF: " << m_leaf.m_strProcessName << " " << szMachineID << "." << szProcessID
       << "\n";
    return os.str();
}

} // namespace service
} // namespace mega
