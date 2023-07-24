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

#ifndef CLIENT_24_MAY_2022
#define CLIENT_24_MAY_2022

#include "service/network/logical_thread.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "service/network/receiver.hpp"
#include "service/network/sender_factory.hpp"
#include "service/network/network.hpp"

#include "common/assert_verify.hpp"

#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

#include <string>
#include <thread>
#include <functional>
#include <iostream>

namespace mega::network
{

class Client
{
    using Strand = boost::asio::strand< boost::asio::io_context::executor_type >;

public:
    Client( boost::asio::io_context& ioContext, LogicalThreadManager& logicalthreadManager,
            const std::string& strServiceIP, short portNumber );
    ~Client();

    boost::asio::io_context& getIOContext() const { return m_ioContext; }
    Sender::Ptr              getSender() const { return m_pSender; }

    void stop();
    void disconnected();

private:
    boost::asio::io_context& m_ioContext;
    Traits::Resolver         m_resolver;
    Strand                   m_strand;
    Traits::Socket           m_socket;
    Traits::EndPoint         m_endPoint;
    SocketReceiver           m_receiver;
    Sender::Ptr              m_pSender;
};

} // namespace mega::network

#endif // CLIENT_24_MAY_2022
