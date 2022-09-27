
#ifndef ENDPOINT_25_MAY_2022
#define ENDPOINT_25_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/messages.hxx"

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/spawn.hpp"
#include "boost/asio/experimental/concurrent_channel.hpp"
#include "boost/asio/experimental/channel.hpp"

#include "boost/archive/binary_oarchive.hpp"

#include <cstddef>
#include <vector>

namespace mega
{
namespace network
{

using ChannelMsg = Message;

using ConcurrentChannel
    = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, ChannelMsg ) >;
using Channel = boost::asio::experimental::channel< void( boost::system::error_code, ChannelMsg ) >;

ConnectionID makeConnectionID( boost::asio::ip::tcp::socket& socket );

} // namespace network
} // namespace mega

#endif // ENDPOINT_25_MAY_2022
