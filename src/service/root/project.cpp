#include "request.hpp"

#include "root.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
std::string RootRequestConversation::GetVersion( boost::asio::yield_context& yield_ctx ) 
{ 
    std::ostringstream os;
    os << "Hello from root: " << m_root.m_strProcessName << "\n";
    {
        for( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
        {
            network::root_daemon::Request_Sender sender( *this, *pConnection, yield_ctx );
            network::project::Request_Encoder encoder( [ &sender ]( const network::Message& msg )
                {
                    return sender.RootAll( msg );
                });

            os << encoder.GetVersion() << "\n";
        }
    }
    return os.str(); 
}

}
}