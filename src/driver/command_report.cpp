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

#include "reports/reports.hpp"

#include "service/network/log.hpp"

#include "utilities/project.hpp"

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

void command( bool bHelp, const std::vector< std::string >& args )
{
    std::string             reportURL;
    boost::filesystem::path projectPath, outputFilePath;

    namespace po = boost::program_options;

    po::options_description commandOptions( " Process schematic files" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "URL",               po::value< std::string >( &reportURL ),                      "Report URL to generate" )
            ( "project_install",   po::value< boost::filesystem::path >( &projectPath ),        "Path to Megastructure Project" )
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
        Table::Ptr pRoot = std::make_shared< Table >();

        for( int y = 0; y != 10; ++y )
        {
            std::vector< Element::Ptr > row;
            for( int x = 0; x != 4; ++x )
            {
                auto pElement = std::make_shared< Element >();
                switch( x )
                {
                    case 0:
                        pElement->m_properties.push_back( std::make_shared< Property >( "Style", "awesome" ) );
                        break;
                    case 1:
                        pElement->m_properties.push_back(
                            std::make_shared< Property >( "Type", mega::TypeID::make_context( x, y ) ) );
                        break;
                    case 2:
                        pElement->m_properties.push_back( std::make_shared< Property >( "MPO", mega::MPO( 1, x, y ) ) );
                        break;
                    case 3:
                        pElement->m_properties.push_back(
                            std::make_shared< Property >( "SubTypeInstance", mega::SubTypeInstance( x, y ) ) );
                        break;
                }
                row.push_back( pElement );
            }
            {
                SVG::Ptr pSVG = std::make_shared< SVG >();

                auto pElement1 = std::make_shared< Element >();
                pElement1->m_properties.push_back( std::make_shared< Property >( "Node", "1" ) );
                pElement1->m_properties.push_back( std::make_shared< Property >( "MPO", mega::MPO( 1, 2, 3 ) ) );

                auto pElement2 = std::make_shared< Element >();
                pElement2->m_properties.push_back( std::make_shared< Property >( "Node", "2" ) );

                auto pElement3 = std::make_shared< Element >();
                pElement3->m_properties.push_back( std::make_shared< Property >( "Node", "3" ) );

                auto pElement4 = std::make_shared< Element >();
                pElement4->m_properties.push_back( std::make_shared< Property >( "Node", "4" ) );

                pSVG->m_children.push_back( pElement1 );
                pSVG->m_children.push_back( pElement2 );
                pSVG->m_children.push_back( pElement3 );
                pSVG->m_children.push_back( pElement4 );

                {
                    auto pEdge1      = std::make_shared< Edge >();
                    pEdge1->m_source = pElement1;
                    pEdge1->m_target = pElement2;
                    pEdge1->m_properties.push_back( std::make_shared< Property >( "color", "FF0000" ) );
                    pSVG->m_edges.push_back( pEdge1 );
                }
                {
                    auto pEdge2      = std::make_shared< Edge >();
                    pEdge2->m_source = pElement1;
                    pEdge2->m_target = pElement3;
                    pSVG->m_edges.push_back( pEdge2 );
                }
                {
                    auto pEdge      = std::make_shared< Edge >();
                    pEdge->m_source = pElement1;
                    pEdge->m_target = pElement4;
                    pSVG->m_edges.push_back( pEdge );
                }
                row.push_back( pSVG );
            }
            {
                TreeNode::Ptr pTree = std::make_shared< TreeNode >();
                pTree->m_properties.push_back( std::make_shared< Property >( "Tree", "is good" ) );
                {
                    auto pTree2 = std::make_shared< TreeNode >();
                    pTree2->m_properties.push_back( std::make_shared< Property >( "Style", "awesome 1" ) );
                    pTree->m_children.push_back( pTree2 );
                    {
                        auto pElement = std::make_shared< Element >();
                        pElement->m_properties.push_back( std::make_shared< Property >( "Nested", "1" ) );
                        pTree2->m_children.push_back( pElement );
                    }
                    {
                        auto pElement = std::make_shared< Element >();
                        pElement->m_properties.push_back( std::make_shared< Property >( "Nested", "2" ) );
                        pTree2->m_children.push_back( pElement );
                    }
                    {
                        auto pTree3 = std::make_shared< TreeNode >();
                        pTree3->m_properties.push_back( std::make_shared< Property >( "Nested", "3" ) );
                        pTree2->m_children.push_back( pTree3 );
                        {
                            auto pElement = std::make_shared< Element >();
                            pElement->m_properties.push_back( std::make_shared< Property >( "Nested", "1" ) );
                            pTree3->m_children.push_back( pElement );
                        }
                        {
                            auto pElement = std::make_shared< Element >();
                            pElement->m_properties.push_back( std::make_shared< Property >( "Nested", "2" ) );
                            pTree3->m_children.push_back( pElement );
                        }
                    }
                }
                {
                    auto pElement = std::make_shared< Element >();
                    pElement->m_properties.push_back( std::make_shared< Property >( "Style", "awesome 2" ) );
                    pTree->m_children.push_back( pElement );
                }
                row.push_back( pTree );
            }
            pRoot->m_rows.push_back( row );
        }

        std::ostringstream os;
        mega::reports::renderHTML( pRoot, os );

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
