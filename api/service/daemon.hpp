#ifndef DAEMON_25_MAY_2022
#define DAEMON_25_MAY_2022

#include "service/network/client.hpp"
#include "service/network/server.hpp"
#include "service/network/activity_manager.hpp"

#include "boost/asio/io_context.hpp"

namespace mega
{
namespace service
{
class Daemon
{
    class RequestActivityFactory : public network::ActivityFactory
    {
    public:
        RequestActivityFactory( Daemon& daemon )
            : m_daemon( daemon )
        {
        }
        virtual network::Activity::Ptr
        createRequestActivity( const network::Header&       msgHeader,
                               const network::ConnectionID& originatingConnectionID ) const;

    private:
        Daemon& m_daemon;
    };
    friend class DaemonRequestActivity;

public:
    Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP );
    ~Daemon();

private:
    RequestActivityFactory   m_requestActivityFactory;
    network::ActivityManager m_activityManager;
    network::Client          m_rootClient;
    network::Server          m_hostServer;
    network::Server          m_workerServer;
};
} // namespace service
} // namespace mega

#endif // DAEMON_25_MAY_2022
