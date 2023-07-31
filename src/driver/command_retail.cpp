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

#include "driver/retail/templates.hpp"
#include "driver/retail/invocations.hpp"
#include "driver/retail/interface.hpp"
#include "driver/retail/generator_retail.hpp"

#include "database/common/environment_archive.hpp"
#include "database/common/archive.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

namespace driver::retail
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path inputArchiveFilePath, templateDir, outputSourceFilePath;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Generate retail archive" );
    {
        // clang-format off
            commandOptions.add_options()
                ( "input",    po::value< boost::filesystem::path >( &inputArchiveFilePath ),        "Compilation Archive" )
                ( "template_dir", po::value< boost::filesystem::path >( &templateDir ),             "Template directory" )
                ( "output",   po::value< boost::filesystem::path >( &outputSourceFilePath ),        "Source file to generate" )
                ;
        // clang-format on
    }

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        VERIFY_RTE_MSG( !outputSourceFilePath.empty(), "Missing output source file path" );
        VERIFY_RTE_MSG( boost::filesystem::exists( inputArchiveFilePath ),
                        "Failed to locate input archive: " << inputArchiveFilePath.string() );

        VERIFY_RTE_MSG(
            boost::filesystem::exists( templateDir ), "Failed to locate template directory: " << templateDir.string() );
        const boost::filesystem::path retailCodeTemplateFilePath = templateDir / "retail.jinja";
        const boost::filesystem::path contextTemplateFilePath    = templateDir / "retail_interface.jinja";
        const boost::filesystem::path invocationTemplateFilePath = templateDir / "retail_invocation.jinja";
        const boost::filesystem::path relationsTemplateFilePath  = templateDir / "retail_relation.jinja";
        const boost::filesystem::path operationsTemplateFilePath = templateDir / "operations.jinja";

        VERIFY_RTE_MSG( boost::filesystem::exists( retailCodeTemplateFilePath ),
                        "Failed to locate retail code template file: " << retailCodeTemplateFilePath.string() );
        VERIFY_RTE_MSG( boost::filesystem::exists( contextTemplateFilePath ),
                        "Failed to locate retail code template file: " << contextTemplateFilePath.string() );
        VERIFY_RTE_MSG( boost::filesystem::exists( invocationTemplateFilePath ),
                        "Failed to locate retail code template file: " << invocationTemplateFilePath.string() );
        VERIFY_RTE_MSG( boost::filesystem::exists( relationsTemplateFilePath ),
                        "Failed to locate retail code template file: " << relationsTemplateFilePath.string() );
        VERIFY_RTE_MSG( boost::filesystem::exists( operationsTemplateFilePath ),
                        "Failed to locate retail code template file: " << operationsTemplateFilePath.string() );

        std::string strCode;

        {
            ::inja::Environment injaEnvironment;
            {
                injaEnvironment.set_trim_blocks( true );
            }

            TemplateEngine templateEngine( injaEnvironment,
                                           retailCodeTemplateFilePath,
                                           contextTemplateFilePath,
                                           invocationTemplateFilePath,
                                           relationsTemplateFilePath );

            mega::io::ArchiveEnvironment environment( inputArchiveFilePath );
            const mega::io::Manifest     manifest( environment, environment.project_manifest() );
            using namespace FinalStage;
            Database database( environment );
            std::cout << "Loaded database: " << environment.getVersion() << std::endl;

            strCode
                = RetailGen::generate( templateEngine, environment, manifest, database, operationsTemplateFilePath );
        }

        boost::filesystem::updateFileIfChanged( outputSourceFilePath, strCode );
        std::cout << "Generated: " << outputSourceFilePath.string() << std::endl;
    }
}
} // namespace driver::retail
