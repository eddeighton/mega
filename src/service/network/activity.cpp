#include "service/network/activity.hpp"
#include "service/network/activity_manager.hpp"

#include <iostream>

namespace mega
{
namespace network
{

Activity::Activity( ActivityManager& activityManager, const ActivityID& activityID,
                    std::optional< ConnectionID > originatingConnectionID )
    : m_activityManager( activityManager )
    , m_activityID( activityID )
    , m_originatingEndPoint( originatingConnectionID )
    , m_requestChannel( activityManager.getIOContext() )
    , m_responseChannel( activityManager.getIOContext() )
{
}

Activity::~Activity()
{
    // std::cout << "Activity Completed: " << getActivityID() << std::endl;
}

void Activity::completed() { m_activityManager.activityCompleted( shared_from_this() ); }

MessageVariant Activity::receiveRequest( boost::asio::yield_context yield_ctx )
{
    return m_requestChannel.async_receive( yield_ctx );
}

void Activity::sendRequest( const MessageVariant& msg )
{
    m_requestChannel.async_send( boost::system::error_code(), msg,
                                 []( boost::system::error_code ec )
                                 {
                                     if ( ec )
                                     {
                                         THROW_RTE( "Failed to send request on channel: " << ec.what() );
                                     }
                                 } );
}

MessageVariant Activity::receiveResponse( boost::asio::yield_context yield_ctx )
{
    return m_responseChannel.async_receive( yield_ctx );
}

void Activity::sendResponse( const MessageVariant& msg )
{
    m_responseChannel.async_send( boost::system::error_code(), msg,
                                  []( boost::system::error_code ec )
                                  {
                                      if ( ec )
                                      {
                                          THROW_RTE( "Failed to send response on channel: " << ec.what() );
                                      }
                                  } );
}

} // namespace network
} // namespace mega