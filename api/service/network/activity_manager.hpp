#ifndef ACTIVITY_MANAGER_26_MAY_2022
#define ACTIVITY_MANAGER_26_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/network/activity.hpp"

#include <mutex>

namespace mega
{
namespace network
{

class ActivityManager
{
    using ActivityPtrMap = std::map< ActivityID, Activity::Ptr >;

public:
    ActivityManager( boost::asio::io_context& ioContext );

    boost::asio::io_context& getIOContext() const;

    void activityStarted( Activity::Ptr pActivity );
    void activityCompleted( Activity::Ptr pActivity );

    Activity::Ptr findExistingActivity( const network::ActivityID& activityID ) const;

protected:
    boost::asio::io_context& m_ioContext;

private:
    ActivityPtrMap m_activities;
    mutable std::recursive_mutex m_mutex;
};

class ActivityFactory
{
public:
    virtual Activity::Ptr createRequestActivity( const network::ActivityID&   activityID,
                                                 const network::ConnectionID& originatingConnectionID ) const = 0;
};

} // namespace network
} // namespace mega

#endif // ACTIVITY_MANAGER_26_MAY_2022
