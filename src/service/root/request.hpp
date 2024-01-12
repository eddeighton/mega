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

#ifndef REQUEST_22_SEPT_2022
#define REQUEST_22_SEPT_2022

#include "service/network/logical_thread.hpp"

#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"

#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/report.hxx"
#include "service/protocol/model/stash.hxx"
#include "service/protocol/model/job.hxx"
#include "service/protocol/model/sim.hxx"

namespace mega::service
{
class Root;

class RootRequestLogicalThread : public network::InThreadLogicalThread,

                                 public network::daemon_root::Impl,

                                 public network::mpo::Impl,
                                 public network::project::Impl,
                                 public network::enrole::Impl,
                                 public network::status::Impl,
                                 public network::report::Impl,
                                 public network::stash::Impl,
                                 public network::job::Impl,
                                 public network::sim::Impl
{
public:
    RootRequestLogicalThread( Root& root, const network::LogicalThreadID& logicalthreadID );
    virtual ~RootRequestLogicalThread();

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;

    // helpers
    network::root_daemon::Request_Sender getDaemonSender( boost::asio::yield_context& yield_ctx );
    network::Message broadcastAll( const network::Message& msg, boost::asio::yield_context& yield_ctx );
    network::Message broadcastExe( const network::Message& msg, boost::asio::yield_context& yield_ctx );

    template < typename RequestEncoderType >
    RequestEncoderType getExeRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ sender = getDaemonSender( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return sender.RootExe( msg ); },
                                   getID() );
    }
    template < typename RequestEncoderType >
    RequestEncoderType getExeBroadcastRequest( boost::asio::yield_context& yield_ctx )
    {
        RootRequestLogicalThread* pThis = this;
        return RequestEncoderType( [ pThis, &yield_ctx ]( const network::Message& msg ) mutable
                                   { return pThis->broadcastExe( msg, yield_ctx ); },
                                   getID() );
    }
    template < typename RequestEncoderType >
    RequestEncoderType getAllBroadcastRequest( boost::asio::yield_context& yield_ctx )
    {
        RootRequestLogicalThread* pThis = this;
        return RequestEncoderType( [ pThis, &yield_ctx ]( const network::Message& msg ) mutable
                                   { return pThis->broadcastAll( msg, yield_ctx ); },
                                   getID() );
    }

    // network::daemon_root::Impl
    virtual network::Message TermRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ExeRoot( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ToolRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message PythonRoot( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ReportRoot( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message LeafRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message DaemonRoot( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;

    // network::mpo::Impl
    virtual network::Message MPRoot( const network::Message& request, const runtime::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPODown( const network::Message& request, const runtime::MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPDown( const network::Message& request, const runtime::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPUp( const network::Message& request, const runtime::MP& mp,
                                   boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPOUp( const network::Message& request, const runtime::MPO& mpo,
                                    boost::asio::yield_context& yield_ctx ) override;

    // network::project::Impl
    virtual MegastructureInstallation GetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override;

    // network::enrole::Impl
    virtual runtime::MachineID EnroleDaemon( boost::asio::yield_context& yield_ctx ) override;
    virtual runtime::MP EnroleLeafWithRoot( const std::string& startupUUID, const runtime::MachineID& daemonMachineID,
                                            boost::asio::yield_context& yield_ctx ) override;
    virtual void        EnroleLeafDisconnect( const runtime::MP& mp, boost::asio::yield_context& yield_ctx ) override;
    virtual std::vector< runtime::MachineID > EnroleGetDaemons( boost::asio::yield_context& yield_ctx ) override;
    virtual std::vector< runtime::MP >        EnroleGetProcesses( const runtime::MachineID&   machineID,
                                                                  boost::asio::yield_context& yield_ctx ) override;
    virtual std::vector< runtime::MPO >       EnroleGetMPO( const runtime::MP&          machineProcess,
                                                            boost::asio::yield_context& yield_ctx ) override;
    virtual runtime::MP                       EnroleCreateExecutor( const runtime::MachineID&   daemonMachineID,
                                                                    boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual network::Status GetNetworkStatus( boost::asio::yield_context& yield_ctx ) override;

    // network::report::Impl
    virtual mega::reports::Container GetReport( const mega::reports::URL&                      url,
                                                const std::vector< mega::reports::Container >& report,
                                                boost::asio::yield_context&                    yield_ctx ) override;
    virtual mega::reports::Container GetNetworkReport( const mega::reports::URL&   url,
                                                       boost::asio::yield_context& yield_ctx ) override;

    // network::stash::Impl
    virtual void              StashClear( boost::asio::yield_context& yield_ctx ) override;
    virtual void              StashStash( const boost::filesystem::path& filePath,
                                          const task::DeterminantHash&   determinant,
                                          boost::asio::yield_context&    yield_ctx ) override;
    virtual bool              StashRestore( const boost::filesystem::path& filePath,
                                            const task::DeterminantHash&   determinant,
                                            boost::asio::yield_context&    yield_ctx ) override;
    virtual task::FileHash    BuildGetHashCode( const boost::filesystem::path& filePath,
                                                boost::asio::yield_context&    yield_ctx ) override;
    virtual void              BuildSetHashCode( const boost::filesystem::path& filePath,
                                                const task::FileHash&          hashCode,
                                                boost::asio::yield_context&    yield_ctx ) override;
    virtual mega::SymbolTable BuildGetSymbolTable( boost::asio::yield_context& yield_ctx ) override;
    virtual mega::SymbolTable BuildNewSymbols( const mega::SymbolRequest&  request,
                                               boost::asio::yield_context& yield_ctx ) override;

    // network::job::Impl
    virtual std::vector< network::LogicalThreadID >
    JobStart( const utilities::ToolChain&                                   toolChain,
              const pipeline::Configuration&                                configuration,
              const network::LogicalThreadID&                               rootLogicalThreadID,
              const std::vector< std::vector< network::LogicalThreadID > >& jobs,
              boost::asio::yield_context&                                   yield_ctx ) override
    {
        std::vector< network::LogicalThreadID > result;
        for( const auto& j : jobs )
        {
            for( const auto& k : j )
                result.push_back( k );
        }
        return result;
    }

    // network::sim::Impl
    virtual runtime::TimeStamp SimLockRead( const runtime::MPO& requestingMPO, const runtime::MPO& targetMPO,
                                            boost::asio::yield_context& yield_ctx ) override;
    virtual runtime::TimeStamp SimLockWrite( const runtime::MPO& requestingMPO, const runtime::MPO& targetMPO,
                                             boost::asio::yield_context& yield_ctx ) override;
    virtual void               SimLockRelease( const runtime::MPO&         requestingMPO,
                                               const runtime::MPO&         targetMPO,
                                               const network::Transaction& transaction,
                                               boost::asio::yield_context& yield_ctx ) override;

protected:
    Root& m_root;
};

} // namespace mega::service

#endif // REQUEST_22_SEPT_2022