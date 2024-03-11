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

#include "environment/environment_archive.hpp"
#include "database/FinalStage.hxx"

#include "database_reporters/factory.hpp"

#include "environment/environment.hpp"

#include "log/log.hpp"
#include "service/terminal.hpp"

#include "mega/values/service/url.hpp"
#include "mega/values/service/project.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/interface.hpp"
} // namespace FinalStage

namespace driver::report
{

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    std::string             projectName;
    std::string             reportURL, reportType;
    boost::filesystem::path outputFilePath, templateDir;
    bool                    bClearTempFiles = true, bListReports = false;

    using Version = mega::service::Program::Version;
    Version::Value programVersionValue = Version::MAX;

    namespace po = boost::program_options;

    po::options_description commandOptions( " Process schematic files" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "name",       po::value< std::string >( &projectName ),                   "Project Name" )
            ( "version",    po::value< Version::Value >( &programVersionValue ),        "Program Version" )
            ( "URL",        po::value< std::string >( &reportURL ),                     "Report URL to generate" )
            ( "type",       po::value< std::string >( &reportType ),                     "Report Type to generate" )
            ( "templates",  po::value< boost::filesystem::path >( &templateDir ),       "Path to report renderer templates" )
            ( "clear_temp", po::value< bool >( &bClearTempFiles ),                      "Clear temporary files" )
            ( "list",       po::bool_switch( &bListReports ),                           "List available report type" )
            ( "output",     po::value< boost::filesystem::path >( &outputFilePath ),    "Output file" )
            ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "name", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    std::optional< mega::service::Program::Version > programVersionOpt;
    {
        if( programVersionValue != Version::MAX )
        {
            programVersionOpt = Version{ programVersionValue };
        }
    }

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        if( bListReports )
        {
            std::vector< report::ReporterID > reportIDs;
            mega::reporters::getDatabaseReporterIDs( reportIDs );
            for( const auto& reporter : reportIDs )
            {
                std::cout << reporter << "\n";
            }
        }
        else
        {
            using namespace mega::service;

            if( templateDir.empty() )
            {
                // attempt to determine template folder from mega structure installation
                mega::service::Terminal terminal( log );
                const auto              result = terminal.GetMegastructureInstallation();
                templateDir                    = result.getInstallationPath() / "templates";
            }

            VERIFY_RTE_MSG( !templateDir.empty(), "Failed to locate Megastructure template folder" );
            VERIFY_RTE_MSG( !projectName.empty(), "Invalid project name" );

            const Project project( projectName );
            Program       program;

            {
                if( !programVersionOpt.has_value() )
                {
                    auto programOpt = Program::latest( project, Environment::workBin() );
                    VERIFY_RTE_MSG( programOpt.has_value(), "No latest version found for project: " << project );
                    program = programOpt.value();
                }
                else
                {
                    program = Program( project, programVersionOpt.value() );
                }
            }

            const auto programManifest = Environment::load( program );
            const auto databaseArchive = programManifest.getDatabase();

            using namespace mega::reports;
            using namespace mega::reporters;

            VERIFY_RTE_MSG( boost::filesystem::exists( databaseArchive ),
                            "Failed to locate project database at: " << databaseArchive.string() );

            if( !reportType.empty() )
            {
                VERIFY_RTE_MSG( reportURL.empty(), "Cannot specify both report type AND url" );
                std::ostringstream os;
                os << "/?report=" << reportType;
                reportURL = os.str();
            }
            else
            {
                VERIFY_RTE_MSG( !reportURL.empty(), "Missing report URL or type specification" );
            }

            const report::URL url = boost::urls::parse_origin_form( reportURL ).value();

            {
                mega::io::ArchiveEnvironment environment( databaseArchive );
                mega::io::Manifest           manifest( environment, environment.project_manifest() );

                const std::optional< Container > resultOpt
                    = mega::reporters::generateCompilationReport( url, CompilationReportArgs{ manifest, environment } );
                VERIFY_RTE_MSG( resultOpt.has_value(), "Failed to generate any report for: " << url.c_str() );

                struct Linker : mega::reports::Linker
                {
                    const report::URL& m_url;
                    Linker( const report::URL& url )
                        : m_url( url )
                    {
                    }
                    std::optional< report::URL > link( const mega::reports::Value& value ) const override
                    {
                        /*if( auto pTypeID = boost::get< mega::TypeID >( &value ) )
                        {
                            URL url = m_url;
                            url.set_fragment( mega::reports::toString( value ) );
                            return url;
                        }*/
                        return {};
                    }
                } linker{ url };

                HTMLRenderer::JavascriptShortcuts shortcuts;
                HTMLRenderer                      renderer( templateDir, shortcuts, bClearTempFiles );

                std::ostringstream os;
                renderer.render( resultOpt.value(), linker, os );

                try
                {
                    boost::filesystem::updateFileIfChanged( outputFilePath, os.str() );
                }
                catch( std::exception& ex )
                {
                    THROW_RTE( "Error generating report: " << outputFilePath.string() << " exception: " << ex.what() );
                }
            }
        }
    }
}

} // namespace driver::report
