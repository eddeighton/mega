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

#include "service/terminal.hpp"

#include "request.hpp"

#include "pipeline/configuration.hpp"

#include "service/network/logical_thread.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "service/network/end_point.hpp"
#include "log/log.hpp"

#include "mega/values/service/logical_thread_id.hpp"

#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/pipeline.hxx"
#include "service/protocol/model/host.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/stash.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/term_leaf.hxx"

#include "common/requireSemicolon.hpp"

#include <spdlog/spdlog.h>

#include <boost/asio/use_future.hpp>
#include <boost/system/detail/error_code.hpp>

#include <exception>
#include <optional>
#include <future>

namespace mega::service
{

template < typename TLogicalThreadFunctor >
class GenericLogicalThread : public TerminalRequestLogicalThread
{
    TLogicalThreadFunctor m_functor;

public:
    GenericLogicalThread( Terminal& terminal, const network::LogicalThreadID& logicalthreadID,
                          TLogicalThreadFunctor&& functor )
        : TerminalRequestLogicalThread( terminal, logicalthreadID )
        , m_functor( functor )
    {
    }
    void run( boost::asio::yield_context& yield_ctx ) { m_functor( *this, m_terminal.getLeafSender(), yield_ctx ); }
};

Terminal::Terminal( network::Log log, short daemonPortNumber )
    : network::LogicalThreadManager( network::Node::makeProcessName( network::Node::Terminal ), m_io_context )
    , m_log( log )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf( m_receiverChannel.getSender(), network::Node::Terminal, daemonPortNumber )
{
    m_receiverChannel.run( m_leaf.getLeafSender() );
    m_leaf.startup();
}

Terminal::~Terminal()
{
    m_receiverChannel.stop();
    m_io_context.run();
}

void Terminal::shutdown()
{
    m_receiverChannel.stop();
}

network::LogicalThreadBase::Ptr Terminal::joinLogicalThread( const network::Message& msg )
{
    return network::LogicalThreadBase::Ptr( new TerminalRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
}

network::Message Terminal::routeGenericRequest( const network::LogicalThreadID& logicalthreadID,
                                                const network::Message& message, RouterFactory router )
{
    SPDLOG_TRACE( "Terminal::rootRequest" );
    class RootLogicalThread : public TerminalRequestLogicalThread
    {
    public:
        using ResultType = std::optional< std::variant< network::Message, std::exception_ptr > >;

        RootLogicalThread( Terminal& terminal, const network::LogicalThreadID& logicalthreadID,
                           const network::Message& msg, RouterFactory& router, ResultType& result )
            : TerminalRequestLogicalThread( terminal, logicalthreadID )
            , m_router( router )
            , m_message( msg )
            , m_result( result )
        {
            SPDLOG_TRACE( "Terminal::rootRequest::RootLogicalThread::ctor {}", m_message );
        }
        void run( boost::asio::yield_context& yield_ctx )
        {
            try
            {
                SPDLOG_TRACE( "Terminal::rootRequest::RootLogicalThread::run {}", m_message );
                m_result = m_router( *this, m_terminal.getLeafSender(), yield_ctx )( m_message );
            }
            catch( std::exception& ex )
            {
                m_result = std::current_exception();
            }
        }

    private:
        RouterFactory&   m_router;
        network::Message m_message;
        ResultType&      m_result;
    };

    RootLogicalThread::ResultType result;
    {
        logicalthreadInitiated(
            std::make_shared< RootLogicalThread >( *this, logicalthreadID, message, router, result ) );
    }

    while( !result.has_value() )
        m_io_context.run_one();

    if( result->index() == 1 )
    {
        std::rethrow_exception( std::get< std::exception_ptr >( result.value() ) );
    }
    else
    {
        return std::get< network::Message >( result.value() );
    }
}

Terminal::RouterFactory Terminal::makeTermRoot()
{
    return []( network::LogicalThread& con, network::Sender::Ptr pSender, boost::asio::yield_context& yield_ctx )
    {
        return [ &con, pSender, &yield_ctx ]( const network::Message& msg ) -> network::Message
        { return network::term_leaf::Request_Sender( con, pSender, yield_ctx ).TermRoot( msg ); };
    };
}

Terminal::RouterFactory Terminal::makeMP( runtime::MP mp )
{
    return [ mp ]( network::LogicalThread& con, network::Sender::Ptr pSender, boost::asio::yield_context& yield_ctx )
    {
        return [ mp, &con, pSender, &yield_ctx ]( const network::Message& msg ) -> network::Message
        { return network::mpo::Request_Sender( con, pSender, yield_ctx ).MPUp( msg, mp ); };
    };
}

Terminal::RouterFactory Terminal::makeMPO( runtime::MPO mpo )
{
    return [ mpo ]( network::LogicalThread& con, network::Sender::Ptr pSender, boost::asio::yield_context& yield_ctx )
    {
        return [ mpo, &con, pSender, &yield_ctx ]( const network::Message& msg ) -> network::Message
        { return network::mpo::Request_Sender( con, pSender, yield_ctx ).MPOUp( msg, mpo ); };
    };
}

MegastructureInstallation Terminal::GetMegastructureInstallation()
{
    //
    return getRootRequest< network::project::Request_Encoder >().GetMegastructureInstallation();
}

void Terminal::ClearStash()
{
    //
    getRootRequest< network::stash::Request_Encoder >().StashClear();
}

network::Status Terminal::GetNetworkStatus()
{
    return getRootRequest< network::status::Request_Encoder >().GetNetworkStatus();
}

pipeline::PipelineResult Terminal::PipelineRun( const pipeline::Configuration& pipelineConfig )
{
    return getRootRequest< network::pipeline::Request_Encoder >().PipelineRun( pipelineConfig );
}

runtime::MP Terminal::ExecutorCreate( runtime::MachineID daemonMachineID )
{
    return getRootRequest< network::enrole::Request_Encoder >().EnroleCreateExecutor( daemonMachineID );
}
void Terminal::ExecutorDestroy( const runtime::MP& mp )
{
    return getMPRequest< network::enrole::Request_Encoder >( mp ).EnroleDestroy();
}

runtime::MPO Terminal::SimCreate( const runtime::MP& mp )
{
    return getMPRequest< network::sim::Request_Encoder >( mp ).SimCreate();
}

void Terminal::SimDestroy( const runtime::MPO& mpo )
{
    return getMPORequest< network::sim::Request_Encoder >( mpo ).SimDestroy();
}

runtime::TimeStamp Terminal::SimRead( const runtime::MPO& from, const runtime::MPO& to )
{
    return getMPORequest< network::sim::Request_Encoder >( to ).SimLockRead( from, to );
}
runtime::TimeStamp Terminal::SimWrite( const runtime::MPO& from, const runtime::MPO& to )
{
    return getMPORequest< network::sim::Request_Encoder >( to ).SimLockWrite( from, to );
}
void Terminal::SimRelease( const runtime::MPO& from, const runtime::MPO& to )
{
    return getMPORequest< network::sim::Request_Encoder >( to ).SimLockRelease( from, to, network::Transaction{} );
}

std::string Terminal::PingMP( const runtime::MP& mp, const std::string& strMsg )
{
    return getMPRequest< network::status::Request_Encoder >( mp ).Ping( strMsg );
}

std::string Terminal::PingMPO( const runtime::MPO& mpo, const std::string& strMsg )
{
    return getMPORequest< network::status::Request_Encoder >( mpo ).Ping( strMsg );
}

void Terminal::SimErrorCheck( const runtime::MPO& mpo )
{
    return getMPORequest< network::sim::Request_Encoder >( mpo ).SimErrorCheck();
}
void Terminal::ProgramLoad( const service::Program& program, const runtime::MP& mp )
{
    return getMPRequest< network::host::Request_Encoder >( mp ).LoadProgram( program );
}
void Terminal::ProgramUnload( const runtime::MP& mp )
{
    return getMPRequest< network::host::Request_Encoder >( mp ).UnloadProgram();
}
service::Program Terminal::ProgramGet( const runtime::MP& mp )
{
    return getMPRequest< network::host::Request_Encoder >( mp ).GetProgram();
}
} // namespace mega::service
