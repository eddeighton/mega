
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

#include <boost/beast/http.hpp>

#include <functional>
#include <string>

namespace mega::httpserver
{

class NotFoundException : public std::runtime_error
{
public:
    NotFoundException( const std::string& strError )
        : std::runtime_error( strError )
    {
    }
};

using HTTPString     = boost::beast::http::string_body;
using ReportFunction = std::function< HTTPString::value_type(
    const boost::beast::http::request< HTTPString >&, boost::asio::yield_context& ) >;

void runHTPPServer( boost::asio::io_context&       ioc,
                    boost::asio::ip::tcp::endpoint endpoint,
                    ReportFunction&                reportCallback,
                    boost::asio::yield_context&    yield );

} // namespace mega::httpserver