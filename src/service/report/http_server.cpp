
#include "http_server.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/spawn.hpp>

#include <boost/config.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace mega::httpserver
{
namespace
{
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http  = beast::http;          // from <boost/beast/http.hpp>
namespace net   = boost::asio;          // from <boost/asio.hpp>
using tcp       = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type( beast::string_view path )
{
    using beast::iequals;
    auto const ext = [ &path ]
    {
        auto const pos = path.rfind( "." );
        if( pos == beast::string_view::npos )
            return beast::string_view{};
        return path.substr( pos );
    }();
    if( iequals( ext, ".htm" ) )
        return "text/html";
    if( iequals( ext, ".html" ) )
        return "text/html";
    if( iequals( ext, ".php" ) )
        return "text/html";
    if( iequals( ext, ".css" ) )
        return "text/css";
    if( iequals( ext, ".txt" ) )
        return "text/plain";
    if( iequals( ext, ".js" ) )
        return "application/javascript";
    if( iequals( ext, ".json" ) )
        return "application/json";
    if( iequals( ext, ".xml" ) )
        return "application/xml";
    if( iequals( ext, ".swf" ) )
        return "application/x-shockwave-flash";
    if( iequals( ext, ".flv" ) )
        return "video/x-flv";
    if( iequals( ext, ".png" ) )
        return "image/png";
    if( iequals( ext, ".jpe" ) )
        return "image/jpeg";
    if( iequals( ext, ".jpeg" ) )
        return "image/jpeg";
    if( iequals( ext, ".jpg" ) )
        return "image/jpeg";
    if( iequals( ext, ".gif" ) )
        return "image/gif";
    if( iequals( ext, ".bmp" ) )
        return "image/bmp";
    if( iequals( ext, ".ico" ) )
        return "image/vnd.microsoft.icon";
    if( iequals( ext, ".tiff" ) )
        return "image/tiff";
    if( iequals( ext, ".tif" ) )
        return "image/tiff";
    if( iequals( ext, ".svg" ) )
        return "image/svg+xml";
    if( iequals( ext, ".svgz" ) )
        return "image/svg+xml";
    return "application/text";
}

// Return a response for the given request.
//
// The concrete type of the response message (which depends on the
// request), is type-erased in message_generator.
template < class Body, class Allocator >
http::message_generator handle_request( ReportFunction&                                          reportCallback,
                                        http::request< Body, http::basic_fields< Allocator > >&& req,
                                        net::yield_context&                                      yield )
{
    // Returns a bad request response
    auto const bad_request = [ &req ]( beast::string_view why )
    {
        http::response< http::string_body > res{ http::status::bad_request, req.version() };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( req.keep_alive() );
        res.body() = std::string( why );
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [ &req ]( beast::string_view target )
    {
        http::response< http::string_body > res{ http::status::not_found, req.version() };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( req.keep_alive() );
        res.body() = "The resource '" + std::string( target ) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error = [ &req ]( beast::string_view what )
    {
        http::response< http::string_body > res{ http::status::internal_server_error, req.version() };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( req.keep_alive() );
        res.body() = "An error occurred: '" + std::string( what ) + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if( req.method() != http::verb::get && req.method() != http::verb::head )
    {
        return bad_request( "Unknown HTTP-method" );
    }

    try
    {
        http::string_body::value_type body = reportCallback( req, yield );

        // Cache the size since we need it after the move
        auto const size = body.size();

        // Respond to HEAD request
        if( req.method() == http::verb::head )
        {
            http::response< http::empty_body > res{ http::status::ok, req.version() };
            res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
            res.set( http::field::content_type, "text/html" );
            res.content_length( size );
            res.keep_alive( req.keep_alive() );
            return res;
        }
        else
        {
            // Respond to GET request
            http::response< http::string_body > res{ std::piecewise_construct, std::make_tuple( std::move( body ) ),
                                                     std::make_tuple( http::status::ok, req.version() ) };
            res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
            res.set( http::field::content_type, "text/html" );
            res.content_length( size );
            res.keep_alive( req.keep_alive() );
            return res;
        }
    }
    catch( NotFoundException& ex )
    {
        return not_found( ex.what() );
    }
    catch( std::exception& ex )
    {
        return server_error( ex.what() );
    }
    catch( ... )
    {
        return server_error( "Unknown error" );
    }
}

//------------------------------------------------------------------------------

// Report a failure
void fail( beast::error_code ec, char const* what )
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Handles an HTTP server connection
void do_session( beast::tcp_stream& stream, ReportFunction& reportCallback, net::yield_context& yield )
{
    beast::error_code ec;

    // This buffer is required to persist across reads
    beast::flat_buffer buffer;

    // This lambda is used to send messages
    for( ;; )
    {
        // Set the timeout.
        stream.expires_after( std::chrono::seconds( 30 ) );

        // Read a request
        http::request< http::string_body > req;
        http::async_read( stream, buffer, req, yield[ ec ] );
        if( ec == http::error::end_of_stream )
            break;
        if( ec )
            return fail( ec, "read" );

        // Handle the request
        http::message_generator msg = handle_request( reportCallback, std::move( req ), yield );

        // Determine if we should close the connection
        bool keep_alive = msg.keep_alive();

        // Send the response
        beast::async_write( stream, std::move( msg ), yield[ ec ] );

        if( ec )
            return fail( ec, "write" );

        if( !keep_alive )
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            break;
        }
    }

    // Send a TCP shutdown
    stream.socket().shutdown( tcp::socket::shutdown_send, ec );

    // At this point the connection is closed gracefully
}

} // namespace
//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
void runHTPPServer( net::io_context&    ioc,
                    tcp::endpoint       endpoint,
                    ReportFunction&     reportCallback,
                    net::yield_context& yield_ctx )
{
    beast::error_code ec;

    // Open the acceptor
    tcp::acceptor acceptor( ioc );
    acceptor.open( endpoint.protocol(), ec );
    if( ec )
        return fail( ec, "open" );

    // Allow address reuse
    acceptor.set_option( net::socket_base::reuse_address( true ), ec );
    if( ec )
        return fail( ec, "set_option" );

    // Bind to the server address
    acceptor.bind( endpoint, ec );
    if( ec )
        return fail( ec, "bind" );

    // Start listening for connections
    acceptor.listen( net::socket_base::max_listen_connections, ec );
    if( ec )
        return fail( ec, "listen" );

    for( ;; )
    {
        tcp::socket socket( ioc );
        acceptor.async_accept( socket, yield_ctx[ ec ] );
        if( ec )
        {
            fail( ec, "accept" );
        }
        else
        {
            boost::asio::spawn( acceptor.get_executor(),

                                [ &socket, &reportCallback ]( net::yield_context yield )
                                {
                                    beast::tcp_stream tcpStream( std::move( socket ) );
                                    do_session( tcpStream, reportCallback, yield );
                                }
                                //, boost::asio::detached
            );
        }
    }
}

} // namespace mega::httpserver
