#include "service/network/activity_manager.hpp"
#include "service/protocol/common/header.hpp"

namespace mega
{
namespace network
{

ActivityManager::ActivityManager( boost::asio::io_context& ioContext, ActivityFactory& activityFactory )
    : m_ioContext( ioContext )
    , m_activityFactory( activityFactory )
{
}

boost::asio::io_context& ActivityManager::getIOContext() const { return m_ioContext; }

void ActivityManager::activityStarted( Activity::Ptr pActivity )
{
    m_activities.insert( std::make_pair( pActivity->getActivityID(), pActivity ) );
}

void ActivityManager::activityCompleted( Activity::Ptr pActivity )
{
    ActivityPtrMap::const_iterator iFind = m_activities.find( pActivity->getActivityID() );
    VERIFY_RTE( iFind != m_activities.end() );
    m_activities.erase( iFind );
}

Activity::Ptr ActivityManager::findExistingActivity( const ActivityID& activityID ) const
{
    ActivityPtrMap::const_iterator iFind = m_activities.find( activityID );
    if ( iFind != m_activities.end() )
    {
        return iFind->second;
    }
    else
    {
        return Activity::Ptr();
    }
}

Activity::Ptr ActivityManager::startRequestActivity( const ActivityID& activityID,
                                                     const ConnectionID& originatingEndPointID )
{
    Activity::Ptr pActivity = m_activityFactory.createRequestActivity( *this, activityID, originatingEndPointID );

    boost::asio::spawn(
        m_ioContext, [ pActivity ]( boost::asio::yield_context yield_ctx ) { pActivity->run( yield_ctx ); } );

    activityStarted( pActivity );
    return pActivity;
}

} // namespace network
} // namespace mega