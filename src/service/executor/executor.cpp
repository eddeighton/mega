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
#include "service/network/log.hpp"

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

Executor::Executor( boost::asio::io_context& io_context, U64 numThreads, short daemonPortNumber,
                    ProcessClock& processClock, network::Node::Type nodeType )
    : network::LogicalThreadManager( network::makeProcessName( nodeType ), io_context )
    , m_io_context( io_context )
    , m_numThreads( numThreads )
    , m_processClock( processClock )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf( m_receiverChannel.getSender(), nodeType, daemonPortNumber )
{
    m_receiverChannel.run( m_leaf.getLeafSender() );
    m_leaf.startup();

    m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
        m_leaf.getMegastructureInstallation().getParserPath(), "g_parserSymbol" );

    updateActiveProjectToClock();
}

void Executor::updateActiveProjectToClock()
{
    // fire and forget to the plugin the active project
    mega::U64 unityDBHashCode = 0U;
    {
        if( auto dbHashCodeOpt = m_leaf.getUnityDBHashCode() )
        {
            unityDBHashCode = dbHashCodeOpt.value().get();
        }
    }
    m_processClock.setActiveProject( m_leaf.getActiveProject(), unityDBHashCode );
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

class ExecutorShutdown : public ExecutorRequestLogicalThread
{
    std::promise< void >&          m_promise;
    std::vector< Simulation::Ptr > m_simulations;

public:
    ExecutorShutdown( Executor& exe, std::promise< void >& promise, std::vector< Simulation::Ptr > simulations )
        : ExecutorRequestLogicalThread( exe, exe.createLogicalThreadID() )
        , m_promise( promise )
        , m_simulations( simulations )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        SPDLOG_TRACE( "ExecutorShutdown run" );
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
    std::vector< Simulation::Ptr > simulations;
    getSimulations( simulations );
    if( !simulations.empty() )
    {
        SPDLOG_WARN( "Simulations still running when shutting executor down" );
        std::promise< void >            promise;
        std::future< void >             future = promise.get_future();
        logicalthreadInitiated( std::make_shared< ExecutorShutdown >( *this, promise, simulations ) );
        future.get();
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
std::shared_ptr< Simulation > Executor::getSimulation( const mega::MPO& mpo ) const
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

mega::MPO Executor::createSimulation( network::LogicalThread& callingLogicalThread,
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
    const mega::MPO              simMPO = rq.SimCreate();
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
        const mega::reference root = pSim->getThisRoot();
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
