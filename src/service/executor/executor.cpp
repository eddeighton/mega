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

#include "service/executor.hpp"

#include "request.hpp"
#include "job.hpp"
#include "simulation.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/project.hxx"
#include "service/protocol/model/sim.hxx"

#include "parser/parser.hpp"

#include "boost/dll.hpp"
#include "boost/dll/shared_library_load_mode.hpp"

#include <optional>
#include <future>
#include <thread>

namespace mega
{
namespace service
{

template < typename TConversationFunctor >
class GenericConversation : public ExecutorRequestConversation
{
    TConversationFunctor m_functor;

public:
    GenericConversation( Executor& executor, const network::ConversationID& conversationID,
                         const network::ConnectionID& originatingConnectionID, TConversationFunctor&& functor )
        : ExecutorRequestConversation( executor, conversationID, originatingConnectionID )
        , m_functor( functor )
    {
    }

    void run( boost::asio::yield_context& yield_ctx ) { m_functor( *this, m_executor.getLeafSender(), yield_ctx ); }
};

Executor::Executor( boost::asio::io_context& io_context, int numThreads )
    : network::ConversationManager( network::makeProcessName( network::Node::Executor ), io_context )
    , m_io_context( io_context )
    , m_numThreads( numThreads )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf(
          [ &m_receiverChannel = m_receiverChannel ]()
          {
              m_receiverChannel.run( network::makeProcessName( network::Node::Executor ) );
              return m_receiverChannel.getSender();
          }(),
          network::Node::Executor )
{
    // determine megastructure installation from the root so can load the parser dll
    // initialise the runtime using root active project if there is one
    {
        Executor& thisRef = *this;
        auto      func    = [ &thisRef ](
                        network::ConversationBase& con, network::Sender& sender, boost::asio::yield_context& yield_ctx )
        {
            network::exe_leaf::Request_Sender exe_leaf( con, sender, yield_ctx );
            network::project::Request_Encoder project(
                [ &exe_leaf ]( const network::Message& msg ) { return exe_leaf.ExeRoot( msg ); }, con.getID() );

            thisRef.m_megastructureInstallation = project.GetMegastructureInstallation();
            VERIFY_RTE_MSG( !thisRef.m_megastructureInstallation.isEmpty(),
                            "Invalid mega structure installation returned from root" );
            thisRef.m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
                thisRef.m_megastructureInstallation.getParserPath(), "g_parserSymbol" );

            auto currentProject = project.GetProject();
            if ( !currentProject.isEmpty() && boost::filesystem::exists( currentProject.getProjectDatabase() ) )
            {
                mega::runtime::initialiseRuntime( thisRef.m_megastructureInstallation, currentProject );
                SPDLOG_TRACE(
                    "Executor runtime initialised with project: {}", currentProject.getProjectInstallPath().string() );
            }
            else
            {
                SPDLOG_TRACE( "Could not initialised runtime.  No active project" );
            }
        };
        conversationInitiated( network::ConversationBase::Ptr( new GenericConversation(
                                   *this, createConversationID( getLeafSender().getConnectionID() ),
                                   getLeafSender().getConnectionID(), std::move( func ) ) ),
                               m_leaf );
    }
}

Executor::~Executor()
{
    m_receiverChannel.stop();
    SPDLOG_TRACE( "Executor shutdown" );
}

void Executor::shutdown() { m_receiverChannel.stop(); }

network::ConversationBase::Ptr Executor::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                           const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new ExecutorRequestConversation( *this, getMsgReceiver( msg ), originatingConnectionID ) );
}
/*
void Executor::reportSimulations( network::Status& status ) const
{
    ReadLock lock( m_mutex );
    for( const auto& [ id, pSim ] : m_simulations )
    {
        network::Status simStatus;

    }

}*/

void Executor::getSimulations( std::vector< std::shared_ptr< Simulation > >& simulations ) const
{
    ReadLock lock( m_mutex );
    for ( const auto& [ id, pSim ] : m_simulations )
    {
        simulations.push_back( pSim );
    }
}
std::shared_ptr< Simulation > Executor::getSimulation( const mega::MPO& mpo ) const
{
    ReadLock lock( m_mutex );
    auto     iFind = m_simulations.find( mpo );
    if ( iFind != m_simulations.end() )
    {
        return iFind->second;
    }
    else
        return Simulation::Ptr{};
}

mega::MPO Executor::createSimulation( network::ConversationBase&  callingConversation,
                                      boost::asio::yield_context& yield_ctx )
{
    Simulation::Ptr pSimulation;
    {
        WriteLock lock( m_mutex );
        // NOTE: duplicate of ConversationManager::createConversationID
        const network::ConversationID id( ++m_nextConversationID, getLeafSender().getConnectionID() );
        pSimulation = std::make_shared< Simulation >( *this, id );
        m_conversations.insert( std::make_pair( pSimulation->getID(), pSimulation ) );
        spawnInitiatedConversation( pSimulation, getLeafSender() );
        SPDLOG_TRACE( "Executor::createSimulation {}", pSimulation->getID() );
    }

    network::sim::Request_Sender rq( callingConversation, pSimulation->getID(), *pSimulation, yield_ctx );
    const mega::MPO              simMPO = rq.SimCreate();
    SPDLOG_TRACE( "Executor::createSimulation sinCreate returned {}", simMPO );

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

void Executor::conversationCompleted( network::ConversationBase::Ptr pConversation )
{
    if ( Simulation::Ptr pSim = std::dynamic_pointer_cast< Simulation >( pConversation ) )
    {
        WriteLock lock( m_mutex );
        m_simulations.erase( pSim->getThisMPO() );
    }
    network::ConversationManager::conversationCompleted( pConversation );
}

} // namespace service
} // namespace mega
