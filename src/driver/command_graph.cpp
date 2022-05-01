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

#include "command_utils.hpp"

#include "database/model/FinalStage.hxx"

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environments.hpp"

#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"
#include "common/stash.hpp"

#include "nlohmann/json.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
    namespace graph
    {
        std::string getContextFullTypeName( FinalStage::Interface::Context* pContext )
        {
            using namespace FinalStage;

            std::ostringstream os;

            bool bFirst = true;
            while ( pContext )
            {
                if ( !bFirst )
                {
                    os << "_";
                }
                else
                {
                    bFirst = false;
                }
                os << pContext->get_identifier();
                pContext = dynamic_database_cast< Interface::Context >( pContext->get_parent() );
            }

            return os.str();
        }

        void addProperties( nlohmann::json& node, const std::vector< FinalStage::Interface::DimensionTrait* >& dimensions )
        {
            using namespace FinalStage;
            using namespace FinalStage::Interface;
            for ( DimensionTrait* pDimension : dimensions )
            {
                nlohmann::json property
                    = nlohmann::json::object( { { "name", pDimension->get_id()->get_str() }, { "value", pDimension->get_type() } } );
                node[ "properties" ].push_back( property );
            }
        }

        void recurse( nlohmann::json& data, FinalStage::Interface::Context* pContext )
        {
            using namespace FinalStage;
            using namespace FinalStage::Interface;

            std::ostringstream os;
            if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
            {
                os << "Namespace: " << pContext->get_identifier();
            }
            else if ( Abstract* pAbstract = dynamic_database_cast< Abstract >( pContext ) )
            {
                os << "Abstract: " << pContext->get_identifier();
            }
            else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
            {
                os << "Action: " << pContext->get_identifier();
            }
            else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
            {
                os << "Event: " << pContext->get_identifier();
            }
            else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
            {
                os << "Function: " << pContext->get_identifier();
            }
            else if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
            {
                os << "Object: " << pContext->get_identifier();
            }
            else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
            {
                os << "Link: " << pContext->get_identifier();
            }
            else
            {
                THROW_RTE( "Unknown context type" );
            }

            nlohmann::json node = nlohmann::json::object(
                { { "name", getContextFullTypeName( pContext ) }, { "label", os.str() }, { "properties", nlohmann::json::array() } } );

            if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
            {
                addProperties( node, pNamespace->get_dimension_traits() );
            }
            else if ( Abstract* pAbstract = dynamic_database_cast< Abstract >( pContext ) )
            {
            }
            else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
            {
                addProperties( node, pAction->get_dimension_traits() );
            }
            else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
            {
                addProperties( node, pEvent->get_dimension_traits() );
            }
            else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
            {
                nlohmann::json arguments
                    = nlohmann::json::object( { { "name", "arguments" }, { "value", pFunction->get_arguments_trait()->get_str() } } );
                node[ "properties" ].push_back( arguments );
                nlohmann::json return_type
                    = nlohmann::json::object( { { "name", "return type" }, { "value", pFunction->get_return_type_trait()->get_str() } } );
                node[ "properties" ].push_back( return_type );
            }
            else if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
            {
                addProperties( node, pObject->get_dimension_traits() );
            }
            else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
            {
            }
            else
            {
                THROW_RTE( "Unknown context type" );
            }

            data[ "nodes" ].push_back( node );

            for ( Interface::Context* pChildContext : pContext->get_children() )
            {
                recurse( data, pChildContext );

                nlohmann::json edge = nlohmann::json::object( { { "from", getContextFullTypeName( pContext ) },
                                                                { "to", getContextFullTypeName( pChildContext ) },
                                                                { "colour", "000000" } } );
                data[ "edges" ].push_back( edge );
            }
        }

        void command( bool bHelp, const std::vector< std::string >& args )
        {
            std::string             strGraphType;
            boost::filesystem::path rootSourceDir, rootBuildDir, outputFilePath;

            namespace po = boost::program_options;
            po::options_description commandOptions( " Generate graph json data" );
            {
                // clang-format off
                commandOptions.add_options()
                    ( "src_dir",    po::value< boost::filesystem::path >( &rootSourceDir ),                     "Source directory" )
                    ( "build_dir",  po::value< boost::filesystem::path >( &rootBuildDir ),                      "Build directory" )
                    ( "type",       po::value< std::string >( &strGraphType )->default_value( "interface" ),    "graph type" )
                    ( "output",     po::value< boost::filesystem::path >( &outputFilePath ),                    "output file to generate" )
                    ;
                // clang-format on
            }

            po::variables_map vm;
            po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
            po::notify( vm );

            if ( bHelp )
            {
                std::cout << commandOptions << "\n";
            }
            else
            {
                try
                {
                    std::ostringstream osOutput;
                    {
                        if ( strGraphType == "interface" )
                        {
                            mega::io::BuildEnvironment environment( rootSourceDir, rootBuildDir );
                            mega::io::Manifest         manifest( environment, environment.project_manifest() );

                            using namespace FinalStage;

                            nlohmann::json data
                                = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

                            for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
                            {
                                Database database( environment, sourceFilePath );
                                for ( Interface::Root* pRoot : database.many< Interface::Root >( sourceFilePath ) )
                                {
                                    for ( Interface::Context* pChildContext : pRoot->get_children() )
                                    {
                                        recurse( data, pChildContext );
                                    }
                                }
                            }

                            osOutput << data;
                        }
                        else
                        {
                            THROW_RTE( "Unknown graph type" );
                        }
                    }

                    boost::filesystem::updateFileIfChanged( outputFilePath, osOutput.str() );
                }
                catch ( std::exception& ex )
                {
                    THROW_RTE( "Error generating graph: " << ex.what() );
                }
            }
        }
    } // namespace graph
} // namespace driver
