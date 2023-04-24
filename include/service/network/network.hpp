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

#ifndef MEGA_NETWORK_16_MAY_2022
#define MEGA_NETWORK_16_MAY_2022

#include "mega/native_types.hpp"

#include <boost/asio/ip/tcp.hpp>
// #include <boost/asio/ip/udp.hpp>

namespace mega::network
{

short MegaDaemonPort();
short MegaRootPort();

// For systems not supporting segmented stacks - need a stack size of coroutines
//                                                    0x0FFFFF; // 1M bytes
//                                                    0x200000; // 2,097,152
//static const mega::U64 NON_SEGMENTED_STACK_SIZE     = 0x400000; // 4,194,304
static const mega::U64 NON_SEGMENTED_STACK_SIZE     = 0x800000; // 8 MB
//                                                    0xAFFFFF; // 11,534,335

struct Traits
{
    using Resolver = boost::asio::ip::tcp::resolver;
    using Socket   = boost::asio::ip::tcp::socket;
    using EndPoint = boost::asio::ip::tcp::endpoint;
    using Acceptor = boost::asio::ip::tcp::acceptor;

    // using Resolver = boost::asio::ip::udp::resolver;
    // using Socket   = boost::asio::ip::udp::socket;
    // using EndPoint = boost::asio::ip::udp::endpoint;

    // using Acceptor = boost::asio::ip::udp::acceptor;
};

} // namespace mega::network

#endif // MEGA_NETWORK_16_MAY_2022
