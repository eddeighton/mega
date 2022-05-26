#ifndef ACTIVITY_24_MAY_2022
#define ACTIVITY_24_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/host_daemon.hxx"
#include "service/protocol/model/daemon_host.hxx"

#include "common/assert_verify.hpp"

#include "boost/asio/spawn.hpp"

#include <memory>

namespace mega
{
namespace network
{

class ActivityManager;

class Activity : public std::enable_shared_from_this< Activity >
{
public:
    using Ptr = std::shared_ptr< Activity >;

    Activity( ActivityManager& activityManager, const ActivityID& activityID,
              std::optional< ConnectionID > originatingConnectionID = std::nullopt );
    virtual ~Activity();

    const ActivityID&                    getActivityID() const { return m_activityID; }
    const std::optional< ConnectionID >& getOriginatingEndPointID() const { return m_originatingEndPoint; }

    virtual void run( boost::asio::yield_context yield_ctx ) = 0;

    void completed();

    MessageVariant receiveRequest( boost::asio::yield_context yield_ctx );
    void           sendRequest( const MessageVariant& msg );
    MessageVariant receiveResponse( boost::asio::yield_context yield_ctx );
    void           sendResponse( const MessageVariant& msg );

protected:
    ActivityManager&              m_activityManager;
    ActivityID                    m_activityID;
    std::optional< ConnectionID > m_originatingEndPoint;
    MessageChannel                m_requestChannel;
    MessageChannel                m_responseChannel;
};

} // namespace network
} // namespace mega

#endif // ACTIVITY_24_MAY_2022
