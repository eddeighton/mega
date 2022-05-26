
#ifndef SERVICE_24_MAY_2022
#define SERVICE_24_MAY_2022

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

class Host
{
    friend class RequestActivity;
    class HostActivityFactory : public network::ActivityFactory
    {
        Host& m_host;

    public:
        HostActivityFactory( Host& host )
            : m_host( host )
        {
        }
        virtual network::Activity::Ptr
        createRequestActivity( const network::ActivityID&   activityID,
                               const network::ConnectionID& originatingConnectionID ) const;
    };

public:
    Host( std::optional< const std::string > optName = std::nullopt );
    ~Host();

    std::string                GetVersion();
    std::vector< std::string > ListHosts();

private:
    HostActivityFactory      m_activityFactory;
    boost::asio::io_context  m_io_context;
    network::ActivityManager m_activityManager;
    network::Client          m_client;
    using ExecutorType = decltype( m_io_context.get_executor() );
    boost::asio::executor_work_guard< ExecutorType > m_work_guard;
    std::thread                                      m_io_thread;
};

} // namespace service
} // namespace mega

#endif // SERVICE_24_MAY_2022
