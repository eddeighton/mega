
#ifndef RECEIVER_24_MAY_2022
#define RECEIVER_24_MAY_2022

#include "service/protocol/common/decoder.hpp"

#include "service/network/activity.hpp"

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
    Receiver( ActivityManager& activityManager, Decoder& decoder, boost::asio::ip::tcp::socket& socket );

    void receive( boost::asio::yield_context yield_ctx );

    template < typename TExecutor >
    void run( TExecutor& strandOrIOContext )
    {
        Receiver& receiver = *this;
        boost::asio::spawn(
            strandOrIOContext, [ &receiver ]( boost::asio::yield_context yield ) { receiver.receive( yield ); } );
    }

private:
    void updateLastActivityTime();

private:
    ActivityManager&                      m_activityManager;
    Decoder&                              m_decoder;
    boost::asio::ip::tcp::socket&         m_socket;
    boost::asio::steady_timer::time_point m_lastActivityTime;
};

} // namespace network
} // namespace mega

#endif // RECEIVER_24_MAY_2022
