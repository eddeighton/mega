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

#include "reports/renderer.hpp"

#include "service/network/log.hpp"

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

class TestReporter : public mega::reports::Reporter
{
public:
    using ID = std::string;

    mega::reports::ReporterID getID() override { return "test"; }

    std::optional< std::string > link( const mega::reports::Value& value ) override { return {}; }

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

class SymbolsReporter : public mega::reports::Reporter
{
    mega::io::Environment& m_environment;
    FinalStage::Database&  m_database;

public:
    using ID = std::string;

    SymbolsReporter( mega::io::Environment& environment, FinalStage::Database& database )
        : m_environment( environment )
        , m_database( database )
    {
    }

    mega::reports::ReporterID getID() override { return "symbols"; }

    std::optional< std::string > link( const mega::reports::Value& value ) override { return {}; }

    mega::reports::Container generate( const mega::reports::URL& url ) override
    {
        using namespace FinalStage;
        using namespace std::string_literals;
        using namespace mega::reports;

        Branch branch{ { "Symbols" } };

        auto pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        {
            Table concrete{ { "Concrete Type ID", "Interface Type ID", "Full Type Name", "Component" } };
            for( auto [ typeID, pConcreteID ] : pSymbolTable->get_concrete_type_ids() )
            {
                auto pVertex = pConcreteID->get_vertex();

                concrete.m_rows.push_back( ContainerVector{
                    Line{ typeID }, Line{ Concrete::getConcreteInterfaceTypeID( pVertex ) },
                    Line{ Concrete::printContextFullType( pVertex ) }, Line{ pVertex->get_component()->get_name() } }

                );
            }
            branch.m_elements.push_back( std::move( concrete ) );
        }
        {
            Table interface {
                {
                    "Interface Type ID", "Full Type Name", "Data Type", "Erased Type", "Mangle", "Simple", "Size",
                        "Alignment"
                }
            };
            for( auto [ typeID, pInterfaceTypeID ] : pSymbolTable->get_interface_type_ids() )
            {
                if( pInterfaceTypeID->get_context().has_value() )
                {
                    interface.m_rows.push_back( ContainerVector{
                        Line{ typeID },
                        Line{ Interface::printIContextFullType( pInterfaceTypeID->get_context().value() ) } } );
                }
                else if( pInterfaceTypeID->get_dimension().has_value() )
                {
                    Interface::DimensionTrait* pDimension = pInterfaceTypeID->get_dimension().value();
                    interface.m_rows.push_back( ContainerVector{
                        Line{ typeID }, Line{ Interface::printDimensionTraitFullType( pDimension ) },
                        Line{ pDimension->get_canonical_type() }, Line{ pDimension->get_erased_type() },
                        Line{ pDimension->get_mangle()->get_mangle() },
                        Line{ std::to_string( pDimension->get_simple() ) },
                        Line{ std::to_string( pDimension->get_size() ) },
                        Line{ std::to_string( pDimension->get_alignment() ) } } );
                }
                else if( pInterfaceTypeID->get_link().has_value() )
                {
                    Interface::LinkTrait* pLink = pInterfaceTypeID->get_link().value();
                    interface.m_rows.push_back( ContainerVector{ Line{ typeID },
                                                                 Line{ Interface::printLinkTraitFullType( pLink ) },
                                                                 Line{ pLink->get_relation()->get_id() } } );
                }
                else
                {
                    THROW_RTE( "Interface TypeID: " << typeID << " has no context or dimension" );
                }
            }
            branch.m_elements.push_back( std::move( interface ) );
        }

        return branch;
    }
};

class InterfaceReporter : public mega::reports::Reporter
{
    std::string            m_strLinkTarget;
    mega::io::Manifest&    m_manifest;
    mega::io::Environment& m_environment;
    FinalStage::Database&  m_database;

public:
    InterfaceReporter( const std::string&     strLinkTarget,
                       mega::io::Manifest&    manifest,
                       mega::io::Environment& environment,
                       FinalStage::Database&  database )
        : m_strLinkTarget( strLinkTarget )
        , m_manifest( manifest )
        , m_environment( environment )
        , m_database( database )
    {
    }

    mega::reports::ReporterID getID() override { return "interface"; }

    std::optional< std::string > link( const mega::reports::Value& value ) override
    {
        /*if( auto pval = std::get_if< mega::reports::CompileTimeIdentities >( &value ) )
        {
            if( auto pTypeID = std::get_if< mega::TypeID >( pval ) )
            {
                std::ostringstream os;
                os << "file:///home/foobar/test_Debug/test2.html#" << *pTypeID;
                return os.str();
                // mega::reports::URL url;
                // url.reportID           = getID();
                // url.reporterLinkTarget = getID();
                // url.url                = os.str();
                // return url;
            }
        }*/

        return {};
    }

    void addProperties( mega::reports::Branch& typeIDs, mega::reports::Branch& parentBranch,
                        const std::vector< FinalStage::Interface::DimensionTrait* >& dimensions )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;
        using namespace std::string_literals;
        using namespace mega::reports;

        if( !dimensions.empty() )
        {
            Table table{ { "Dimension", "Identifier", "Canon" } };
            Table table2{ { "Data" } };

            for( DimensionTrait* pDimension : dimensions )
            {
                table2.m_rows.push_back( { Line{ pDimension->get_interface_id() } } );

                if( auto pUser = db_cast< UserDimensionTrait >( pDimension ) )
                {
                    table.m_rows.push_back(
                        { Line{ "User" }, Line{ getIdentifier( pDimension ) }, Line{ pUser->get_canonical_type() } } );
                }
                else if( auto pCompiler = db_cast< CompilerDimensionTrait >( pDimension ) )
                {
                    table.m_rows.push_back(
                        { Line{ "Comp" }, Line{ getIdentifier( pDimension ) }, Line{ std::string{ mega::psz_bitset } } } );
                }
                else
                {
                    THROW_RTE( "Unknown dimension trait type" );
                }
            }
            typeIDs.m_elements.push_back( std::move( table2 ) );
            parentBranch.m_elements.emplace_back( std::move( table ) );
        }
    }

    void recurse( mega::reports::Branch& typeIDs, mega::reports::Branch& parentBranch,
                  FinalStage::Interface::IContext* pContext )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;
        using namespace std::string_literals;
        using namespace mega::reports;

        Branch branch;
        branch.m_bookmark = pContext->get_interface_id();

        typeIDs.m_elements.push_back( Line{ pContext->get_interface_id() } );

        if( auto pNamespace = db_cast< Namespace >( pContext ) )
        {
            branch.m_label = TextVector{ { "Namespace", Interface::getIdentifier( pContext ) } };
            addProperties( typeIDs, branch, pNamespace->get_dimension_traits() );
        }
        else if( auto pAbstract = db_cast< Abstract >( pContext ) )
        {
            branch.m_label = TextVector{ { "Interface", Interface::getIdentifier( pContext ) } };

            // addInheritance( pAbstract->get_inheritance_trait(), node );
        }
        else if( auto pAction = db_cast< Action >( pContext ) )
        {
            branch.m_label = TextVector{ { "Action", Interface::getIdentifier( pContext ) } };
            addProperties( typeIDs, branch, pAction->get_dimension_traits() );

            // addInheritance( pAction->get_inheritance_trait(), node );
            // addTransition( pAction->get_transition_trait(), node );
        }
        else if( auto pComponent = db_cast< Component >( pContext ) )
        {
            branch.m_label = TextVector{ { "Component", Interface::getIdentifier( pContext ) } };
            addProperties( typeIDs, branch, pComponent->get_dimension_traits() );

            // addInheritance( pComponent->get_inheritance_trait(), node );
            // addTransition( pComponent->get_transition_trait(), node );
        }
        else if( auto pState = db_cast< State >( pContext ) )
        {
            branch.m_label = TextVector{ { "State", Interface::getIdentifier( pContext ) } };
            addProperties( typeIDs, branch, pState->get_dimension_traits() );

            // addInheritance( pState->get_inheritance_trait(), node );
            // addTransition( pState->get_transition_trait(), node );
        }
        else if( auto pEvent = db_cast< Event >( pContext ) )
        {
            branch.m_label = TextVector{ { "Event", Interface::getIdentifier( pContext ) } };
            addProperties( typeIDs, branch, pEvent->get_dimension_traits() );

            // addInheritance( pEvent->get_inheritance_trait(), node );
        }
        else if( auto pInterupt = db_cast< Interupt >( pContext ) )
        {
            branch.m_label = TextVector{ { "Interupt", Interface::getIdentifier( pContext ) } };

            // addTransition( pInterupt->get_transition_trait(), node );
            // addEvent( pInterupt->get_events_trait(), node );
        }
        else if( auto pFunction = db_cast< Function >( pContext ) )
        {
            branch.m_label = TextVector{ { "Function", Interface::getIdentifier( pContext ) } };

            // {
            //     nlohmann::json     arguments;
            //     std::ostringstream osArgs;
            //     osArgs << pFunction->get_arguments_trait()->get_args();
            //     PROP( arguments, "arguments", osArgs.str() );
            //     node[ "properties" ].push_back( arguments );
            // }
            // {
            //     nlohmann::json return_type;
            //     PROP( return_type, "return type", pFunction->get_return_type_trait()->get_str() );
            //     node[ "properties" ].push_back( return_type );
            // }
        }
        else if( auto pObject = db_cast< Object >( pContext ) )
        {
            branch.m_label = TextVector{ { "Object", Interface::getIdentifier( pContext ) } };
            addProperties( typeIDs, branch, pObject->get_dimension_traits() );

            // addInheritance( pObject->get_inheritance_trait(), node );
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }

        for( Interface::IContext* pChildContext : pContext->get_children() )
        {
            recurse( typeIDs, branch, pChildContext );
        }

        parentBranch.m_elements.emplace_back( std::move( branch ) );
    }

    mega::reports::Container generate( const mega::reports::URL& url ) override
    {
        using namespace FinalStage;
        using namespace std::string_literals;
        using namespace mega::reports;

        Table root{ { "TypeID", "Tree" } };

        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            Database database( m_environment, sourceFilePath );

            Branch typeIDs( { { sourceFilePath.path().string() } } );
            Branch fileBranch( { { sourceFilePath.extension().string() } } );
            for( Interface::Root* pRoot : database.many< Interface::Root >( sourceFilePath ) )
            {
                for( Interface::IContext* pContext : pRoot->get_children() )
                {
                    recurse( typeIDs, fileBranch, pContext );
                }
            }
            root.m_rows.push_back( { typeIDs, fileBranch } );
        }

        return root;
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

        const mega::Project project( projectPath );

        VERIFY_RTE_MSG( boost::filesystem::exists( project.getProjectDatabase() ),
                        "Failed to locate project database at: " << project.getProjectDatabase().string() );

        mega::io::ArchiveEnvironment environment( project.getProjectDatabase() );
        mega::io::Manifest           manifest( environment, environment.project_manifest() );

        using namespace FinalStage;
        Database database( environment, environment.project_manifest() );

        Renderer renderer( templateDir );
        {
            renderer.registerReporter( std::make_unique< TestReporter >() );
            renderer.registerReporter(
                std::make_unique< InterfaceReporter >( outputFilePath.string(), manifest, environment, database ) );
            renderer.registerReporter( std::make_unique< SymbolsReporter >( environment, database ) );
        }

        std::ostringstream os;

        mega::reports::URL url;
        {
            std::ostringstream osURL;
            osURL << "file:///home/foobar/test_Debug/test2.htm";
            url.reportID           = reportURL;
            url.reporterLinkTarget = reportURL;
            url.url                = osURL.str();
        }

        renderer.generate( url, os );

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
