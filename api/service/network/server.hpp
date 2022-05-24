#ifndef SERVER_24_MAY_2022
#define SERVER_24_MAY_2022

#include "receiver.hpp"

#include "boost/asio/execution_context.hpp"
#include "boost/asio/thread_pool.hpp"
#include "boost/asio/strand.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/ip/tcp.hpp"

#include <string>
#include <memory>
#include <set>

namespace mega
{
namespace network
{
class Server
{
    using ExecutionContextType = boost::asio::thread_pool;

    class Connection
    {
    public:
        using Ptr    = std::shared_ptr< Connection >;
        using Strand = boost::asio::strand< ExecutionContextType::executor_type >;

        Connection( Server& server, ExecutionContextType& execution_context );

        Strand&                       getStrand() { return m_strand; }
        boost::asio::ip::tcp::socket& getSocket() { return m_socket; }
        const std::string&            getName() const { return m_strName; }

        void start();

    private:
        Server&                      m_server;
        Strand                       m_strand;
        boost::asio::ip::tcp::socket m_socket;
        boost::asio::steady_timer    m_watchDogTimer;
        Receiver                     m_receiver;
        std::string                  m_strName;
    };

public:
    Server( ExecutionContextType& execution_context );

    void waitForConnection();
    void onConnect( Connection::Ptr pNewConnection, const boost::system::error_code& ec );
    void onDisconnected( Connection::Ptr pConnection );

private:
    ExecutionContextType&          m_execution_context;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::set< Connection::Ptr >    m_connections;
};

} // namespace network
} // namespace mega

#endif // SERVER_24_MAY_2022
