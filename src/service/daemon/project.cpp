
#include "request.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
std::string DaemonRequestConversation::GetVersion( const std::vector< std::string >& version,
                                                   boost::asio::yield_context&       yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::GetVersion" );
    std::ostringstream os;
    const std::size_t  szMachineID = m_daemon.m_mpo.getMachineID();
    os << "    DAEMON: " << m_daemon.m_strProcessName << " " << szMachineID << "\n";
    for ( const std::string& str : version )
    {
        os << str << "\n";
    }
    return os.str();
}

} // namespace service
} // namespace mega
