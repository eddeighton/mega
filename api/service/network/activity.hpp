#ifndef ACTIVITY_24_MAY_2022
#define ACTIVITY_24_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/messages.hxx"
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
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, MessageVariant ) >;
public:

    using Ptr = std::shared_ptr< Activity >;

    Activity( ActivityManager& activityManager, const ActivityID& activityID,
              std::optional< ConnectionID > originatingConnectionID = std::nullopt );
    virtual ~Activity();

    const ActivityID&                    getActivityID() const { return m_activityID; }
    const std::optional< ConnectionID >& getOriginatingEndPointID() const { return m_originatingEndPoint; }

    MessageVariant receive( boost::asio::yield_context& yield_ctx );
    void           send( const MessageVariant& msg );

private:
    void requestStarted( const ConnectionID& connectionID );
    void requestCompleted();
public:
    class RequestStack
    {
        const char* m_pszMsg;
        boost::asio::steady_timer::time_point m_startTime;
        Activity& activity;
        RequestStack( RequestStack& ) = delete;
        RequestStack& operator=(RequestStack&) = delete;
    public:
        RequestStack( const char* pszMsg, Activity& activity, const ConnectionID& connectionID );
        ~RequestStack();
    };
    friend class Activity::RequestStack;

protected:
    void run_one( boost::asio::yield_context& yield_ctx );

protected:
    friend class ActivityManager;
    // this is called by ActivityManager but can be overridden in initiating activities
    virtual void run( boost::asio::yield_context& yield_ctx );

    virtual bool dispatchRequest( const MessageVariant& msg, boost::asio::yield_context& yield_ctx ) = 0;
    virtual void error( const ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
        = 0;

public:
    // this is used by the Request_Encode generated classes
    MessageVariant dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx );

private:
    void dispatchRequestImpl( const MessageVariant& msg, boost::asio::yield_context& yield_ctx );

protected:
    ActivityManager&              m_activityManager;
    ActivityID                    m_activityID;
    std::optional< ConnectionID > m_originatingEndPoint;
    std::vector< ConnectionID >   m_stack;
    MessageChannel                m_channel;
};

} // namespace network
} // namespace mega

#endif // ACTIVITY_24_MAY_2022
