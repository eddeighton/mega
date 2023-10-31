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

#include "database/archive.hpp"
#include "database/FinalStage.hxx"
#include "database/manifest.hxx"
#include "database/component_info.hpp"
#include "database/component_type.hpp"
#include "database/sources.hpp"
#include "database/serialisation.hpp"

#include "environment/environment_build.hpp"
#include "environment/environment_archive.hpp"

#include "compiler/cmake.hpp"

#include "mega/common_strings.hpp"
#include "mega/mangle/traits.hpp"

#include "service/network/log.hpp"
#include "common/assert_verify.hpp"
#include "common/stash.hpp"
#include "common/requireSemicolon.hpp"

#include "nlohmann/json.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace FinalStage

namespace driver
{
namespace graph
{
std::string graphVizEscape( const std::string& str )
{
    std::ostringstream os;
    for( auto c : str )
    {
        switch( c )
        {
            case '<':
                os << "\\<";
                break;
            case '>':
                os << "\\>";
                break;
            case '|':
                os << "\\|";
                break;
            case ':':
                os << "\\:";
                break;
            default:
                os << c;
                break;
        }
    }
    return os.str();
}
// label = "{{ node.label }}({{ node.concrete_id }},{{ node.type_id }},{{ node.symbol }})
// {% for base in node.bases %}{% if loop.is_first%} : {% else %}, {% endif %}{{ base.label }}({{ base.type_id }},{{
// base.symbol }}){% endfor %}
// {% for property in node.properties %}|{{ property.name }}({{ property.type_id }},{{ property.symbol }}): {{
// property.value }}{% endfor %}"

// label = "{{ node.label }}{% for property in node.properties %}|{{ property.name }}: {{ property.value }}{% endfor %}"

nlohmann::json make_node( const std::string& strName, const std::string& strLabel )
{
    nlohmann::json node = nlohmann::json::object(
        { { "name", strName }, { "label", strLabel }, { "properties", nlohmann::json::array() } } );
    return node;
}

#define NODE( node, name, label )                                                    \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _osLabel; _osLabel << label; \
                                     node = make_node( name, _osLabel.str() ); )

nlohmann::json make_property( const std::string& strName, const std::string& strValue )
{
    nlohmann::json property
        = nlohmann::json::object( { { "name", strName }, { "value", graphVizEscape( strValue ) } } );
    return property;
}

#define PROP( node, name, value )                                                    \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _osValue; _osValue << value; \
                                     node = make_property( name, _osValue.str() ); )

std::string getNodeInfo( FinalStage::Interface::IContext* pContext )
{
    std::ostringstream os;
    os << "(sym:" << pContext->get_symbol_id() << ",type:" << pContext->get_interface_id() << ")";
    return os.str();
}

void addProperties( nlohmann::json& node, const std::vector< FinalStage::Interface::DimensionTrait* >& dimensions )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;
    for( DimensionTrait* pDimension : dimensions )
    {
        if( auto pUser = db_cast< UserDimensionTrait >( pDimension ) )
        {
            nlohmann::json property;
            PROP( property, getIdentifier( pDimension ),
                  "(sym:" << pDimension->get_symbol_id() << ",type:" << pDimension->get_interface_id() << ") "
                          << pUser->get_canonical_type() );
            node[ "properties" ].push_back( property );
        }
        else if( auto pCompiler = db_cast< CompilerDimensionTrait >( pDimension ) )
        {
            nlohmann::json property;
            PROP( property, getIdentifier( pDimension ),
                  "(sym:" << pDimension->get_symbol_id() << ",type:" << pDimension->get_interface_id() << ") "
                          << mega::psz_bitset );
            node[ "properties" ].push_back( property );
        }
        else
        {
            THROW_RTE( "Unknown dimension trait type" );
        }
    }
}

void addInheritance( const std::optional< ::FinalStage::Interface::InheritanceTrait* >& inheritance,
                     nlohmann::json&                                                    node )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    if( inheritance.has_value() )
    {
        for( IContext* pInherited : inheritance.value()->get_contexts() )
        {
            nlohmann::json property;
            PROP( property, "Inherited",
                  "id " << pInherited->get_identifier() << "(sym:" << pInherited->get_symbol_id()
                        << ",type:" << pInherited->get_interface_id() << ")" );
            node[ "properties" ].push_back( property );
        }
    }
}

std::string printTupleVarTypePath( const std::vector< ::FinalStage::Interface::TypePathVariant* >& typePathVariant )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    std::ostringstream osTuple;
    osTuple << "[";
    bool bFirstVariant = true;
    for( const TypePathVariant* pVariant : typePathVariant )
    {
        if( bFirstVariant )
            bFirstVariant = false;
        else
            osTuple << ",";
        osTuple << "<";

        bool bFirstPath = true;
        for( const TypePath* pPath : pVariant->get_sequence() )
        {
            if( bFirstPath )
                bFirstPath = false;
            else
                osTuple << ",";

            bool bFirstSymbol = true;
            for( const Symbols::SymbolTypeID* pSymbol : pPath->get_types() )
            {
                if( bFirstSymbol )
                    bFirstSymbol = false;
                else
                    osTuple << ".";
                osTuple << pSymbol->get_symbol();
            }
        }
        osTuple << ">";
    }
    osTuple << "]";

    return osTuple.str();
}

void addEvent( ::FinalStage::Interface::EventTypeTrait* pEvent, nlohmann::json& node )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    {
        nlohmann::json property;
        PROP( property, "Event", printTupleVarTypePath( pEvent->get_tuple() ) );
        node[ "properties" ].push_back( property );
    }
}

void addTransition( const std::optional< ::FinalStage::Interface::TransitionTypeTrait* >& transition,
                    nlohmann::json&                                                       node )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    if( transition.has_value() )
    {
        TransitionTypeTrait* pTransition = transition.value();

        if( pTransition->get_is_successor() )
        {
            nlohmann::json property;
            PROP( property, "Transition", "Successor" );
            node[ "properties" ].push_back( property );
        }
        else if( pTransition->get_is_predecessor() )
        {
            nlohmann::json property;
            PROP( property, "Transition", "Predecessor" );
            node[ "properties" ].push_back( property );
        }
        else
        {
            nlohmann::json property;
            PROP( property, "Transition", "Completion" );
            node[ "properties" ].push_back( property );
        }

        {
            nlohmann::json property;
            PROP( property, "Transition Type", printTupleVarTypePath( pTransition->get_tuple() ) );
            node[ "properties" ].push_back( property );
        }
    }
}

void recurse( nlohmann::json& data, FinalStage::Interface::IContext* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    std::ostringstream os;

    nlohmann::json node;
    NODE( node, Interface::printIContextFullType( pContext, "_" ), "" );

    if( auto pNamespace = db_cast< Namespace >( pContext ) )
    {
        os << "Namespace: " << Interface::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pNamespace->get_dimension_traits() );
    }
    else if( auto pAbstract = db_cast< Abstract >( pContext ) )
    {
        os << "Interface: " << Interface::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addInheritance( pAbstract->get_inheritance_trait(), node );
    }
    else if( auto pAction = db_cast< Action >( pContext ) )
    {
        os << "Action: " << Interface::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addInheritance( pAction->get_inheritance_trait(), node );
        addProperties( node, pAction->get_dimension_traits() );
        addTransition( pAction->get_transition_trait(), node );
    }
    else if( auto pComponent = db_cast< Component >( pContext ) )
    {
        os << "Component: " << Interface::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addInheritance( pComponent->get_inheritance_trait(), node );
        addProperties( node, pComponent->get_dimension_traits() );
        addTransition( pComponent->get_transition_trait(), node );
    }
    else if( auto pState = db_cast< State >( pContext ) )
    {
        os << "State: " << Interface::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addInheritance( pState->get_inheritance_trait(), node );
        addProperties( node, pState->get_dimension_traits() );
        addTransition( pState->get_transition_trait(), node );
    }
    else if( auto pEvent = db_cast< Event >( pContext ) )
    {
        os << "Event: " << Interface::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addInheritance( pEvent->get_inheritance_trait(), node );
        addProperties( node, pEvent->get_dimension_traits() );
    }
    else if( auto pInterupt = db_cast< Interupt >( pContext ) )
    {
        os << "Interupt: " << Interface::getIdentifier( pInterupt ) << " " << getNodeInfo( pInterupt );
        node[ "label" ] = os.str();
        addTransition( pInterupt->get_transition_trait(), node );
        addEvent( pInterupt->get_events_trait(), node );
    }
    else if( auto pDecider = db_cast< Decider >( pContext ) )
    {
    }
    else if( auto pFunction = db_cast< Function >( pContext ) )
    {
        os << "Function: " << Interface::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        {
            nlohmann::json     arguments;
            std::ostringstream osArgs;
            osArgs << pFunction->get_arguments_trait()->get_args();
            PROP( arguments, "arguments", osArgs.str() );
            node[ "properties" ].push_back( arguments );
        }
        {
            nlohmann::json return_type;
            PROP( return_type, "return type", pFunction->get_return_type_trait()->get_str() );
            node[ "properties" ].push_back( return_type );
        }
    }
    else if( auto pObject = db_cast< Object >( pContext ) )
    {
        os << "Object: " << Interface::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addInheritance( pObject->get_inheritance_trait(), node );
        addProperties( node, pObject->get_dimension_traits() );
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    data[ "nodes" ].push_back( node );

    for( Interface::IContext* pChildContext : pContext->get_children() )
    {
        recurse( data, pChildContext );

        nlohmann::json edge
            = nlohmann::json::object( { { "from", Interface::printIContextFullType( pContext, "_" ) },
                                        { "to", Interface::printIContextFullType( pChildContext, "_" ) },
                                        { "colour", "000000" } } );
        data[ "edges" ].push_back( edge );
    }
}

std::string getNodeInfo( FinalStage::Concrete::Context* pContext )
{
    std::ostringstream os;
    os << "(sym:" << pContext->get_interface()->get_symbol_id()
       << ",type:" << pContext->get_interface()->get_interface_id() << ",con:" << pContext->get_concrete_id() << ")";
    return os.str();
}
std::string getNodeInfo( FinalStage::Concrete::Dimensions::User* pUserDimension )
{
    std::ostringstream os;
    os << "(sym:" << pUserDimension->get_interface_dimension()->get_symbol_id()
       << ",type:" << pUserDimension->get_interface_dimension()->get_interface_id()
       << ",con:" << pUserDimension->get_concrete_id() << ")";
    return os.str();
}

std::string getNodeInfo( FinalStage::Concrete::Dimensions::Link* pLinkDimension )
{
    std::ostringstream os;
    os << "(sym:" << pLinkDimension->get_interface_link()->get_symbol_id()
       << ",type:" << pLinkDimension->get_interface_link()->get_interface_id()
       << ",con:" << pLinkDimension->get_concrete_id() << ")";
    return os.str();
}

std::string getNodeInfo( FinalStage::Concrete::Dimensions::Bitset* pBitsetDimension )
{
    std::ostringstream os;
    os << "(sym:" << pBitsetDimension->get_interface_compiler_dimension()->get_symbol_id()
       << ",type:" << pBitsetDimension->get_interface_compiler_dimension()->get_interface_id()
       << ",con:" << pBitsetDimension->get_concrete_id() << ")";
    return os.str();
}

void addProperties( nlohmann::json& node, const std::vector< FinalStage::Concrete::Dimensions::User* >& dimensions )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    using namespace FinalStage::Concrete::Dimensions;
    for( User* pDimension : dimensions )
    {
        nlohmann::json property;
        PROP( property, getIdentifier( pDimension ),
              getNodeInfo( pDimension ) << " " << pDimension->get_interface_dimension()->get_canonical_type() );
        node[ "properties" ].push_back( property );
    }
}

void recurse( nlohmann::json& data, FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    std::ostringstream os;

    nlohmann::json node;
    NODE( node, Concrete::printContextFullType( pContext, "_" ), "" );

    if( Namespace* pNamespace = db_cast< Namespace >( pContext ) )
    {
        os << "Namespace: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pNamespace->get_dimensions() );
    }
    else if( Component* pComponent = db_cast< Component >( pContext ) )
    {
        os << "Component: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pComponent->get_dimensions() );
    }
    else if( Action* pAction = db_cast< Action >( pContext ) )
    {
        os << "Action: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pAction->get_dimensions() );
    }
    else if( State* pState = db_cast< State >( pContext ) )
    {
        os << "State: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pState->get_dimensions() );
    }
    else if( Event* pEvent = db_cast< Event >( pContext ) )
    {
        os << "Event: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pEvent->get_dimensions() );
    }
    else if( Interupt* pInterupt = db_cast< Interupt >( pContext ) )
    {
        os << "Interupt: " << Concrete::getIdentifier( pInterupt ) << " " << getNodeInfo( pInterupt );
        node[ "label" ] = os.str();
        // addProperties( node, pInterupt->get_interface_interupt()->get_events_trait() );
    }
    else if( Function* pFunction = db_cast< Function >( pContext ) )
    {
        os << "Function: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        {
            std::ostringstream osArgs;
            osArgs << pFunction->get_interface_function()->get_arguments_trait()->get_args();
            nlohmann::json arguments;
            PROP( arguments, "arguments", osArgs.str() );
            node[ "properties" ].push_back( arguments );
        }
        {
            nlohmann::json return_type;
            PROP( return_type, "return type", pFunction->get_interface_function()->get_return_type_trait()->get_str() );
            node[ "properties" ].push_back( return_type );
        }
    }
    else if( Object* pObject = db_cast< Object >( pContext ) )
    {
        os << "Object: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        addProperties( node, pObject->get_dimensions() );
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    data[ "nodes" ].push_back( node );

    for( Concrete::Context* pChildContext : pContext->get_children() )
    {
        recurse( data, pChildContext );

        nlohmann::json edge = nlohmann::json::object( { { "from", Concrete::printContextFullType( pContext, "_" ) },
                                                        { "to", Concrete::printContextFullType( pChildContext, "_" ) },
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

    for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );
        for( Interface::Root* pRoot : database.many< Interface::Root >( sourceFilePath ) )
        {
            for( Interface::IContext* pChildContext : pRoot->get_children() )
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

    for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );
        for( Concrete::Root* pRoot : database.many< Concrete::Root >( sourceFilePath ) )
        {
            for( Concrete::Context* pContext : pRoot->get_children() )
            {
                recurse( data, pContext );
            }
        }
    }
    os << data;
}

void createGraphNode( std::set< FinalStage::Interface::LinkTrait* >& links, FinalStage::Interface::LinkTrait* pLink,
                      nlohmann::json& data, bool bInterface )
{
    using namespace FinalStage;

    /*std::ostringstream os;
    if( bInterface )
        os << "Interface: " << Concrete::printContextFullType< Interface::IContext >( pLink );
    else
        os << "Link: " << Concrete::printContextFullType< Interface::IContext >( pLink );

    if( links.find( pLink ) == links.end() )
    {
        nlohmann::json node;
        NODE( node, Concrete::printContextFullType< Interface::IContext >( pLink ), os.str() );
        data[ "nodes" ].push_back( node );
        links.insert( pLink );
    }*/
}

void createEdge( FinalStage::Interface::LinkTrait*                                                              pFrom,
                 FinalStage::Interface::LinkTrait*                                                              pTo,
                 std::set< std::pair< FinalStage::Interface::LinkTrait*, FinalStage::Interface::LinkTrait* > >& edges,
                 nlohmann::json&                                                                                data )
{
    using namespace FinalStage;

    using LinkPair = std::pair< Interface::LinkTrait*, Interface::LinkTrait* >;
    // LinkPair p{ pFrom < pTo ? pFrom : pTo, pFrom < pTo ? pTo : pFrom };
    LinkPair p{ pFrom, pTo };
    /*if( !edges.count( p ) )
    {
        nlohmann::json edge
            = nlohmann::json::object( { { "from", getDimensionFullTypeName< Interface::IContext >( pFrom ) },
                                        { "to", getDimensionFullTypeName< Interface::IContext >( pTo ) },
                                        { "colour", "000000" } } );
        data[ "edges" ].push_back( edge );
        edges.insert( p );
    }*/
}

void generateHyperGraphViz( std::ostream& os, mega::io::Environment& environment, mega::io::Manifest& manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    std::set< Interface::LinkTrait* > links;
    using LinkPair = std::pair< Interface::LinkTrait*, Interface::LinkTrait* >;
    std::set< LinkPair > edges;

    for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );

        for( Concrete::Graph::Vertex* pVertex : database.many< Concrete::Graph::Vertex >( sourceFilePath ) )
        {
            if( db_cast< Concrete::Dimensions::User >( pVertex ) )
            {
                continue;
            }
            if( db_cast< Concrete::ContextGroup >( pVertex ) )
            {
                if( !db_cast< Concrete::Context >( pVertex ) )
                    continue;
            }

            auto           strVertexName = Concrete::printContextFullType( pVertex, "_" );
            nlohmann::json node;
            NODE( node, strVertexName, strVertexName );
            data[ "nodes" ].push_back( node );

            for( auto pEdge : pVertex->get_out_edges() )
            {
                std::string strColour = "000000";
                switch( pEdge->get_type().get() )
                {
                    case mega::EdgeType::eMonoSingularMandatory:
                    case mega::EdgeType::ePolySingularMandatory:
                    case mega::EdgeType::eMonoNonSingularMandatory:
                    case mega::EdgeType::ePolyNonSingularMandatory:
                    case mega::EdgeType::eMonoSingularOptional:
                    case mega::EdgeType::ePolySingularOptional:
                    case mega::EdgeType::eMonoNonSingularOptional:
                    case mega::EdgeType::ePolyNonSingularOptional:

                    case mega::EdgeType::ePolyParent:
                        strColour = "FF0000";
                        break;
                    case mega::EdgeType::eDim:
                        strColour = "";
                        break;
                    case mega::EdgeType::eParent:
                        strColour = "0000FF";
                        break;
                    case mega::EdgeType::eChildSingular:
                        strColour = "000000";
                        break;
                    case mega::EdgeType::eChildNonSingular:
                        strColour = "00FF00";
                        break;
                    case mega::EdgeType::eLink:
                        strColour = "909000";
                        break;
                    case mega::EdgeType::TOTAL_EDGE_TYPES:
                        break;
                }

                if( !strColour.empty() )
                {
                    nlohmann::json edge = nlohmann::json::object(
                        { { "from", Concrete::printContextFullType( pEdge->get_source(), "_" ) },
                          { "to", Concrete::printContextFullType( pEdge->get_target(), "_" ) },
                          { "colour", strColour } } );
                    data[ "edges" ].push_back( edge );
                }
            }
        }
    }
    os << data;
}

void recurseTree( nlohmann::json& data, FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    std::ostringstream os;

    if( Namespace* pNamespace = db_cast< Namespace >( pContext ) )
    {
    }
    else if( State* pState = db_cast< State >( pContext ) )
    {
    }
    else if( Event* pEvent = db_cast< Event >( pContext ) )
    {
    }
    else if( Interupt* pInterupt = db_cast< Interupt >( pContext ) )
    {
    }
    else if( Decider* pDecider = db_cast< Decider >( pContext ) )
    {
    }
    else if( Function* pFunction = db_cast< Function >( pContext ) )
    {
    }
    else if( Object* pObject = db_cast< Object >( pContext ) )
    {
        nlohmann::json node;
        NODE( node, Concrete::printContextFullType( pContext ), "" );
        os << "Object: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
        node[ "label" ] = os.str();
        data[ "nodes" ].push_back( node );
    }
    /*else if( Link* pLink = db_cast< Link >( pContext ) )
    {
        {
            nlohmann::json node;
            NODE( node, Concrete::printContextFullType< Concrete::Context >( pContext ), "" );
            os << "Link: " << Concrete::getIdentifier( pContext ) << " " << getNodeInfo( pContext );
            node[ "label" ] = os.str();
            data[ "nodes" ].push_back( node );
        }

        Interface::LinkInterface* pLinkInterface = pLink->get_link_interface();
        HyperGraph::Relation*     pRelation      = pLinkInterface->get_relation();

        const mega::Ownership ownership = pRelation->get_ownership();

        // const mega::DerivationDirection derivation =
        //     pLinkInterface->get_link_trait()->get_derivation();

        if( pRelation->get_source_interface() == pLinkInterface )
        {
            if( ownership.get() == mega::Ownership::eOwnTarget )
            {
                for( auto pTarget : pRelation->get_targets() )
                {
                    for( auto pConcreteLink : pTarget->get_concrete() )
                    {
                        nlohmann::json edge = nlohmann::json::object(
                            { { "from", Concrete::printContextFullType< Concrete::Context >( pLink ) },
                              { "to", Concrete::printContextFullType< Concrete::Context >( pConcreteLink ) },
                              { "colour", "FF0000" } } );
                        data[ "edges" ].push_back( edge );
                    }
                }
            }
        }
        else if( pRelation->get_target_interface() == pLinkInterface )
        {
            if( ownership.get() == mega::Ownership::eOwnSource )
            {
                for( auto pTarget : pRelation->get_sources() )
                {
                    for( auto pConcreteLink : pTarget->get_concrete() )
                    {
                        nlohmann::json edge = nlohmann::json::object(
                            { { "from", Concrete::printContextFullType< Concrete::Context >( pLink ) },
                              { "to", Concrete::printContextFullType< Concrete::Context >( pConcreteLink ) },
                              { "colour", "FF0000" } } );
                        data[ "edges" ].push_back( edge );
                    }
                }
            }
        }
        else
        {
            THROW_RTE( "Invalid relation" );
        }
    }*/
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    for( Concrete::Context* pChildContext : pContext->get_children() )
    {
        recurseTree( data, pChildContext );

        // THROW_TODO;
        /*if( db_cast< Object >( pChildContext ) || db_cast< Link >( pChildContext ) )
        {
            nlohmann::json edge
                = nlohmann::json::object( { { "from", Concrete::printContextFullType< Concrete::Context >( pContext ) },
                                            { "to", Concrete::printContextFullType< Concrete::Context >( pChildContext )
        }, { "colour", "000000" } } ); data[ "edges" ].push_back( edge );
        }*/
    }
}

void generateTreeGraphViz( std::ostream& os, mega::io::Environment& environment, mega::io::Manifest& manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );
        for( Concrete::Root* pRoot : database.many< Concrete::Root >( sourceFilePath ) )
        {
            for( Concrete::Context* pContext : pRoot->get_children() )
            {
                recurseTree( data, pContext );
            }
        }
    }
    os << data;
}

std::string createMemoryNode( FinalStage::Concrete::Object* pObject, nlohmann::json& data )
{
    using namespace FinalStage;

    std::ostringstream osName;
    osName << "object_" << Concrete::printContextFullType( pObject );

    nlohmann::json node;
    NODE( node, osName.str(), Concrete::printContextFullType( pObject ) << getNodeInfo( pObject ) );
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

    if( MemoryLayout::SimpleBuffer* pSimpleBuffer = db_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
    {
        os << "Simple";
    }
    else
    {
        THROW_RTE( "Unknown buffer type" );
    }

    nlohmann::json node;
    NODE( node, osName.str(), os.str() << " size " << pBuffer->get_size() << " align" << pBuffer->get_alignment() );
    data[ "nodes" ].push_back( node );
    return osName.str();
}

std::string createMemoryNode( const std::string& strBufferName, FinalStage::MemoryLayout::Part* pPart,
                              nlohmann::json& data )
{
    using namespace FinalStage;

    std::ostringstream osName;
    osName << strBufferName << "part_" << pPart->_get_object_info().getFileID() << "_"
           << pPart->_get_object_info().getType() << "_" << pPart->_get_object_info().getIndex();

    std::ostringstream os;
    os << "Part: ";

    nlohmann::json node;
    NODE( node, osName.str(),
          os.str() << " size " << pPart->get_size() << " offset " << pPart->get_offset() << " align "
                   << pPart->get_alignment() << " instances " << pPart->get_total_domain_size() );

    for( auto p : pPart->get_user_dimensions() )
    {
        nlohmann::json property;
        PROP( property, Concrete::getIdentifier( p ),
              getNodeInfo( p ) << " offset " << p->get_offset() << " size " << p->get_interface_dimension()->get_size()
                               << " alignment " << p->get_interface_dimension()->get_alignment() );
        node[ "properties" ].push_back( property );
    }

    for( auto p : pPart->get_link_dimensions() )
    {
        nlohmann::json property;
        PROP( property, Concrete::getIdentifier( p ),
              getNodeInfo( p ) << " offset " << p->get_offset() << " singular " << p->get_singular() );
        node[ "properties" ].push_back( property );
    }

    for( auto p : pPart->get_bitset_dimensions() )
    {
        nlohmann::json property;
        PROP( property, Concrete::getIdentifier( p ),
              getNodeInfo( p ) << " offset " << p->get_offset());
        node[ "properties" ].push_back( property );
    }
    // THROW_TODO;
    /*for( auto p : pPart->get_link_dimensions() )
    {
        Concrete::Link* pLink = p->get_link();

        std::ostringstream osValue;
        {
            if( pLink->get_singular() )
            {
                osValue << "Single";
            }
            else
            {
                osValue << "Many";
            }
        }
        {
            nlohmann::json property;
            PROP( property, pLink->get_link()->get_identifier(),
                  getNodeInfo( p ) << " offset " << p->get_offset() << " value " << osValue.str() );
            node[ "properties" ].push_back( property );
        }
    }*/
    data[ "nodes" ].push_back( node );
    return osName.str();
}

void generateMemoryGraphViz( std::ostream& os, mega::io::Environment& environment, mega::io::Manifest& manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );

        for( Concrete::Object* pObject : database.many< Concrete::Object >( sourceFilePath ) )
        {
            const std::string strObject = createMemoryNode( pObject, data );
            for( MemoryLayout::Buffer* pBuffer : pObject->get_buffers() )
            {
                const std::string strBuffer = createMemoryNode( pBuffer, data );
                data[ "edges" ].push_back(
                    nlohmann::json::object( { { "from", strObject }, { "to", strBuffer }, { "colour", "0000FF" } } ) );

                for( MemoryLayout::Part* pPart : pBuffer->get_parts() )
                {
                    const std::string strPart = createMemoryNode( strBuffer, pPart, data );
                    data[ "edges" ].push_back( nlohmann::json::object(
                        { { "from", strBuffer }, { "to", strPart }, { "colour", "00FF00" } } ) );
                }
            }
        }
    }
    os << data;
}

void generateUnityGraphViz( std::ostream& os, mega::io::Environment& environment, mega::io::Manifest& manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    Database database( environment, environment.project_manifest() );

    for( UnityAnalysis::Binding* pBinding : database.many< UnityAnalysis::Binding >( environment.project_manifest() ) )
    {
        std::string strName;
        {
            UnityAnalysis::Prefab* pPrefab = db_cast< UnityAnalysis::Prefab >( pBinding->get_binding() );
            UnityAnalysis::Manual* pManual = db_cast< UnityAnalysis::Manual >( pBinding->get_binding() );
            VERIFY_RTE( pPrefab || pManual );
            strName = pPrefab ? pPrefab->get_guid() : pManual->get_name();
        }

        Concrete::Object* pObject = pBinding->get_object();

        std::ostringstream osObjectName;
        osObjectName << "object_" << Concrete::printContextFullType( pObject );

        nlohmann::json node;
        {
            NODE( node, osObjectName.str(), Concrete::printContextFullType( pObject, "::" ) << getNodeInfo( pObject ) );
            {
                nlohmann::json property;
                PROP( property, "Name", strName );
                node[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property;
                PROP( property, "Type", pBinding->get_binding()->get_typeName() );
                node[ "properties" ].push_back( property );
            }
        }
        data[ "nodes" ].push_back( node );

        for( const auto& [ pDim, pBinding ] : pBinding->get_dataBindings() )
        {
            nlohmann::json dataNode;

            std::ostringstream osName;
            osName << "dim_" << Concrete::printContextFullType( pDim );
            NODE( dataNode, osName.str(), Concrete::printContextFullType( pDim, "::" ) << getNodeInfo( pDim ) );
            {
                nlohmann::json property;
                PROP( property, "Type", pBinding->get_typeName() );
                dataNode[ "properties" ].push_back( property );
            }
            data[ "nodes" ].push_back( dataNode );

            data[ "edges" ].push_back( nlohmann::json::object(
                { { "from", osObjectName.str() }, { "to", osName.str() }, { "colour", "0000FF" } } ) );
        }

        // THROW_TODO;
        /*
            for( const auto& [ pLink, pBinding ] : pBinding->get_linkBindings() )
            {
                nlohmann::json dataNode;

                std::ostringstream osName;
                osName << "link_" << Concrete::printContextFullType( pLink );
                NODE( dataNode, osName.str(), Concrete::printContextFullType( pLink, "::" ) << getNodeInfo( pLink ) );
                {
                    nlohmann::json property;
                    PROP( property, "Type", pBinding->get_typeName() );
                    dataNode[ "properties" ].push_back( property );
                }
                data[ "nodes" ].push_back( dataNode );

                data[ "edges" ].push_back( nlohmann::json::object(
                    { { "from", osObjectName.str() }, { "to", osName.str() }, { "colour", "00FF00" } } ) );
            }*/
    }

    os << data;
}
/*
std::string getBlockName( FinalStage::Automata::Block* pBlock )
{
    using namespace FinalStage;
    std::string strBlockName;
    if( db_cast< Automata::Sequence >( pBlock ) )
    {
        strBlockName = "Sequence";
    }
    else if( db_cast< Automata::Repeat >( pBlock ) )
    {
        strBlockName = "Repeat";
    }
    else if( db_cast< Automata::Alternative >( pBlock ) )
    {
        strBlockName = "Alternative";
    }
    else if( db_cast< Automata::InteruptHandler >( pBlock ) )
    {
        strBlockName = "Interupt";
    }
    else if( db_cast< Automata::EventHandler >( pBlock ) )
    {
        strBlockName = "Event";
    }
    else
    {
        strBlockName = "Uknown";
    }
    return strBlockName;
}

std::string getBlockID( const std::string& strAutomataName, FinalStage::Automata::Block* pBlock )
{
    std::ostringstream osBlockID;
    osBlockID << strAutomataName << pBlock->get_id();
    return osBlockID.str();
}
*/
/*
void automataRecurse( nlohmann::json& data, const std::string& strAutomataName,
    std::unordered_set< std::string >& actions, FinalStage::Automata::Block* pBlock )
{
    using namespace FinalStage;

    const std::string strBlockID   = getBlockID( strAutomataName, pBlock );
    const std::string strBlockName = getBlockName( pBlock );

    nlohmann::json node;
    {
        NODE( node, strBlockID, strBlockName );
    }
    data[ "nodes" ].push_back( node );

    for( auto pAction : pBlock->get_actions() )
    {
        std::ostringstream osActionName;
        osActionName << "action_" << strAutomataName << Concrete::printContextFullType( pAction );
        if( actions.find( osActionName.str() ) == actions.end() )
        {
            nlohmann::json node;
            NODE( node, osActionName.str(), Concrete::printContextFullType( pAction, "::" ) << getNodeInfo( pAction ) );
            data[ "nodes" ].push_back( node );
            actions.insert( osActionName.str() );
        }

        data[ "edges" ].push_back( nlohmann::json::object( { { "from", getBlockID( strAutomataName, pBlock ) },
                                                             { "to", osActionName.str() },
                                                             { "colour", "FF0000" } } ) );
    }

    Automata::Block* pLast = pBlock;
    for( auto pNode : pBlock->get_nodes() )
    {
        if( auto pNestedBlock = db_cast< Automata::Block >( pNode ) )
        {
            data[ "edges" ].push_back( nlohmann::json::object( { { "from", getBlockID( strAutomataName, pLast ) },
                                                                 { "to", getBlockID( strAutomataName, pNestedBlock ) },
                                                                 { "colour", "00FF00" } } ) );

            automataRecurse( data, strAutomataName, actions, pNestedBlock );
            pLast = pNestedBlock;
        }
    }
}
*/
void generateAutomataGraphViz( std::ostream& os, mega::io::Environment& environment, mega::io::Manifest& manifest )
{
    using namespace FinalStage;

    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    std::unordered_set< std::string > actions;

    for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        Database database( environment, sourceFilePath );

        /*for( Automata::Start* pAutomata : database.many< Automata::Start >( sourceFilePath ) )
        {
            std::ostringstream osAutomataName;
            osAutomataName << "automata_" << Concrete::printContextFullType( pAutomata );

            nlohmann::json node;
            {
                NODE(
                    node, osAutomataName.str(), Concrete::printContextFullType( pAutomata, "::" ) << getNodeInfo(
        pAutomata ) );
                {
                    nlohmann::json property;
                    PROP( property, "Name", pAutomata->get_identifier() );
                    node[ "properties" ].push_back( property );
                }
            }
            data[ "nodes" ].push_back( node );

            automataRecurse( data, osAutomataName.str(), actions, pAutomata->get_sequence() );

            data[ "edges" ].push_back(
                nlohmann::json::object( { { "from", osAutomataName.str() },
                                          { "to", getBlockID( osAutomataName.str(), pAutomata->get_sequence() ) },
                                          { "colour", "000000" } } ) );
        }*/
    }

    os << data;
}

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    std::string             strGraphType;
    boost::filesystem::path srcDir, buildDir, databaseArchivePath, outputFilePath;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Generate graph json data from archive or source/build directories" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "src_dir",    po::value< boost::filesystem::path >( &srcDir ),                            "Source directory" )
            ( "build_dir",  po::value< boost::filesystem::path >( &buildDir ),                          "Build directory" )
            ( "database",   po::value< boost::filesystem::path >( &databaseArchivePath ),               "Database archive path" )
            ( "type",       po::value< std::string >( &strGraphType )->default_value( "interface" ),    "graph type" )
            ( "output",     po::value< boost::filesystem::path >( &outputFilePath ),                    "output file to generate" )
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
        std::ostringstream osOutput;
        {
            std::unique_ptr< mega::io::Environment >       pEnvironment;
            std::unique_ptr< mega::io::Directories > m_pDirectories;

            if( !databaseArchivePath.empty() )
            {
                pEnvironment = std::make_unique< mega::io::ArchiveEnvironment >( databaseArchivePath );
            }
            else if( !srcDir.empty() && !buildDir.empty() )
            {
                m_pDirectories = std::make_unique< mega::io::Directories >(
                    mega::io::Directories{ srcDir, buildDir, "", "" } );
                pEnvironment = std::make_unique< mega::io::BuildEnvironment >( *m_pDirectories );
            }
            else
            {
                THROW_RTE( "Must specify either archive OR source and build dir" );
            }

            mega::io::Manifest manifest( *pEnvironment, pEnvironment->project_manifest() );

            if( strGraphType == "interface" )
            {
                generateInterfaceGraphVizInterface( osOutput, *pEnvironment, manifest );
            }
            else if( strGraphType == "concrete" )
            {
                generateInterfaceGraphVizConcrete( osOutput, *pEnvironment, manifest );
            }
            else if( strGraphType == "hyper" )
            {
                generateHyperGraphViz( osOutput, *pEnvironment, manifest );
            }
            else if( strGraphType == "tree" )
            {
                generateTreeGraphViz( osOutput, *pEnvironment, manifest );
            }
            else if( strGraphType == "memory" )
            {
                generateMemoryGraphViz( osOutput, *pEnvironment, manifest );
            }
            else if( strGraphType == "unity" )
            {
                generateUnityGraphViz( osOutput, *pEnvironment, manifest );
            }
            else if( strGraphType == "automata" )
            {
                generateAutomataGraphViz( osOutput, *pEnvironment, manifest );
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
        catch( std::exception& ex )
        {
            THROW_RTE( "Error generating graph: " << outputFilePath.string() << " exception: " << ex.what() );
        }
    }
}
} // namespace graph
} // namespace driver
