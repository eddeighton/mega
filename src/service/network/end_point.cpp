
#include "service/network/end_point.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/messages.hxx"

#include <boost/asio/write.hpp>
#include "boost/asio/streambuf.hpp"
#include <boost/archive/binary_oarchive.hpp>

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

    // construct buffer with size header
    std::vector< char > buf( streambuffer.size() + sizeof( MessageSize ) );
    {
        const MessageSize      networkSize = htonl( static_cast< MessageSize >( streambuffer.size() ) );
        const std::string_view pData( reinterpret_cast< const char* >( &networkSize ), sizeof( MessageSize ) );
        std::copy( pData.begin(), pData.end(), buf.begin() );

        const char* pszData = boost::asio::buffer_cast< const char* >( streambuffer.data() );
        std::copy( pszData, pszData + streambuffer.size(), buf.begin() + sizeof( MessageSize ) );
    }

    // write the data in single operation
    {
        const std::size_t szBytesWritten = boost::asio::async_write( socket, boost::asio::buffer( buf ), yield_ctx[ ec ] );
        if ( !ec )
        {
            VERIFY_RTE( szBytesWritten == buf.size() );
        }
        else
        {
            return ec;
        }
    }

    return ec;
}

boost::system::error_code sendErrorResponse( const ActivityID& activityID, boost::asio::ip::tcp::socket& socket,
                                             const std::string& strErrorMsg, boost::asio::yield_context& yield_ctx )
{
    boost::asio::streambuf streambuffer;
    {
        boost::archive::binary_oarchive oa( streambuffer );
        {
            const Header header( MSG_Error_Response::ID, activityID );
            oa&          header;
        }
        oa& strErrorMsg;
    }
    return send( streambuffer, socket, yield_ctx );
}

} // namespace network
} // namespace mega