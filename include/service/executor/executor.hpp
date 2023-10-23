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

#ifndef EXECUTOR_27_MAY_2022
#define EXECUTOR_27_MAY_2022

#include "clock.hpp"

#include "service/leaf.hpp"

#include "service/network/logical_thread_manager.hpp"
#include "mega/values/compilation/megastructure_installation.hpp"

#include "parser/parser.hpp"
#include "mega/values/service/logical_thread_id.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <optional>
#include <string>
#include <optional>
#include <vector>
#include <thread>

namespace mega::service
{
class Simulation;

class Executor : public network::LogicalThreadManager
{
    friend class ExecutorRequestLogicalThread;
    friend class JobLogicalThread;
    friend class Simulation;
    friend class Platform;
    friend class PlayerNetwork;

    using SimulationMap = std::unordered_map< mega::MPO, std::shared_ptr< Simulation >, mega::MPO::Hash >;

public:
    Executor( boost::asio::io_context& io_context,
              network::Log             log,
              U64                      numThreads,
              short                    daemonPortNumber,
              ProcessClock&            processClock,
              network::Node            nodeType );

    ~Executor() override;

    void getGeneralStatusReport( const mega::reports::URL& url, mega::reports::Branch& report );
    
    void shutdown();
    void shutdown( boost::asio::yield_context& yield_ctx );

    U64 getNumThreads() const { return m_numThreads; }

    // network::LogicalThreadManager
    network::LogicalThreadBase::Ptr joinLogicalThread( const network::Message& msg ) override;

    network::Sender::Ptr getLeafSender() { return m_leaf.getLeafSender(); }

    void                          getSimulations( std::vector< std::shared_ptr< Simulation > >& simulations ) const;
    std::shared_ptr< Simulation > getSimulation( const mega::MPO& mpo ) const;
    mega::MPO createSimulation( network::LogicalThread& callingLogicalThread, boost::asio::yield_context& yield_ctx );
    void      simulationTerminating( std::shared_ptr< Simulation > pSimulation );
    void      logicalthreadCompleted( network::LogicalThreadBase::Ptr pLogicalThread ) override;

    void updateActiveProjectToClock();

private:
    network::Log                             m_log;
    boost::asio::io_context&                 m_io_context;
    U64                                      m_numThreads;
    ProcessClock&                            m_processClock;
    boost::shared_ptr< EG_PARSER_INTERFACE > m_pParser;
    network::ReceiverChannel                 m_receiverChannel;
    Leaf                                     m_leaf;
    SimulationMap                            m_simulations;
    MegastructureInstallation                m_megastructureInstallation;
};

} // namespace mega::service

#endif // EXECUTOR_27_MAY_2022
