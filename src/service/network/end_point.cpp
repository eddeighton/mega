
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/messages.hxx"

#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>

#include <algorithm>
#include <sstream>

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

boost::system::error_code send( const SendBuffer& buffer, boost::asio::ip::tcp::socket& socket,
                                boost::asio::yield_context& yield_ctx )
{
    boost::system::error_code ec;
    {
        const std::size_t szBytesWritten
            = boost::asio::async_write( socket, boost::asio::buffer( buffer ), yield_ctx[ ec ] ); //
        if ( !ec )
        {
            VERIFY_RTE( szBytesWritten == buffer.size() );
        }
        else
        {
            return ec;
        }
    }
    return ec;
}

void sendErrorResponse( const ActivityID& activityID, boost::asio::ip::tcp::socket& socket,
                                             const std::string& strErrorMsg, boost::asio::yield_context& yield_ctx )
{
    if( const boost::system::error_code ec =  send( socket, yield_ctx,
                 [ & ]( boost::archive::binary_oarchive& oa )
                 {
                     const Header header( MSG_Error_Response::ID, activityID );
                     oa&          header;
                     oa&          strErrorMsg;
                 } ) )
    {
        THROW_RTE( "Error sending: " << ec.what() );
    }
    else
    {
        SPDLOG_TRACE( "Sent error response for activity: {} msg: {}", activityID.getID(), strErrorMsg );
    }
}

} // namespace network
} // namespace mega