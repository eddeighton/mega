#include "request.hpp"

#include "root.hpp"

namespace mega
{
namespace service
{

// network::project::Impl

std::string RootRequestConversation::CollateVersions( boost::asio::yield_context& yield_ctx )
{
    RootRequestConversation*          pThis = this;
    network::project::Request_Encoder project( [ pThis, &yield_ctx ]( const network::Message& msg )
                                               { return pThis->RootLeafBroadcast( msg, yield_ctx ); } );
    return project.GetVersion( {} );
}

std::string RootRequestConversation::GetVersion( const std::vector< std::string >& version,
                                                 boost::asio::yield_context&       yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::GetVersion" );
    std::ostringstream os;
    os << "ROOT: " << m_root.m_strProcessName << " " << "\n";
    for ( const std::string& str : version )
    {
        os << str << "\n";
    }
    return os.str();
}

} // namespace service
} // namespace mega
