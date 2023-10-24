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

#include "reports/renderer_html.hpp"

#include "database_reporters/factory.hpp"

#include "service/network/log.hpp"

#include "mega/values/service/url.hpp"
#include "mega/values/service/project.hpp"
#include "mega/values/compilation/type_id.hpp"

#include "mega/mangle/traits.hpp"
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
#include "compiler/concrete.hpp"
} // namespace FinalStage

namespace driver::report
{
/*
class TestReporter : public mega::reports::Reporter
{
public:
    using ID = std::string;

    mega::reports::ReporterID getID() override { return "test"; }

    mega::reports::Container generate( const mega::reports::URL& url ) override
    {
        using namespace std::string_literals;
        using namespace mega::reports;

        Table table{ { "Line"s, "Multiline"s, "Branch"s, "Graph"s } };

        for( int i = 0; i != 10; ++i )
        {
            // clang-format off
            table.m_rows.emplace_back
            (
                ContainerVector
                {
                    Line{ "Ed was here"s },
                    Multiline
                    {
                        {
                            "MPO: "s,
                            mega::MPO{ 1, 2, 3 },
                            " MP: "s,
                            mega::MP{ 1, 2 }
                        }
                    },
                    Branch
                    {
                        { "BranchLabel"s },
                        ContainerVector
                        {
                            Line{ "Branch Element 1"s },
                            Branch
                            {
                                { "NestedBranch"s },
                                ContainerVector
                                {
                                    Line{ "Element 1"s },
                                    Line{ "Element 2"s },
                                    Multiline
                                    {
                                        {
                                            "MPO: "s,
                                            mega::MPO{ 1, 2, 3 },
                                            " MP: "s,
                                            mega::MP{ 1, 2 }
                                        }
                                    },
                                    Line{ "Element 4"s },
                                }
                            },
                            Line{ "Branch Element 3"s },
                            Line{ "Branch Element 4"s },
                        }
                    },
                    Graph
                    {
                        {
                            Graph::Node{ {{ "Node 1"s }, { "MPO"s, mega::MPO{ 3,2,1 } }, { "Type"s,
mega::TypeID::make_context( 123,321 ) } } }, Graph::Node{ {{ "Node 2"s }}, Colour::red }, Graph::Node{ {{ "Node 3"s }},
Colour::blue }, Graph::Node{ {{ "Node 4"s }}, Colour::green }, Graph::Node{ {{ "Node 5"s }}, Colour::orange },
                            Graph::Node{ {{ "Node 6"s }}}
                        },
                        {
                            Graph::Edge{ 0, 1 },
                            Graph::Edge{ 1, 2 },
                            Graph::Edge{ 2, 3 },
                            Graph::Edge{ 3, 4, Colour::green },
                            Graph::Edge{ 4, 5 },

                            Graph::Edge{ 2, 4 },
                            Graph::Edge{ 4, 1 },
                            Graph::Edge{ 3, 2 }
                        }
                    }
                }
            );
            // clang-format on
        }

        return table;
    }
};
*/

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    std::string             reportURL;
    boost::filesystem::path projectPath, outputFilePath, templateDir;
    bool                    bClearTempFiles = true;

    namespace po = boost::program_options;

    po::options_description commandOptions( " Process schematic files" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "URL",               po::value< std::string >( &reportURL ),                      "Report URL to generate" )
            ( "project_install",   po::value< boost::filesystem::path >( &projectPath ),        "Path to Megastructure Project" )
            ( "templates",         po::value< boost::filesystem::path >( &templateDir ),        "Path to report renderer templates" )
            ( "output",            po::value< boost::filesystem::path >( &outputFilePath ),     "Output file" )
            ( "clear_temp",        po::value< bool >( &bClearTempFiles ),                       "Clear temporary files" )
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
        using namespace mega::reports;
        using namespace mega::reporters;

        const mega::reports::URL url = boost::urls::parse_origin_form( reportURL ).value();
        const mega::Project      project( projectPath );
        VERIFY_RTE_MSG( boost::filesystem::exists( project.getProjectDatabase() ),
                        "Failed to locate project database at: " << project.getProjectDatabase().string() );

        mega::io::ArchiveEnvironment environment( project.getProjectDatabase() );
        mega::io::Manifest           manifest( environment, environment.project_manifest() );
        FinalStage::Database         database( environment, environment.project_manifest() );

        Container result = mega::reporters::generateCompilationReport(
            url, CompilationReportArgs{ manifest, environment, database } );

        struct Linker : mega::reports::Linker
        {
            const mega::reports::URL& m_url;
            Linker( const mega::reports::URL& url )
                : m_url( url )
            {
            }
            std::optional< mega::reports::URL > link( const mega::reports::Value& value ) const override
            {
                if( auto pTypeID = boost::get< mega::TypeID >( &value ) )
                {
                    URL url = m_url;
                    url.set_fragment( mega::reports::toString( value ) );
                    return url;
                }
                return {};
            }
        } linker{ url };

        std::ostringstream os;

        HTMLRenderer renderer( templateDir, bClearTempFiles );
        renderer.render( result, linker, os );

        try
        {
            boost::filesystem::updateFileIfChanged( outputFilePath, os.str() );
        }
        catch( std::exception& ex )
        {
            THROW_RTE( "Error generating graph: " << outputFilePath.string() << " exception: " << ex.what() );
        }
    }
}

} // namespace driver::report
