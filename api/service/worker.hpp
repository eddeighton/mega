
#ifndef WORKER_27_MAY_2022
#define WORKER_27_MAY_2022

#include "service/network/client.hpp"
#include "service/network/activity_manager.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <optional>
#include <string>
#include <optional>
#include <vector>
#include <thread>

namespace mega
{
namespace service
{

class Worker
{
    friend class WorkerRequestActivity;
    friend class JobActivity;

    class HostActivityFactory : public network::ActivityFactory
    {
        Worker& m_worker;

    public:
        HostActivityFactory( Worker& host )
            : m_worker( host )
        {
        }
        virtual network::Activity::Ptr
        createRequestActivity( const network::Header&       msgHeader,
                               const network::ConnectionID& originatingConnectionID ) const;
    };

public:
    Worker( boost::asio::io_context& io_context, int numThreads );
    ~Worker();
    void shutdown();

    int getNumThreads() const { return m_numThreads; }

private:
    HostActivityFactory      m_activityFactory;
    boost::asio::io_context& m_io_context;
    int                      m_numThreads;
    network::ActivityManager m_activityManager;
    network::Client          m_client;
};

} // namespace service
} // namespace mega

#endif // WORKER_27_MAY_2022
