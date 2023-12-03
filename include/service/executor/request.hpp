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

#ifndef EXECUTOR_REQUEST_22_JUNE_2022
#define EXECUTOR_REQUEST_22_JUNE_2022

#include "service/network/logical_thread.hpp"

#include "service/protocol/model/leaf_exe.hxx"
#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/job.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/report.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/enrole.hxx"

namespace mega::service
{

class Executor;

class ExecutorRequestLogicalThread : public network::ConcurrentLogicalThread,
                                     public network::leaf_exe::Impl,
                                     public network::mpo::Impl,
                                     public network::job::Impl,
                                     public network::status::Impl,
                                     public network::report::Impl,
                                     public network::sim::Impl,
                                     public network::project::Impl,
                                     public network::enrole::Impl
{
protected:
    Executor& m_executor;

public:
    ExecutorRequestLogicalThread( Executor& executor, const network::LogicalThreadID& logicalthreadID );
    virtual ~ExecutorRequestLogicalThread();

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    // helpers
    network::exe_leaf::Request_Sender getLeafRequest( boost::asio::yield_context& yield_ctx );
    network::mpo::Request_Sender      getMPRequest( boost::asio::yield_context& yield_ctx );

    template < typename RequestEncoderType >
    RequestEncoderType getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getLeafRequest( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return rootRequest.ExeRoot( msg ); },
                                   getID() );
    }

    template < typename RequestEncoderType >
    RequestEncoderType getDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getLeafRequest( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return rootRequest.ExeDaemon( msg ); },
                                   getID() );
    }

    // network::leaf_exe::Impl - NOTE: RootSimRun not implemented here
    virtual network::Message RootAllBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExeBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExe( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    // network::mpo::Impl
    virtual network::Message MPODown( const network::Message& request, const MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPDown( const network::Message& request, const MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx ) override;

    // network::report::Impl
    virtual mega::reports::Container GetReport( const mega::reports::URL&                      url,
                                                const std::vector< mega::reports::Container >& report,
                                                boost::asio::yield_context&                    yield_ctx ) override;
                                                
    // network::job::Impl - note also in JobLogicalThread
    virtual std::vector< network::LogicalThreadID >
    JobStart( const utilities::ToolChain&                                   toolChain,
              const pipeline::Configuration&                                configuration,
              const network::LogicalThreadID&                               rootLogicalThreadID,
              const std::vector< std::vector< network::LogicalThreadID > >& jobs,
              boost::asio::yield_context&                                   yield_ctx ) override;

    // network::sim::Impl
    virtual MPO SimCreate( boost::asio::yield_context& yield_ctx ) override;

    // network::enrole::Impl
    virtual void EnroleDestroy( boost::asio::yield_context& yield_ctx ) override;
};

} // namespace mega::service

#endif // EXECUTOR_REQUEST_22_JUNE_2022