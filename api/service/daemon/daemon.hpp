#ifndef DAEMON_25_MAY_2022
#define DAEMON_25_MAY_2022

#include "service/network/server.hpp"
#include "service/network/activity_manager.hpp"

#include "boost/asio/io_context.hpp"

namespace mega
{
namespace service
{
class Daemon
{
    class DaemonActivityFactory : public network::ActivityFactory
    {
    public:
        DaemonActivityFactory( Daemon& daemon );
        virtual network::Activity::Ptr createRequestActivity( network::ActivityManager&  activityManager,
                                                              const network::ActivityID& activityID,
                                                              const network::ConnectionID& originatingConnectionID ) const;

    private:
        Daemon& m_daemon;
    };
    friend class RequestActivity;

public:
    Daemon( boost::asio::io_context& ioContext );

private:
    DaemonActivityFactory m_activityFactory;
    network::Server       m_server;
};
} // namespace service
} // namespace mega

#endif // DAEMON_25_MAY_2022
