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

#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/receiver.hpp"
#include "service/network/sender.hpp"

#include "common/assert_verify.hpp"

#include "boost/asio/strand.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/io_context.hpp"

#include <string>
#include <thread>
#include <functional>
#include <iostream>

namespace mega
{
namespace network
{

class Client : public Sender
{
    using Strand = boost::asio::strand< boost::asio::io_context::executor_type >;

public:
    Client( boost::asio::io_context& ioContext, ConversationManager& conversationManager,
            const std::string& strServiceIP, const std::string& strServiceName );
    ~Client();

    boost::asio::io_context& getIOContext() const { return m_ioContext; }

    void stop();
    void disconnected();

    // Sender
    virtual ConnectionID              getConnectionID() const { return m_pSender->getConnectionID(); }
    virtual boost::system::error_code send( const Message& msg, boost::asio::yield_context& yield_ctx )
    {
        return m_pSender->send( msg, yield_ctx );
    }
    virtual void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        m_pSender->sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }

private:
    boost::asio::io_context&       m_ioContext;
    boost::asio::ip::tcp::resolver m_resolver;
    Strand                         m_strand;
    boost::asio::ip::tcp::socket   m_socket;
    boost::asio::ip::tcp::endpoint m_endPoint;
    SocketReceiver                 m_receiver;
    ConnectionID                   m_connectionID;
    Sender::Ptr                    m_pSender;
};

} // namespace network
} // namespace mega

#endif // CLIENT_24_MAY_2022
