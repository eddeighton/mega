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

#include "database/common/archive.hpp"
#include "database/model/FinalStage.hxx"

#include "database/model/manifest.hxx"

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environment_build.hpp"
#include "database/common/environment_archive.hpp"

#include "database/types/sources.hpp"
#include "utilities/cmake.hpp"

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

const std::string& getIdentifier( FinalStage::Interface::IContext* pContext ) { return pContext->get_identifier(); }
const std::string& getIdentifier( FinalStage::Concrete::Context* pContext )
{
    return pContext->get_interface()->get_identifier();
}

template < typename TContextType >
std::string getContextFullTypeName( TContextType* pContext )
{
    using namespace FinalStage;

    std::vector< TContextType* > path;
    {
        while ( pContext )
        {
            path.push_back( pContext );
            pContext = dynamic_database_cast< TContextType >( pContext->get_parent() );
        }
        std::reverse( path.begin(), path.end() );
    }

    std::ostringstream os;
    {
        bool bFirst = true;
        for ( TContextType* pIter : path )
        {
            if ( !bFirst )
            {
                os << "_";
            }
            else
            {
                bFirst = false;
            }
            os << getIdentifier( pIter );
        }
    }

    return os.str();
}

void addProperties( nlohmann::json& node, const std::vector< FinalStage::Interface::DimensionTrait* >& dimensions )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;
    for ( DimensionTrait* pDimension : dimensions )
    {
        nlohmann::json property = nlohmann::json::object( { { "name", pDimension->get_id()->get_str() },
                                                            { "type_id", pDimension->get_type_id() },
                                                            { "symbol", pDimension->get_symbol() },
                                                            { "value", pDimension->get_type() } } );
        node[ "properties" ].push_back( property );
    }
}

void addProperties( nlohmann::json& node, const std::vector< FinalStage::Concrete::Dimensions::User* >& dimensions )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    using namespace FinalStage::Concrete::Dimensions;
    for ( User* pDimension : dimensions )
    {
        nlohmann::json property
            = nlohmann::json::object( { { "name", pDimension->get_interface_dimension()->get_id()->get_str() },
                                        { "type_id", pDimension->get_interface_dimension()->get_type_id() },
                                        { "symbol", pDimension->get_interface_dimension()->get_symbol() },
                                        { "value", pDimension->get_interface_dimension()->get_type() } } );
        node[ "properties" ].push_back( property );
    }
}

void addInheritance( const std::optional< ::FinalStage::Interface::InheritanceTrait* >& inheritance,
                     nlohmann::json&                                                    node )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    if ( inheritance.has_value() )
    {
        for ( IContext* pInherited : inheritance.value()->get_contexts() )
        {
            nlohmann::json base = nlohmann::json::object( { { "label", pInherited->get_identifier() },
                                                            { "type_id", pInherited->get_type_id() },
                                                            { "symbol", pInherited->get_symbol() } } );
            node[ "bases" ].push_back( base );
        }
    }
}

void recurse( nlohmann::json& data, FinalStage::Interface::IContext* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    std::ostringstream os;

    nlohmann::json node
        = nlohmann::json::object( { { "name", getContextFullTypeName< FinalStage::Interface::IContext >( pContext ) },
                                    { "label", "" },
                                    { "concrete_id", "" },
                                    { "type_id", pContext->get_type_id() },
                                    { "symbol", pContext->get_symbol() },
                                    { "bases", nlohmann::json::array() },
                                    { "properties", nlohmann::json::array() } } );

    if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
    {
        os << "Namespace: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pNamespace->get_dimension_traits() );
    }
    else if ( Abstract* pAbstract = dynamic_database_cast< Abstract >( pContext ) )
    {
        os << "Abstract: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        addInheritance( pAbstract->get_inheritance_trait(), node );
    }
    else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
    {
        os << "Action: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        addInheritance( pAction->get_inheritance_trait(), node );
        addProperties( node, pAction->get_dimension_traits() );
    }
    else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
    {
        os << "Event: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        addInheritance( pEvent->get_inheritance_trait(), node );
        addProperties( node, pEvent->get_dimension_traits() );
    }
    else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
    {
        os << "Function: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        nlohmann::json arguments
            = nlohmann::json::object( { { "name", "arguments" },
                                        { "type_id", "" },
                                        { "symbol", "" },
                                        { "value", pFunction->get_arguments_trait()->get_str() } } );
        node[ "properties" ].push_back( arguments );
        nlohmann::json return_type
            = nlohmann::json::object( { { "name", "return type" },
                                        { "type_id", "" },
                                        { "symbol", "" },
                                        { "value", pFunction->get_return_type_trait()->get_str() } } );
        node[ "properties" ].push_back( return_type );
    }
    else if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
    {
        os << "Object: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        addInheritance( pObject->get_inheritance_trait(), node );
        addProperties( node, pObject->get_dimension_traits() );
    }
    else if ( LinkInterface* pLinkInterface = dynamic_database_cast< LinkInterface >( pContext ) )
    {
        os << "LinkInterface: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
    }
    else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
    {
        os << "Link: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    data[ "nodes" ].push_back( node );

    for ( Interface::IContext* pChildContext : pContext->get_children() )
    {
        recurse( data, pChildContext );

        nlohmann::json edge
            = nlohmann::json::object( { { "from", getContextFullTypeName< Interface::IContext >( pContext ) },
                                        { "to", getContextFullTypeName< Interface::IContext >( pChildContext ) },
                                        { "colour", "000000" } } );
        data[ "edges" ].push_back( edge );
    }
}

void recurse( nlohmann::json& data, FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    std::ostringstream os;

    nlohmann::json node = nlohmann::json::object( { { "name", getContextFullTypeName< Concrete::Context >( pContext ) },
                                                    { "label", "" },
                                                    { "concrete_id", pContext->get_concrete_id() },
                                                    { "type_id", pContext->get_interface()->get_type_id() },
                                                    { "symbol", pContext->get_interface()->get_symbol() },
                                                    { "bases", nlohmann::json::array() },
                                                    { "properties", nlohmann::json::array() } } );

    if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
    {
        os << "Namespace: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pNamespace->get_dimensions() );
    }
    else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
    {
        os << "Action: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        // addInheritance( pAction->get_inheritance(), node );
        addProperties( node, pAction->get_dimensions() );
    }
    else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
    {
        os << "Event: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        // addInheritance( pEvent->get_inheritance(), node );
        addProperties( node, pEvent->get_dimensions() );
    }
    else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
    {
        os << "Function: " << getIdentifier( pContext );
        node[ "label" ]          = os.str();
        nlohmann::json arguments = nlohmann::json::object(
            { { "name", "arguments" },
              { "type_id", "" },
              { "symbol", "" },
              { "value", pFunction->get_interface_function()->get_arguments_trait()->get_str() } } );
        node[ "properties" ].push_back( arguments );
        nlohmann::json return_type = nlohmann::json::object(
            { { "name", "return type" },
              { "type_id", "" },
              { "symbol", "" },
              { "value", pFunction->get_interface_function()->get_return_type_trait()->get_str() } } );
        node[ "properties" ].push_back( return_type );
    }
    else if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
    {
        os << "Object: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
        // addInheritance( pObject->get_inheritance(), node );
        addProperties( node, pObject->get_dimensions() );
    }
    else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
    {
        os << "Link: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
    }
    else if ( Buffer* pBuffer = dynamic_database_cast< Buffer >( pContext ) )
    {
        os << "Buffer: " << getIdentifier( pContext );
        node[ "label" ] = os.str();
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    data[ "nodes" ].push_back( node );

    for ( Concrete::Context* pChildContext : pContext->get_children() )
    {
        recurse( data, pChildContext );

        nlohmann::json edge
            = nlohmann::json::object( { { "from", getContextFullTypeName< Concrete::Context >( pContext ) },
                                        { "to", getContextFullTypeName< Concrete::Context >( pChildContext ) },
                                        { "colour", "000000" } } );
        data[ "edges" ].push_back( edge );
    }
}

void generateInterfaceGraphVizInterface( std::ostream&          os,
                                         mega::io::Environment& environment,
                                         mega::io::Manifest&    manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );
        for ( Interface::Root* pRoot : database.many< Interface::Root >( sourceFilePath ) )
        {
            for ( Interface::IContext* pChildContext : pRoot->get_children() )
            {
                recurse( data, pChildContext );
            }
        }
    }
    os << data;
}

void generateInterfaceGraphVizConcrete( std::ostream&          os,
                                        mega::io::Environment& environment,
                                        mega::io::Manifest&    manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );
        for ( Concrete::Root* pRoot : database.many< Concrete::Root >( sourceFilePath ) )
        {
            for ( Concrete::Context* pContext : pRoot->get_children() )
            {
                recurse( data, pContext );
            }
        }
    }
    os << data;
}

void createGraphNode( std::set< FinalStage::Interface::Link* >& links, FinalStage::Interface::Link* pLink,
                      nlohmann::json& data, bool bInterface )
{
    using namespace FinalStage;

    std::ostringstream os;
    if ( bInterface )
        os << "Interface: " << getContextFullTypeName< Interface::IContext >( pLink );
    else
        os << "Link: " << getContextFullTypeName< Interface::IContext >( pLink );

    if ( links.find( pLink ) == links.end() )
    {
        nlohmann::json node
            = nlohmann::json::object( { { "name", getContextFullTypeName< Interface::IContext >( pLink ) },
                                        { "label", os.str() },
                                        { "concrete_id", "" }, // pLink->get_concrete_id()
                                        { "type_id", pLink->get_type_id() },
                                        { "symbol", pLink->get_symbol() },
                                        { "bases", nlohmann::json::array() },
                                        { "properties", nlohmann::json::array() } } );
        data[ "nodes" ].push_back( node );
        links.insert( pLink );
    }
}

void createEdge( FinalStage::Interface::Link*                                                         pFrom,
                 FinalStage::Interface::Link*                                                         pTo,
                 std::set< std::pair< FinalStage::Interface::Link*, FinalStage::Interface::Link* > >& edges,
                 nlohmann::json&                                                                      data )
{
    using namespace FinalStage;

    using LinkPair = std::pair< Interface::Link*, Interface::Link* >;
    // LinkPair p{ pFrom < pTo ? pFrom : pTo, pFrom < pTo ? pTo : pFrom };
    LinkPair p{ pFrom, pTo };
    if ( !edges.count( p ) )
    {
        nlohmann::json edge
            = nlohmann::json::object( { { "from", getContextFullTypeName< Interface::IContext >( pFrom ) },
                                        { "to", getContextFullTypeName< Interface::IContext >( pTo ) },
                                        { "colour", "000000" } } );
        data[ "edges" ].push_back( edge );
        edges.insert( p );
    }
}

void generateHyperGraphViz( std::ostream& os, mega::io::Environment& environment, mega::io::Manifest& manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    std::set< Interface::Link* > links;
    using LinkPair = std::pair< Interface::Link*, Interface::Link* >;
    std::set< LinkPair > edges;

    for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );

        for ( Concrete::Link* pLink : database.many< Concrete::Link >( sourceFilePath ) )
        {
            Interface::Link*      pInterfaceLink = pLink->get_link();
            HyperGraph::Relation* pRelation      = pInterfaceLink->get_relation();

            Interface::LinkInterface* pSourceInterface = pRelation->get_source_interface();
            Interface::LinkInterface* pTargetInterface = pRelation->get_target_interface();

            Interface::Link* pSource = pRelation->get_source();
            Interface::Link* pTarget = pRelation->get_target();

            createGraphNode( links, pSourceInterface, data, true );
            createGraphNode( links, pTargetInterface, data, true );
            createEdge( pSourceInterface, pTargetInterface, edges, data );

            if ( pSourceInterface != pSource )
            {
                createGraphNode( links, pSource, data, false );
                createEdge( pSourceInterface, pSource, edges, data );
            }

            if ( pTargetInterface != pTarget )
            {
                createGraphNode( links, pTarget, data, false );
                createEdge( pTargetInterface, pTarget, edges, data );
            }
        }
    }
    os << data;
}

std::string createMemoryNode( FinalStage::Concrete::Object* pObject, nlohmann::json& data )
{
    using namespace FinalStage;

    std::ostringstream osName;
    osName << "object_" << getContextFullTypeName( pObject );

    nlohmann::json node = nlohmann::json::object( { { "name", osName.str() },
                                                    { "label", getContextFullTypeName( pObject ) },
                                                    { "concrete_id", pObject->get_concrete_id() },
                                                    { "type_id", pObject->get_interface()->get_type_id() },
                                                    { "symbol", pObject->get_interface()->get_symbol() },
                                                    { "bases", nlohmann::json::array() },
                                                    { "properties", nlohmann::json::array() } } );
    data[ "nodes" ].push_back( node );
    return osName.str();
}

std::string createMemoryNode( FinalStage::MemoryLayout::Buffer* pBuffer, nlohmann::json& data )
{
    using namespace FinalStage;

    std::ostringstream osName;
    std::ostringstream os;

    osName << "buffer_" << pBuffer->_get_object_info().getFileID() << "_" << pBuffer->_get_object_info().getType()
           << "_" << pBuffer->_get_object_info().getIndex();

    if ( MemoryLayout::SimpleBuffer* pSimpleBuffer = dynamic_database_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
    {
        os << "Simple";
    }
    else if ( MemoryLayout::NonSimpleBuffer* pNonSimpleBuffer
              = dynamic_database_cast< MemoryLayout::NonSimpleBuffer >( pBuffer ) )
    {
        os << "NonSimple";
    }
    else if ( MemoryLayout::GPUBuffer* pGPUBuffer = dynamic_database_cast< MemoryLayout::GPUBuffer >( pBuffer ) )
    {
        os << "GPU";
    }
    else
    {
        THROW_RTE( "Unknown buffer type" );
    }

    nlohmann::json node = nlohmann::json::object( { { "name", osName.str() },
                                                    { "label", os.str() },
                                                    { "concrete_id", "" },
                                                    { "type_id", "" },
                                                    { "symbol", "" },
                                                    { "bases", nlohmann::json::array() },
                                                    { "properties", nlohmann::json::array() } } );
    data[ "nodes" ].push_back( node );
    return osName.str();
}

std::string createMemoryNode( FinalStage::MemoryLayout::Part* pPart, nlohmann::json& data )
{
    using namespace FinalStage;

    std::ostringstream osName;
    osName << "part_" << getContextFullTypeName( pPart->get_context() );

    std::ostringstream os;
    os << "Part: " << getContextFullTypeName( pPart->get_context() );

    nlohmann::json node = nlohmann::json::object( { { "name", osName.str() },
                                                    { "label", os.str() },
                                                    { "concrete_id", "" },
                                                    { "type_id", "" },
                                                    { "symbol", "" },
                                                    { "bases", nlohmann::json::array() },
                                                    { "properties", nlohmann::json::array() } } );

    for ( auto p : pPart->get_user_dimensions() )
    {
        nlohmann::json property
            = nlohmann::json::object( { { "name", p->get_interface_dimension()->get_id()->get_str() },
                                        { "type_id", p->get_interface_dimension()->get_type_id() },
                                        { "symbol", p->get_interface_dimension()->get_symbol() },
                                        { "value", p->get_interface_dimension()->get_type() } } );
        node[ "properties" ].push_back( property );
    }
    for ( auto p : pPart->get_allocation_dimensions() )
    {
        if ( Concrete::Dimensions::Allocator* pAllocatorDimension
             = dynamic_database_cast< Concrete::Dimensions::Allocator >( p ) )
        {
            Allocators::Allocator* pAllocator = pAllocatorDimension->get_allocator();
            Concrete::Context*     pAllocated = pAllocator->get_allocated_context();

            std::ostringstream osValue;
            {
                if ( Allocators::Nothing* pNothing = dynamic_database_cast< Allocators::Nothing >( pAllocator ) )
                {
                    osValue << "Nothing";
                }
                else if ( Allocators::Singleton* pSingleton
                          = dynamic_database_cast< Allocators::Singleton >( pAllocator ) )
                {
                    osValue << "Singleton";
                }
                else if ( Allocators::Range32* pRange32 = dynamic_database_cast< Allocators::Range32 >( pAllocator ) )
                {
                    osValue << "Range32";
                }
                else if ( Allocators::Range64* pRange64 = dynamic_database_cast< Allocators::Range64 >( pAllocator ) )
                {
                    osValue << "Range64";
                }
                else if ( Allocators::RangeAny* pRangeAny
                          = dynamic_database_cast< Allocators::RangeAny >( pAllocator ) )
                {
                    osValue << "RangeAny";
                }
                else
                {
                    THROW_RTE( "Unknown allocator type" );
                }
            }

            nlohmann::json property
                = nlohmann::json::object( { { "name", pAllocated->get_interface()->get_identifier() },
                                            { "type_id", pAllocated->get_concrete_id() },
                                            { "symbol", "" },
                                            { "value", osValue.str() } } );
            node[ "properties" ].push_back( property );
        }
        else
        {
            THROW_RTE( "Unknown allocator dimension type" );
        }
    }
    for ( auto p : pPart->get_link_dimensions() )
    {
        Concrete::Link* pLink = p->get_link();

        std::ostringstream osValue;
        {
            if ( Concrete::Dimensions::LinkSingle* pLinkSingle
                 = dynamic_database_cast< Concrete::Dimensions::LinkSingle >( p ) )
            {
                osValue << "Single";
            }
            else if ( Concrete::Dimensions::LinkMany* pLinkMany
                      = dynamic_database_cast< Concrete::Dimensions::LinkMany >( p ) )
            {
                osValue << "Many";
            }
            else
            {
                THROW_RTE( "Unknown link reference type" );
            }
        }

        nlohmann::json property = nlohmann::json::object( { { "name", pLink->get_link()->get_identifier() },
                                                            { "type_id", pLink->get_link()->get_type_id() },
                                                            { "symbol", "" },
                                                            { "value", osValue.str() } } );
        node[ "properties" ].push_back( property );
    }
    data[ "nodes" ].push_back( node );
    return osName.str();
}

void generateMemoryGraphViz( std::ostream& os, mega::io::Environment& environment, mega::io::Manifest& manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );

        for ( Concrete::Object* pObject : database.many< Concrete::Object >( sourceFilePath ) )
        {
            const std::string strObject = createMemoryNode( pObject, data );
            for ( MemoryLayout::Buffer* pBuffer : pObject->get_buffers() )
            {
                const std::string strBuffer = createMemoryNode( pBuffer, data );
                data[ "edges" ].push_back(
                    nlohmann::json::object( { { "from", strObject }, { "to", strBuffer }, { "colour", "0000FF" } } ) );

                for ( MemoryLayout::Part* pPart : pBuffer->get_parts() )
                {
                    const std::string strPart = createMemoryNode( pPart, data );
                    data[ "edges" ].push_back( nlohmann::json::object(
                        { { "from", strBuffer }, { "to", strPart }, { "colour", "00FF00" } } ) );
                }
            }
        }
    }
    os << data;
}

void command( bool bHelp, const std::vector< std::string >& args )
{
    std::string             strGraphType;
    boost::filesystem::path srcDir, buildDir, databaseArchivePath, outputFilePath;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Generate graph json data" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "src_dir",    po::value< boost::filesystem::path >( &srcDir ),                            "Source directory" )
            ( "build_dir",  po::value< boost::filesystem::path >( &buildDir ),                          "Build directory" )
            ( "database",   po::value< boost::filesystem::path >( &databaseArchivePath ),               "Path to database archive" )
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
        std::ostringstream osOutput;
        {
            std::unique_ptr< mega::io::Environment > pEnvironment;
            mega::compiler::Directories              directories{ srcDir, buildDir, "", "" };

            if ( !databaseArchivePath.empty() )
            {
                pEnvironment.reset( new mega::io::ArchiveEnvironment( databaseArchivePath ) );
            }
            else
            {
                pEnvironment.reset( new mega::io::BuildEnvironment( directories ) );
            }

            mega::io::Manifest manifest( *pEnvironment, pEnvironment->project_manifest() );

            if ( strGraphType == "interface" )
            {
                generateInterfaceGraphVizInterface( osOutput, *pEnvironment, manifest );
            }
            else if ( strGraphType == "concrete" )
            {
                generateInterfaceGraphVizConcrete( osOutput, *pEnvironment, manifest );
            }
            else if ( strGraphType == "hyper" )
            {
                generateHyperGraphViz( osOutput, *pEnvironment, manifest );
            }
            else if ( strGraphType == "memory" )
            {
                generateMemoryGraphViz( osOutput, *pEnvironment, manifest );
            }
            else
            {
                THROW_RTE( "Unknown graph type" );
            }
        }

        try
        {
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
