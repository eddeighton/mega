
#include "service/tool.hpp"

#include "mega/common.hpp"
#include "mega/execution_context.hpp"
#include "mega/root.hpp"

#include "runtime/runtime.hpp"

#include "service/network/conversation.hpp"
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

    network::tool_leaf::Request_Encode getToolRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::tool_leaf::Request_Encode( *this, m_tool.getLeafSender(), yield_ctx );
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
class GenericConversation : public ToolRequestConversation, public mega::ExecutionContext
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
        execution_resume( this );

        m_pYieldContext = &yield_ctx;

        const std::pair< bool, mega::ExecutionIndex > result = getToolRequest( yield_ctx ).ToolCreateExecutionContext();
        VERIFY_RTE_MSG( result.first, "Failed to acquire execution context" );
        SPDLOG_INFO( "Acquired execution context: {}", result.second );
        m_executionIndex = result.second;

        std::unique_ptr< Root, void ( * )( Root* ) > pRoot(
            mega::runtime::allocateRoot( result.second ), []( Root* pRoot ) { mega::runtime::releaseRoot( pRoot ); } );

        m_functor( yield_ctx );

        m_pYieldContext = nullptr;

        execution_suspend();
    }

    // mega::ExecutionContext
    virtual ExecutionIndex getThisExecutionIndex() override
    {
        return m_executionIndex;
    }
    virtual std::string acquireMemory( ExecutionIndex executionIndex )
    {
        VERIFY_RTE( m_pYieldContext );
        SPDLOG_INFO( "acquireMemory called with: {}", executionIndex );
        return getToolRequest( *m_pYieldContext ).ToolAcquireMemory( executionIndex );
    }

    virtual LogicalAddress allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID )
    {
        VERIFY_RTE( m_pYieldContext );
        SPDLOG_INFO( "allocateLogical called with: {} {}", executionIndex, objectTypeID );
        return LogicalAddress{ getToolRequest( *m_pYieldContext ).ToolAllocateLogical( executionIndex, objectTypeID ) };
    }
    virtual void deAllocateLogical( ExecutionIndex executionIndex, LogicalAddress logicalAddress )
    {
        VERIFY_RTE( m_pYieldContext );
        SPDLOG_INFO( "deAllocate called with: {} {}", executionIndex, logicalAddress );
        getToolRequest( *m_pYieldContext ).ToolDeAllocateLogical( executionIndex, Address{ logicalAddress } );
    }

    boost::asio::yield_context* m_pYieldContext = nullptr;
    mega::ExecutionIndex m_executionIndex;
};

Tool::Tool()
    : network::ConversationManager( network::makeProcessName( network::Node::Tool ), m_io_context )
    , m_receiverChannel( m_io_context, *this )
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
    m_io_context.run();
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
        auto func = [ &exceptionResult, &function ]( boost::asio::yield_context& yield_ctx )
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
        network::ConversationBase::Ptr pConversation(
            new GenericConversation( *this, createConversationID( getLeafSender().getConnectionID() ),
                                     getLeafSender().getConnectionID(), std::move( func ) ) );

        conversationInitiated( pConversation, getLeafSender() );
    }

    while ( !exceptionResult.has_value() )
        m_io_context.run_one();

    if ( exceptionResult->index() == 1 )
        std::rethrow_exception( std::get< std::exception_ptr >( exceptionResult.value() ) );
}

} // namespace service
} // namespace mega
