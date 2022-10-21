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

#include "service/tool.hpp"

#include "request.hpp"

#include "service/mpo_context.hpp"

#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/address.hxx"
#include "service/protocol/model/stash.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/project.hxx"

#include "log/log.hpp"

#include "common/requireSemicolon.hpp"

#include <spdlog/spdlog.h>
#include <boost/filesystem.hpp>

#include <thread>

namespace mega::service
{

template < typename TConversationFunctor >
class GenericConversation : public ToolRequestConversation, public mega::MPOContext
{
    Tool&                m_tool;
    TConversationFunctor m_functor;

public:
    GenericConversation( Tool& tool, const network::ConversationID& conversationID,
                         const network::ConnectionID& originatingConnectionID, TConversationFunctor&& functor )
        : ToolRequestConversation( tool, conversationID, originatingConnectionID )
        , mega::MPOContext( conversationID )
        , m_tool( tool )
        , m_functor( functor )
    {
    }

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override
    {
        return ToolRequestConversation::dispatchRequest( msg, yield_ctx );
    }

    network::tool_leaf::Request_Sender getToolRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::tool_leaf::Request_Sender( *this, m_tool.getLeafSender(), yield_ctx );
    }
    network::mpo::Request_Sender getMPRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::mpo::Request_Sender( *this, m_tool.getLeafSender(), yield_ctx );
    }
    virtual network::address::Request_Encoder getRootAddressRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getToolRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
                 { return leafRequest.ToolRoot( msg ); },
                 getID() };
    }
    virtual network::enrole::Request_Encoder getRootEnroleRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getToolRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
                 { return leafRequest.ToolRoot( msg ); },
                 getID() };
    }
    virtual network::stash::Request_Encoder getRootStashRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getToolRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
                 { return leafRequest.ToolRoot( msg ); },
                 getID() };
    }
    virtual network::memory::Request_Encoder getDaemonMemoryRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getToolRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
                 { return leafRequest.ToolDaemon( msg ); },
                 getID() };
    }
    virtual network::runtime::Request_Sender getLeafRuntimeRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { *this, m_tool.getLeafSender(), *m_pYieldContext };
    }
    virtual network::jit::Request_Sender getLeafJITRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { *this, m_tool.getLeafSender(), *m_pYieldContext };
    }

    virtual network::mpo::Request_Sender getMPRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return getMPRequest( *m_pYieldContext );
    }

    void run( boost::asio::yield_context& yield_ctx ) override
    {
        SPDLOG_TRACE( "TOOL: run function" );
        network::sim::Request_Encoder request(
            [ rootRequest = getMPRequest( yield_ctx ) ]( const network::Message& msg ) mutable
            { return rootRequest.MPRoot( msg, mega::MP{} ); },
            getID() );
        request.SimStart();
        SPDLOG_TRACE( "TOOL: run complete" );

        // run complete will stop the receiver and ioservice.run() will complete.
        m_tool.runComplete();
    }

    virtual void RootSimRun( const mega::MPO& mpo, const mega::reference& root, const std::string& strMemory,
                             boost::asio::yield_context& yield_ctx ) override
    {
        initSharedMemory( mpo, root, strMemory );

        m_tool.setRoot( root );
        m_tool.setMPO( mpo );

        setMPOContext( this );
        m_pYieldContext = &yield_ctx;

        // note the runtime will query getThisMPO while creating the root
        SPDLOG_TRACE( "TOOL: Acquired mpo context: {}", mpo );
        {
            m_functor( yield_ctx );
        }
        SPDLOG_TRACE( "TOOL: Releasing mpo context: {}", mpo );

        m_pYieldContext = nullptr;
        resetMPOContext();
    }

    // mega::MPOContext
    // clock
    virtual TimeStamp cycle() override { return TimeStamp{}; }
    virtual F32       ct() override { return F32{}; }
    virtual F32       dt() override { return F32{}; }
};

Tool::Tool( short daemonPortNumber )
    : network::ConversationManager( network::makeProcessName( network::Node::Tool ), m_io_context )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf(
          [ &m_receiverChannel = m_receiverChannel ]()
          {
              m_receiverChannel.run( network::makeProcessName( network::Node::Tool ) );
              return m_receiverChannel.getSender();
          }(),
          network::Node::Tool, daemonPortNumber )
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
void Tool::runComplete() { m_receiverChannel.stop(); }

network::ConversationBase::Ptr Tool::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new ToolRequestConversation( *this, msg.getReceiverID(), originatingConnectionID ) );
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

    // run until m_tool.runComplete();
    m_io_context.run();

    if ( exceptionResult->index() == 1 )
        std::rethrow_exception( std::get< std::exception_ptr >( exceptionResult.value() ) );
}

} // namespace mega::service
