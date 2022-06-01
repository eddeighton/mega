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

#include "pipeline/configuration.hpp"

#include "pipeline/stash.hpp"
#include "service/host.hpp"

#include "service/network/log.hpp"

#include "pipeline/pipeline.hpp"

#include "utilities/cmake.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/program_options/value_semantic.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/stash.hpp"

#include <iostream>
#include <spdlog/spdlog.h>

namespace driver
{
namespace pipeline
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path logDir      = boost::filesystem::current_path(), stashDir, pipelineXML;
    bool                    bRunLocally = false;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Execute a Megastructure Pipeline" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "log_dir",            po::value< boost::filesystem::path >( &logDir ),        "Build log directory" )
        ( "stash_dir",          po::value< boost::filesystem::path >( &stashDir ),      "Stash directory" )
        ( "configuration",      po::value< boost::filesystem::path >( &pipelineXML ),   "Pipeline Configuration XML File" )
        ( "local",              po::bool_switch( &bRunLocally ),                        "Run locally" )
        ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "dir", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if ( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        mega::pipeline::Configuration pipelineConfig;
        boost::filesystem::loadAsciiFile( pipelineXML, pipelineConfig.data() );

        if ( bRunLocally )
        {
            VERIFY_RTE_MSG( !stashDir.empty(), "Local pipeline execution requires stash directry" );
            task::Stash stash( stashDir );

            mega::pipeline::Pipeline::Ptr pPipeline = mega::pipeline::Registry::getPipeline( pipelineConfig );

            bool bContinue = true;
            struct ProgressReport : public mega::pipeline::Progress
            {
                bool& m_bContinue;
                ProgressReport( bool& bContinue )
                    : m_bContinue( bContinue )
                {
                }
                virtual void onStarted( const std::string& strMsg ) 
                { 
                    SPDLOG_INFO( strMsg );
                }
                virtual void onProgress( const std::string& strMsg ) 
                { 
                    SPDLOG_INFO( strMsg ); 
                }
                virtual void onFailed( const std::string& strMsg )
                {
                    SPDLOG_INFO( strMsg );
                    m_bContinue = false;
                }
                virtual void onCompleted( const std::string& strMsg ) 
                { 
                    SPDLOG_INFO( strMsg );
                }
            } progressReporter( bContinue );

            struct StashImpl : public mega::pipeline::Stash
            {
                task::Stash& m_stash;
                StashImpl( task::Stash& stash )
                    : m_stash( stash )
                {
                }

                virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath )
                {
                    return m_stash.getBuildHashCode( filePath );
                }
                virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode )
                {
                    m_stash.setBuildHashCode( filePath, hashCode );
                }
                virtual void stash( const boost::filesystem::path& file, task::DeterminantHash code )
                {
                    m_stash.stash( file, code );
                }
                virtual bool restore( const boost::filesystem::path& file, task::DeterminantHash code )
                {
                    return m_stash.restore( file, code );
                }
            } stashImpl( stash );

            mega::pipeline::Schedule schedule = pPipeline->getSchedule();
            while ( !schedule.isComplete() && bContinue )
            {
                bool bProgress = false;
                for ( const mega::pipeline::TaskDescriptor& task : schedule.getReady() )
                {
                    pPipeline->execute( task, progressReporter, stashImpl );
                    if ( !bContinue )
                        break;
                    schedule.complete( task );
                    bProgress = true;
                }
                VERIFY_RTE_MSG( bProgress, "Failed to make progress executing pipeline" );
            }
        }
        else
        {
            try
            {
                auto logThreads
                    = mega::network::configureLog( logDir.native(), "interface_build", mega::network::fromStr( "warn" ),
                                                   mega::network::fromStr( "info" ) );
                {
                    mega::service::Host host;
                    host.PipelineRun( pipelineConfig );
                }
                // spdlog::drop("interface_build");
                // spdlog::shutdown();
            }
            catch ( std::exception& ex )
            {
                THROW_RTE( "Could not connect to Megastructure Service: " << ex.what() );
            }
        }
    }
}
} // namespace pipeline
} // namespace driver
