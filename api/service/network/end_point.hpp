
#ifndef ENDPOINT_25_MAY_2022
#define ENDPOINT_25_MAY_2022

#include "service/protocol/common/header.hpp"

#include "common/assert_verify.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/streambuf.hpp"
#include <boost/asio/spawn.hpp>

#include <cstddef>

namespace mega
{
namespace network
{
ConnectionID getConnectionID( boost::asio::ip::tcp::socket& socket );

boost::system::error_code send( boost::asio::streambuf& streambuffer, boost::asio::ip::tcp::socket& socket,
                                boost::asio::yield_context& yield_ctx );

boost::system::error_code sendErrorResponse( const ActivityID& activityID, boost::asio::ip::tcp::socket& socket,
                                             const std::string& strErrorMsg, boost::asio::yield_context& yield_ctx );

} // namespace network
} // namespace mega

#endif // ENDPOINT_25_MAY_2022
