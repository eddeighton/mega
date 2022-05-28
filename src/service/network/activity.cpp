#include "service/network/activity.hpp"
#include "service/network/activity_manager.hpp"

#include "service/protocol/model/messages.hxx"

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
    , m_channel( activityManager.getIOContext() )
    , m_stackDepth( 0U )
    , m_bStarted( false )
{
}

Activity::~Activity()
{
    // std::cout << "Activity Completed: " << getActivityID() << std::endl;
}

bool Activity::isComplete() const { return m_bStarted && ( m_stackDepth == 0U ); }

void Activity::requestStarted()
{
    m_bStarted = true;
    ++m_stackDepth;
}

void Activity::requestCompleted()
{
    VERIFY_RTE( m_stackDepth > 0U );
    --m_stackDepth;
    if ( m_stackDepth == 0U )
    {
        m_activityManager.activityCompleted( shared_from_this() );
    }
}

void Activity::run( boost::asio::yield_context yield_ctx )
{
    while ( !isComplete() )
    {
        const network::MessageVariant msg = receive( yield_ctx );
        if( !dispatch( msg, yield_ctx ) )
        {
            THROW_RTE( "Failed to dispatch message: " << msg );
        }
    }
}

bool Activity::dispatch( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
{
    if ( msg.index() == network::MSG_Complete_Request::ID )
    {
        requestCompleted();
        return true;
    }
    else
    {
        return false;
    }
}

MessageVariant Activity::receive( boost::asio::yield_context yield_ctx )
{
    return m_channel.async_receive( yield_ctx );
}

void Activity::send( const MessageVariant& msg )
{
    m_channel.async_send( boost::system::error_code(), msg,
                          [ &msg ]( boost::system::error_code ec )
                          {
                              if ( ec )
                              {
                                  THROW_RTE( "Failed to send request on channel: " << msg << " : " << ec.what() );
                              }
                          } );
}

MessageVariant Activity::dispatchRequestsUntilResponse( boost::asio::yield_context yield_ctx )
{
    MessageVariant msg;
    while ( true )
    {
        msg = receive( yield_ctx );
        if ( isRequest( msg ) )
        {
            dispatch( msg, yield_ctx );
        }
        else
        {
            break;
        }
    }
    return msg;
}

} // namespace network
} // namespace mega