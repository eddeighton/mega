
#include "service/network/network.hpp"

#include "common/assert_verify.hpp"

#include "boost/bind/bind.hpp"

#include <boost/asio/read_until.hpp>
#include <boost/asio/ts/netfwd.hpp>
#include <iostream>
#include <iterator>

namespace mega
{
namespace network
{
const char* getVersion() { return "0.0.0.0"; }

short MegaDaemonPort() { return 4237; }

const char* MegaDaemonServiceName() { return "megadaemon"; }

short MegaRootPort() { return 4238; }

const char* MegaRootServiceName() { return "megaroot"; }

/*
Receiver::Receiver( ASIOQueueType& io_context, boost::asio::ip::tcp::socket& socket, MessageHandler& messageHandler )
    : m_io_context( io_context )
    , m_socket( socket )
    , m_messageHandler( messageHandler )
{
    updateLastActivityTime();
}

void Receiver::updateLastActivityTime() { m_lastActivityTime = std::chrono::steady_clock::now(); }

void Receiver::receive( boost::asio::yield_context yield_ctx )
{
    // std::shared_ptr< boost::array< char, sizeof( size_t ) > > pBuffer( new boost::array< char, sizeof( size_t ) > );

    // std::shared_ptr< HelloMsg > pMsg = std::make_shared< HelloMsg >();
    while ( m_socket.is_open() )
    {
        boost::system::error_code ec;
        std::size_t szBytesTransferred = m_socket.async_read_some( m_streamBuffer.prepare( 128 ), yield_ctx[ ec ] );

        if ( !ec )
        {
            m_streamBuffer.commit( szBytesTransferred );
            while ( m_streamBuffer.in_avail() >= sizeof( HelloMsg ) )
            {
                HelloMsg     msg;
                std::istream is( &m_streamBuffer );

                is.read( msg.msg.data(), 128 );

                // m_streamBuffer.consume( 128U );
                std::string str = msg.msg.data();
                VERIFY_RTE_MSG( str == TEST_MESSAGE, "Bad data: " << str << " : " << TEST_MESSAGE );
                m_messageHandler.onHelloRequest( msg );
            }

            updateLastActivityTime();

            m_io_context.post( yield_ctx );
        }
        else
        {
            THROW_RTE( "Failed to read" );
        }
    }

    // boost::bind( &Receiver::onReadSizeHandler, shared_from_this(), boost::asio::placeholders::error,
    //              boost::asio::placeholders::bytes_transferred ) );
}


Sender::Sender( boost::asio::ip::tcp::socket& socket )
    : m_socket( socket )
{
}

void Sender::onWriteHandler( const boost::system::error_code& ec, std::size_t szBytesTransferred,
                             std::shared_ptr< HelloMsg > pMsg )
{
    if ( !ec )
    {
        // success...
        VERIFY_RTE_MSG( sizeof( HelloMsg ) == szBytesTransferred,
                        "Size written not expected: " << sizeof( HelloMsg ) << " : " << szBytesTransferred );
    }
    else if ( ec.failed() )
    {
        std::cout << "onWriteHandler Error: " << ec.what() << std::endl;
    }
}

void Sender::send_hello_request( const std::string& strMsg )
{
    std::shared_ptr< HelloMsg > pMsg = std::make_shared< HelloMsg >();
    pMsg->msg                        = { 0 };
    std::copy( strMsg.begin(), strMsg.end(), pMsg->msg.begin() );

    boost::asio::async_write(
        m_socket, boost::asio::buffer( pMsg->msg ),
        boost::bind( &Sender::onWriteHandler, shared_from_this(), boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred, pMsg ) );

    mega::HelloRequest hello;
    {
        hello.set_msg( strMsg );
    }

    mega::Header header;
    {
        header.set_state( mega::Header_State_Request );
        header.set_type( mega::Header_Type_Hello );
        header.set_sender( 0 );
        header.set_activity( 0 );
        header.set_id( 0 );
        header.set_size( hello.ByteSizeLong() );
    }

    std::shared_ptr< std::vector< char > > pBuffer(
        new std::vector< char >( sizeof( size_t ) + header.ByteSizeLong() + hello.ByteSizeLong() ) );

    // write the 4 byte size for header size
    {
        const std::size_t uiSize = htonl( header.ByteSizeLong() );
        const char*       pFrom  = reinterpret_cast< const char* >( &uiSize );
        std::copy( pFrom, pFrom + sizeof( size_t ), reinterpret_cast< char* >( pBuffer->data() ) );
    }

    // write the header
    header.SerializeToArray( pBuffer->data() + sizeof( size_t ), header.ByteSizeLong() );

    // write the payload
    hello.SerializeToArray( pBuffer->data() + sizeof( size_t ) + header.ByteSizeLong(), hello.ByteSizeLong() );

    boost::asio::async_write(
        m_socket, boost::asio::buffer( *pBuffer ),
        boost::bind( &Sender::onWriteHandler, shared_from_this(), boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred, pBuffer ) );
}

void Sender::send_hello_response( const std::string& strMsg )
{
    VERIFY_RTE( strMsg.size() < sizeof( HelloMsg ) );

    std::shared_ptr< HelloMsg > pMsg = std::make_shared< HelloMsg >();
    pMsg->msg                        = { 0 };
    std::copy( strMsg.begin(), strMsg.end(), pMsg->msg.begin() );

    boost::asio::async_write(
        m_socket, boost::asio::buffer( pMsg->msg ),
        boost::bind( &Sender::onWriteHandler, shared_from_this(), boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred, pMsg ) );
    
mega::HelloResponse hello;
{
hello.set_msg( strMsg );
}

mega::Header header;
{
header.set_state( mega::Header_State_Completion );
header.set_type( mega::Header_Type_Hello );
header.set_sender( 0 );
header.set_activity( 0 );
header.set_id( 0 );
header.set_size( hello.ByteSizeLong() );
}

std::shared_ptr< std::vector< char > > pBuffer(
new std::vector< char >( sizeof( size_t ) + header.ByteSizeLong() + hello.ByteSizeLong() ) );

// write the 4 byte size for header size
{
const std::size_t uiSize = htonl( header.ByteSizeLong() );
const char*       pFrom  = reinterpret_cast< const char* >( &uiSize );
std::copy( pFrom, pFrom + sizeof( size_t ), reinterpret_cast< char* >( pBuffer->data() ) );
}

// write the header
header.SerializeToArray( pBuffer->data() + sizeof( size_t ), header.ByteSizeLong() );

// write the payload
hello.SerializeToArray( pBuffer->data() + sizeof( size_t ) + header.ByteSizeLong(), hello.ByteSizeLong() );

boost::asio::async_write(
m_socket, boost::asio::buffer( *pBuffer ),
boost::bind( &Sender::onWriteHandler, shared_from_this(), boost::asio::placeholders::error,
    boost::asio::placeholders::bytes_transferred, pBuffer ) );
}

void Sender::send_heartbeat_request()
{
    mega::HeartBeatRequest heartbeat;
    {
        heartbeat.set_time( 0 );
    }

    mega::Header header;
    {
        header.set_state( mega::Header_State_Request );
        header.set_type( mega::Header_Type_HeartBeat );
        header.set_sender( 0 );
        header.set_activity( 0 );
        header.set_id( 0 );
        header.set_size( heartbeat.ByteSizeLong() );
    }

    std::shared_ptr< std::vector< char > > pBuffer(
        new std::vector< char >( sizeof( size_t ) + header.ByteSizeLong() + heartbeat.ByteSizeLong() ) );

    // write the 4 byte size for header size
    {
        const std::size_t uiSize = htonl( header.ByteSizeLong() );
        const char*       pFrom  = reinterpret_cast< const char* >( &uiSize );
        std::copy( pFrom, pFrom + sizeof( size_t ), reinterpret_cast< char* >( pBuffer->data() ) );
    }

    // write the header
    header.SerializeToArray( pBuffer->data() + sizeof( size_t ), header.ByteSizeLong() );

    // write the payload
    heartbeat.SerializeToArray( pBuffer->data() + sizeof( size_t ) + header.ByteSizeLong(), heartbeat.ByteSizeLong() );

    boost::asio::async_write(
        m_socket, boost::asio::buffer( *pBuffer ),
        boost::bind( &Sender::onWriteHandler, shared_from_this(), boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred, pBuffer ) );
}

void Sender::send_heartbeat_response()
{
    mega::HeartBeatResponse heartbeat;
    {
        heartbeat.set_time( 0 );
    }

    mega::Header header;
    {
        header.set_state( mega::Header_State_Completion );
        header.set_type( mega::Header_Type_HeartBeat );
        header.set_sender( 0 );
        header.set_activity( 0 );
        header.set_id( 0 );
        header.set_size( heartbeat.ByteSizeLong() );
    }

    std::shared_ptr< std::vector< char > > pBuffer(
        new std::vector< char >( sizeof( size_t ) + header.ByteSizeLong() + heartbeat.ByteSizeLong() ) );

    // write the 4 byte size for header size
    {
        const std::size_t uiSize = htonl( header.ByteSizeLong() );
        const char*       pFrom  = reinterpret_cast< const char* >( &uiSize );
        std::copy( pFrom, pFrom + sizeof( size_t ), reinterpret_cast< char* >( pBuffer->data() ) );
    }

    // write the header
    header.SerializeToArray( pBuffer->data() + sizeof( size_t ), header.ByteSizeLong() );

    // write the payload
    heartbeat.SerializeToArray( pBuffer->data() + sizeof( size_t ) + header.ByteSizeLong(), heartbeat.ByteSizeLong() );

    boost::asio::async_write(
        m_socket, boost::asio::buffer( *pBuffer ),
        boost::bind( &Sender::onWriteHandler, shared_from_this(), boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred, pBuffer ) );
}
*/

} // namespace network
} // namespace mega
