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

#include "pipeline/configuration.hpp"
#include "pipeline/stash.hpp"
#include "pipeline/pipeline.hpp"

#include "parser/parser.hpp"

#include "compiler/cmake.hpp"
#include "mega/values/compilation/tool_chain_hash.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/stash.hpp"

#include <spdlog/spdlog.h>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/dll.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <iostream>

namespace driver::execute_pipeline
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path stashDir, pipelineXML, outputPipelineResultPath, inputPipelineResultPath, toolchainXML;
    bool                    bRunLocally = false, bForceNoStash = false, bExecuteUpTo = false;
    std::string             strTaskName, strSourceFile;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Execute a Megastructure Pipeline" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "configuration",      po::value< boost::filesystem::path >( &pipelineXML ),               "Pipeline Configuration XML File. ( Default argument )" )
        ( "result_out",         po::value< boost::filesystem::path >( &outputPipelineResultPath ),  "Output Pipeline Result XML File. ( Optional )" )
        ( "local",              po::bool_switch( &bRunLocally ),                                    "Run locally" )
        ( "result_in",          po::value< boost::filesystem::path >( &inputPipelineResultPath ),   "Input Pipeline Result XML File ( Local only )" )
        ( "stash_dir",          po::value< boost::filesystem::path >( &stashDir ),                  "Stash directory ( Local only )" )
        ( "toolchain_xml",      po::value< boost::filesystem::path >( &toolchainXML ),              "Toolchain XML file ( Local only )" )
        ( "task",               po::value< std::string >( &strTaskName ),                           "Specific task to run. ( Local only )" )
        ( "source",             po::value< std::string >( &strSourceFile ),                         "Source file for specific task. ( Local only )" )
        ( "force_no_stash",     po::bool_switch( &bForceNoStash ),                                  "Prevent stash restore for specified task. ( Local only )" )
        ( "execute_up_to",      po::bool_switch( &bExecuteUpTo ),                                   "Only execute up to the specified task. ( Local only )" )
        ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "configuration", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if( !strTaskName.empty() )
    {
        VERIFY_RTE_MSG( bRunLocally, "Must run locally to specify task" );
    }
    if( !strSourceFile.empty() )
    {
        VERIFY_RTE_MSG( bRunLocally, "Must run locally to specify task" );
        VERIFY_RTE_MSG( !strTaskName.empty(), "Source file requires task specification" );
    }
    if( !inputPipelineResultPath.empty() )
    {
        VERIFY_RTE_MSG( bRunLocally, "Must run locally to specify prior build result" );
    }

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        mega::pipeline::Configuration pipelineConfig;
        boost::filesystem::loadAsciiFile( pipelineXML, pipelineConfig.data() );

        std::optional< mega::pipeline::PipelineResult > pipelineResult;
        {
            if( bRunLocally )
            {
                SPDLOG_INFO( "Running pipeline locally" );
                mega::utilities::ToolChain toolchain;
                {
                    VERIFY_RTE_MSG( boost::filesystem::exists( toolchainXML ),
                                    "Failed to locate toolchain file: " << toolchainXML.string() );
                    auto pInStream = boost::filesystem::createBinaryInputFileStream( toolchainXML );
                    boost::archive::xml_iarchive ia( *pInStream );
                    ia&                          boost::serialization::make_nvp( "toolchain", toolchain );
                }
                pipelineResult = runPipelineLocally( stashDir, toolchain, pipelineConfig, strTaskName, strSourceFile,
                                                     inputPipelineResultPath, bForceNoStash, bExecuteUpTo, std::cout );
            }
            else
            {
                try
                {
                    mega::service::Terminal terminal;
                    pipelineResult = terminal.PipelineRun( pipelineConfig );
                }
                catch( std::exception& ex )
                {
                    THROW_RTE( "Exception executing pipeline: " << ex.what() );
                }
            }
        }

        VERIFY_RTE_MSG( pipelineResult.has_value(), "Failed to get pipeline result" );

        if( pipelineResult.value().getSuccess() && !outputPipelineResultPath.empty() )
        {
            auto pOutStream = boost::filesystem::createBinaryOutputFileStream( outputPipelineResultPath );
            boost::archive::xml_oarchive archive( *pOutStream );
            archive&                     boost::serialization::make_nvp( "PipelineResult", pipelineResult.value() );
        }

        if( !pipelineResult.value().getSuccess() )
        {
            SPDLOG_WARN( "Pipeline failed: {}", pipelineResult.value().getMessage() );
            THROW_RTE( "Pipeline failed: " << pipelineResult.value().getMessage() );
        }
    }
}
} // namespace driver::execute_pipeline
