
#include "request.hpp"

namespace mega
{
namespace service
{

// network::project::Impl
std::string LeafRequestConversation::GetVersion( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Hello from root: " << m_leaf.m_strProcessName << "\n";
    {
        /*network::leaf_exe::Request_Sender sender( *this, m_leaf.getNodeChannelSender(), yield_ctx );
        network::project::Request_Encoder encoder( [ &sender ]( const network::Message& msg )
            {
                return sender.RootAll( msg );
            });

        os << encoder.GetVersion() << "\n";*/
    }
    return os.str();
}

} // namespace service
} // namespace mega