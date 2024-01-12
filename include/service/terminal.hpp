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

#ifndef SERVICE_24_MAY_2022
#define SERVICE_24_MAY_2022

#include "service/leaf.hpp"

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "service/network/network.hpp"
#include "log/log.hpp"

#include "service/network/logical_thread_manager.hpp"
#include "mega/values/service/logical_thread_id.hpp"
#include "mega/values/compilation/megastructure_installation.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <optional>
#include <string>
#include <optional>
#include <vector>
#include <thread>
#include <functional>

namespace mega::service
{

class Terminal : public network::LogicalThreadManager
{
    friend class TerminalRequestLogicalThread;

public:
    Terminal( network::Log log, short daemonPortNumber = network::MegaDaemonPort() );
    ~Terminal();

    void shutdown();

    // network::LogicalThreadManager
    virtual network::LogicalThreadBase::Ptr joinLogicalThread( const network::Message& msg );

    MegastructureInstallation GetMegastructureInstallation();
    void                      ClearStash();
    network::Status           GetNetworkStatus();
    pipeline::PipelineResult  PipelineRun( const pipeline::Configuration& pipelineConfig );
    runtime::MP               ExecutorCreate( runtime::MachineID daemonMachineID );
    void                      ExecutorDestroy( const runtime::MP& mp );
    runtime::MPO              SimCreate( const runtime::MP& mp );
    void                      SimDestroy( const runtime::MPO& mpo );
    runtime::TimeStamp        SimRead( const runtime::MPO& from, const runtime::MPO& to );
    runtime::TimeStamp        SimWrite( const runtime::MPO& from, const runtime::MPO& to );
    void                      SimRelease( const runtime::MPO& from, const runtime::MPO& to );
    std::string               PingMP( const runtime::MP& mp, const std::string& strMsg );
    std::string               PingMPO( const runtime::MPO& mpo, const std::string& strMsg );
    void                      SimErrorCheck( const runtime::MPO& mpo );
    void                      ProgramLoad( const service::Program& program, const runtime::MP& mp );
    void                      ProgramUnload( const runtime::MP& mp );
    service::Program          ProgramGet( const runtime::MP& mp );

    network::Sender::Ptr getLeafSender() { return m_leaf.getLeafSender(); }

private:
    using Router        = std::function< network::Message( const network::Message& ) >;
    using RouterFactory = std::function< Router(
        network::LogicalThread&, network::Sender::Ptr, boost::asio::yield_context& yield_ctx ) >;

    RouterFactory makeTermRoot();
    RouterFactory makeMP( runtime::MP mp );
    RouterFactory makeMPO( runtime::MPO mpo );

    network::Message routeGenericRequest( const network::LogicalThreadID& logicalthreadID,
                                          const network::Message&         message,
                                          RouterFactory                   router );

    template < typename RequestType >
    RequestType getRootRequest()
    {
        const network::LogicalThreadID logicalthreadID;
        using namespace std::placeholders;
        return RequestType(
            std::bind( &Terminal::routeGenericRequest, this, logicalthreadID, _1, makeTermRoot() ), logicalthreadID );
    }

    template < typename RequestType >
    RequestType getMPRequest( runtime::MP mp )
    {
        const network::LogicalThreadID logicalthreadID;
        using namespace std::placeholders;
        return RequestType(
            std::bind( &Terminal::routeGenericRequest, this, logicalthreadID, _1, makeMP( mp ) ), logicalthreadID );
    }

    template < typename RequestType >
    RequestType getMPORequest( runtime::MPO mpo )
    {
        const network::LogicalThreadID logicalthreadID;
        using namespace std::placeholders;
        return RequestType(
            std::bind( &Terminal::routeGenericRequest, this, logicalthreadID, _1, makeMPO( mpo ) ), logicalthreadID );
    }

private:
    network::Log             m_log;
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    Leaf                     m_leaf;
};

} // namespace mega::service

#endif // SERVICE_24_MAY_2022
