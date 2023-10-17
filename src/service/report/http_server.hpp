
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

#include <boost/beast/http.hpp>

#include <functional>
#include <string>

namespace mega::httpserver
{

boost::beast::string_view mime_type( boost::beast::string_view path );

class NotFoundException : public std::runtime_error
{
public:
    NotFoundException( const std::string& strError )
        : std::runtime_error( strError )
    {
    }
};

using ReportFactory = std::function< void( boost::asio::ip::tcp::socket& ) >;

void runHTPPServer( boost::asio::io_context&       ioc,
                    boost::asio::ip::tcp::endpoint endpoint,
                    ReportFactory&                 reportFactory,
                    boost::asio::yield_context&    yield );

} // namespace mega::httpserver