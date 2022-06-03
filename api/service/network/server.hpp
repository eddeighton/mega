#ifndef SERVER_24_MAY_2022
#define SERVER_24_MAY_2022

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/host_daemon.hxx"

#include "service/network/activity.hpp"
#include "service/network/activity_manager.hpp"
#include "service/network/receiver.hpp"

#include "boost/asio/strand.hpp"
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
public:
    class Connection : public std::enable_shared_from_this< Connection >
    {
        friend class Server;

    public:
        using Ptr    = std::shared_ptr< Connection >;
        using Strand = boost::asio::strand< boost::asio::io_context::executor_type >;

        Connection( Server& server, boost::asio::io_context& ioContext, ActivityManager& activityManager,
                    ActivityFactory& activityFactory );
        ~Connection();

        Strand&                       getStrand() { return m_strand; }
        boost::asio::ip::tcp::socket& getSocket() { return m_socket; }
        const std::string&            getName() const { return m_strName; }

    protected:
        void start();
        void stop();
        void disconnected();

    private:
        Server&                      m_server;
        Strand                       m_strand;
        boost::asio::ip::tcp::socket m_socket;
        Receiver                     m_receiver;
        std::string                  m_strName;
    };

    using ConnectionMap = std::map< ConnectionID, Connection::Ptr >;

public:
    Server( boost::asio::io_context& ioContext, ActivityManager& activityManager, ActivityFactory& activityFactory,
            short port );
    ~Server();

    boost::asio::io_context& getIOContext() const { return m_ioContext; }
    Connection::Ptr          getConnection( const ConnectionID& connectionID );
    const ConnectionMap&     getConnections() const { return m_connections; }

    void stop();
    void waitForConnection();
    void onConnect( Connection::Ptr pNewConnection, const boost::system::error_code& ec );
    void onDisconnected( Connection::Ptr pConnection );

private:
    boost::asio::io_context&       m_ioContext;
    ActivityManager&               m_activityManager;
    ActivityFactory&               m_activityFactory;
    boost::asio::ip::tcp::acceptor m_acceptor;
    ConnectionMap                  m_connections;
};

} // namespace network
} // namespace mega

#endif // SERVER_24_MAY_2022
