
#ifndef SERVICE_24_MAY_2022
#define SERVICE_24_MAY_2022

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "service/network/client.hpp"
#include "service/network/activity_manager.hpp"

#include "service/protocol/common/pipeline_result.hpp"

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
    friend class HostRequestActivity;
    class HostActivityFactory : public network::ActivityFactory
    {
        Host& m_host;

    public:
        HostActivityFactory( Host& host )
            : m_host( host )
        {
        }
        virtual network::Activity::Ptr
        createRequestActivity( const network::Header&       msgHeader,
                               const network::ConnectionID& originatingConnectionID ) const;
    };

public:
    Host( std::optional< const std::string > optName = std::nullopt );
    ~Host();
    void shutdown();
    bool running() { return !m_io_context.stopped(); }

    std::string                        GetVersion();
    std::vector< std::string >         ListHosts();
    std::vector< network::ActivityID > listActivities();
    network::PipelineResult            PipelineRun( const pipeline::Configuration& pipelineConfig );
    bool                               Shutdown();

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
