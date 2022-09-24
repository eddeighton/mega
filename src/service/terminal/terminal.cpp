
#include "service/terminal.hpp"

#include "request.hpp"

#include "pipeline/configuration.hpp"

#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/term_leaf.hxx"
#include "service/protocol/model/pipeline.hxx"
#include "service/protocol/model/status.hxx"

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
    void run( boost::asio::yield_context& yield_ctx ) { m_functor( *this, m_terminal.getLeafSender(), yield_ctx ); }
};

Terminal::Terminal( std::optional< const std::string > optName /* = std::nullopt*/ )
    : network::ConversationManager( network::makeProcessName( network::Node::Terminal ), m_io_context )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf(
          // NOTE: must ensure the receiver connectionID is initialised before calling getSender
          [ &m_receiverChannel = m_receiverChannel ]()
          {
              m_receiverChannel.run( network::makeProcessName( network::Node::Terminal ) );
              return m_receiverChannel.getSender();
          }(),
          network::Node::Terminal )
{
}

Terminal::~Terminal()
{
    m_receiverChannel.stop();
    m_io_context.run();
}

void Terminal::shutdown() { m_receiverChannel.stop(); }

network::ConversationBase::Ptr Terminal::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                           const network::Header&       header,
                                                           const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new TerminalRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
}

network::Message Terminal::rootRequest( const network::Message& message )
{
    SPDLOG_TRACE( "Terminal::rootRequest" );
    class RootConversation : public TerminalRequestConversation
    {
    public:
        using ResultType = std::optional< std::variant< network::Message, std::exception_ptr > >;

        RootConversation( Terminal& terminal, const network::ConversationID& conversationID,
                          const network::ConnectionID& originatingConnectionID, const network::Message& msg,
                          ResultType& result )
            : TerminalRequestConversation( terminal, conversationID, originatingConnectionID )
            , m_message( msg )
            , m_result( result )
        {
            SPDLOG_TRACE( "Terminal::rootRequest::RootConversation::ctor {}", network::getMsgName( m_message ) );
        }
        void run( boost::asio::yield_context& yield_ctx )
        {
            try
            {
                SPDLOG_TRACE( "Terminal::rootRequest::RootConversation" );
                network::term_leaf::Request_Sender leaf( *this, m_terminal.getLeafSender(), yield_ctx );
                m_result = leaf.TermRoot( m_message );
            }
            catch ( std::exception& ex )
            {
                m_result = std::current_exception();
            }
        }

    private:
        network::Message m_message;
        ResultType&      m_result;
    };

    RootConversation::ResultType result;
    {
        auto& sender         = getLeafSender();
        auto  connectionID   = sender.getConnectionID();
        auto  conversationID = createConversationID( connectionID );
        conversationInitiated( network::ConversationBase::Ptr(
                                   new RootConversation( *this, conversationID, connectionID, message, result ) ),
                               sender );
    }

    while ( !result.has_value() )
        m_io_context.run_one();

    if ( result->index() == 1 )
    {
        std::rethrow_exception( std::get< std::exception_ptr >( result.value() ) );
    }
    else
    {
        return std::get< network::Message >( result.value() );
    }
}

network::Status Terminal::GetNetworkStatus()
{
    return getRootRequest< network::status::Request_Encoder >().GetNetworkStatus();
}

network::PipelineResult Terminal::PipelineRun( const pipeline::Configuration& pipelineConfig )
{
    return getRootRequest< network::pipeline::Request_Encoder >().PipelineRun( pipelineConfig );
}

} // namespace service
} // namespace mega
