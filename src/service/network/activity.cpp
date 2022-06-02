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
{
}

Activity::~Activity() {}

MessageVariant Activity::receive( boost::asio::yield_context& yield_ctx )
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
                                  SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg, ec.what() );
                                  THROW_RTE( "Failed to send request on channel: " << msg << " : " << ec.what() );
                              }
                          } );
}

void Activity::requestStarted( const ConnectionID& connectionID )
{
    //
    m_stack.push_back( connectionID );
}

void Activity::requestCompleted()
{
    VERIFY_RTE( !m_stack.empty() );
    m_stack.pop_back();
    if ( m_stack.empty() )
    {
        m_activityManager.activityCompleted( shared_from_this() );
    }
}

// run is ALWAYS call for each activity after it is created
void Activity::run( boost::asio::yield_context& yield_ctx )
{
    try
    {
        do
        {
            run_one( yield_ctx );
        } while ( !m_stack.empty() );
    }
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "Activity: {} exception: {}", getActivityID().getID(), ex.what() );
        m_activityManager.activityCompleted( shared_from_this() );
    }
}

void Activity::run_one( boost::asio::yield_context& yield_ctx )
{
    const MessageVariant msg = receive( yield_ctx );
    dispatchRequestImpl( msg, yield_ctx );
}

MessageVariant Activity::dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
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
    if ( msg.index() == MSG_Error_Response::ID )
    {
        throw std::runtime_error( std::get< MSG_Error_Response >( msg ).what );
    }
    return msg;
}

void Activity::dispatchRequestImpl( const MessageVariant& msg, boost::asio::yield_context& yield_ctx )
{
    Activity::RequestStack stack( *this, getConnectionID( msg ) );
    try
    {
        ASSERT( isRequest( msg ) );
        if ( !dispatchRequest( msg, yield_ctx ) )
        {
            SPDLOG_ERROR( "Failed to dispatch request: {} on activity: {}", msg, getActivityID().getID() );
            THROW_RTE( "Failed to dispatch request message: " << msg );
        }
    }
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "Activity: {} exception: {}", getActivityID().getID(), ex.what() );
        error( m_stack.back(), ex.what(), yield_ctx );
    }
}

} // namespace network
} // namespace mega