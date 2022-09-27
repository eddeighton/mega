
#include "service/leaf.hpp"

#include "request.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/enrole.hxx"

#include "service/protocol/common/header.hpp"

#include "mega/common.hpp"

#include "runtime/mpo_context.hpp"

#include "common/requireSemicolon.hpp"

#include "boost/system/detail/error_code.hpp"

namespace mega
{
namespace service
{

class LeafEnrole : public LeafRequestConversation
{
    std::promise< void >& m_promise;

public:
    LeafEnrole( Leaf& leaf, const network::ConnectionID& originatingConnectionID, std::promise< void >& promise )
        : LeafRequestConversation( leaf, leaf.createConversationID( originatingConnectionID ), originatingConnectionID )
        , m_promise( promise )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        network::enrole::Request_Encoder encoder(
            [ daemonSender = getDaemonSender( yield_ctx ) ]( const network::Message& msg ) mutable
            { return daemonSender.LeafDaemon( msg ); } );
        m_leaf.m_mp = encoder.EnroleLeafWithDaemon( m_leaf.getType() );
        SPDLOG_TRACE( "Leaf enrole mp: {}", m_leaf.m_mp );
        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
    }
};

Leaf::Leaf( network::Sender::Ptr pSender, network::Node::Type nodeType )
    : network::ConversationManager( network::makeProcessName( network::Node::Leaf ), m_io_context )
    , m_pSender( std::move( pSender ) )
    , m_nodeType( nodeType )
    , m_io_context( 1 ) // single threaded concurrency hint
    , m_receiverChannel( m_io_context, *this )
    , m_client( m_io_context, *this, "localhost", mega::network::MegaDaemonServiceName() )
    , m_work_guard( m_io_context.get_executor() )
    , m_io_thread( [ &io_context = m_io_context ]() { io_context.run(); } )
{
    m_receiverChannel.run( network::makeProcessName( network::Node::Leaf ) );

    m_pSelfSender = m_receiverChannel.getSender();

    {
        std::promise< void > promise;
        std::future< void >  future = promise.get_future();
        conversationInitiated(
            std::make_shared< LeafEnrole >( *this, getDaemonSender().getConnectionID(), promise ), getDaemonSender() );
        future.get();
    }
}

Leaf::~Leaf()
{
    m_client.stop();
    m_receiverChannel.stop();
    m_work_guard.reset();
    m_io_thread.join();
}
/*
void Leaf::shutdown()
{
    m_client.stop();
    m_receiverChannel.stop();
}
*/
// network::ConversationManager
network::ConversationBase::Ptr Leaf::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Header&       header,
                                                       const network::Message&      msg )
{
    switch ( m_nodeType )
    {
        case network::Node::Leaf:
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Executor:
            return network::ConversationBase::Ptr(
                new LeafRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
            break;
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::TOTAL_NODE_TYPES:
        default:
            THROW_RTE( "Leaf: Unknown leaf type" );
            break;
    }
}

} // namespace service
} // namespace mega
