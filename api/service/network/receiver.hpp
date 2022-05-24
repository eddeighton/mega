
#ifndef RECEIVER_24_MAY_2022
#define RECEIVER_24_MAY_2022

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/streambuf.hpp"
#include "boost/asio/spawn.hpp"
#include <boost/asio/execution_context.hpp>

namespace mega
{
namespace network
{

class Receiver
{
public:
    Receiver( boost::asio::ip::tcp::socket& socket );

    void receive( boost::asio::yield_context yield_ctx );

    template< typename TExecutor >
    void run( TExecutor& executor )
    {
        Receiver& receiver = *this;
        boost::asio::spawn( executor, [ &receiver ]( boost::asio::yield_context yield ) { receiver.receive( yield ); } );
    }

private:
    void updateLastActivityTime();

private:
    boost::asio::ip::tcp::socket&         m_socket;
    boost::asio::steady_timer::time_point m_lastActivityTime;
    boost::asio::streambuf                m_streamBuffer;
};

} // namespace network
} // namespace mega

#endif // RECEIVER_24_MAY_2022
