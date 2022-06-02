#ifndef ACTIVITY_MANAGER_26_MAY_2022
#define ACTIVITY_MANAGER_26_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/network/activity.hpp"

#include <shared_mutex>
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

    std::vector< ActivityID > reportActivities() const;
    ActivityID                createActivityID( const ConnectionID& connectionID ) const;
    void                      activityStarted( Activity::Ptr pActivity );
    void                      activityCompleted( Activity::Ptr pActivity );

    Activity::Ptr findExistingActivity( const network::ActivityID& activityID ) const;

protected:
    boost::asio::io_context& m_ioContext;

private:
    ActivityPtrMap            m_activities;
    mutable std::shared_mutex m_mutex;
    using WriteLock = std::unique_lock< std::shared_mutex >;
    using ReadLock  = std::shared_lock< std::shared_mutex >;

    mutable ActivityID::ID m_nextActivityID = 0U;
};

class ActivityFactory
{
public:
    virtual Activity::Ptr createRequestActivity( const network::Header&       msgHeader,
                                                 const network::ConnectionID& originatingConnectionID ) const = 0;
};

} // namespace network
} // namespace mega

#endif // ACTIVITY_MANAGER_26_MAY_2022
