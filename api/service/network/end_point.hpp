
#ifndef ENDPOINT_25_MAY_2022
#define ENDPOINT_25_MAY_2022

#include "service/protocol/common/header.hpp"

#include "common/assert_verify.hpp"

#include "boost/asio/ip/tcp.hpp"
#include <boost/asio/spawn.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <cstddef>
#include <vector>

namespace mega
{
namespace network
{
ConnectionID getConnectionID( boost::asio::ip::tcp::socket& socket );

// boost::asio::streambuf
using SendBuffer = std::vector< char >;

boost::system::error_code send( const SendBuffer& buffer, boost::asio::ip::tcp::socket& socket,
                                boost::asio::yield_context& yield_ctx );

template < typename TSerializationFunctor >
inline boost::system::error_code send( boost::asio::ip::tcp::socket& socket, boost::asio::yield_context& yield_ctx,
                                       TSerializationFunctor&& functor )
{
    boost::interprocess::basic_vectorbuf< SendBuffer > os;
    {
        boost::archive::binary_oarchive oa( os );
        functor( oa );
    }
    const MessageSize size = os.vector().size();
    std::string_view  sizeView( reinterpret_cast< const char* >( &size ), sizeof( MessageSize ) );
    SendBuffer        buffer;
    buffer.reserve( size + sizeof( MessageSize ) );
    std::copy( sizeView.begin(), sizeView.end(), std::back_inserter( buffer ) );
    std::copy( os.vector().begin(), os.vector().end(), std::back_inserter( buffer ) );
    return send( buffer, socket, yield_ctx );
}

void sendErrorResponse( const ActivityID& activityID, boost::asio::ip::tcp::socket& socket,
                                             const std::string& strErrorMsg, boost::asio::yield_context& yield_ctx );


} // namespace network
} // namespace mega

#endif // ENDPOINT_25_MAY_2022
