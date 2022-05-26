
#ifndef RECEIVER_24_MAY_2022
#define RECEIVER_24_MAY_2022

#include "service/protocol/common/decoder.hpp"

#include "service/network/activity.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/spawn.hpp"

#include <functional>

namespace mega
{
namespace network
{

class Receiver
{
public:
    Receiver( ActivityManager& activityManager, Decoder& decoder, boost::asio::ip::tcp::socket& socket,
              std::function< void() > disconnectHandler );

    template < typename TExecutor >
    void run( TExecutor& strandOrIOContext )
    {
        Receiver& receiver = *this;
        boost::asio::spawn(
            strandOrIOContext, [ &receiver ]( boost::asio::yield_context yield ) { receiver.receive( yield ); } );
    }
    void stop() { m_bContinue = false; }

private:
    void receive( boost::asio::yield_context yield_ctx );
    void updateLastActivityTime();

private:
    bool                                  m_bContinue = true;
    ActivityManager&                      m_activityManager;
    Decoder&                              m_decoder;
    boost::asio::ip::tcp::socket&         m_socket;
    std::function< void() >               m_disconnectHandler;
    boost::asio::steady_timer::time_point m_lastActivityTime;
};

} // namespace network
} // namespace mega

#endif // RECEIVER_24_MAY_2022
