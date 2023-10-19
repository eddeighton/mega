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

#include "database/common/environment_archive.hpp"
#include "database/model/FinalStage.hxx"

#include "reports/renderer.hpp"

#include "service/network/log.hpp"

#include "utilities/project.hpp"

#include "mega/type_id.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace driver::report
{

class TestReporter : public mega::reports::Reporter
{
public:
    using ID = std::string;

    mega::reports::ReporterID           getID() override { return "test"; }
    std::optional< mega::reports::URL > link( const mega::reports::Value& value ) override { return {}; }

    mega::reports::Container generate( const mega::reports::URL& url ) override
    {
        using namespace std::string_literals;
        using namespace mega::reports;

        Table table{ { "Line", "Multiline", "Branch", "Graph" } };

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
                            "MPO: ",
                            mega::MPO{ 1, 2, 3 }, 
                            " MP: ",
                            mega::MP{ 1, 2 } 
                        } 
                    },
                    Branch
                    {
                        { "BranchLabel" },
                        ContainerVector
                        {
                            Line{ "Branch Element 1"s }, 
                            Branch
                            {
                                { "NestedBranch" },
                                ContainerVector
                                {
                                    Line{ "Element 1"s }, 
                                    Line{ "Element 2"s }, 
                                    Multiline
                                    { 
                                        { 
                                            "MPO: ",
                                            mega::MPO{ 1, 2, 3 }, 
                                            " MP: ",
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
                            Graph::Node{ {{ "Node 1" }, { "MPO", mega::MPO{ 3,2,1 } }, { "Type", mega::TypeID::make_context( 123,321 ) } } },
                            Graph::Node{ {{ "Node 2" }}, Colour::red },
                            Graph::Node{ {{ "Node 3" }}, Colour::blue },
                            Graph::Node{ {{ "Node 4" }}, Colour::green },
                            Graph::Node{ {{ "Node 5" }}, Colour::orange },
                            Graph::Node{ {{ "Node 6" }}}
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

void command( bool bHelp, const std::vector< std::string >& args )
{
    std::string             reportURL;
    boost::filesystem::path projectPath, outputFilePath, templateDir;

    namespace po = boost::program_options;

    po::options_description commandOptions( " Process schematic files" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "URL",               po::value< std::string >( &reportURL ),                      "Report URL to generate" )
            ( "project_install",   po::value< boost::filesystem::path >( &projectPath ),        "Path to Megastructure Project" )
            ( "templates",         po::value< boost::filesystem::path >( &templateDir ),        "Path to report renderer templates" )
            ( "output",            po::value< boost::filesystem::path >( &outputFilePath ),     "Output file" )
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

        Renderer renderer( templateDir );
        {
            renderer.registerReporter( std::make_unique< TestReporter >() );
        }

        std::ostringstream os;
        renderer.generate( URL{ "test" }, os );

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
