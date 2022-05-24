#include "service/network/receiver.hpp"
#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <memory>

namespace mega
{
namespace network
{

Receiver::Receiver( boost::asio::ip::tcp::socket& socket )
    : m_socket( socket )
{
    updateLastActivityTime();
}

void Receiver::updateLastActivityTime() { m_lastActivityTime = std::chrono::steady_clock::now(); }

void Receiver::receive( boost::asio::yield_context yield_ctx )
{
    while ( m_socket.is_open() )
    {
        boost::system::error_code ec;
        const std::size_t         szBytesTransferred
            = m_socket.async_read_some( m_streamBuffer.prepare( 128 ), yield_ctx[ ec ] );

        if ( !ec )
        {
            m_streamBuffer.commit( szBytesTransferred );
            /*while ( m_streamBuffer.in_avail() >= sizeof( HelloMsg ) )
            {
                HelloMsg     msg;
                std::istream is( &m_streamBuffer );

                is.read( msg.msg.data(), 128 );

                // m_streamBuffer.consume( 128U );
                std::string str = msg.msg.data();
                VERIFY_RTE_MSG( str == TEST_MESSAGE, "Bad data: " << str << " : " << TEST_MESSAGE );
                m_messageHandler.onHelloRequest( msg );
            }*/

            // boost::asio::thread_pool tp;
            // tp.scheduler().post(

            // boost::asio::io_context ioc(
            // const boost::asio::io_context& ioc = dynamic_cast< const boost::asio::io_context& >(
            // *m_executor.context() );

            // m_executor.context()

            // return async_initiate<LegacyCompletionHandler, void ()>(
            //     initiate_post(), handler, this);

            updateLastActivityTime();

            //boost::asio::post( yield_ctx );

            // m_executor.execute( yield_ctx );
            // boost::asio::io_context ioc;
            // ioc.post( yield_ctx );

            // m_executor.execute( yield_ctx );
        }
        else
        {
            m_socket.close();
        }
    }
}

} // namespace network
} // namespace mega
