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
#include "service/terminal.hpp"

#include "pipeline/pipeline.hpp"

#include "utilities/cmake.hpp"

#include "parser/parser.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/program_options/value_semantic.hpp"
#include "boost/dll.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/stash.hpp"
#include "utilities/tool_chain_hash.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

namespace driver
{
namespace pipeline
{

mega::network::PipelineResult runPipeline( const boost::filesystem::path&       stashDir,
                                           const mega::utilities::ToolChain&    toolChain,
                                           const mega::pipeline::Configuration& pipelineConfig,
                                           const std::string& strTaskName, const std::string& strSourceFile,
                                           const boost::filesystem::path& buildPipelineResultPath )
{
    VERIFY_RTE_MSG( !stashDir.empty(), "Local pipeline execution requires stash directry" );
    task::Stash stash( stashDir );

    // load previous builds hash codes
    if ( !buildPipelineResultPath.empty() )
    {
        mega::network::PipelineResult buildPipelineResult;
        auto pInFileStream = boost::filesystem::createBinaryInputFileStream( buildPipelineResultPath );
        boost::archive::xml_iarchive archive( *pInFileStream );
        archive& boost::serialization::make_nvp( "PipelineResult", buildPipelineResult );
        stash.setBuildHashCodes( buildPipelineResult.getBuildHashCodes() );
    }

    std::ostringstream            osLog;
    mega::pipeline::Pipeline::Ptr pPipeline = mega::pipeline::Registry::getPipeline( pipelineConfig, osLog );
    SPDLOG_INFO( "{}", osLog.str() );

    mega::network::PipelineResult pipelineResult( true, "", stash.getBuildHashCodes() );

    struct ProgressReport : public mega::pipeline::Progress
    {
        task::Stash&                   m_stash;
        mega::network::PipelineResult& m_pipelineResult;
        ProgressReport( mega::network::PipelineResult& pipelineResult, task::Stash& stash )
            : m_pipelineResult( pipelineResult )
            , m_stash( stash )
        {
        }
        virtual void onStarted( const std::string& strMsg ) { SPDLOG_INFO( strMsg ); }
        virtual void onProgress( const std::string& strMsg ) { SPDLOG_INFO( strMsg ); }
        virtual void onFailed( const std::string& strMsg )
        {
            SPDLOG_INFO( strMsg );
            m_pipelineResult = mega::network::PipelineResult( false, strMsg, m_stash.getBuildHashCodes() );
        }
        virtual void onCompleted( const std::string& strMsg ) { SPDLOG_INFO( strMsg ); }
    } progressReporter( pipelineResult, stash );

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

    struct Dependencies : public mega::pipeline::DependencyProvider
    {
        boost::shared_ptr< EG_PARSER_INTERFACE > m_pParser;
        Dependencies( const mega::utilities::ToolChain& toolChain )
        {
            try
            {
                m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
                    toolChain.parserDllPath, "g_parserSymbol", boost::dll::load_mode::append_decorations );
            }
            catch ( std::exception& ex )
            {
                THROW_RTE( "Failed to load parser dll with exception: " << ex.what() );
            }
        }
        EG_PARSER_INTERFACE* getParser() { return m_pParser.get(); }
    } dependencies( toolChain );

    mega::pipeline::Schedule schedule = pPipeline->getSchedule( progressReporter, stashImpl );

    if ( !strTaskName.empty() && !strSourceFile.empty() )
    {
        std::optional< mega::pipeline::TaskDescriptor > taskOpt = schedule.getTask( strTaskName, strSourceFile );
        VERIFY_RTE_MSG( taskOpt.has_value(),
                        "Failed to locate task with name: " << strTaskName << " and source file: " << strSourceFile );
        pPipeline->execute( taskOpt.value(), progressReporter, stashImpl, dependencies );
    }
    else
    {
        while ( !schedule.isComplete() && pipelineResult.getSuccess() )
        {
            bool bProgress = false;
            for ( const mega::pipeline::TaskDescriptor& task : schedule.getReady() )
            {
                pPipeline->execute( task, progressReporter, stashImpl, dependencies );
                if ( !pipelineResult.getSuccess() )
                    break;
                schedule.complete( task );
                bProgress = true;
            }
            VERIFY_RTE_MSG( bProgress, "Failed to make progress executing pipeline" );
        }
        if ( pipelineResult.getSuccess() )
        {
            pipelineResult = mega::network::PipelineResult( true, "", stash.getBuildHashCodes() );
        }
    }

    return pipelineResult;
}

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path stashDir, pipelineXML, pipelineResultPath, buildPipelineResultPath, toolchainXML;
    bool                    bRunLocally = false;
    std::string             strTaskName, strSourceFile;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Execute a Megastructure Pipeline" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "stash_dir",          po::value< boost::filesystem::path >( &stashDir ),                  "Stash directory" )
        ( "toolchain_xml",      po::value< boost::filesystem::path >( &toolchainXML ),              "Toolchain XML file" )
        ( "configuration",      po::value< boost::filesystem::path >( &pipelineXML ),               "Pipeline Configuration XML File" )
        ( "result",             po::value< boost::filesystem::path >( &pipelineResultPath ),        "Pipeline Result XML File" )
        ( "build_result",       po::value< boost::filesystem::path >( &buildPipelineResultPath ),   "Pipeline Result XML File" )
        ( "local",              po::bool_switch( &bRunLocally ),                                    "Run locally" )

        ( "task", po::value< std::string >( &strTaskName ), "Specific task to run.  Must provide source file also." )
        ( "source", po::value< std::string >( &strSourceFile ), "Source file for specific task" )
        ;
        // clang-format on
    }

    if ( !strTaskName.empty() )
    {
        VERIFY_RTE_MSG( bRunLocally, "Must run locally to specify task" );
        VERIFY_RTE_MSG( !strSourceFile.empty(), "Task requires source file specification" );
    }
    if ( !strSourceFile.empty() )
    {
        VERIFY_RTE_MSG( bRunLocally, "Must run locally to specify task" );
        VERIFY_RTE_MSG( !strTaskName.empty(), "Source file requires task specification" );
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
        mega::utilities::ToolChain toolchain;
        {
            VERIFY_RTE_MSG( boost::filesystem::exists( toolchainXML ),
                            "Failed to locate toolchain file: " << toolchainXML.string() );
            auto                         pInStream = boost::filesystem::createBinaryInputFileStream( toolchainXML );
            boost::archive::xml_iarchive ia( *pInStream );
            ia&                          boost::serialization::make_nvp( "toolchain", toolchain );
        }

        mega::pipeline::Configuration pipelineConfig;
        boost::filesystem::loadAsciiFile( pipelineXML, pipelineConfig.data() );

        std::optional< mega::network::PipelineResult > pipelineResult;
        {
            if ( bRunLocally )
            {
                pipelineResult = runPipeline(
                    stashDir, toolchain, pipelineConfig, strTaskName, strSourceFile, buildPipelineResultPath );
            }
            else
            {
                try
                {
                    mega::service::Terminal terminal;

                    SPDLOG_INFO( "Running pipeline using Megastructure service" );
                    try
                    {
                        pipelineResult = terminal.PipelineRun( pipelineConfig );
                    }
                    catch ( std::exception& ex )
                    {
                        SPDLOG_INFO( "Megastructure service unavailable so running pipeline locally" );
                        THROW_RTE( "Exception executing pipeline: " << ex.what() );
                    }
                }
                catch ( std::exception& ex )
                {
                    SPDLOG_INFO( "Megastructure service unavailable so running pipeline locally" );
                    pipelineResult = runPipeline( stashDir, toolchain, pipelineConfig, strTaskName, strSourceFile, {} );
                }
            }
        }

        VERIFY_RTE_MSG( pipelineResult.has_value(), "Failed to get pipeline result" );

        if ( pipelineResult.value().getSuccess() && !pipelineResultPath.empty() )
        {
            auto pOutStream = boost::filesystem::createBinaryOutputFileStream( pipelineResultPath );
            boost::archive::xml_oarchive archive( *pOutStream );
            archive&                     boost::serialization::make_nvp( "PipelineResult", pipelineResult.value() );
        }

        if ( !pipelineResult.value().getSuccess() )
        {
            SPDLOG_INFO( "Pipeline failed: {}", pipelineResult.value().getMessage() );
            THROW_RTE( "Pipeline failed: " << pipelineResult.value().getMessage() );
        }
    }
}
} // namespace pipeline
} // namespace driver
