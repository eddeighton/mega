
#include "service/tool.hpp"

#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/leaf_tool.hxx"
#include "service/protocol/model/tool_leaf.hxx"

#include "common/requireSemicolon.hpp"

#include "boost/system/detail/error_code.hpp"
#include "boost/bind/bind.hpp"

#include <thread>

namespace mega
{
namespace service
{

class ToolRequestConversation : public network::InThreadConversation, public network::leaf_tool::Impl
{
protected:
    Tool& m_tool;

public:
    ToolRequestConversation( Tool& tool, const network::ConversationID& conversationID,
                             const network::ConnectionID& originatingConnectionID )
        : InThreadConversation( tool, conversationID, originatingConnectionID )
        , m_tool( tool )
    {
    }

    virtual bool dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::leaf_tool::Impl::dispatchRequest( msg, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override
    {
        if ( ( m_tool.getLeafSender().getConnectionID() == connection )
             || ( m_tool.m_receiverChannel.getSender()->getConnectionID() == connection ) )
        {
            m_tool.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
        }
        else
        {
            // This can happen when initiating request has received exception - in which case
            SPDLOG_ERROR( "Tool: {} {}", connection, strErrorMsg );
            THROW_RTE( "Tool: Critical error in error handler" );
        }
    }

    network::leaf_tool::Response_Encode getLeafResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::leaf_tool::Response_Encode( *this, m_tool.getLeafSender(), yield_ctx );
    }

    virtual void RootListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        SPDLOG_INFO( "Tool RootListNetworkNodes" );
        getLeafResponse( yield_ctx ).RootListNetworkNodes( { m_tool.getProcessName() } );
    }
};

template < typename TConversationFunctor >
class GenericConversation : public ToolRequestConversation
{
    TConversationFunctor m_functor;

public:
    GenericConversation( Tool& terminal, const network::ConversationID& conversationID,
                         const network::ConnectionID& originatingConnectionID, TConversationFunctor&& functor )
        : ToolRequestConversation( terminal, conversationID, originatingConnectionID )
        , m_functor( functor )
    {
    }

    void run( boost::asio::yield_context& yield_ctx )
    {
        ConversationBase::RequestStack stack( "GenericConversation", *this, m_tool.getLeafSender().getConnectionID() );
        m_functor( *this, m_tool.getLeafSender(), yield_ctx );
    }
};

Tool::Tool()
    : network::ConversationManager( network::makeProcessName( network::Node::Tool ), m_io_context )
    , m_receiverChannel( m_io_context, *this )
    //, m_work_guard( m_io_context.get_executor() )
    , m_leaf(
          [ &m_receiverChannel = m_receiverChannel ]()
          {
              m_receiverChannel.run( network::makeProcessName( network::Node::Tool ) );
              return m_receiverChannel.getSender();
          }(),
          network::Node::Tool )
{
}

Tool::~Tool()
{
    m_receiverChannel.stop();
    // m_work_guard.reset();
}

network::ConversationBase::Ptr Tool::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Header&       header,
                                                       const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new ToolRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
}

void Tool::run( Tool::Functor& function )
{
    std::optional< std::variant< int, std::exception_ptr > > exceptionResult;
    {
        auto func = [ &exceptionResult, &function ](
                        network::ConversationBase& con, network::Sender& sender, boost::asio::yield_context& yield_ctx )
        {
            try
            {
                function( yield_ctx );
                exceptionResult = 0;
            }
            catch ( std::exception& ex )
            {
                exceptionResult = std::current_exception();
            }
        };
        conversationInitiated( network::ConversationBase::Ptr( new GenericConversation(
                                   *this, createConversationID( getLeafSender().getConnectionID() ),
                                   getLeafSender().getConnectionID(), std::move( func ) ) ),
                               getLeafSender() );
    }
    while ( !exceptionResult.has_value() )
        m_io_context.run_one();

    if ( exceptionResult->index() == 1 )
        std::rethrow_exception( std::get< std::exception_ptr >( exceptionResult.value() ) );
}

} // namespace service
} // namespace mega
