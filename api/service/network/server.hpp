#ifndef SERVER_24_MAY_2022
#define SERVER_24_MAY_2022

#include "service/protocol/common/header.hpp"

#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/receiver.hpp"
#include "service/network/sender.hpp"

#include "boost/asio/strand.hpp"
#include "boost/asio/ip/tcp.hpp"

#include <string>
#include <memory>
#include <set>
#include <functional>

namespace mega
{
namespace network
{
class Server
{
public:
    class Connection : public std::enable_shared_from_this< Connection >, public Sender
    {
        friend class Server;

        using DisconnectCallback = std::function< void( const ConnectionID& connectionID ) >;

    public:
        using Ptr    = std::shared_ptr< Connection >;
        using Strand = boost::asio::strand< boost::asio::io_context::executor_type >;

        Connection( Server& server, boost::asio::io_context& ioContext, ConversationManager& conversationManager );
        ~Connection();

        const std::optional< Node::Type >& getTypeOpt() const { return m_typeOpt; }
        const std::optional< mega::MPO >&  getMPOOpt() const { return m_mpoOpt; }
        const std::string&                 getName() const { return m_strName; }

        void setType( Node::Type type ) { m_typeOpt = type; }
        void setMPO( mega::MPO mpo ) { m_mpoOpt = mpo; }

        // Sender
        virtual ConnectionID              getConnectionID() const { return m_pSender->getConnectionID(); }
        virtual boost::system::error_code send( const ConversationID& conversationID, const Message& msg,
                                                boost::asio::yield_context& yield_ctx )
        {
            return m_pSender->send( conversationID, msg, yield_ctx );
        }
        virtual void sendErrorResponse( const ConversationID& conversationID, const std::string& strErrorMsg,
                                        boost::asio::yield_context& yield_ctx )
        {
            m_pSender->sendErrorResponse( conversationID, strErrorMsg, yield_ctx );
        }

        template < typename TFunctor >
        void setDisconnectCallback( TFunctor&& functor )
        {
            VERIFY_RTE_MSG( !m_disconnectCallback.has_value(), "Duplicate disconnect callback set" );
            m_disconnectCallback = functor;
        }

    protected:
        const ConnectionID&           getSocketConnectionID() const { return m_connectionID.value(); }
        Strand&                       getStrand() { return m_strand; }
        boost::asio::ip::tcp::socket& getSocket() { return m_socket; }
        void                          start();
        void                          stop();
        void                          disconnected();

    private:
        Server&                             m_server;
        Strand                              m_strand;
        boost::asio::ip::tcp::socket        m_socket;
        SocketReceiver                      m_receiver;
        std::optional< ConnectionID >       m_connectionID;
        std::string                         m_strName;
        Sender::Ptr                         m_pSender;
        std::optional< Node::Type >         m_typeOpt;
        std::optional< DisconnectCallback > m_disconnectCallback;
        std::optional< mega::MPO >          m_mpoOpt;
    };

    using ConnectionMap = std::map< ConnectionID, Connection::Ptr >;

public:
    Server( boost::asio::io_context& ioContext, ConversationManager& conversationManager, short port );
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
    ConversationManager&           m_conversationManager;
    boost::asio::ip::tcp::acceptor m_acceptor;
    ConnectionMap                  m_connections;
};

} // namespace network
} // namespace mega

#endif // SERVER_24_MAY_2022
