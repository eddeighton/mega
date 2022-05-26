#ifndef ACTIVITY_MANAGER_26_MAY_2022
#define ACTIVITY_MANAGER_26_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/network/activity.hpp"

namespace mega
{
namespace network
{

class ActivityManager;

class ActivityFactory
{
public:
    virtual Activity::Ptr createRequestActivity( ActivityManager&             activityManager,
                                                 const network::ActivityID&   activityID,
                                                 const network::ConnectionID& originatingConnectionID ) const = 0;
};

class ActivityManager
{
    using ActivityPtrMap = std::map< ActivityID, Activity::Ptr >;

public:
    ActivityManager( boost::asio::io_context& ioContext, ActivityFactory& activityFactory );

    boost::asio::io_context& getIOContext() const;

    void activityStarted( Activity::Ptr pActivity );
    void activityCompleted( Activity::Ptr pActivity );

    Activity::Ptr findExistingActivity( const network::ActivityID& activityID ) const;
    Activity::Ptr startRequestActivity( const network::ActivityID& activityID,
                                        const ConnectionID&        originatingEndPointID );

protected:
    boost::asio::io_context& m_ioContext;
    ActivityFactory&         m_activityFactory;

private:
    ActivityPtrMap m_activities;
};

} // namespace network
} // namespace mega

#endif // ACTIVITY_MANAGER_26_MAY_2022
