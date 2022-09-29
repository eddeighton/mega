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

#include "mega/common.hpp"
#include "mega/root.hpp"

#include "runtime/runtime_api.hpp"
#include "runtime/mpo_context.hpp"

#include "service/lock_tracker.hpp"
#include "service/network/conversation.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/leaf_tool.hxx"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/address.hxx"
#include "service/protocol/model/stash.hxx"

#include "common/requireSemicolon.hpp"

#include "boost/system/detail/error_code.hpp"
#include "boost/bind/bind.hpp"

#include <thread>

namespace mega
{
namespace service
{

template < typename TConversationFunctor >
class GenericConversation : public ToolRequestConversation, public mega::MPOContext, public network::sim::Impl
{
    Tool&                m_tool;
    TConversationFunctor m_functor;

public:
    GenericConversation( Tool& tool, const network::ConversationID& conversationID,
                         const network::ConnectionID& originatingConnectionID, TConversationFunctor&& functor )
        : ToolRequestConversation( tool, conversationID, originatingConnectionID )
        , m_tool( tool )
        , m_functor( functor )
    {
    }

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override
    {
        network::Message result;
        if ( result = network::sim::Impl::dispatchRequest( msg, yield_ctx ); result )
            return result;
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

    virtual void RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        m_tool.setMPO( mpo );

        MPOContext::resume( this );
        m_pYieldContext = &yield_ctx;

        // note the runtime will query getThisMPO while creating the root
        SPDLOG_TRACE( "TOOL: Acquired mpo context: {}", m_tool.getMPO() );
        {
            m_pExecutionRoot = std::make_shared< mega::runtime::MPORoot >( m_tool.getMPO() );
            {
                m_functor( yield_ctx );
            }
            m_pExecutionRoot.reset();
        }
        SPDLOG_TRACE( "TOOL: Releasing mpo context: {}", m_tool.getMPO() );

        m_pYieldContext = nullptr;
        MPOContext::suspend();
    }

    //////////////////////////
    // mega::MPOContext
    virtual SimIDVector getSimulationIDs() override
    {
        VERIFY_RTE( m_pYieldContext );
        // return getToolRequest( *m_pYieldContext ).ToolSimList();
        THROW_RTE( "TODO" );
    }

    virtual SimID createSimulation() override
    {
        VERIFY_RTE( m_pYieldContext );
        SimID result;
        return result;
    }
    virtual mega::reference getRoot( const SimID& simID ) override
    {
        VERIFY_RTE( m_pYieldContext );
        THROW_RTE( "TODO" );
        mega::reference result;

        // auto toolRequest = getToolRequest( *m_pYieldContext );
        //  if( toolRequest.ToolSimReadLock( getID(), simID ) )
        //{
        //     const MPO mpo = toolRequest.ToolGetMPO( simID );
        //     return runtime::get_root( mpo );
        // }
        //  else
        //{
        //      THROW_RTE( "Failed to acquire read lock on simID: " << simID );
        //  }
        return result;
    }

    mega::reference getRoot() override
    {
        ASSERT( m_pExecutionRoot.get() );
        return m_pExecutionRoot->root();
    }

    // mega::MPOContext
    virtual MPO getThisMPO() override { return m_tool.getMPO(); }

    virtual std::string acquireMemory( MPO mpo ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getDaemonRequest< network::memory::Request_Encoder >( *m_pYieldContext ).AcquireSharedMemory( mpo );
    }

    virtual MPO getNetworkAddressMPO( NetworkAddress networkAddress ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootRequest< network::address::Request_Encoder >( *m_pYieldContext )
            .GetNetworkAddressMPO( networkAddress );
    }
    virtual NetworkAddress getRootNetworkAddress( MPO mpo ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootRequest< network::address::Request_Encoder >( *m_pYieldContext ).GetRootNetworkAddress( mpo );
    }
    virtual NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootRequest< network::address::Request_Encoder >( *m_pYieldContext )
            .AllocateNetworkAddress( mpo, objectTypeID );
    }
    virtual void deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress ) override
    {
        VERIFY_RTE( m_pYieldContext );
        getRootRequest< network::address::Request_Encoder >( *m_pYieldContext )
            .DeAllocateNetworkAddress( mpo, networkAddress );
    }
    virtual void stash( const std::string& filePath, mega::U64 determinant ) override
    {
        VERIFY_RTE( m_pYieldContext );
        getRootRequest< network::stash::Request_Encoder >( *m_pYieldContext ).StashStash( filePath, determinant );
    }
    virtual bool restore( const std::string& filePath, mega::U64 determinant ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootRequest< network::stash::Request_Encoder >( *m_pYieldContext )
            .StashRestore( filePath, determinant );
    }

    virtual bool readLock( MPO mpo ) override
    {
        // SPDLOG_TRACE( "readLock from: {} to: {}", m_tool.getMPO(), mpo );
        VERIFY_RTE( m_pYieldContext );
        // const network::ConversationID id = getToolRequest( *m_pYieldContext ).ToolGetMPOContextID( mpo );

        /*if ( getToolRequest( *m_pYieldContext ).ToolSimReadLock( getID(), id ) )
        {
            m_lockTracker.onRead( mpo );
            return true;
        }
        else
        {
            return false;
        }*/
        THROW_RTE( "TODO" );
    }

    virtual bool writeLock( MPO mpo ) override
    {
        // SPDLOG_TRACE( "writeLock from: {} to: {}", m_tool.getMPO(), mpo );
        VERIFY_RTE( m_pYieldContext );
        /*const network::ConversationID id = getToolRequest( *m_pYieldContext ).ToolGetMPOContextID( mpo );
        if ( getToolRequest( *m_pYieldContext ).ToolSimWriteLock( getID(), id ) )
        {
            m_lockTracker.onWrite( mpo );
            return true;
        }
        else
        {
            return false;
        }*/
        THROW_RTE( "TODO" );
    }

    virtual void releaseLock( MPO mpo ) override
    {
        // SPDLOG_TRACE( "releaseLock from: {} to: {}", m_tool.getMPO(), mpo );
        /*VERIFY_RTE( m_pYieldContext );
        const network::ConversationID id = getToolRequest( *m_pYieldContext ).ToolGetMPOContextID( mpo );
        getToolRequest( *m_pYieldContext ).ToolSimReleaseLock( getID(), id );
        m_lockTracker.onRelease( mpo );*/
        THROW_RTE( "TODO" );
    }

    boost::asio::yield_context*               m_pYieldContext = nullptr;
    std::shared_ptr< mega::runtime::MPORoot > m_pExecutionRoot;
    LockTracker                               m_lockTracker;
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
void Tool::runComplete() { m_receiverChannel.stop(); }

network::ConversationBase::Ptr Tool::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new ToolRequestConversation( *this, network::getMsgReceiver( msg ), originatingConnectionID ) );
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

} // namespace service
} // namespace mega
