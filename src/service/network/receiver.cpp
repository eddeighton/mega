#include "service/network/receiver.hpp"
#include "service/network/activity_manager.hpp"
#include "service/network/end_point.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/serialisation.hpp"

#include "common/assert_verify.hpp"

#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/read.hpp>

#include <memory>
#include <iostream>

namespace mega
{
namespace network
{

Receiver::Receiver( ActivityManager& activityManager, ActivityFactory& activityFactory,
                    boost::asio::ip::tcp::socket& socket, std::function< void() > disconnectHandler )
    : m_activityManager( activityManager )
    , m_activityFactory( activityFactory )
    , m_socket( socket )
    , m_disconnectHandler( disconnectHandler )
{
    updateLastActivityTime();
}

void Receiver::updateLastActivityTime() { m_lastActivityTime = std::chrono::steady_clock::now(); }

void Receiver::receive( boost::asio::yield_context yield_ctx )
{
    static const std::size_t            MessageSizeSize = sizeof( network::MessageSize );
    boost::asio::streambuf              streamBuffer;
    boost::system::error_code           ec;
    std::size_t                         szBytesTransferred = 0U;
    std::array< char, MessageSizeSize > buf;
    Header                              header;

    while ( m_bContinue && m_socket.is_open() )
    {
        // read message size
        network::MessageSize size = 0U;
        {
            while ( m_bContinue && m_socket.is_open() )
            {
                szBytesTransferred = boost::asio::async_read( m_socket, boost::asio::buffer( buf ), yield_ctx[ ec ] );
                if ( !ec )
                {
                    if ( szBytesTransferred == MessageSizeSize )
                    {
                        size = ntohl( *reinterpret_cast< const network::MessageSize* >( buf.data() ) );
                        break;
                    }
                    else
                    {
                        //THROW_RTE( "Failed to read message size" );
                        m_socket.close();
                    }
                }
                else // if( ec.failed() )
                {
                    if ( ec == boost::asio::error::eof )
                    {
                        std::cout << "Socket closed" << std::endl;
                        m_bContinue = false;
                        break;
                    }
                    // log error and close
                    //THROW_RTE( "Socket error: " << ec.what() );
                    m_socket.close();
                }
            }
        }

        // read message
        while ( m_bContinue && m_socket.is_open() )
        {
            szBytesTransferred = boost::asio::async_read( m_socket, streamBuffer.prepare( size ), yield_ctx[ ec ] );
            if ( !ec )
            {
                VERIFY_RTE( size == szBytesTransferred );
                streamBuffer.commit( size );

                {
                    boost::archive::binary_iarchive ia( streamBuffer );

                    ia& header;

                    Activity::Ptr pActivity = m_activityManager.findExistingActivity( header.getActivityID() );
                    if ( !pActivity )
                    {
                        pActivity = m_activityFactory.createRequestActivity(
                            header.getActivityID(), getConnectionID( m_socket ) );
                        m_activityManager.activityStarted( pActivity );
                        std::cout << "Receive to new: " << header << std::endl;
                    }
                    else
                    {
                        std::cout << "Receive to existing: " << header << std::endl;
                    }

                    decode( ia, header, pActivity );
                }

                streamBuffer.consume( size );

                updateLastActivityTime();
                break;
            }
            else // if( ec.failed() )
            {
                if ( ec == boost::asio::error::eof )
                {
                    std::cout << "Socket closed" << std::endl;
                    m_bContinue = false;
                    break;
                }
                // log error and close
                //THROW_RTE( "Socket error: " << ec.what() );
                m_socket.close();
            }
        }
    }
    m_disconnectHandler();
}

} // namespace network
} // namespace mega
