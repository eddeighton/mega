
#include "http_server.hpp"

#include "common/assert_verify.hpp"

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


} // namespace
//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
void runHTPPServer( net::io_context&    ioc,
                    tcp::endpoint       endpoint,
                    ReportFactory&      reportFactory,
                    net::yield_context& yield_ctx )
{
    beast::error_code ec;

    // Open the acceptor
    tcp::acceptor acceptor( ioc );
    acceptor.open( endpoint.protocol(), ec );
    if( ec )
    {
        THROW_RTE( "Failed to open endpoint: " << ec.message() );
    }

    // Allow address reuse
    acceptor.set_option( net::socket_base::reuse_address( true ), ec );
    if( ec )
    {
        THROW_RTE( "Failed to reuse address: " << ec.message() );
    }

    // Bind to the server address
    acceptor.bind( endpoint, ec );
    if( ec )
    {
        THROW_RTE( "Failed to bind endpoint: " << ec.message() );
    }

    // Start listening for connections
    acceptor.listen( net::socket_base::max_listen_connections, ec );
    if( ec )
    {
        THROW_RTE( "Failed to listen on endpoint: " << ec.message() );
    }

    for( ;; )
    {
        tcp::socket socket( ioc );
        acceptor.async_accept( socket, yield_ctx[ ec ] );
        if( ec )
        {
            THROW_RTE( "Failed to accept response: " << ec.message() );
        }
        else
        {
            reportFactory( socket );
        }
    }
}

} // namespace mega::httpserver
