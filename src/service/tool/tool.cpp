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

#include "runtime/api.hpp"
#include "runtime/context.hpp"

#include "service/lock_tracker.hpp"
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

#include "common/requireSemicolon.hpp"

#include <spdlog/spdlog.h>
#include <boost/filesystem.hpp>

#include <thread>

namespace mega::service
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
        {
            network::project::Request_Encoder projectRequest(
                [ rootRequest = getToolRequest( yield_ctx ) ]( const network::Message& msg ) mutable
                { return rootRequest.ToolRoot( msg ); },
                getID() );

            const auto currentProject            = projectRequest.GetProject();
            const auto megaStructureInstallation = projectRequest.GetMegastructureInstallation();

            if ( !currentProject.isEmpty() && boost::filesystem::exists( currentProject.getProjectDatabase() ) )
            {
                network::memory::Request_Encoder memoryRequest(
                    [ rootRequest = getToolRequest( yield_ctx ) ]( const network::Message& msg ) mutable
                    { return rootRequest.ToolDaemon( msg ); },
                    getID() );

                const auto memoryConfig = memoryRequest.GetSharedMemoryConfig();

                mega::runtime::initialiseRuntime( megaStructureInstallation, currentProject, memoryConfig.getMemory(),
                                                  memoryConfig.getMutex(), memoryConfig.getMap() );
                SPDLOG_TRACE(
                    "Executor runtime initialised with project: {}", currentProject.getProjectInstallPath().string() );
            }
            else
            {
                SPDLOG_TRACE( "Could not initialised runtime.  No active project" );
            }
        }

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
    virtual MPOContext::MachineIDVector getMachines() override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootRequest< network::enrole::Request_Encoder >( *m_pYieldContext ).EnroleGetDaemons();
    }
    virtual MPOContext::MachineProcessIDVector getProcesses( MachineID machineID ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootRequest< network::enrole::Request_Encoder >( *m_pYieldContext ).EnroleGetProcesses( machineID );
    }
    virtual MPOContext::MPOVector getMPO( MP machineProcess ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootRequest< network::enrole::Request_Encoder >( *m_pYieldContext ).EnroleGetMPO( machineProcess );
    }
    virtual MPO getThisMPO() override { return m_tool.getMPO(); }
    virtual MPO constructMPO( MP machineProcess ) override
    {
        SPDLOG_TRACE( "Tool constructMPO: {}", machineProcess );
        network::sim::Request_Encoder request(
            [ mpoRequest = getMPRequest( *m_pYieldContext ), machineProcess ]( const network::Message& msg ) mutable
            { return mpoRequest.MPRoot( msg, machineProcess ); },
            getID() );
        return request.SimCreate();
    }
    virtual mega::reference getRoot( MPO mpo ) override { return mega::runtime::get_root( mpo ); }
    virtual mega::reference getThisRoot() override { return m_pExecutionRoot->root(); }

    // clock
    virtual TimeStamp cycle() override { return TimeStamp{}; }
    virtual F32       ct() override { return F32{}; }
    virtual F32       dt() override { return F32{}; }

    // log
    virtual void info( const reference& ref, const std::string& str ) override
    {
        //
    }
    virtual void warn( const reference& ref, const std::string& str ) override
    {
        //
    }
    virtual void error( const reference& ref, const std::string& str ) override
    {
        //
    }
    virtual void write( const reference& ref, bool bShared, U64 size, const void* pData ) override
    {
        SPDLOG_TRACE( "Tool::write: {} {} {}", ref, bShared, size );
    }

    // mega::MPOContext
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
        VERIFY_RTE( m_pYieldContext );

        network::sim::Request_Encoder request(
            [ mpoRequest = getMPRequest( *m_pYieldContext ), mpo ]( const network::Message& msg ) mutable
            { return mpoRequest.MPOUp( msg, mpo ); },
            getID() );

        if ( request.SimLockRead( m_tool.getMPO() ) )
        {
            m_lockTracker.onRead( mpo );
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual bool writeLock( MPO mpo ) override
    {
        VERIFY_RTE( m_pYieldContext );

        network::sim::Request_Encoder request(
            [ mpoRequest = getMPRequest( *m_pYieldContext ), mpo ]( const network::Message& msg ) mutable
            { return mpoRequest.MPOUp( msg, mpo ); },
            getID() );

        if ( request.SimLockWrite( m_tool.getMPO() ) )
        {
            m_lockTracker.onWrite( mpo );
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual void cycleComplete() override
    {
        for ( MPO writeLock : m_lockTracker.getWrites() )
        {
            VERIFY_RTE( m_pYieldContext );
            network::sim::Request_Encoder request(
                [ mpoRequest = getMPRequest( *m_pYieldContext ), writeLock ]( const network::Message& msg ) mutable
                { return mpoRequest.MPOUp( msg, writeLock ); },
                getID() );
            request.SimLockRelease( m_tool.getMPO() );
            m_lockTracker.onRelease( writeLock );
        }

        for ( MPO writeLock : m_lockTracker.getReads() )
        {
            VERIFY_RTE( m_pYieldContext );
            network::sim::Request_Encoder request(
                [ mpoRequest = getMPRequest( *m_pYieldContext ), writeLock ]( const network::Message& msg ) mutable
                { return mpoRequest.MPOUp( msg, writeLock ); },
                getID() );
            request.SimLockRelease( m_tool.getMPO() );
            m_lockTracker.onRelease( writeLock );
        }
    }

    boost::asio::yield_context*               m_pYieldContext = nullptr;
    std::shared_ptr< mega::runtime::MPORoot > m_pExecutionRoot;
    LockTracker                               m_lockTracker;
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

} // namespace mega::service
