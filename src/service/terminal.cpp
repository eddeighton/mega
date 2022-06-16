
#include "service/terminal.hpp"

#include "pipeline/configuration.hpp"

#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/term_leaf.hxx"

#include "common/requireSemicolon.hpp"

#include "spdlog/spdlog.h"

#include "boost/asio/use_future.hpp"
#include "boost/system/detail/error_code.hpp"

#include <exception>
#include <optional>
#include <future>

namespace mega
{
namespace service
{

class TerminalRequestConversation : public network::Conversation, public network::leaf_term::Impl
{
protected:
    Terminal& m_terminal;

public:
    TerminalRequestConversation( Terminal& terminal, const network::ConversationID& conversationID,
                                 const network::ConnectionID& originatingConnectionID )
        : Conversation( terminal, conversationID, originatingConnectionID )
        , m_terminal( terminal )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_term::Impl::dispatchRequest( msg, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
    {
        if ( m_terminal.getLeafSender().getConnectionID() == connection )
        {
            m_terminal.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Terminal Critical error in error handler" );
        }
    }

    network::leaf_term::Response_Encode getLeafResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_term::Response_Encode( *this, m_terminal.getLeafSender(), yield_ctx );
    }

    virtual void RootListNetworkNodes( boost::asio::yield_context& yield_ctx )
    {
        getLeafResponse( yield_ctx ).RootListNetworkNodes( { m_terminal.getProcessName() } );
    }
};

template < typename TConversationFunctor >
class GenericConversation : public TerminalRequestConversation
{
    TConversationFunctor m_functor;

public:
    GenericConversation( Terminal& terminal, const network::ConversationID& conversationID,
                         const network::ConnectionID& originatingConnectionID, TConversationFunctor&& functor )
        : TerminalRequestConversation( terminal, conversationID, originatingConnectionID )
        , m_functor( functor )
    {
    }

    void run( boost::asio::yield_context& yield_ctx )
    {
        Conversation::RequestStack stack( "GenericConversation", *this, m_terminal.getLeafSender().getConnectionID() );
        m_functor( *this, m_terminal.getLeafSender(), yield_ctx );
    }
};

Terminal::Terminal( std::optional< const std::string > optName /* = std::nullopt*/ )
    : network::ConversationManager( network::Node::toStr( network::Node::Terminal ), m_io_context )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf( m_receiverChannel.getSender(), network::Node::Terminal )
{
    std::ostringstream os;
    os << network::Node::toStr( network::Node::Terminal ) << "_" << std::this_thread::get_id();
    network::ConnectionID connectionID = os.str();
    m_receiverChannel.run( connectionID );
}

Terminal::~Terminal()
{
    m_receiverChannel.stop();
    m_io_context.run();
}
/*
bool Terminal::Shutdown()
{
    SIMPLE_REQUEST( bool, daemon.Shutdown();
                    boost::asio::post( [ &promise = promise ]() { promise.set_value( true ); } ); );
}
*/
void Terminal::shutdown() { m_receiverChannel.stop(); }

network::PipelineResult Terminal::PipelineRun( const mega::pipeline::Configuration& pipelineConfig )
{
    std::optional< network::PipelineResult > result;

    {
        auto func = [ &result, &pipelineConfig ](
                        network::ConversationBase& con, network::Sender& sender, boost::asio::yield_context& yield_ctx )
        {
            network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );
            result = leaf.TermPipelineRun( pipelineConfig );
        };
        conversationStarted( network::ConversationBase::Ptr(
            new GenericConversation( *this, createConversationID( getLeafSender().getConnectionID() ),
                                     getLeafSender().getConnectionID(), std::move( func ) ) ) );
    }
    while ( !result.has_value() )
        m_io_context.run_one();
    return result.value();
}

network::ConversationBase::Ptr Terminal::joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                           const network::Header&         header,
                                                           const network::MessageVariant& msg )
{
    return network::Conversation::Ptr(
        new TerminalRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
}

std::vector< std::string > Terminal::listNetworkNodes()
{
    std::optional< std::vector< std::string > > result;
    {
        auto func = [ &result ](
                        network::ConversationBase& con, network::Sender& sender, boost::asio::yield_context& yield_ctx )
        {
            network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );
            result = leaf.TermListNetworkNodes();
        };
        conversationStarted( network::ConversationBase::Ptr(
            new GenericConversation( *this, createConversationID( getLeafSender().getConnectionID() ),
                                     getLeafSender().getConnectionID(), std::move( func ) ) ) );
    }
    while ( !result.has_value() )
        m_io_context.run_one();
    return result.value();
}

} // namespace service
} // namespace mega
