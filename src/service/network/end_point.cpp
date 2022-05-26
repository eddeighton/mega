
#include "service/network/end_point.hpp"

#include <boost/asio/write.hpp>

#include <algorithm>

namespace mega
{
namespace network
{
    
ConnectionID getConnectionID( boost::asio::ip::tcp::socket& socket )
{
    std::ostringstream os;
    os << socket.local_endpoint() << "-" << socket.remote_endpoint();
    return os.str();
}

boost::system::error_code send( boost::asio::streambuf& streambuffer, boost::asio::ip::tcp::socket& socket,
                                boost::asio::yield_context& yield_ctx )
{
    boost::system::error_code ec;
    // write the size
    {
        const MessageSize      networkSize = htonl( static_cast< MessageSize >( streambuffer.size() ) );
        const std::string_view pData( reinterpret_cast< const char* >( &networkSize ), sizeof( MessageSize ) );
        const std::size_t      szBytesWritten
            = boost::asio::async_write( socket, boost::asio::buffer( pData ), yield_ctx[ ec ] );
        if ( !ec )
        {
            VERIFY_RTE( szBytesWritten == sizeof( MessageSize ) );
        }
        else
        {
            return ec;
        }
    }

    // write the message
    {
        const std::size_t szBytesWritten = boost::asio::async_write( socket, streambuffer.data(), yield_ctx[ ec ] );
        if ( !ec )
        {
            VERIFY_RTE( szBytesWritten == streambuffer.size() );
        }
        else
        {
            return ec;
        }
    }

    return ec;
}
} // namespace network
} // namespace mega