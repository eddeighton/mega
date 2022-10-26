//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef ENDPOINT_25_MAY_2022
#define ENDPOINT_25_MAY_2022

#include "network.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/messages.hxx"

#include <boost/asio/spawn.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>
#include <boost/asio/experimental/channel.hpp>

#include <boost/archive/binary_oarchive.hpp>

#include <cstddef>
#include <vector>

namespace mega::network
{

using ChannelMsg = Message;

using ConcurrentChannel
    = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, ChannelMsg ) >;
using Channel = boost::asio::experimental::channel< void( boost::system::error_code, ChannelMsg ) >;

ConnectionID makeConnectionID( Traits::Socket& socket );

} // namespace mega::network

#endif // ENDPOINT_25_MAY_2022
