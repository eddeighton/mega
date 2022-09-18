
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

class TerminalRequestConversation : public network::InThreadConversation, public network::leaf_term::Impl
{
protected:
    Terminal& m_terminal;

public:
    TerminalRequestConversation( Terminal& terminal, const network::ConversationID& conversationID,
                                 const network::ConnectionID& originatingConnectionID )
        : InThreadConversation( terminal, conversationID, originatingConnectionID )
        , m_terminal( terminal )
    {
    }

    virtual bool dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::leaf_term::Impl::dispatchRequest( msg, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override
    {
        if ( ( m_terminal.getLeafSender().getConnectionID() == connection )
             || ( m_terminal.m_receiverChannel.getSender()->getConnectionID() == connection ) )
        {
            m_terminal.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else
        {
            // This can happen when initiating request has received exception - in which case
            SPDLOG_ERROR( "Terminal: {} {}", connection, strErrorMsg );
            THROW_RTE( "Terminal: Critical error in error handler" );
        }
    }

    network::leaf_term::Response_Encode getLeafResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_term::Response_Encode( *this, m_terminal.getLeafSender(), yield_ctx );
    }

    virtual void RootListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        getLeafResponse( yield_ctx ).RootListNetworkNodes( { m_terminal.getProcessName() } );
    }

    void RootShutdown( boost::asio::yield_context& yield_ctx ) override
    {
        SPDLOG_TRACE( "RootShutdown received" );
        getLeafResponse( yield_ctx ).RootShutdown();
        boost::asio::post( [ &terminal = m_terminal ]() { terminal.shutdown(); } );
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
        // ConversationBase::RequestStack stack(
        //     "GenericConversation", *this, m_terminal.getLeafSender().getConnectionID() );
        m_functor( *this, m_terminal.getLeafSender(), yield_ctx );
    }
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

#define GENERIC_MSG_NO_RESULT( msg_name )                                                                \
    {                                                                                                    \
        std::optional< std::variant< bool, std::exception_ptr > > result;                                \
        {                                                                                                \
            auto func = [ &result ]( network::ConversationBase& con, network::Sender& sender,            \
                                     boost::asio::yield_context& yield_ctx )                             \
            {                                                                                            \
                network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );                       \
                try                                                                                      \
                {                                                                                        \
                    leaf.msg_name();                                                                     \
                    result = true;                                                                       \
                }                                                                                        \
                catch ( std::exception & ex )                                                            \
                {                                                                                        \
                    result = std::current_exception();                                                   \
                }                                                                                        \
            };                                                                                           \
            conversationInitiated( network::ConversationBase::Ptr( new GenericConversation(              \
                                       *this, createConversationID( getLeafSender().getConnectionID() ), \
                                       getLeafSender().getConnectionID(), std::move( func ) ) ),         \
                                   getLeafSender() );                                                    \
        }                                                                                                \
        while ( !result.has_value() )                                                                    \
            m_io_context.run_one();                                                                      \
                                                                                                         \
        if ( result->index() == 1 )                                                                      \
            std::rethrow_exception( std::get< std::exception_ptr >( result.value() ) );                  \
    }

#define GENERIC_MSG( result_type, msg_name )                                                             \
    {                                                                                                    \
        std::optional< std::variant< result_type, std::exception_ptr > > result;                         \
        {                                                                                                \
            auto func = [ &result ]( network::ConversationBase& con, network::Sender& sender,            \
                                     boost::asio::yield_context& yield_ctx )                             \
            {                                                                                            \
                network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );                       \
                try                                                                                      \
                {                                                                                        \
                    result = leaf.msg_name();                                                            \
                }                                                                                        \
                catch ( std::exception & ex )                                                            \
                {                                                                                        \
                    result = std::current_exception();                                                   \
                }                                                                                        \
            };                                                                                           \
            conversationInitiated( network::ConversationBase::Ptr( new GenericConversation(              \
                                       *this, createConversationID( getLeafSender().getConnectionID() ), \
                                       getLeafSender().getConnectionID(), std::move( func ) ) ),         \
                                   getLeafSender() );                                                    \
        }                                                                                                \
        while ( !result.has_value() )                                                                    \
            m_io_context.run_one();                                                                      \
                                                                                                         \
        if ( result->index() == 1 )                                                                      \
            std::rethrow_exception( std::get< std::exception_ptr >( result.value() ) );                  \
        else                                                                                             \
            return std::get< result_type >( result.value() );                                            \
    }

#define GENERIC_MSG_ARG1_NORESULT( msg_name, arg1 )                                                      \
    {                                                                                                    \
        std::optional< std::variant< bool, std::exception_ptr > > result;                                \
        {                                                                                                \
            auto func = [ &result, &arg1 ]( network::ConversationBase& con, network::Sender& sender,     \
                                            boost::asio::yield_context& yield_ctx )                      \
            {                                                                                            \
                network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );                       \
                try                                                                                      \
                {                                                                                        \
                    leaf.msg_name( arg1 );                                                               \
                    result = true;                                                                       \
                }                                                                                        \
                catch ( std::exception & ex )                                                            \
                {                                                                                        \
                    result = std::current_exception();                                                   \
                }                                                                                        \
            };                                                                                           \
            conversationInitiated( network::ConversationBase::Ptr( new GenericConversation(              \
                                       *this, createConversationID( getLeafSender().getConnectionID() ), \
                                       getLeafSender().getConnectionID(), std::move( func ) ) ),         \
                                   getLeafSender() );                                                    \
        }                                                                                                \
        while ( !result.has_value() )                                                                    \
            m_io_context.run_one();                                                                      \
                                                                                                         \
        if ( result->index() == 1 )                                                                      \
            std::rethrow_exception( std::get< std::exception_ptr >( result.value() ) );                  \
    }

#define GENERIC_MSG_ARG1( result_type, msg_name, arg1 )                                                  \
    {                                                                                                    \
        std::optional< std::variant< result_type, std::exception_ptr > > result;                         \
        {                                                                                                \
            auto func = [ &result, &arg1 ]( network::ConversationBase& con, network::Sender& sender,     \
                                            boost::asio::yield_context& yield_ctx )                      \
            {                                                                                            \
                network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );                       \
                try                                                                                      \
                {                                                                                        \
                    result = leaf.msg_name( arg1 );                                                      \
                }                                                                                        \
                catch ( std::exception & ex )                                                            \
                {                                                                                        \
                    result = std::current_exception();                                                   \
                }                                                                                        \
            };                                                                                           \
            conversationInitiated( network::ConversationBase::Ptr( new GenericConversation(              \
                                       *this, createConversationID( getLeafSender().getConnectionID() ), \
                                       getLeafSender().getConnectionID(), std::move( func ) ) ),         \
                                   getLeafSender() );                                                    \
        }                                                                                                \
        while ( !result.has_value() )                                                                    \
            m_io_context.run_one();                                                                      \
                                                                                                         \
        if ( result->index() == 1 )                                                                      \
            std::rethrow_exception( std::get< std::exception_ptr >( result.value() ) );                  \
        else                                                                                             \
            return std::get< result_type >( result.value() );                                            \
    }

#define GENERIC_MSG_ARG2_NORESULT( msg_name, arg1, arg2 )                                                   \
    {                                                                                                       \
        std::optional< std::variant< bool, std::exception_ptr > > result;                                   \
        {                                                                                                   \
            auto func = [ &result, &arg1, &arg2 ]( network::ConversationBase& con, network::Sender& sender, \
                                                   boost::asio::yield_context& yield_ctx )                  \
            {                                                                                               \
                network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );                          \
                try                                                                                         \
                {                                                                                           \
                    leaf.msg_name( arg1, arg2 );                                                            \
                    result = true;                                                                          \
                }                                                                                           \
                catch ( std::exception & ex )                                                               \
                {                                                                                           \
                    result = std::current_exception();                                                      \
                }                                                                                           \
            };                                                                                              \
            conversationInitiated( network::ConversationBase::Ptr( new GenericConversation(                 \
                                       *this, createConversationID( getLeafSender().getConnectionID() ),    \
                                       getLeafSender().getConnectionID(), std::move( func ) ) ),            \
                                   getLeafSender() );                                                       \
        }                                                                                                   \
        while ( !result.has_value() )                                                                       \
            m_io_context.run_one();                                                                         \
                                                                                                            \
        if ( result->index() == 1 )                                                                         \
            std::rethrow_exception( std::get< std::exception_ptr >( result.value() ) );                     \
    }

#define GENERIC_MSG_ARG2( result_type, msg_name, arg1, arg2 )                                               \
    {                                                                                                       \
        std::optional< std::variant< result_type, std::exception_ptr > > result;                            \
        {                                                                                                   \
            auto func = [ &result, &arg1, &arg2 ]( network::ConversationBase& con, network::Sender& sender, \
                                                   boost::asio::yield_context& yield_ctx )                  \
            {                                                                                               \
                network::term_leaf::Request_Encode leaf( con, sender, yield_ctx );                          \
                try                                                                                         \
                {                                                                                           \
                    result = leaf.msg_name( arg1, arg2 );                                                   \
                }                                                                                           \
                catch ( std::exception & ex )                                                               \
                {                                                                                           \
                    result = std::current_exception();                                                      \
                }                                                                                           \
            };                                                                                              \
            conversationInitiated( network::ConversationBase::Ptr( new GenericConversation(                 \
                                       *this, createConversationID( getLeafSender().getConnectionID() ),    \
                                       getLeafSender().getConnectionID(), std::move( func ) ) ),            \
                                   getLeafSender() );                                                       \
        }                                                                                                   \
        while ( !result.has_value() )                                                                       \
            m_io_context.run_one();                                                                         \
                                                                                                            \
        if ( result->index() == 1 )                                                                         \
            std::rethrow_exception( std::get< std::exception_ptr >( result.value() ) );                     \
        else                                                                                                \
            return std::get< result_type >( result.value() );                                               \
    }

std::vector< std::string > Terminal::ListNetworkNodes()
{
    //
    GENERIC_MSG( std::vector< std::string >, TermListNetworkNodes );
}

network::PipelineResult Terminal::PipelineRun( const mega::pipeline::Configuration& pipelineConfig )
{
    //
    GENERIC_MSG_ARG1( network::PipelineResult, TermPipelineRun, pipelineConfig );
}

bool Terminal::SetProject( const mega::network::Project& project )
{
    //
    GENERIC_MSG_ARG1( bool, TermSetProject, project );
}

network::MegastructureInstallation Terminal::GetMegastructureInstallation()
{
    //
    GENERIC_MSG( mega::network::MegastructureInstallation, TermGetMegastructureInstallation );
}

mega::network::Project Terminal::GetProject()
{
    //
    GENERIC_MSG( mega::network::Project, TermGetProject );
}

bool Terminal::NewInstallation( const mega::network::Project& project )
{
    //
    GENERIC_MSG_ARG1( bool, TermNewInstallation, project );
}

network::ConversationID Terminal::SimNew()
{
    //
    GENERIC_MSG( network::ConversationID, TermSimNew );
}

void Terminal::SimDestroy( const network::ConversationID& simID )
{
    //
    GENERIC_MSG_ARG1_NORESULT( TermSimDestroy, simID );
}

bool Terminal::SimRead( const network::ConversationID& owningID, const network::ConversationID& simID )
{
    //
    GENERIC_MSG_ARG2( bool, TermSimReadLock, owningID, simID );
}
bool Terminal::SimWrite( const network::ConversationID& owningID, const network::ConversationID& simID )
{
    //
    GENERIC_MSG_ARG2( bool, TermSimWriteLock, owningID, simID );
}
void Terminal::SimRelease( const network::ConversationID& owningID, const network::ConversationID& simID )
{
    //
    GENERIC_MSG_ARG2_NORESULT( TermSimReleaseLock, owningID, simID );
}
std::vector< network::ConversationID > Terminal::SimList()
{
    //
    GENERIC_MSG( std::vector< network::ConversationID >, TermSimList );
}

void Terminal::ClearStash()
{
    //
    GENERIC_MSG_NO_RESULT( TermClearStash );
}

mega::U64 Terminal::Capacity()
{
    //
    GENERIC_MSG( mega::U64, TermCapacity );
}

void Terminal::Shutdown()
{
    //
    GENERIC_MSG_NO_RESULT( TermShutdown );
}

} // namespace service
} // namespace mega
