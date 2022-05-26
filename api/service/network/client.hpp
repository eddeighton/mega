#ifndef CLIENT_24_MAY_2022
#define CLIENT_24_MAY_2022

#include "service/protocol/model/host_daemon.hxx"
#include "service/protocol/model/daemon_host.hxx"

#include "service/network/activity.hpp"
#include "service/network/activity_manager.hpp"
#include "service/network/receiver.hpp"

#include "common/assert_verify.hpp"

#include "boost/asio/strand.hpp"
#include "boost/asio/ip/tcp.hpp"

#include <boost/asio/execution_context.hpp>
#include <string>
#include <thread>
#include <functional>
#include <iostream>

namespace mega
{
namespace network
{

class Client
{
    using Strand = boost::asio::strand< boost::asio::io_context::executor_type >;

public:
    Client( boost::asio::io_context& ioContext, ActivityManager& activityManager, ActivityFactory& activityFactory,
            const std::string& strServiceIP, const std::string& strServiceName );
    ~Client();

    boost::asio::io_context&      getIOContext() const { return m_ioContext; }
    boost::asio::ip::tcp::socket& getSocket() { return m_socket; }

    void stop();
    void disconnected();

private:
    boost::asio::io_context&       m_ioContext;
    boost::asio::ip::tcp::resolver m_resolver;
    Strand                         m_strand;
    boost::asio::ip::tcp::socket   m_socket;
    boost::asio::ip::tcp::endpoint m_endPoint;
    Receiver                       m_receiver;
};

} // namespace network
} // namespace mega

#endif // CLIENT_24_MAY_2022
