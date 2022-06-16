
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
/*
class TerminalRequestConversation : public network::Conversation, public network::daemon_terminal::Impl
{
    Terminal& m_terminal;

public:
    TerminalRequestConversation( Terminal& terminal, const network::ConversationID& conversationID,
                         const network::ConnectionID& originatingConnectionID )
        : Conversation( terminal.m_conversationManager, conversationID, originatingConnectionID )
        , m_terminal( terminal )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_terminal::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
    {
        if ( network::getConnectionID( m_terminal.m_client.getSocket() ) == connection )
        {
            network::sendErrorResponse( getConversationID(), m_terminal.m_client.getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Terminal Critical error in error handler" );
        }
    }

    network::daemon_terminal::Response_Encode getDaemonResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_terminal::Response_Encode( *this, m_terminal.m_client.getSocket(), yield_ctx );
    }

    virtual void ReportActivities( boost::asio::yield_context& yield_ctx )
    {
        std::vector< network::ConversationID > activities;

        for ( const auto& id : m_conversationManager.reportActivities() )
        {
            activities.push_back( id );
        }

        getDaemonResponse( yield_ctx ).ReportActivities( activities );
    }
    virtual void ExecuteShutdown( boost::asio::yield_context& yield_ctx )
    {
        getDaemonResponse( yield_ctx ).ExecuteShutdown();

        boost::asio::post( [ &terminal = m_terminal ]() { terminal.shutdown(); } );
    }
};

template < typename TResultType, typename TConversationFunctor >
class GenericConversation : public TerminalRequestConversation
{
    Terminal&                        m_terminal;
    network::Client&             m_client;
    std::promise< TResultType >& m_promise;
    TConversationFunctor             m_functor;

public:
    GenericConversation( Terminal& terminal, network::ConversationManager& conversationManager, network::Client& client,
                     const network::ConnectionID& originatingConnectionID, std::promise< TResultType >& promise,
                     TConversationFunctor&& functor )
        : TerminalRequestConversation(
            terminal, conversationManager.createConversationID( originatingConnectionID ), originatingConnectionID )
        , m_terminal( terminal )
        , m_client( client )
        , m_promise( promise )
        , m_functor( functor )
    {
    }

    void run( boost::asio::yield_context& yield_ctx )
    {
        Conversation::RequestStack stack( "GenericConversation", *this, network::getConnectionID( m_client.getSocket() )
); try
        {
            m_functor( m_promise, m_client, *this, yield_ctx );
        }
        catch ( std::exception& ex )
        {
            boost::asio::post( [ &promise = m_promise, ex = std::current_exception() ]()
                               { promise.set_exception( ex ); } );
            throw;
        }
    }
};

// clang-format off
#define SIMPLE_REQUEST( TResult, Code ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON(\
        using ResultType = TResult; \
        std::promise< ResultType > promise; \
        std::future< ResultType > fResult = promise.get_future(); \
        try \
        { \
            m_conversationManager.conversationStarted \
            ( \
                network::Conversation::Ptr \
                ( \
                    new GenericConversation \
                    ( \
                        *this, m_conversationManager, m_client, network::getConnectionID( m_client.getSocket() ),
promise, \
                        []( std::promise< ResultType >& promise, network::Client& client, \
                                network::Conversation& conversation, boost::asio::yield_context& yield_ctx ) \
                        { \
                            network::terminal_daemon::Request_Encode daemon( conversation, client.getSocket(), yield_ctx
);
\
                            { \
                                Code; \
                            } \
                        } \
                    ) \
                ) \
            ); \
            return fResult.get(); \
        }\
        catch( std::exception& ex )\
        {\
            std::cout << "Exception: " << ex.what() << std::endl;\
            return TResult{};\
        }\
    )
// clang-format on

std::string Terminal::GetVersion()
{
    SIMPLE_REQUEST(
        std::string,
        boost::asio::post( [ &promise = promise, result = daemon.GetVersion() ]() { promise.set_value( result ); } ); );
}

std::vector< std::string > Terminal::ListTerminals()
{
    SIMPLE_REQUEST(
        std::vector< std::string >,
        boost::asio::post( [ &promise = promise, result = daemon.ListTerminals() ]() { promise.set_value( result ); } );
);
}

std::vector< network::ConversationID > Terminal::listActivities()
{
    //
    SIMPLE_REQUEST( std::vector< network::ConversationID >,
                    boost::asio::post( [ &promise = promise, result = daemon.ListActivities() ]()
                                       { promise.set_value( result ); } ); );
}

network::PipelineResult Terminal::PipelineRun( const mega::pipeline::Configuration& pipelineConfig )
{
    using ResultType = network::PipelineResult;
    std::promise< ResultType > promise;
    std::future< ResultType >  fResult = promise.get_future();

    auto functor = [ &pipelineConfig ]( std::promise< ResultType >& promise, network::Client& client,
                                        network::Conversation& conversation, boost::asio::yield_context& yield_ctx )
    {
        network::terminal_daemon::Request_Encode daemon( conversation, client.getSocket(), yield_ctx );
        {
            auto result = daemon.PipelineRun( pipelineConfig );
            boost::asio::post( [ &promise = promise, result ]() { promise.set_value( result ); } );
        }
    };

    auto pConversation = new GenericConversation< ResultType, decltype( functor ) >(
        *this, m_conversationManager, m_client, network::getConnectionID( m_client.getSocket() ), promise,
        std::move( functor ) );

    try
    {
        m_conversationManager.conversationStarted( network::Conversation::Ptr( pConversation ) );
        return fResult.get();
    }
    catch ( std::exception& ex )
    {
        return network::PipelineResult( false, ex.what() );
    }
}
*/

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
    return network::PipelineResult( false, "Not implemented" );
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
    std::vector< std::string > result;
    {
        auto func = [ &result ](
                        network::ConversationBase& con, network::Sender& sender, boost::asio::yield_context& yield_ctx )
        {
            network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );
            result = leaf.TermListNetworkNodes();
        };
        conversationStarted( network::ConversationBase::Ptr( new GenericConversation(
            *this, createConversationID( getLeafSender().getConnectionID() ), getLeafSender().getConnectionID(), std::move( func ) ) ) );
    }

    while ( result.empty() )
    {
        m_io_context.run_one();
    }

    return result;
}

} // namespace service
} // namespace mega
