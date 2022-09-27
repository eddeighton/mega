
#include "root.hpp"
#include "request.hpp"
#include "pipeline.hpp"
#include "job.hpp"
#include "simulation.hpp"

#include "mega/common.hpp"

#include "runtime/mpo_context.hpp"

#include "service/network/conversation.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/pipeline_result.hpp"

#include "pipeline/task.hpp"
#include "pipeline/pipeline.hpp"

#include "utilities/serialization_helpers.hpp"

#include "version/version.hpp"

#include <common/file.hpp>

#include "spdlog/fmt/chrono.h"
#include "spdlog/stopwatch.h"

#include "boost/archive/xml_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "boost/dll/runtime_symbol_info.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/process/environment.hpp"
#include "boost/system/detail/error_code.hpp"
#include "boost/asio/experimental/concurrent_channel.hpp"

#include <iostream>
#include <limits>
#include <memory>

namespace mega
{
namespace service
{

Root::Root( boost::asio::io_context& ioContext )
    : network::ConversationManager( network::makeProcessName( network::Node::Root ), ioContext )
    , m_server( ioContext, *this, network::MegaRootPort() )
    , m_stash( boost::filesystem::current_path() / "stash" )
{
    loadConfig();
    m_server.waitForConnection();
}

void Root::loadConfig()
{
    const boost::filesystem::path configFile = boost::filesystem::current_path() / "config.xml";
    if ( boost::filesystem::exists( configFile ) )
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

void Root::onDaemonDisconnect( const network::ConnectionID& connectionID, mega::MP mp )
{
    SPDLOG_TRACE( "Root::onDaemonDisconnect {} {}", connectionID, mp );
    m_server.unmapConnection( mp );
    onDisconnect( connectionID );
    m_mpoManager.daemonDisconnect( mp );
}

void Root::shutdown()
{
    m_server.stop();
    // SPDLOG_TRACE( "Root shutdown" );
}

network::MegastructureInstallation Root::getMegastructureInstallation()
{
    if ( !m_megastructureInstallationOpt.has_value() )
    {
        const boost::filesystem::path currentProcessPath = boost::dll::program_location();
        VERIFY_RTE( !currentProcessPath.empty() );

        const boost::filesystem::path binPath = currentProcessPath.parent_path();
        VERIFY_RTE( binPath.has_parent_path() );

        const boost::filesystem::path installationPath = binPath.parent_path();

        m_megastructureInstallationOpt = network::MegastructureInstallation{ installationPath };
    }
    return m_megastructureInstallationOpt.value();
}

network::ConversationBase::Ptr Root::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Message&      msg )
{
    SPDLOG_TRACE( "Root::joinConversation {}", network::getMsgNameFromID( network::getMsgID( msg ) ) );
    switch ( network::getMsgID( msg ) )
    {
        case network::mpo::MSG_MPRoot_Request::ID:
        {
            const network::mpo::MSG_MPRoot_Request& actualMsg
                = network::mpo::MSG_MPRoot_Request::get( msg );
            switch ( actualMsg.request.index )
            {
                case network::sim::MSG_SimStart_Request::ID:
                    return network::ConversationBase::Ptr( new RootSimulation(
                        *this, getMsgReceiver( msg ), originatingConnectionID, actualMsg.mp ) );
            }
        }
        break;
        case network::daemon_root::MSG_ExeRoot_Request::ID:
        {
            const network::daemon_root::MSG_ExeRoot_Request& actualMsg
                = network::daemon_root::MSG_ExeRoot_Request::get( msg );
            switch ( actualMsg.request.index )
            {
                case network::job::MSG_JobReadyForWork_Request::ID:
                    return network::ConversationBase::Ptr(
                        new RootJobConversation( *this, getMsgReceiver( msg ), originatingConnectionID ) );
            }
        }
        break;
        case network::daemon_root::MSG_TermRoot_Request::ID:
        {
            const network::daemon_root::MSG_TermRoot_Request& actualMsg
                = network::daemon_root::MSG_TermRoot_Request::get( msg );
            switch ( actualMsg.request.index )
            {
                case network::pipeline::MSG_PipelineRun_Request::ID:
                    return network::ConversationBase::Ptr(
                        new RootPipelineConversation( *this, getMsgReceiver( msg ), originatingConnectionID ) );
            }
        }
        break;
    }
    return network::ConversationBase::Ptr(
        new RootRequestConversation( *this, getMsgReceiver( msg ), originatingConnectionID ) );
}

} // namespace service
} // namespace mega
