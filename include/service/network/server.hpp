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
#include "logical_thread_manager.hpp"
#include "receiver.hpp"
#include "sender_factory.hpp"

#include "mega/values/service/logical_thread_id.hpp"

#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <memory>
#include <set>
#include <functional>
#include <variant>

namespace mega::network
{
class Server
{
public:
    class Connection : public std::enable_shared_from_this< Connection >
    {
        friend class Server;

        using DisconnectCallback = std::function< void() >;
        using Strand             = boost::asio::strand< boost::asio::io_context::executor_type >;

    public:
        using Label = std::variant< MachineID, MP, MPO >;
        using Ptr   = std::shared_ptr< Connection >;

        Connection( Server& server, boost::asio::io_context& ioContext, LogicalThreadManager& logicalthreadManager );

        bool        isSender( Sender::Ptr pSender ) { return m_pSender == pSender; }
        Sender::Ptr getSender() const { return m_pSender; }

        const std::optional< Node >&    getTypeOpt() const { return m_typeOpt; }
        const std::optional< Label >&   getLabel() const { return m_labelOpt; }

        void setType( Node type ) { m_typeOpt = type; }
        void setLabel( Label label ) { m_labelOpt = label; }

        template < typename TFunctor >
        void setDisconnectCallback( TFunctor&& functor )
        {
            VERIFY_RTE_MSG( !m_disconnectCallback.has_value(), "Duplicate disconnect callback set" );
            m_disconnectCallback = functor;
        }

    protected:
        Strand&         getStrand() { return m_strand; }
        Traits::Socket& getSocket() { return m_socket; }
        void            start();
        void            stop();
        void            disconnected();

    private:
        Server&                             m_server;
        Strand                              m_strand;
        Traits::Socket                      m_socket;
        SocketReceiver                      m_receiver;
        Sender::Ptr                         m_pSender;
        std::optional< Node >               m_typeOpt;
        std::optional< DisconnectCallback > m_disconnectCallback;
        std::optional< Label >              m_labelOpt;
    };

    using ConnectionMap = std::set< Connection::Ptr >;

public:
    Server( boost::asio::io_context& ioContext, LogicalThreadManager& logicalthreadManager, short port );

    const auto getEndPoint() const { return m_acceptor.local_endpoint(); }

    boost::asio::io_context& getIOContext() const { return m_ioContext; }
    const ConnectionMap&     getConnections() const { return m_connections; }

    Connection::Ptr getConnection( Sender::Ptr pSender )
    {
        for( auto pCon : m_connections )
        {
            if( pCon->isSender( pSender ) )
            {
                return pCon;
            }
        }
        return {};
    }

    using ConnectionLabelMap = std::map< Connection::Label, Connection::Ptr >;

    Connection::Ptr findConnection( Connection::Label label )
    {
        auto iFind = m_connectionLabels.find( label );
        if( iFind != m_connectionLabels.end() )
            return iFind->second;
        else
            return {};
    }

    void labelConnection( Connection::Label label, Connection::Ptr pConnection )
    {
        pConnection->setLabel( label );
        m_connectionLabels.insert( { label, pConnection } );
    }

    void unLabelConnection( Connection::Label label ) { m_connectionLabels.erase( label ); }

    struct ConnectionLabelRAII
    {
        Server&                          server;
        const Server::Connection::Label& label;
        Connection::Ptr                  pConnection;
        ConnectionLabelRAII( Server& server, const Server::Connection::Label& label, Connection::Ptr pConnection )
            : server( server )
            , label( label )
            , pConnection( pConnection )
        {
            server.labelConnection( label, pConnection );
        }
        ~ConnectionLabelRAII() { server.unLabelConnection( label ); }
    };

    void stop();
    void waitForConnection();
    void onConnect( Connection::Ptr pNewConnection, const boost::system::error_code& ec );
    void onDisconnected( Connection::Ptr pConnection );

private:
    boost::asio::io_context& m_ioContext;
    LogicalThreadManager&    m_logicalThreadManager;
    Traits::Acceptor         m_acceptor;
    ConnectionMap            m_connections;
    ConnectionLabelMap       m_connectionLabels;
};

} // namespace mega::network

#endif // SERVER_24_MAY_2022
