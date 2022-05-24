
#ifndef MEGA_NETWORK_16_MAY_2022
#define MEGA_NETWORK_16_MAY_2022

#include "boost/array.hpp"
#include "boost/asio.hpp"
#include "boost/asio/spawn.hpp"
#include "boost/asio/experimental/concurrent_channel.hpp"
#include "boost/coroutine/all.hpp"

#include <boost/asio/streambuf.hpp>
#include <vector>
#include <memory>

namespace mega
{
namespace network
{
const char* getVersion();

short       MegaDaemonPort();
const char* MegaDaemonServiceName();
short       MegaRootPort();
const char* MegaRootServiceName();
/*
static const char* TEST_MESSAGE
    = "Ed was here, Ed was here, Ed was here, Ed was here, Ed was here, Ed was here, Ed was here, Ed was here, Ed was "
      "here";

struct HelloMsg
{
    std::array< char, 128 > msg;
};

using ASIOQueueType = boost::asio::io_context;
// using ASIOQueueType = boost::asio::thread_pool;

using ChannelType =  boost::asio::experimental::concurrent_channel< void( boost::system::error_code, int ) >;

class Receiver : public std::enable_shared_from_this< Receiver >
{
public:
    using Ptr = std::shared_ptr< Receiver >;

    class MessageHandler
    {
    public:
        virtual void onHelloRequest( const HelloMsg& rq ) = 0;
    };

    Receiver( ASIOQueueType& io_context, boost::asio::ip::tcp::socket& socket, MessageHandler& messageHandler );

    const boost::asio::steady_timer::time_point& getLastActivityTime() const { return m_lastActivityTime; }
    void                                         updateLastActivityTime();

    void receive( boost::asio::yield_context yield_ctx );
    // void onReadSizeHandler( const boost::system::error_code& ec, std::size_t szBytesTransferred );

protected:
    ASIOQueueType&                        m_io_context;
    boost::asio::ip::tcp::socket&         m_socket;
    MessageHandler&                       m_messageHandler;
    mega::Header                          m_receiveHeader;
    boost::asio::steady_timer::time_point m_lastActivityTime;
    boost::asio::streambuf                m_streamBuffer;
};

class Sender : public std::enable_shared_from_this< Sender >
{
public:
    using Ptr = std::shared_ptr< Sender >;

    Sender( boost::asio::ip::tcp::socket& socket );

    void send_hello_request( const std::string& strMsg );
    void send_hello_response( const std::string& strMsg );
    // void send_heartbeat_request();
    // void send_heartbeat_response();

    void onWriteHandler( const boost::system::error_code& ec, std::size_t szBytesTransferred,
                         std::shared_ptr< HelloMsg > pMsg );

protected:
    boost::asio::ip::tcp::socket& m_socket;
    // std::vector< char >           m_writeBuffer1;
    // std::vector< char >           m_writeBuffer2;
    // std::vector< char >           m_writeBuffer3;
    // std::vector< char >           m_writeBuffer4;
};
*/
} // namespace service
} // namespace mega

#endif // MEGA_NETWORK_16_MAY_2022
