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

#include "root.hpp"
#include "request.hpp"
#include "pipeline.hpp"
#include "job.hpp"
#include "simulation.hpp"

#include "service/network/logical_thread.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "mega/values/service/logical_thread_id.hpp"
#include "mega/values/compilation/megastructure_installation.hpp"

#include "pipeline/pipeline_result.hpp"

#include "pipeline/task.hpp"
#include "pipeline/pipeline.hpp"

#include <common/file.hpp>
#include "common/serialisation.hpp"
#include "common/processID.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/process/environment.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>

#include <iostream>
#include <limits>
#include <memory>

namespace mega::service
{

Root::Root( boost::asio::io_context& ioContext, network::Log log, const boost::filesystem::path& stashFolder, short portNumber )
    : network::LogicalThreadManager( network::Node::makeProcessName( network::Node::Root ), ioContext )
    , m_log( log )
    , m_stashFolder( stashFolder )
    , m_server( ioContext, *this, portNumber )
    , m_stash( m_stashFolder )
{
    {
        std::ostringstream os;
        os << network::Node( network::Node::Root );
        common::ProcessID::setDescription( os.str().c_str() );
    }

    loadConfig();
    m_server.waitForConnection();
}

void Root::getGeneralStatusReport( mega::reports::Branch& report )
{
    using namespace mega::reports;
    using namespace std::string_literals;

    const auto megaInstall = getMegastructureInstallation();

    //spdlog::logger

    Table table;
    // clang-format off
    table.m_rows.push_back( { Line{ "     Process: "s }, Line{ m_strProcessName } } );
    table.m_rows.push_back( { Line{ "          IP: "s }, Line{ m_server.getEndPoint().address().to_string() } } );
    table.m_rows.push_back( { Line{ "        PORT: "s }, Line{ std::to_string( m_server.getEndPoint().port() ) } } );
    table.m_rows.push_back( { Line{ "Installation: "s }, Line{ megaInstall,   URL::makeFile( megaInstall.getInstallationPath() ) } } );
    table.m_rows.push_back( { Line{ "     Project: "s }, Line{ getProject(),  URL::makeFile( getProject().getProjectInstallPath() ) } } );
    table.m_rows.push_back( { Line{ "Stash Folder: "s }, Line{ m_stashFolder, URL::makeFile( m_stashFolder ) } } );
    table.m_rows.push_back( { Line{ "    Log File: "s }, Line{ m_log.logFile, URL::makeFile( m_log.logFile ) } } );
    // clang-format on
    
    report.m_elements.push_back( table );
}

void Root::loadConfig()
{
    const boost::filesystem::path configFile = boost::filesystem::current_path() / "config.xml";
    if( boost::filesystem::exists( configFile ) )
    {
        std::unique_ptr< boost::filesystem::ifstream > pFileStream
            = boost::filesystem::createBinaryInputFileStream( configFile );
        {
            boost::archive::xml_iarchive xml( *pFileStream );
            xml&                         boost::serialization::make_nvp( "config", m_config );
        }
    }
}

void Root::saveConfig()
{
    const boost::filesystem::path configFile = boost::filesystem::current_path() / "config.xml";

    std::unique_ptr< boost::filesystem::ofstream > pFileStream
        = boost::filesystem::createBinaryOutputFileStream( configFile );
    {
        boost::archive::xml_oarchive xml( *pFileStream );
        xml&                         boost::serialization::make_nvp( "config", m_config );
    }
}

void Root::setStartupUUIDMP( const std::string& strUUID, MP mp )
{
    m_startupUUIDs[ strUUID ] = mp;
}

std::optional< MP > Root::getAndResetStartupUUID( const std::string& strUUID )
{
    std::optional< MP > result;

    auto iFind = m_startupUUIDs.find( strUUID );
    if( iFind != m_startupUUIDs.end() )
    {
        result = iFind->second;
        m_startupUUIDs.erase( iFind );
    }

    return result;
}

void Root::onDaemonDisconnect( mega::MachineID machineID )
{
    SPDLOG_TRACE( "Root::onDaemonDisconnect {}", machineID );
    m_server.unLabelConnection( machineID );
    // onDisconnect();
    m_mpoManager.daemonDisconnect( machineID );
}

void Root::shutdown()
{
    m_server.stop();
    // SPDLOG_TRACE( "Root shutdown" );
}

MegastructureInstallation Root::getMegastructureInstallation()
{
    if( !m_megastructureInstallationOpt.has_value() )
    {
        const boost::filesystem::path currentProcessPath = boost::dll::program_location();
        VERIFY_RTE( !currentProcessPath.empty() );

        const boost::filesystem::path binPath = currentProcessPath.parent_path();
        VERIFY_RTE( binPath.has_parent_path() );

        const boost::filesystem::path installationPath = binPath.parent_path();

        m_megastructureInstallationOpt = MegastructureInstallation{ installationPath };
    }
    return m_megastructureInstallationOpt.value();
}

network::LogicalThreadBase::Ptr Root::joinLogicalThread( const network::Message& msg )
{
    SPDLOG_TRACE( "Root::joinLogicalThread {}", msg.getLogicalThreadID() );
    switch( msg.getID() )
    {
        case network::mpo::MSG_MPRoot_Request::ID:
        {
            const network::mpo::MSG_MPRoot_Request& actualMsg = network::mpo::MSG_MPRoot_Request::get( msg );
            switch( actualMsg.request.getID() )
            {
                case network::sim::MSG_SimStart_Request::ID:
                    return network::LogicalThreadBase::Ptr(
                        new RootSimulation( *this, msg.getLogicalThreadID(), actualMsg.mp ) );
            }
        }
        break;
        case network::daemon_root::MSG_ExeRoot_Request::ID:
        {
            const network::daemon_root::MSG_ExeRoot_Request& actualMsg
                = network::daemon_root::MSG_ExeRoot_Request::get( msg );
            switch( actualMsg.request.getID() )
            {
                case network::job::MSG_JobReadyForWork_Request::ID:
                    return network::LogicalThreadBase::Ptr(
                        new RootJobLogicalThread( *this, msg.getLogicalThreadID() ) );
            }
        }
        break;
        case network::daemon_root::MSG_TermRoot_Request::ID:
        {
            const network::daemon_root::MSG_TermRoot_Request& actualMsg
                = network::daemon_root::MSG_TermRoot_Request::get( msg );
            switch( actualMsg.request.getID() )
            {
                case network::pipeline::MSG_PipelineRun_Request::ID:
                    return network::LogicalThreadBase::Ptr(
                        new RootPipelineLogicalThread( *this, msg.getLogicalThreadID() ) );
            }
        }
        break;
    }
    return network::LogicalThreadBase::Ptr( new RootRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
}

} // namespace mega::service
