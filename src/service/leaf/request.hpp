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

#ifndef REQUEST_LEAF_23_SEPT_2022
#define REQUEST_LEAF_23_SEPT_2022

#include "llvm_compiler.hpp"

#include "service/leaf.hpp"

#include "service/protocol/model/term_leaf.hxx"
#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/python_leaf.hxx"
#include "service/protocol/model/report_leaf.hxx"
#include "service/protocol/model/daemon_leaf.hxx"
#include "service/protocol/model/mpo.hxx"

#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/leaf_exe.hxx"
#include "service/protocol/model/leaf_tool.hxx"
#include "service/protocol/model/leaf_python.hxx"
#include "service/protocol/model/leaf_report.hxx"
#include "service/protocol/model/leaf_term.hxx"

#include "service/protocol/model/status.hxx"
#include "service/protocol/model/report.hxx"
#include "service/protocol/model/job.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/jit.hxx"
#include "service/protocol/model/enrole.hxx"

// #include "service/protocol/common/context.hpp"

namespace mega::service
{

class LeafRequestLogicalThread : public network::InThreadLogicalThread,

                                 public network::term_leaf::Impl,
                                 public network::exe_leaf::Impl,
                                 public network::tool_leaf::Impl,
                                 public network::python_leaf::Impl,
                                 public network::report_leaf::Impl,
                                 public network::daemon_leaf::Impl,

                                 public network::mpo::Impl,
                                 public network::status::Impl,
                                 public network::report::Impl,
                                 public network::job::Impl,
                                 public network::memory::Impl,
                                 public network::jit::Impl,
                                 public network::project::Impl,
                                 public network::enrole::Impl
{
protected:
    Leaf& m_leaf;

public:
    LeafRequestLogicalThread( Leaf& leaf, const network::LogicalThreadID& logicalthreadID );
    virtual ~LeafRequestLogicalThread();

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;

    network::leaf_daemon::Request_Sender getDaemonSender( boost::asio::yield_context& yield_ctx );
    network::leaf_exe::Request_Sender    getExeSender( boost::asio::yield_context& yield_ctx );
    network::leaf_tool::Request_Sender   getToolSender( boost::asio::yield_context& yield_ctx );
    network::leaf_python::Request_Sender getPythonSender( boost::asio::yield_context& yield_ctx );
    network::leaf_report::Request_Sender getReportSender( boost::asio::yield_context& yield_ctx );
    network::leaf_term::Request_Sender   getTermSender( boost::asio::yield_context& yield_ctx );

    network::mpo::Request_Sender getMPOUpSender( boost::asio::yield_context& yield_ctx );
    network::mpo::Request_Sender getMPODownSender( boost::asio::yield_context& yield_ctx );

    LLVMCompilerImpl getLLVMCompiler( boost::asio::yield_context& yield_ctx )
    {
        THROW_TODO;
        // return { *this, m_leaf.getDaemonSender(), m_leaf.getMegastructureInstallation(), m_leaf.getActiveProject(),
        //          yield_ctx };
    }

    // network::term_leaf::Impl
    virtual network::Message TermRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;

    // network::exe_leaf::Impl
    virtual network::Message ExeRoot( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ExeDaemon( const network::Message&     request,
                                        boost::asio::yield_context& yield_ctx ) override;

    // network::tool_leaf::Impl
    virtual network::Message ToolRoot( const network::Message&     request,
                                       boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ToolDaemon( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;

    // network::python_leaf::Impl
    virtual network::Message PythonRoot( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message PythonDaemon( const network::Message&     request,
                                           boost::asio::yield_context& yield_ctx ) override;

    // network::report_leaf::Impl
    virtual network::Message ReportRoot( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ReportDaemon( const network::Message&     request,
                                           boost::asio::yield_context& yield_ctx ) override;

    // network::mpo::Impl
    virtual network::Message MPRoot( const network::Message& request, const MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPDown( const network::Message& request, const MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPUp( const network::Message& request, const MP& mp,
                                   boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPODown( const network::Message& request, const MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPOUp( const network::Message& request, const MPO& mpo,
                                    boost::asio::yield_context& yield_ctx ) override;

    // network::daemon_leaf::Impl
    virtual network::Message RootAllBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExeBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message RootExe( const network::Message& request, boost::asio::yield_context& yield_ctx ) override;
    virtual void RootSimRun( const MPO& mpo, boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message DaemonLeafBroadcast( const network::Message&     request,
                                                  boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx ) override;

    // network::report::Impl
    virtual mega::reports::Container GetReport( const mega::reports::URL&                      url,
                                                const std::vector< mega::reports::Container >& report,
                                                boost::asio::yield_context&                    yield_ctx ) override;

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

    // network::memory::Impl
    virtual void      MPODestroyed( const MPO& mpo, boost::asio::yield_context& yield_ctx ) override;
    virtual reference NetworkToHeap( const reference& ref, const TimeStamp& lockCycle,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual reference NetworkAllocate( const MPO& parent, const TypeID& objectTypeID, const TimeStamp& lockCycle,
                                       boost::asio::yield_context& yield_ctx ) override;

    // public network::jit::Impl
    virtual void   GetAllocator( const TypeID&               objectTypeID,
                                 const U64&                  jitAllocatorPtr,
                                 boost::asio::yield_context& yield_ctx ) override;
    virtual void   ExecuteJIT( const runtime::JITFunctor& func, boost::asio::yield_context& yield_ctx ) override;
    virtual TypeID GetInterfaceTypeID( const mega::TypeID&         concreteTypeID,
                                       boost::asio::yield_context& yield_ctx ) override;
};

} // namespace mega::service

#endif // REQUEST_LEAF_23_SEPT_2022
