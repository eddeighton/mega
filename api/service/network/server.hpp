//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef SERVER_24_MAY_2022
#define SERVER_24_MAY_2022

#include "network.hpp"
#include "conversation_manager.hpp"
#include "receiver.hpp"
#include "sender.hpp"

#include "service/protocol/common/header.hpp"

#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <memory>
#include <set>
#include <functional>

namespace mega::network
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
        const std::optional< mega::MP >&   getMPOpt() const { return m_mpOpt; }
        const std::string&                 getName() const { return m_strName; }

        void setType( Node::Type type ) { m_typeOpt = type; }
        void setMP( mega::MP mp ) { m_mpOpt = mp; }

        // Sender
        virtual ConnectionID              getConnectionID() const { return m_pSender->getConnectionID(); }
        virtual boost::system::error_code send( const Message& msg, boost::asio::yield_context& yield_ctx )
        {
            return m_pSender->send( msg, yield_ctx );
        }
        virtual void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                        boost::asio::yield_context& yield_ctx )
        {
            m_pSender->sendErrorResponse( msg, strErrorMsg, yield_ctx );
        }

        template < typename TFunctor >
        void setDisconnectCallback( TFunctor&& functor )
        {
            VERIFY_RTE_MSG( !m_disconnectCallback.has_value(), "Duplicate disconnect callback set" );
            m_disconnectCallback = functor;
        }

    protected:
        const ConnectionID& getSocketConnectionID() const { return m_connectionID.value(); }
        Strand&             getStrand() { return m_strand; }
        Traits::Socket&     getSocket() { return m_socket; }
        void                start();
        void                stop();
        void                disconnected();

    private:
        Server&                             m_server;
        Strand                              m_strand;
        Traits::Socket                      m_socket;
        SocketReceiver                      m_receiver;
        std::optional< ConnectionID >       m_connectionID;
        std::string                         m_strName;
        Sender::Ptr                         m_pSender;
        std::optional< Node::Type >         m_typeOpt;
        std::optional< DisconnectCallback > m_disconnectCallback;
        std::optional< mega::MP >           m_mpOpt;
    };

    using ConnectionMap    = std::map< ConnectionID, Connection::Ptr >;
    using ConnectionMPOMap = std::unordered_map< mega::MPO, Connection::Ptr, mega::MPO::Hash >;
    using ConnectionMPMap  = std::unordered_map< mega::MP, Connection::Ptr, mega::MP::Hash >;

public:
    Server( boost::asio::io_context& ioContext, ConversationManager& conversationManager, short port );

    boost::asio::io_context& getIOContext() const { return m_ioContext; }
    Connection::Ptr          getConnection( const ConnectionID& connectionID );
    const ConnectionMap&     getConnections() const { return m_connections; }
    Connection::Ptr          findConnection( const mega::MPO& mpo ) const;
    Connection::Ptr          findConnection( const mega::MP& mp ) const;
    void                     mapConnection( const mega::MPO& mpo, Connection::Ptr pConnection );
    void                     unmapConnection( const mega::MPO& mpo );
    void                     mapConnection( const mega::MP& mp, Connection::Ptr pConnection );
    void                     unmapConnection( const mega::MP& mp );

    struct MPOConnection
    {
        Server&          server;
        const mega::MPO& mpo;
        Connection::Ptr  pConnection;
        MPOConnection( Server& server, const mega::MPO& mpo, Connection::Ptr pConnection )
            : server( server )
            , mpo( mpo )
            , pConnection( pConnection )
        {
            server.mapConnection( mpo, pConnection );
        }
        ~MPOConnection() { server.unmapConnection( mpo ); }
    };

    void stop();
    void waitForConnection();
    void onConnect( Connection::Ptr pNewConnection, const boost::system::error_code& ec );
    void onDisconnected( Connection::Ptr pConnection );

private:
    boost::asio::io_context& m_ioContext;
    ConversationManager&     m_conversationManager;
    Traits::Acceptor         m_acceptor;
    ConnectionMap            m_connections;
    ConnectionMPOMap         m_mpoMap;
    ConnectionMPMap          m_mpMap;
};

} // namespace mega::network

#endif // SERVER_24_MAY_2022
