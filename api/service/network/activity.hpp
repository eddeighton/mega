#ifndef ACTIVITY_24_MAY_2022
#define ACTIVITY_24_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/host_daemon.hxx"
#include "service/protocol/model/daemon_host.hxx"

#include "common/assert_verify.hpp"

#include "boost/asio/spawn.hpp"

#include <cstddef>
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

    MessageVariant receive( boost::asio::yield_context yield_ctx );
    void           send( const MessageVariant& msg );

    bool isComplete() const;
    void requestStarted();
    void requestCompleted();

protected:
    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx );

protected:
    friend class ActivityManager;
    // this is called by ActivityManager but can be overridden in initiating activities
    virtual void run( boost::asio::yield_context yield_ctx );
    virtual void error( const ConnectionID& connectionID, const std::string& strErrorMsg, boost::asio::yield_context yield_ctx ) = 0;
public:
    // this is used by the Request_Encode generated classes
    MessageVariant dispatchRequestsUntilResponse( boost::asio::yield_context yield_ctx );

private:
    bool dispatchRequestImpl( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx );

protected:
    ActivityManager&              m_activityManager;
    ActivityID                    m_activityID;
    std::optional< ConnectionID > m_originatingEndPoint;
    MessageChannel                m_channel;
    std::size_t                   m_stackDepth;
    bool                          m_bStarted;
};

} // namespace network
} // namespace mega

#endif // ACTIVITY_24_MAY_2022
