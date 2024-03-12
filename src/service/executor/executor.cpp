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

#include "service/executor/executor.hpp"

#include "service/executor/request.hpp"
#include "service/executor/simulation.hpp"

#include "job.hpp"

#include "service/network/network.hpp"
#include "log/log.hpp"

#include "service/protocol/model/project.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/sim.hxx"

#include "parser/parser.hpp"

#include <boost/dll.hpp>
#include <boost/dll/shared_library_load_mode.hpp>

#include <optional>
#include <future>
#include <thread>

namespace mega::service
{

Executor::Executor( boost::asio::io_context& io_context, network::Log log, U64 numThreads, short daemonPortNumber,
                    ProcessClock& processClock, network::Node nodeType )
    : network::LogicalThreadManager( network::Node::makeProcessName( nodeType ), io_context )
    , m_io_context( io_context )
    , m_numThreads( numThreads )
    , m_processClock( processClock )
    , m_receiverChannel( m_io_context, *this )
    , m_player( std::move( log ), m_receiverChannel.getSender(), nodeType, daemonPortNumber, processClock )
{
    m_receiverChannel.run( m_player.getLeafSender() );
    m_player.startup();

    m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
        m_player.getMegastructureInstallation().getParserPath(), "g_parserSymbol" );

    // fire and forget to the plugin the active project
    /*mega::U64 unityDBHashCode = 0U;
    {
        if( auto dbHashCodeOpt = m_player.getUnityDBHashCode() )
        {
            unityDBHashCode = dbHashCodeOpt.value().get();
        }
    }*/
}

Executor::~Executor()
{
    try
    {
        shutdown();
    }
    catch( std::exception& ex )
    {
        SPDLOG_ERROR( "Exception shutting down executor: {}", ex.what() );
    }
    catch( ... )
    {
        SPDLOG_ERROR( "Unknown exception shutting down executor" );
    }
}

void Executor::getGeneralStatusReport( const URL& url, Branch& report )
{
    using namespace std::string_literals;

    Table threads;
    for( const auto& threadID : reportLogicalThreads() )
    {
        threads.m_rows.push_back( { Line{ threadID } } );
    }

    Table table;
    // clang-format off
    table.m_rows.push_back( { Line{ "     Process: "s }, Line{ m_strProcessName } } );
    table.m_rows.push_back( { Line{ "     Threads: "s }, Line{ std::to_string( m_numThreads ) } } );
    table.m_rows.push_back( { Line{ "Mega Threads: "s }, threads } );
    // clang-format on

    report.m_elements.push_back( table );
}

class ExecutorShutdownPromise : public ExecutorRequestLogicalThread
{
    std::promise< void >&          m_promise;
    std::vector< Simulation::Ptr > m_simulations;

public:
    ExecutorShutdownPromise( Executor& exe, std::promise< void >& promise, std::vector< Simulation::Ptr > simulations )
        : ExecutorRequestLogicalThread( exe, exe.createLogicalThreadID() )
        , m_promise( promise )
        , m_simulations( simulations )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        SPDLOG_TRACE( "ExecutorShutdownPromise run" );
        for( Simulation::Ptr pSim : m_simulations )
        {
            network::sim::Request_Sender rq( *this, pSim, yield_ctx );
            rq.SimDestroyBlocking();
        }
        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
    }
};

void Executor::shutdown()
{
    SPDLOG_TRACE( "Executor shutdown" );
    {
        std::vector< Simulation::Ptr > simulations;
        getSimulations( simulations );
        if( !simulations.empty() )
        {
            SPDLOG_WARN( "Simulations still running when shutting executor down" );
            std::promise< void > promise;
            std::future< void >  future = promise.get_future();
            logicalthreadInitiated( std::make_shared< ExecutorShutdownPromise >( *this, promise, simulations ) );
            future.get();
        }
    }
    {
        std::vector< Simulation::Ptr > simulations;
        getSimulations( simulations );
        VERIFY_RTE_MSG( simulations.empty(), "Failed to shutdown all simulations" );
    }
    m_receiverChannel.stop();
    SPDLOG_TRACE( "Executor shutdown completed" );
}

class ExecutorShutdown : public ExecutorRequestLogicalThread
{
    std::vector< Simulation::Ptr > m_simulations;
    network::ConcurrentChannel&    completionChannel;

public:
    ExecutorShutdown( Executor& exe, network::ConcurrentChannel& completionChannel,
                      std::vector< Simulation::Ptr > simulations )
        : ExecutorRequestLogicalThread( exe, exe.createLogicalThreadID() )
        , completionChannel( completionChannel )
        , m_simulations( simulations )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        SPDLOG_TRACE( "ExecutorShutdown run async" );
        for( Simulation::Ptr pSim : m_simulations )
        {
            network::sim::Request_Sender rq( *this, pSim, yield_ctx );
            SPDLOG_TRACE( "ExecutorShutdown sending SimDestroyBlocking for: {}", pSim->getThisMPO() );
            rq.SimDestroyBlocking();
            SPDLOG_TRACE( "ExecutorShutdown SimDestroyBlocking returned for: {}", pSim->getThisMPO() );
        }

        boost::system::error_code ec;
        completionChannel.async_send( ec, network::make_disconnect_error_msg( this->getID(), "" ), yield_ctx );
    }
};

void Executor::shutdown( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "Executor shutdown" );
    {
        std::vector< Simulation::Ptr > simulations;
        getSimulations( simulations );
        if( !simulations.empty() )
        {
            SPDLOG_WARN( "Simulations still running when shutting executor down" );
            network::ConcurrentChannel completionChannel( m_io_context );
            logicalthreadInitiated( std::make_shared< ExecutorShutdown >( *this, completionChannel, simulations ) );
            boost::system::error_code ec;
            completionChannel.async_receive( yield_ctx[ ec ] );
        }
    }
    {
        std::vector< Simulation::Ptr > simulations;
        getSimulations( simulations );
        VERIFY_RTE_MSG( simulations.empty(), "Failed to shutdown all simulations" );
    }

    m_receiverChannel.stop();
    SPDLOG_TRACE( "Executor shutdown completed" );
}

network::LogicalThreadBase::Ptr Executor::joinLogicalThread( const network::Message& msg )
{
    SPDLOG_TRACE( "Executor::joinLogicalThread {}", msg.getLogicalThreadID() );
    return network::LogicalThreadBase::Ptr( new ExecutorRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
}

void Executor::getSimulations( std::vector< std::shared_ptr< Simulation > >& simulations ) const
{
    ReadLock lock( m_mutex );
    for( const auto& [ id, pSim ] : m_simulations )
    {
        simulations.push_back( pSim );
    }
}
std::shared_ptr< Simulation > Executor::getSimulation( const runtime::MPO& mpo ) const
{
    ReadLock lock( m_mutex );
    auto     iFind = m_simulations.find( mpo );
    if( iFind != m_simulations.end() )
    {
        return iFind->second;
    }
    else
        return Simulation::Ptr{};
}

runtime::MPO Executor::createSimulation( network::LogicalThread&     callingLogicalThread,
                                         boost::asio::yield_context& yield_ctx )
{
    Simulation::Ptr pSimulation;
    {
        WriteLock lock( m_mutex );
        // NOTE: duplicate of LogicalThreadManager::createLogicalThreadID
        const network::LogicalThreadID id;
        SPDLOG_TRACE( "Executor::createSimulation {} {}", m_strProcessName, id );
        pSimulation = std::make_shared< Simulation >( *this, id, m_processClock );
        m_logicalthreads.insert( std::make_pair( pSimulation->getID(), pSimulation ) );
        spawnInitiatedLogicalThread( pSimulation );
    }

    network::sim::Request_Sender rq( callingLogicalThread, pSimulation, yield_ctx );
    const runtime::MPO           simMPO = rq.SimCreate();
    SPDLOG_TRACE( "Executor::createSimulation SimCreate returned {}", simMPO );
    {
        WriteLock lock( m_mutex );
        m_simulations.insert( { simMPO, pSimulation } );
    }
    return simMPO;
}

void Executor::simulationTerminating( std::shared_ptr< Simulation > pSimulation )
{
    WriteLock lock( m_mutex );
    m_simulations.erase( pSimulation->getThisMPO() );
}

void Executor::logicalthreadCompleted( network::LogicalThreadBase::Ptr pLogicalThread )
{
    if( Simulation::Ptr pSim = std::dynamic_pointer_cast< Simulation >( pLogicalThread ) )
    {
        // if the simulation failed to construct then the mpo will not be initialised yet so check
        const mega::runtime::PointerHeap root = pSim->getThisRoot();
        if( root.valid() )
        {
            WriteLock lock( m_mutex );
            m_simulations.erase( root.getMPO() );
        }
        else
        {
            SPDLOG_WARN( "Executor::logicalthreadCompleted simulation failed to initialise before completing" );
        }
    }
    network::LogicalThreadManager::logicalthreadCompleted( pLogicalThread );
}

} // namespace mega::service
