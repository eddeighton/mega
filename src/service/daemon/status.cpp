
#include "request.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
network::Status DaemonRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                   boost::asio::yield_context&       yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::GetStatus" );
    
    network::Status status{ childNodeStatus };
    {
        std::ostringstream os;
        const std::size_t  szMachineID = m_daemon.m_mpo.getMachineID();
        os << "DAEMON: " << m_daemon.m_strProcessName << " " << szMachineID;
        status.setDescription( os.str() );
    }

    return status;
}

} // namespace service
} // namespace mega
