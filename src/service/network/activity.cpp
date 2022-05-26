#include "service/network/activity.hpp"
#include "service/network/activity_manager.hpp"

namespace mega
{
namespace network
{

Activity::Activity( ActivityManager& activityManager, const ActivityID& activityID,
              std::optional< ConnectionID > originatingConnectionID )
    : m_activityManager( activityManager )
    , m_activityID( activityID )
    , m_originatingEndPoint( originatingConnectionID )
    , m_messageChannel( activityManager.getIOContext() )
{

}

// programming zoomed in :)

Activity::~Activity() {}

} // namespace network
} // namespace mega