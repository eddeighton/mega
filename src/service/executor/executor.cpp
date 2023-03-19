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

Executor::Executor( boost::asio::io_context& io_context, int numThreads, short daemonPortNumber )
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
          network::Node::Executor, daemonPortNumber )
{
    m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
        m_leaf.getMegastructureInstallation().getParserPath(), "g_parserSymbol" );
}

Executor::~Executor()
{
    m_receiverChannel.stop();
    SPDLOG_TRACE( "Executor shutdown" );
}

void Executor::shutdown()
{
    m_receiverChannel.stop();
}

network::ConversationBase::Ptr Executor::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                           const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new ExecutorRequestConversation( *this, msg.getReceiverID(), originatingConnectionID ) );
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
    if( Simulation::Ptr pSim = std::dynamic_pointer_cast< Simulation >( pConversation ) )
    {
        // if the simulation failed to construct then the mpo will not be initialised yet so check
        const mega::reference root = pSim->getThisRoot();
        if( root.is_valid() )
        {
            WriteLock lock( m_mutex );
            m_simulations.erase( root.getMPO() );
        }
        else
        {
            SPDLOG_WARN( "Executor::conversationCompleted simulation failed to initialise before completing" );
        }
    }
    network::ConversationManager::conversationCompleted( pConversation );
}

} // namespace mega::service
