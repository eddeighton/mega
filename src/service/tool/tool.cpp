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

#include "service/protocol/common/conversation_id.hpp"

#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/sim.hxx"
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

namespace
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
    virtual network::sim::Request_Encoder getMPOSimRequest( MPO mpo ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getMPRequest( *m_pYieldContext ), mpo ]( const network::Message& msg ) mutable
                 { return leafRequest.MPOUp( msg, mpo ); },
                 getID() };
    }
    virtual network::memory::Request_Sender getLeafMemoryRequest() override
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

    virtual void RootSimRun( const Project& project, const MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        m_tool.setMPO( mpo );

        setMPOContext( this );
        m_pYieldContext = &yield_ctx;

        // note the runtime will query getThisMPO while creating the root
        SPDLOG_TRACE( "TOOL: Acquired mpo context: {}", mpo );
        {
            createRoot( project, mpo );
            m_functor( yield_ctx );
        }
        SPDLOG_TRACE( "TOOL: Releasing mpo context: {}", mpo );

        m_pYieldContext = nullptr;
        resetMPOContext();
    }

    virtual network::Status GetStatus( const std::vector< network::Status >& childNodeStatus,
                                       boost::asio::yield_context&           yield_ctx ) override
    {
        SPDLOG_TRACE( "ToolRequestConversation::GetStatus" );

        network::Status status{ childNodeStatus };
        {
            std::vector< network::ConversationID > conversations;
            {
                for( const auto& id : m_tool.reportConversations() )
                {
                    if( id != getID() )
                    {
                        conversations.push_back( id );
                    }
                }
            }
            status.setConversationID( conversations );
            status.setMPO( m_tool.getMPO() );
            status.setDescription( m_tool.getProcessName() );

            using MPOTimeStampVec = std::vector< std::pair< MPO, TimeStamp > >;
            using MPOVec          = std::vector< MPO >;
            if( const auto& reads = m_lockTracker.getReads(); !reads.empty() )
                status.setReads( MPOTimeStampVec{ reads.begin(), reads.end() } );
            if( const auto& writes = m_lockTracker.getWrites(); !writes.empty() )
                status.setWrites( MPOTimeStampVec{ writes.begin(), writes.end() } );

            {
                std::ostringstream os;
                os << "Tool: " << m_log.getTimeStamp();
            }

            status.setLogIterator( m_log.getIterator() );

            status.setMemory( m_pMemoryManager->getStatus() );
        }

        return status;
    }
};
} // namespace

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
void Tool::runComplete()
{
    m_receiverChannel.stop();
}

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
            catch( std::exception& ex )
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

    if( exceptionResult->index() == 1 )
        std::rethrow_exception( std::get< std::exception_ptr >( exceptionResult.value() ) );
}

} // namespace mega::service
