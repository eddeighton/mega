
#include "service/network/activity_manager.hpp"
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
    m_activities.insert( std::make_pair( pActivity->getActivityID(), pActivity ) );
    boost::asio::spawn(
        m_ioContext, [ pActivity ]( boost::asio::yield_context yield_ctx ) { pActivity->run( yield_ctx ); } );
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

} // namespace network
} // namespace mega