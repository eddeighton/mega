
#include "service/network/activity_manager.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include <iostream>

namespace mega
{
namespace network
{

ActivityManager::ActivityManager( boost::asio::io_context& ioContext )
    : m_ioContext( ioContext )
{
}

boost::asio::io_context& ActivityManager::getIOContext() const { return m_ioContext; }

void ActivityManager::activityStarted( Activity::Ptr pActivity )
{
    {
        std::scoped_lock< std::recursive_mutex > lock( m_mutex );
        m_activities.insert( std::make_pair( pActivity->getActivityID(), pActivity ) );
    }
    // this is the ONLY way activity is spawned
    boost::asio::spawn(
        m_ioContext, [ pActivity ]( boost::asio::yield_context yield_ctx ) { pActivity->run( yield_ctx ); } );
    SPDLOG_TRACE( "Activity Started id: {} end point: {}", pActivity->getActivityID().getID(),
                  pActivity->getActivityID().getEndpointID() );
}

void ActivityManager::activityCompleted( Activity::Ptr pActivity )
{
    std::scoped_lock< std::recursive_mutex > lock( m_mutex );
    ActivityPtrMap::const_iterator           iFind = m_activities.find( pActivity->getActivityID() );
    VERIFY_RTE( iFind != m_activities.end() );
    m_activities.erase( iFind );
    SPDLOG_TRACE( "Activity Completed id: {} end point: {}", pActivity->getActivityID().getID(),
                  pActivity->getActivityID().getEndpointID() );
}

Activity::Ptr ActivityManager::findExistingActivity( const ActivityID& activityID ) const
{
    std::scoped_lock< std::recursive_mutex > lock( m_mutex );
    ActivityPtrMap::const_iterator           iFind = m_activities.find( activityID );
    if ( iFind != m_activities.end() )
    {
        return iFind->second;
    }
    else
    {
        return Activity::Ptr();
    }
}

} // namespace network
} // namespace mega