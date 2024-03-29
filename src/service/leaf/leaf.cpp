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

#include "service/leaf.hpp"

#include "request.hpp"

#include "environment/environment.hpp"

#include "log/log.hpp"

#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/project.hxx"

#include "mega/values/service/logical_thread_id.hpp"

#include "service/remote_memory_manager.hpp"

#include "common/requireSemicolon.hpp"

#include <boost/system/detail/error_code.hpp>
#include <memory>

namespace mega::service
{

class LeafEnrole : public LeafRequestLogicalThread
{
    std::promise< void >& m_promise;

public:
    LeafEnrole( Leaf& leaf, std::promise< void >& promise )
        : LeafRequestLogicalThread( leaf, leaf.createLogicalThreadID() )
        , m_promise( promise )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        SPDLOG_TRACE( "LeafEnrole run" );

        auto daemonSender = getDaemonSender( yield_ctx );

        std::string startupUUID;
        if( const char* pszValue = std::getenv( network::ENV_PROCESS_UUID ) )
        {
            startupUUID = pszValue;
        }

        // enrole - getting MP
        {
            network::enrole::Request_Encoder encoder( [ &daemonSender ]( const network::Message& msg ) mutable
                                                      { return daemonSender.LeafDaemon( msg ); },
                                                      getID() );
            m_leaf.m_mp = encoder.EnroleLeafWithDaemon( startupUUID, m_leaf.getType() );

            SPDLOG_TRACE( "Leaf enrole mp: {}", m_leaf.m_mp );
        }

        // determine the current project and stuff and initialise the runtime
        {
            network::project::Request_Encoder projectRequest(
                [ &daemonSender ]( const network::Message& msg ) { return daemonSender.LeafRoot( msg ); }, getID() );
            m_leaf.m_megastructureInstallationOpt = projectRequest.GetMegastructureInstallation();
        }

        // set process description
        {
            std::ostringstream os;
            os << m_leaf.m_nodeType << " " << m_leaf.m_mp;
            common::ProcessID::setDescription( os.str().c_str() );
        }

        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
    }
};

Leaf::Leaf( network::Log log, network::Sender::Ptr pSender, network::Node nodeType, short daemonPortNumber )
    : network::LogicalThreadManager( network::Node::makeProcessName( network::Node::Leaf ), m_io_context )
    , m_log( std::move( log ) )
    , m_pSender( pSender )
    , m_nodeType( nodeType )
    , m_io_context( 1 ) // single threaded concurrency hint
    , m_receiverChannel( m_io_context, *this )
    , m_client( m_io_context, *this, "localhost", daemonPortNumber )
    , m_work_guard( m_io_context.get_executor() )
    , m_io_thread( [ &io_context = m_io_context ]() { io_context.run(); } )
{
    m_receiverChannel.run( pSender );
    m_pSelfSender = m_receiverChannel.getSender();
}

void Leaf::getGeneralStatusReport( const URL& url, Branch& report )
{
    using namespace std::string_literals;

    // THROW_TODO;
    // VERIFY_RTE( m_pRemoteMemoryManager );
    // const network::MemoryStatus remoteMemStatus = m_pRemoteMemoryManager->getStatus();

    Table tables;
    {
        Table table;
        // clang-format off
        table.m_rows.push_back( { Line{ "     Process: "s }, Line{ m_strProcessName } } );
        table.m_rows.push_back( { Line{ "   Node Type: "s }, Line{ m_nodeType } } );
        table.m_rows.push_back( { Line{ "          MP: "s }, Line{ m_mp } } );
        table.m_rows.push_back( { Line{ "     Program: "s }, Line{ getRuntime().getProgram() } } );
        table.m_rows.push_back( { Line{ "    Log File: "s }, Line{ getLog().logFile, report::makeFileURL( url, getLog().logFile ) } } );

        // table.m_rows.push_back( { Line{ "  Remote Mem: "s }, Line{ std::to_string( remoteMemStatus.m_heap ) } } );
        // table.m_rows.push_back( { Line{ "  Remote Obj: "s }, Line{ std::to_string( remoteMemStatus.m_object ) } } );
        // clang-format on

        // THROW_TODO;
        // if( m_unityDatabaseHashCode.has_value() )
        // {
        //     table.m_rows.push_back(
        //         { Line{ "Unity DBHash: "s }, Line{ m_unityDatabaseHashCode.value().toHexString() } } );
        // }

        tables.m_rows.push_back( { table } );
    }

    /* if( m_pJIT )
     {
         const network::JITStatus jitStatus = m_pJIT->getStatus();

         Table jitStatusTable;

         // clang-format off
         jitStatusTable.m_rows.push_back( {
             Line{ " Functions: "s }, Line{ std::to_string( jitStatus.m_functionPointers ) },
             Line{ "Allocators: "s }, Line{ std::to_string( jitStatus.m_allocators ) } } );
         jitStatusTable.m_rows.push_back( {
             Line{ "Relations:  "s }, Line{ std::to_string( jitStatus.m_relations ) },
             Line{ "Invocations:"s }, Line{ std::to_string( jitStatus.m_invocations ) } } );
         jitStatusTable.m_rows.push_back( {
             Line{ "Operators:  "s }, Line{ std::to_string( jitStatus.m_operators ) },
             Line{ "Interfaces: "s }, Line{ std::to_string( jitStatus.m_componentManagerStatus.m_interfaceComponents  )
     } } ); jitStatusTable.m_rows.push_back( { Line{ "Python:     "s }, Line{ std::to_string(
     jitStatus.m_componentManagerStatus.m_pythonComponents ) } , Line{ "Decisions:  "s }, Line{ std::to_string(
     jitStatus.m_decisions ) }

             } );
         // clang-format on

         tables.m_rows.back().push_back( jitStatusTable );
     }*/

    report.m_elements.push_back( tables );
}

void Leaf::startup()
{
    std::promise< void > promise;
    std::future< void >  future = promise.get_future();
    logicalthreadInitiated( std::make_shared< LeafEnrole >( *this, promise ) );
    future.get();
    VERIFY_RTE_MSG( m_megastructureInstallationOpt.has_value(), "No Megastructure Installation" );
    m_pRuntime = std::make_unique< runtime::Runtime >( Environment::workTmp(), m_megastructureInstallationOpt.value() );
}

Leaf::~Leaf()
{
    m_client.stop();
    m_receiverChannel.stop();
    m_work_guard.reset();
    m_io_thread.join();
}

// network::LogicalThreadManager
network::LogicalThreadBase::Ptr Leaf::joinLogicalThread( const network::Message& msg )
{
    switch( m_nodeType )
    {
        case network::Node::Leaf:
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Python:
        case network::Node::Report:
        case network::Node::Executor:
        case network::Node::Plugin:
            return network::LogicalThreadBase::Ptr( new LeafRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
            break;
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::TOTAL_NODE_TYPES:
        default:
            THROW_RTE( "Leaf: Unknown leaf type" );
            break;
    }
}

} // namespace mega::service
