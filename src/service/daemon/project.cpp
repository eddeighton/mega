
#include "request.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
std::string DaemonRequestConversation::GetVersion( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Hello from daemon: " << m_daemon.m_strProcessName << "\n";
    {
        for ( auto& [ id, pConnection ] : m_daemon.m_leafServer.getConnections() )
        {
            network::daemon_leaf::Request_Sender sender( *this, *pConnection, yield_ctx );
            network::project::Request_Encoder    encoder( [ &sender ]( const network::Message& msg )
                                                       { return sender.RootAll( msg ); } );

            os << encoder.GetVersion() << "\n";
        }
    }
    return os.str();
}

} // namespace service
} // namespace mega