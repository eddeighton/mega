
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

#ifndef SENDER_15_JUNE_2022
#define SENDER_15_JUNE_2022

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/messages.hxx"

#include "service/network/end_point.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/spawn.hpp"
#include "boost/asio/experimental/concurrent_channel.hpp"
#include "boost/asio/experimental/channel.hpp"

#include <memory>

namespace mega
{
namespace network
{

class Sender
{
public:
    using Ptr         = std::unique_ptr< Sender >;
    virtual ~Sender() = 0;

    virtual ConnectionID              getConnectionID() const                                           = 0;
    virtual boost::system::error_code send( const Message& msg, boost::asio::yield_context& yield_ctx ) = 0;
    virtual void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
        = 0;
};

Sender::Ptr make_socket_sender( boost::asio::ip::tcp::socket& socket, const ConnectionID& connectionID );
Sender::Ptr make_current_channel_sender( ConcurrentChannel& channel, const ConnectionID& connectionID );
Sender::Ptr make_channel_sender( Channel& channel, const ConnectionID& connectionID );

} // namespace network
} // namespace mega

#endif // SENDER_15_JUNE_2022
