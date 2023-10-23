

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

#include "tool_logical_thread.hpp"

namespace mega::service
{

ToolMPOLogicalThread::ToolMPOLogicalThread( Tool&                           tool,
                                            const network::LogicalThreadID& logicalthreadID,
                                            Tool::Functor&&                 functor )
    : ToolRequestLogicalThread( tool, logicalthreadID )
    , mega::MPOContext( getID() )
    , m_tool( tool )
    , m_functor( functor )
{
}

network::Message ToolMPOLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                               boost::asio::yield_context& yield_ctx )
{
    return ToolRequestLogicalThread::dispatchInBoundRequest( msg, yield_ctx );
}

network::tool_leaf::Request_Sender ToolMPOLogicalThread::getToolRequest( boost::asio::yield_context& yield_ctx )
{
    return network::tool_leaf::Request_Sender( *this, m_tool.getLeafSender(), yield_ctx );
}
network::mpo::Request_Sender ToolMPOLogicalThread::getMPRequest( boost::asio::yield_context& yield_ctx )
{
    return network::mpo::Request_Sender( *this, m_tool.getLeafSender(), yield_ctx );
}
network::enrole::Request_Encoder ToolMPOLogicalThread::getRootEnroleRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getToolRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ToolRoot( msg ); },
             getID() };
}
network::stash::Request_Encoder ToolMPOLogicalThread::getRootStashRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getToolRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ToolRoot( msg ); },
             getID() };
}
network::memory::Request_Encoder ToolMPOLogicalThread::getDaemonMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getToolRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ToolDaemon( msg ); },
             getID() };
}
network::sim::Request_Encoder ToolMPOLogicalThread::getMPOSimRequest( MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getMPRequest( *m_pYieldContext ), mpo ]( const network::Message& msg ) mutable
             { return leafRequest.MPOUp( msg, mpo ); },
             getID() };
}
network::memory::Request_Sender ToolMPOLogicalThread::getLeafMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_tool.getLeafSender(), *m_pYieldContext };
}
network::jit::Request_Sender ToolMPOLogicalThread::getLeafJITRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_tool.getLeafSender(), *m_pYieldContext };
}

network::mpo::Request_Sender ToolMPOLogicalThread::getMPRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return getMPRequest( *m_pYieldContext );
}

void ToolMPOLogicalThread::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "TOOL: run function" );
    network::sim::Request_Encoder request(
        [ rootRequest = getMPRequest( yield_ctx ) ]( const network::Message& msg ) mutable
        { return rootRequest.MPRoot( msg, mega::MP{} ); },
        getID() );
    request.SimStart();
    SPDLOG_TRACE( "TOOL: run complete" );

    // run complete will stop the receiver and ioservice.run() will complete.
    m_tool.runComplete();
}

void ToolMPOLogicalThread::RootSimRun( const Project& project, const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    m_tool.setMPO( mpo );

    setMPOContext( this );
    m_pYieldContext = &yield_ctx;

    // note the runtime will query getThisMPO while creating the root
    SPDLOG_TRACE( "TOOL: Acquired mpo context: {}", mpo );
    {
        createRoot( project, mpo );
        m_functor( yield_ctx );
    }
    SPDLOG_TRACE( "TOOL: Releasing mpo context: {}", mpo );

    m_pYieldContext = nullptr;
    resetMPOContext();
}

network::Status ToolMPOLogicalThread::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                 boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "ToolRequestLogicalThread::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::LogicalThreadID > logicalthreads;
        {
            for( const auto& id : m_tool.reportLogicalThreads() )
            {
                if( id != getID() )
                {
                    logicalthreads.push_back( id );
                }
            }
        }
        status.setLogicalThreadID( logicalthreads );
        status.setMPO( m_tool.getMPO() );
        status.setDescription( m_tool.getProcessName() );

        using MPOTimeStampVec = std::vector< std::pair< MPO, TimeStamp > >;
        using MPOVec          = std::vector< MPO >;
        if( const auto& reads = m_lockTracker.getReads(); !reads.empty() )
            status.setReads( MPOTimeStampVec{ reads.begin(), reads.end() } );
        if( const auto& writes = m_lockTracker.getWrites(); !writes.empty() )
            status.setWrites( MPOTimeStampVec{ writes.begin(), writes.end() } );

        {
            std::ostringstream os;
            os << "Tool: " << getLog().getTimeStamp();
        }

        status.setLogIterator( getLog().getIterator() );
        status.setLogFolder( getLog().getLogFolderPath().string() );

        status.setMemory( m_pMemoryManager->getStatus() );
    }

    return status;
}

mega::reports::Container ToolMPOLogicalThread::GetReport( const mega::reports::URL&                      url,
                                                          const std::vector< mega::reports::Container >& report,
                                                          boost::asio::yield_context&                    yield_ctx )
{
    SPDLOG_TRACE( "Tool::GetReport" );
    VERIFY_RTE( report.empty() );
    using namespace mega::reports;
    using namespace std::string_literals;
    Table table;
    table.m_rows.push_back( { Line{ "   Thread ID: "s }, Line{ getID() } } );
    MPOContext::getBasicReport( table );
    return table;
}
} // namespace mega::service
