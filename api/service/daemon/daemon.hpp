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
    class HostRequestActivityFactory : public network::ActivityFactory
    {
    public:
        HostRequestActivityFactory( Daemon& daemon )
            : m_daemon( daemon )
        {
        }
        virtual network::Activity::Ptr
        createRequestActivity( const network::ActivityID&   activityID,
                               const network::ConnectionID& originatingConnectionID ) const;

    private:
        Daemon& m_daemon;
    };
    class WorkerRequestActivityFactory : public network::ActivityFactory
    {
    public:
        WorkerRequestActivityFactory( Daemon& daemon )
            : m_daemon( daemon )
        {
        }
        virtual network::Activity::Ptr
        createRequestActivity( const network::ActivityID&   activityID,
                               const network::ConnectionID& originatingConnectionID ) const;

    private:
        Daemon& m_daemon;
    };
    class RootRequestActivityFactory : public network::ActivityFactory
    {
    public:
        RootRequestActivityFactory( Daemon& daemon )
            : m_daemon( daemon )
        {
        }
        virtual network::Activity::Ptr
        createRequestActivity( const network::ActivityID&   activityID,
                               const network::ConnectionID& originatingConnectionID ) const;

    private:
        Daemon& m_daemon;
    };
    friend class HostRequestActivity;
    friend class WorkerRequestActivity;
    friend class RootRequestActivity;

public:
    Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP );
    ~Daemon();

private:
    RootRequestActivityFactory   m_rootRequestActivityFactory;
    WorkerRequestActivityFactory m_workerRequestActivityFactory;
    HostRequestActivityFactory   m_hostRequestActivityFactory;
    network::ActivityManager     m_activityManager;
    network::Client              m_rootClient;
    network::Server              m_hostServer;
    network::Server              m_workerServer;
};
} // namespace service
} // namespace mega

#endif // DAEMON_25_MAY_2022
