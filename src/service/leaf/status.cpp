
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
        std::ostringstream os;
        const std::size_t  szMachineID = m_leaf.m_mpo.getMachineID();
        const std::size_t  szProcessID = m_leaf.m_mpo.getProcessID();
        os << "LEAF: " << m_leaf.m_strProcessName << " " << szMachineID << "." << szProcessID;
        status.setDescription( os.str() );
    }

    return status;
}

} // namespace service
} // namespace mega
