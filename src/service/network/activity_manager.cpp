
#include "service/network/activity_manager.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include <iostream>

namespace mega
{
namespace network
{

ActivityManager::ActivityManager( const char* pszProcessName, boost::asio::io_context& ioContext )
    : m_pszProcessName( pszProcessName ),
        m_ioContext( ioContext )
{
}

boost::asio::io_context& ActivityManager::getIOContext() const { return m_ioContext; }

std::vector< ActivityID > ActivityManager::reportActivities() const
{
    std::vector< ActivityID > activities;
    {
        ReadLock lock( m_mutex );
        for ( const auto& [ id, pActivity ] : m_activities )
        {
            activities.push_back( id );
        }
    }
    return activities;
}

ActivityID ActivityManager::createActivityID( const ConnectionID& connectionID ) const
{
    WriteLock lock( m_mutex );
    return ActivityID( ++m_nextActivityID, connectionID );
}

void ActivityManager::activityStarted( Activity::Ptr pActivity )
{
    {
        WriteLock lock( m_mutex );
        m_activities.insert( std::make_pair( pActivity->getActivityID(), pActivity ) );
    }
    boost::asio::spawn(
        m_ioContext, [ pActivity ]( boost::asio::yield_context yield_ctx ) { pActivity->run( yield_ctx ); } );
    SPDLOG_TRACE( "Activity Started id: {} end point: {}", pActivity->getActivityID().getID(),
                  pActivity->getActivityID().getConnectionID() );
}

void ActivityManager::activityCompleted( Activity::Ptr pActivity )
{
    {
        WriteLock                      lock( m_mutex );
        ActivityPtrMap::const_iterator iFind = m_activities.find( pActivity->getActivityID() );
        VERIFY_RTE( iFind != m_activities.end() );
        m_activities.erase( iFind );
    }
    SPDLOG_DEBUG( "Activity Completed id: {} end point: {}", pActivity->getActivityID().getID(),
                  pActivity->getActivityID().getConnectionID() );
}

Activity::Ptr ActivityManager::findExistingActivity( const ActivityID& activityID ) const
{
    ReadLock                       lock( m_mutex );
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

} // namespace network
} // namespace mega