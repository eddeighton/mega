#include "service/network/activity.hpp"
#include "service/network/activity_manager.hpp"
#include "service/network/log.hpp"

#include "service/network/end_point.hpp"
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

Activity::~Activity() {}

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

bool Activity::dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
{
    if ( msg.index() == network::MSG_Complete_Request::ID )
    {
        requestCompleted();
        return true;
    }
    else if ( msg.index() == network::MSG_Error_Response::ID )
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
    MessageVariant msg = m_channel.async_receive( yield_ctx );
    if ( msg.index() == MSG_Error_Response::ID )
    {
        throw std::runtime_error( std::get< MSG_Error_Response >( msg ).what );
    }
    return msg;
}

void Activity::send( const MessageVariant& msg )
{
    m_channel.async_send( boost::system::error_code(), msg,
                          [ &msg ]( boost::system::error_code ec )
                          {
                              if ( ec )
                              {
                                  SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg, ec.what() );
                                  THROW_RTE( "Failed to send request on channel: " << msg << " : " << ec.what() );
                              }
                          } );
}

// run is ALWAYS call for each activity after it is created
void Activity::run( boost::asio::yield_context yield_ctx )
{
    try
    {
        while ( !isComplete() )
        {
            const network::MessageVariant msg = receive( yield_ctx );
            if ( !dispatchRequestImpl( msg, yield_ctx ) )
            {
                SPDLOG_ERROR( "Failed to dispatch request: {} on activity: {}", msg, getActivityID().getID() );
                THROW_RTE( "Failed to dispatch request message: " << msg );
            }
        }
    }
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "Activity: {} exception: {}", getActivityID().getID(), ex.what() );
        m_activityManager.activityCompleted( shared_from_this() );
    }
}

MessageVariant Activity::dispatchRequestsUntilResponse( boost::asio::yield_context yield_ctx )
{
    MessageVariant msg;
    while ( true )
    {
        msg = receive( yield_ctx );
        if ( isRequest( msg ) )
        {
            dispatchRequestImpl( msg, yield_ctx );
        }
        else
        {
            break;
        }
    }
    return msg;
}

bool Activity::dispatchRequestImpl( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
{
    try
    {
        return dispatchRequest( msg, yield_ctx );
    }
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "Activity: {} exception: {}", getActivityID().getID(), ex.what() );
        error( getConnectionID( msg ), ex.what(), yield_ctx );
        return true;
    }
}

} // namespace network
} // namespace mega