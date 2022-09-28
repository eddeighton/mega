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
// #include "service/protocol/model/mpo_leaf.hxx"
#include "service/protocol/model/sim.hxx"

#include "common/requireSemicolon.hpp"

#include "boost/system/detail/error_code.hpp"
#include "boost/bind/bind.hpp"

#include <thread>

namespace mega
{
namespace service
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
        , m_tool( tool )
        , m_functor( functor )
    {
    }

    network::tool_leaf::Request_Sender getToolRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::tool_leaf::Request_Sender( *this, m_tool.getLeafSender(), yield_ctx );
    }

    void run( boost::asio::yield_context& yield_ctx ) override
    {
        network::sim::Request_Encoder request(
            [ rootRequest = getToolRequest( yield_ctx ) ]( const network::Message& msg ) mutable
            { return rootRequest.ToolRoot( msg ); },
            getID() );
        request.SimStart();
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
                SPDLOG_TRACE( "TOOL: running function" );
                m_functor( yield_ctx );
                SPDLOG_TRACE( "TOOL: function completed" );
            }
        }

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

    mega::reference getRoot() override { return m_pExecutionRoot->root(); }

    // mega::MPOContext
    virtual MPO getThisMPO() override { return m_tool.getMPO(); }

    virtual std::string acquireMemory( MPO mpo ) override
    {
        VERIFY_RTE( m_pYieldContext );
        // SPDLOG_TRACE( "acquireMemory called with: {}", mpo );
        // return getToolRequest( *m_pYieldContext ).ToolAcquireMemory( mpo );
        THROW_RTE( "TODO" );
    }

    virtual MPO getNetworkAddressMPO( NetworkAddress networkAddress ) override
    {
        VERIFY_RTE( m_pYieldContext );
        // SPDLOG_TRACE( "getNetworkAddressMPO called with: {}", networkAddress );
        // return getToolRequest( *m_pYieldContext ).ToolGetNetworkAddressMPO( networkAddress );
        THROW_RTE( "TODO" );
    }
    virtual NetworkAddress getRootNetworkAddress( MPO mpo ) override
    {
        VERIFY_RTE( m_pYieldContext );
        // SPDLOG_TRACE( "getRootNetworkAddress called with: {} {}", mpo, objectTypeID );
        // return NetworkAddress{ getToolRequest( *m_pYieldContext ).ToolGetRootNetworkAddress( mpo ) };
        THROW_RTE( "TODO" );
    }
    virtual NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID ) override
    {
        VERIFY_RTE( m_pYieldContext );
        // SPDLOG_TRACE( "allocateNetworkAddress called with: {} {}", mpo, objectTypeID );
        // return NetworkAddress{ getToolRequest( *m_pYieldContext ).ToolAllocateNetworkAddress( mpo, objectTypeID ) };
        THROW_RTE( "TODO" );
    }
    virtual void deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress ) override
    {
        VERIFY_RTE( m_pYieldContext );
        // SPDLOG_TRACE( "deAllocate called with: {} {}", mpo, networkAddress );
        // getToolRequest( *m_pYieldContext ).ToolDeAllocateNetworkAddress( mpo, networkAddress );
        THROW_RTE( "TODO" );
    }
    virtual void stash( const std::string& filePath, mega::U64 determinant ) override
    {
        VERIFY_RTE( m_pYieldContext );
        // getToolRequest( *m_pYieldContext ).ToolStash( filePath, determinant );
        THROW_RTE( "TODO" );
    }
    virtual bool restore( const std::string& filePath, mega::U64 determinant ) override
    {
        VERIFY_RTE( m_pYieldContext );
        // return getToolRequest( *m_pYieldContext ).ToolRestore( filePath, determinant );
        THROW_RTE( "TODO" );
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

network::ConversationBase::Ptr Tool::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new ToolRequestConversation( *this, getMsgReceiver( msg ), originatingConnectionID ) );
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
