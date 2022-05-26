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

class Activity
{
public:
    using Ptr    = std::shared_ptr< Activity >;

    Activity( ActivityManager& activityManager, const ActivityID& activityID,
              std::optional< ConnectionID > originatingConnectionID );
    virtual ~Activity();

    const ActivityID&                  getActivityID() const { return m_activityID; }
    const std::optional< ConnectionID >& getOriginatingEndPointID() const { return m_originatingEndPoint; }

    virtual void run( boost::asio::yield_context yield_ctx ) = 0;

    void completed() {}

    MessageVariant receiveMessage( boost::asio::yield_context yield_ctx )
    {
        return m_messageChannel.async_receive( yield_ctx );
    }

    void sendMessage( const MessageVariant& msg )
    {
        m_messageChannel.async_send( boost::system::error_code(), msg,
                                     []( boost::system::error_code ec )
                                     {
                                         if ( ec )
                                         {
                                             THROW_RTE( "Failed to send message on channel: " << ec.what() );
                                         }
                                     } );
    }

protected:
    ActivityManager&            m_activityManager;
    ActivityID                  m_activityID;
    std::optional< ConnectionID > m_originatingEndPoint;
    MessageChannel              m_messageChannel;
};

} // namespace network
} // namespace mega

#endif // ACTIVITY_24_MAY_2022
