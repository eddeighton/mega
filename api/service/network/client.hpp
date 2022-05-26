#ifndef CLIENT_24_MAY_2022
#define CLIENT_24_MAY_2022

#include "service/protocol/model/host_daemon.hxx"
#include "service/protocol/model/daemon_host.hxx"

#include "service/network/activity.hpp"
#include "service/network/activity_manager.hpp"
#include "service/network/receiver.hpp"

#include "common/assert_verify.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/steady_timer.hpp"

#include <boost/asio/execution_context.hpp>
#include <string>
#include <thread>
#include <functional>
#include <iostream>

namespace mega
{
namespace network
{

class Client : public ActivityManager
{
public:
    Client( boost::asio::io_context& ioContext, ActivityFactory& activityFactory, const std::string& strServiceIP );
    ~Client();

    boost::asio::ip::tcp::socket& getSocket() { return m_socket; }

    void spawnActivity( Activity::Ptr pActivity );

private:
    boost::asio::ip::tcp::resolver m_resolver;
    boost::asio::ip::tcp::socket   m_socket;
    boost::asio::ip::tcp::endpoint m_endPoint;
    boost::asio::steady_timer      m_watchDogTimer;
    host_daemon_Decode             m_decoder;
    Receiver                       m_receiver;
};

} // namespace network
} // namespace mega

#endif // CLIENT_24_MAY_2022
