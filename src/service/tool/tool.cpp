
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
        SPDLOG_TRACE( "Tool RootListNetworkNodes" );
        getLeafResponse( yield_ctx ).RootListNetworkNodes( { m_tool.getProcessName() } );
    }

    virtual void RootShutdown( boost::asio::yield_context& yield_ctx ) override
    {
        getLeafResponse( yield_ctx ).RootShutdown();
        boost::asio::post( [ &tool = m_tool ]() { tool.shutdown(); } );
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
        ExecutionContext::resume( this );
        m_pYieldContext = &yield_ctx;
        
        // note the runtime will query getThisExecutionIndex while creating the root
        {
            const std::pair< bool, mega::ExecutionIndex > result = getToolRequest( yield_ctx ).ToolCreateExecutionContext();
            VERIFY_RTE_MSG( result.first, "Failed to acquire execution context" );
            m_executionIndex = result.second;
        }

        SPDLOG_TRACE( "Acquired execution context: {}", m_executionIndex.value() );
        m_executionRoot = mega::runtime::ExecutionRoot( m_executionIndex.value() );

        {
            m_functor( yield_ctx );
        }

        m_pYieldContext = nullptr;
        ExecutionContext::suspend();
    }

    // mega::ExecutionContext
    virtual ExecutionIndex getThisExecutionIndex() override { return m_executionIndex.value(); }

    mega::reference getRoot() override { return m_executionRoot->root(); }

    virtual std::string acquireMemory( ExecutionIndex executionIndex ) override
    {
        VERIFY_RTE( m_pYieldContext );
        SPDLOG_TRACE( "acquireMemory called with: {}", executionIndex );
        return getToolRequest( *m_pYieldContext ).ToolAcquireMemory( executionIndex );
    }

    virtual LogicalAddress allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID ) override
    {
        VERIFY_RTE( m_pYieldContext );
        SPDLOG_TRACE( "allocateLogical called with: {} {}", executionIndex, objectTypeID );
        return LogicalAddress{ getToolRequest( *m_pYieldContext ).ToolAllocateLogical( executionIndex, objectTypeID ) };
    }
    virtual void deAllocateLogical( ExecutionIndex executionIndex, LogicalAddress logicalAddress ) override
    {
        VERIFY_RTE( m_pYieldContext );
        SPDLOG_TRACE( "deAllocate called with: {} {}", executionIndex, logicalAddress );
        getToolRequest( *m_pYieldContext ).ToolDeAllocateLogical( executionIndex, Address{ logicalAddress } );
    }
    virtual void stash( const std::string& filePath, std::size_t determinant ) override
    {
        VERIFY_RTE( m_pYieldContext );
        getToolRequest( *m_pYieldContext ).ToolStash( filePath, determinant );
    }
    virtual bool restore( const std::string& filePath, std::size_t determinant ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getToolRequest( *m_pYieldContext ).ToolRestore( filePath, determinant );
    }

    virtual void readLock( ExecutionIndex executionIndex ) override
    {
        SPDLOG_TRACE( "readLock from: {} to: {}", m_executionIndex.value(), executionIndex );
        VERIFY_RTE( m_pYieldContext );
        const network::ConversationID id
            = getToolRequest( *m_pYieldContext ).ToolGetExecutionContextID( executionIndex );
        getToolRequest( *m_pYieldContext ).ToolSimReadLock( id );
    }

    virtual void writeLock( ExecutionIndex executionIndex ) override
    {
        SPDLOG_TRACE( "writeLock from: {} to: {}", m_executionIndex.value(), executionIndex );
        VERIFY_RTE( m_pYieldContext );
        const network::ConversationID id
            = getToolRequest( *m_pYieldContext ).ToolGetExecutionContextID( executionIndex );
        getToolRequest( *m_pYieldContext ).ToolSimWriteLock( id );
    }

    virtual void releaseLock( ExecutionIndex executionIndex ) override
    {
        SPDLOG_TRACE( "releaseLock from: {} to: {}", m_executionIndex.value(), executionIndex );
        VERIFY_RTE( m_pYieldContext );
        const network::ConversationID id
            = getToolRequest( *m_pYieldContext ).ToolGetExecutionContextID( executionIndex );
        getToolRequest( *m_pYieldContext ).ToolSimReleaseLock( id );
    }

    boost::asio::yield_context*                   m_pYieldContext = nullptr;
    std::optional< mega::ExecutionIndex >         m_executionIndex;
    std::optional< mega::runtime::ExecutionRoot > m_executionRoot;
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

void Tool::shutdown()
{
    // TODO ?
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
