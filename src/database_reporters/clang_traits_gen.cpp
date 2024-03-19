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
#include "reporters.hpp"

#include "database_reporters/factory.hpp"

#include "environment/environment_archive.hpp"
#include "database/ClangTraitsStage.hxx"

#include "report/url.hpp"
#include "mega/values/service/project.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace ClangTraitsStage
{
#include "compiler/interface.hpp"
#include "compiler/concrete.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace ClangTraitsStage

namespace mega::reporters
{

using namespace ClangTraitsStage;
using namespace ClangTraitsStage::ClangTraits;
using namespace std::string_literals;

namespace
{

void addEdges( Graph::Node::ID iPrevious, std::vector< Derivation::Edge* > edges, Graph& graph )
{
    for( auto pEdge : edges )
    {
        auto pNextStep = pEdge->get_next();
        auto pVertex   = pNextStep->get_vertex();
        auto iNext     = graph.m_nodes.size();

        if( db_cast< Derivation::Select >( pNextStep ) )
        {
            graph.m_nodes.push_back( Graph::Node{ { { "SELECT:"s, Concrete::fullTypeName( pVertex ) },
                                                    { "TypeID:"s, pVertex->get_concrete_id()->get_type_id() } },
                                                  Colour::lightyellow,
                                                  std::nullopt,
                                                  pVertex->get_concrete_id()->get_type_id() } );
        }
        else if( db_cast< Derivation::Or >( pNextStep ) )
        {
            graph.m_nodes.push_back( Graph::Node{ { { "OR:"s, Concrete::fullTypeName( pVertex ) },
                                                    { "TypeID:"s, pVertex->get_concrete_id()->get_type_id() } },
                                                  Colour::lightgreen,
                                                  std::nullopt,
                                                  pVertex->get_concrete_id()->get_type_id() } );
        }
        else
        {
            graph.m_nodes.push_back( Graph::Node{ { { "AND:"s, Concrete::fullTypeName( pVertex ) },
                                                    { "TypeID:"s, pVertex->get_concrete_id()->get_type_id() } },
                                                  Colour::lightblue,
                                                  std::nullopt,
                                                  pVertex->get_concrete_id()->get_type_id() } );
        }

        Graph::Edge edge{ iPrevious, iNext };

        if( pEdge->get_eliminated() )
        {
            edge.m_style  = Graph::Edge::Style::dotted;
            edge.m_colour = Colour::red;
        }
        else
        {
            for( auto pHyperEdge : pEdge->get_edges() )
            {
                switch( pHyperEdge->get_type().get() )
                {
                    case EdgeType::eParent:
                        edge.m_colour = Colour::orange;
                        break;
                    case EdgeType::eChildSingular:
                        edge.m_colour = Colour::olive;
                        break;
                    case EdgeType::eChildNonSingular:
                        edge.m_colour = Colour::purple;
                        break;

                    case EdgeType::eDim:
                        edge.m_colour = Colour::red;
                        break;

                    case EdgeType::eLink:
                        edge.m_style  = Graph::Edge::Style::bold;
                        edge.m_colour = Colour::greenyellow;
                        break;

                    case EdgeType::eInterObjectNonOwner:
                    case EdgeType::eInterObjectOwner:
                    case EdgeType::eInterObjectParent:
                        edge.m_style  = Graph::Edge::Style::dashed;
                        edge.m_colour = Colour::darkgreen;
                        break;

                    case EdgeType::TOTAL_EDGE_TYPES:
                    default:
                    {
                        break;
                    }
                }
            }
        }

        /*switch( pEdge->get_precedence() )
        {
            case 0:
            break;
            case 1:
            {
            }
            break;
            case 2:
            {
            }
            break;
            default:
            {
                THROW_RTE( "Unknown precendence type" );
            }
            break;
        }*/

        graph.m_edges.push_back( edge );

        addEdges( iNext, pNextStep->get_edges(), graph );
    }
}

void generateDerivationGraph( Derivation::Root* pRoot, Graph& graph )
{
    // add root node
    graph.m_nodes.push_back( Graph::Node{ { { "Root:"s } }, Colour::lightblue } );
    addEdges( 0, pRoot->get_edges(), graph );
}

void generateDerivationGraph( Derivation::Dispatch* pDispatch, Graph& graph )
{
    auto pVertex = pDispatch->get_vertex();

    graph.m_nodes.push_back( Graph::Node{ { { "Dispatch:"s, Concrete::fullTypeName( pVertex ) },
                                            { "TypeID:"s, pVertex->get_concrete_id()->get_type_id() } },
                                          Colour::lightgreen,
                                          std::nullopt,
                                          pVertex->get_concrete_id()->get_type_id() } );
    addEdges( 0, pDispatch->get_edges(), graph );
}

void generateDerivationGraph( Derivation::Node* pNode, Graph& graph )
{
    if( auto pDispatch = db_cast< Derivation::Dispatch >( pNode ) )
    {
        generateDerivationGraph( pDispatch, graph );
    }
    else if( auto pRoot = db_cast< Derivation::Root >( pNode ) )
    {
        generateDerivationGraph( pRoot, graph );
    }
    else
    {
        THROW_RTE( "Unknown derivation node type" );
    }
}

void recurse( Concrete::Node* pNode, Branch& tree )
{
    using namespace std::string_literals;

    Branch branch;

    branch.m_label = { { Concrete::getKind( pNode ), ": "s, Concrete::getIdentifier( pNode ) } };

    if( auto pInterupt = db_cast< Concrete::Interupt >( pNode ) )
    {
        {
            Branch transitions{ { "Transitions "s } };
            for( auto pTransition : pInterupt->get_transitions() )
            {
                Graph graph;
                generateDerivationGraph( pTransition, graph );
                transitions.m_elements.push_back( graph );
            }
            branch.m_elements.push_back( transitions );
        }
        {
            Branch events{ { "Events "s } };
            for( auto pEvent : pInterupt->get_events() )
            {
                Graph graph;
                generateDerivationGraph( pEvent, graph );
                events.m_elements.push_back( graph );
            }
            branch.m_elements.push_back( events );
        }
        {
            Branch dispatches{ { "Interupt Dispatch"s } };
            for( auto pEventDispatch : pInterupt->get_dispatches() )
            {
                Branch event{ { "Event "s, Concrete::fullTypeName( pEventDispatch->get_event() ) } };
                {
                    Graph graph;
                    generateDerivationGraph( pEventDispatch->get_derivation(), graph );
                    event.m_elements.push_back( graph );
                }
                dispatches.m_elements.push_back( event );
            }
            branch.m_elements.push_back( dispatches );
        }
    }
    else if( auto pState = db_cast< Concrete::State >( pNode ) )
    {
        Branch transitions{ { "Transitions "s } };
        for( auto pTransition : pState->get_transitions() )
        {
            Graph graph;
            generateDerivationGraph( pTransition, graph );
            transitions.m_elements.push_back( graph );
        }
        branch.m_elements.push_back( transitions );
    }
    else if( auto pDecider = db_cast< Concrete::Decider >( pNode ) )
    {
        Branch events{ { "Events "s } };
        for( auto pEvent : pDecider->get_events() )
        {
            Graph graph;
            generateDerivationGraph( pEvent, graph );
            events.m_elements.push_back( graph );
        }
        branch.m_elements.push_back( events );
    }

    for( auto pChild : pNode->get_children() )
    {
        recurse( pChild, branch );
    }
    tree.m_elements.emplace_back( std::move( branch ) );
}

} // namespace

Report ClangTraitsGenReporter::generate( const URL&  )
{
    Branch root{ { ID } };

    Database database( m_args.environment, m_args.environment.project_manifest(), true );

    Branch tree{ { m_args.manifest.getManifestFilePath().path() } };

    auto pRoot = database.one< Concrete::Root >( m_args.manifest.getManifestFilePath() );

    for( Concrete::Node* pNode : pRoot->get_children() )
    {
        recurse( pNode, tree );
    }
    root.m_elements.push_back( { tree } );

    return root;
}

} // namespace mega::reporters
