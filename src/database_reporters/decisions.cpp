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
#include "database/DecisionsStage.hxx"

#include "mega/values/service/url.hpp"
#include "mega/values/service/project.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace DecisionsStage
{
#include "compiler/interface.hpp"
#include "compiler/concrete.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"

namespace Concrete
{
using ContextVector = std::vector< const Context* >;
static ContextVector getSubTypes( const Node* pVertex )
{
    auto pContext       = db_cast< const Context >( pVertex );
    using ContextVector = std::vector< const Context* >;
    ContextVector path;
    while( pContext )
    {
        if( db_cast< const Object >( pContext ) )
        {
            break;
        }
        path.push_back( pContext );
        pContext = db_cast< const Context >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    return path;
}
} // namespace Concrete

} // namespace DecisionsStage

namespace mega::reporters
{

using namespace DecisionsStage;
using namespace DecisionsStage::ClangTraits;

namespace
{

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// BDD

mega::reports::Graph::Node::ID recurseNodesBDD( mega::reports::Graph& graph, Decision::Step* pStep )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Graph::Node node;

    bool bIncludeVariableInfo = true;
    if( auto pAssignments = db_cast< Decision::Assignments >( pStep ) )
    {
        bIncludeVariableInfo = false;
        node.m_rows.push_back( { "Assignment Type"s, "TypeName"s, "Multiplier"s } );
        node.m_colour = Colour::red;

        for( auto pAssignment : pAssignments->get_assignments() )
        {
            auto               pVertex = pAssignment->get_vertex();
            std::ostringstream osMultiplier;
            osMultiplier << "* " << pAssignment->get_instance_multiplier();

            if( pAssignment->get_is_true() )
            {
                node.m_rows.push_back( { "SET"s, Concrete::fullTypeName( pVertex->get_context() ),
                                         pVertex->get_context()->get_concrete_id()->get_type_id(), osMultiplier.str()

                } );
            }
            else
            {
                node.m_rows.push_back( { "UNSET"s, Concrete::fullTypeName( pVertex->get_context() ),
                                         pVertex->get_context()->get_concrete_id()->get_type_id(),
                                         osMultiplier.str() } );
            }
        }
    }
    else if( auto pBoolean = db_cast< Decision::Boolean >( pStep ) )
    {
        auto optDecider = pStep->get_decider();
        if( !optDecider.has_value() )
        {
            node.m_rows.push_back( { "BDD Vertex Type"s, "Boolean"s } );
            node.m_colour = Colour::green;
        }
        else
        {
            auto pDecider = optDecider.value();

            node.m_rows.push_back( { "BDD Vertex Type"s, "Boolean"s } );
            node.m_rows.push_back(
                { "Decider"s, Concrete::fullTypeName( pDecider ), pDecider->get_concrete_id()->get_type_id() } );
            node.m_colour = Colour::blue;
        }
        auto pVertex = pBoolean->get_variable();
        node.m_rows.push_back( { Concrete::fullTypeName( pVertex->get_context() ),
                                 pVertex->get_context()->get_concrete_id()->get_type_id() } );
    }
    else if( auto pSelection = db_cast< Decision::Selection >( pStep ) )
    {
        auto optDecider = pStep->get_decider();
        if( !optDecider.has_value() )
        {
            node.m_rows.push_back( { "BDD Vertex Type"s, "Selection"s } );
            node.m_colour = Colour::green;
        }
        else
        {
            auto pDecider = optDecider.value();
            node.m_rows.push_back( { "BDD Vertex Type"s, "Selection"s } );
            node.m_rows.push_back(
                { "Decider"s, Concrete::fullTypeName( pDecider ), pDecider->get_concrete_id()->get_type_id() } );
            node.m_colour = Colour::blue;
        }
        auto alternatives = pSelection->get_variable_ordering();
        auto altIter      = alternatives.begin();
        for( const auto pVar : pSelection->get_variable_alternatives() )
        {
            node.m_rows.push_back( { std::to_string( *altIter ), Concrete::fullTypeName( pVar->get_context() ),
                                     pVar->get_context()->get_concrete_id()->get_type_id() } );
            ++altIter;
        }
    }
    else
    {
        THROW_RTE( "Unknown decision step type" );
    }

    if( bIncludeVariableInfo )
    {
        for( auto pAssignVar : pStep->get_assignment() )
        {
            node.m_rows.push_back( { "ASSIGN"s, Concrete::fullTypeName( pAssignVar->get_context() ),
                                     pAssignVar->get_context()->get_concrete_id()->get_type_id() } );
        }
        for( auto pTrueVar : pStep->get_vars_true() )
        {
            node.m_rows.push_back( { "TRUE"s, Concrete::fullTypeName( pTrueVar->get_context() ),
                                     pTrueVar->get_context()->get_concrete_id()->get_type_id() } );
        }
        for( auto pFalseVar : pStep->get_vars_false() )
        {
            node.m_rows.push_back( { "FALSE"s, Concrete::fullTypeName( pFalseVar->get_context() ),
                                     pFalseVar->get_context()->get_concrete_id()->get_type_id() } );
        }
    }

    const mega::reports::Graph::Node::ID nodeID = graph.m_nodes.size();
    graph.m_nodes.push_back( node );

    for( auto pChild : pStep->get_children() )
    {
        auto nestedID = recurseNodesBDD( graph, pChild );
        graph.m_edges.push_back( Graph::Edge{ nodeID, nestedID } );
    }

    return nodeID;
}

mega::reports::Graph makeBDDGraph( Decision::DecisionProcedure* pProcedure )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Graph graph;
    graph.m_rankDirection = Graph::RankDirection::TB;

    recurseNodesBDD( graph, pProcedure->get_root() );

    return graph;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Derivation Tree
void addEdges( mega::reports::Graph::Node::ID iPrevious, std::vector< Derivation::Edge* > edges, reports::Graph& graph )
{
    using namespace std::string_literals;
    using namespace mega::reports;

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

void generateDerivationGraph( Derivation::Root* pRoot, reports::Graph& graph )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    // add root node
    graph.m_nodes.push_back( Graph::Node{ { { "Root:"s } }, Colour::lightblue } );
    addEdges( 0, pRoot->get_edges(), graph );
}

void generateDerivationGraph( Derivation::Dispatch* pDispatch, reports::Graph& graph )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    auto pVertex = pDispatch->get_vertex();

    graph.m_nodes.push_back( Graph::Node{ { { "Dispatch:"s, Concrete::fullTypeName( pVertex ) },
                                            { "TypeID:"s, pVertex->get_concrete_id()->get_type_id() } },
                                          Colour::lightgreen,
                                          std::nullopt,
                                          pVertex->get_concrete_id()->get_type_id() } );
    addEdges( 0, pDispatch->get_edges(), graph );
}

void generateDerivationGraph( Derivation::Node* pNode, reports::Graph& graph )
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Truth Table
void generateTruthTable( Concrete::Object* pObject, mega::reports::Branch& tree )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    auto variables = pObject->get_variable_vertices();
    auto table     = pObject->get_truth_table();

    Table truthTable;
    {
        {
            std::vector< Concrete::ContextVector > headings;
            std::size_t                            szLargest = 0;
            for( auto pVar : variables )
            {
                headings.push_back( Concrete::getSubTypes( pVar->get_context() ) );
                szLargest = std::max( headings.back().size(), szLargest );
            }

            // add cascading header rows
            for( std::size_t sz = 0; sz != szLargest; ++sz )
            {
                ContainerVector row;
                for( const auto& c : headings )
                {
                    if( c.size() > sz )
                    {
                        row.push_back( Line{ Concrete::getIdentifier( c[ sz ] ) } );
                    }
                    else
                    {
                        row.push_back( Line{} );
                    }
                }
                truthTable.m_rows.push_back( row );
            }
        }

        for( auto pTruthAssignment : table )
        {
            ContainerVector row;
            {
                auto trueVertex = pTruthAssignment->get_true_vertices();
                for( auto pVar : variables )
                {
                    if( std::find( trueVertex.begin(), trueVertex.end(), pVar ) != trueVertex.end() )
                    {
                        row.push_back( Line{ "T"s, std::nullopt, std::nullopt, Colour::black, Colour::lightgreen } );
                    }
                    else
                    {
                        row.push_back( Line{ "F"s, std::nullopt, std::nullopt, Colour::black, Colour::lightgray } );
                    }
                }
            }
            truthTable.m_rows.push_back( row );
        }
    }

    Branch branch{ { Concrete::fullTypeName( pObject ), " Total: "s, std::to_string( table.size() ) } };
    branch.m_elements.push_back( truthTable );
    tree.m_elements.push_back( branch );
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// And Or Tree
std::size_t recurseAndOrTree( mega::reports::Graph& graph, Automata::Vertex* pVertex,
                              std::vector< std::size_t >& nodes )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    std::string strType;
    Colour      colour      = Colour::lightblue;
    int         borderWidth = 1;
    {
        if( auto pAND = db_cast< Automata::And >( pVertex ) )
        {
            strType = "AND";
            colour  = Colour::lightgreen;
        }
        else if( auto pOR = db_cast< Automata::Or >( pVertex ) )
        {
            strType = "OR";
            colour  = Colour::lightblue;
        }
        else
        {
            THROW_RTE( "Unknown automata vertex type" );
        }
    }

    auto optBitsetStart = pVertex->get_bitset_range_start();

    Graph::Node node{
        {
            { strType, Concrete::getIdentifier( pVertex->get_context() ) },
            { "Concrete TypeID"s, pVertex->get_context()->get_concrete_id()->get_type_id() },
            { "Relative Domain"s, std::to_string( pVertex->get_relative_domain() ) },
            { "First"s, pVertex->get_is_first() ? "true"s : "false"s },
            { "Conditional"s, pVertex->get_is_conditional() ? "true"s : "false"s },
            { "Historical"s, pVertex->get_is_historical() ? "true"s : "false"s },
            { "Requirement"s, pVertex->get_has_requirement() ? "true"s : "false"s },
            // { "Total Domain"s, std::to_string( pVertex->get_context()->get_total_size() ) },
            { "Bitset Start"s, optBitsetStart.has_value() ? std::to_string( optBitsetStart.value() ) : "none"s },
            { "Bitset Range"s, std::to_string( pVertex->get_bitset_range_size() ) },

        },
        pVertex->get_is_conditional() ? Colour::green : Colour::blue,
        std::nullopt,
        pVertex->get_context()->get_concrete_id()->get_type_id(),
        colour,
        pVertex->get_is_conditional() ? 4 : 1

    };

    const std::size_t szNodeIndex = graph.m_nodes.size();

    nodes.push_back( szNodeIndex );
    graph.m_nodes.push_back( node );

    for( auto pChildVert : pVertex->get_children() )
    {
        const auto childNodeIndex = recurseAndOrTree( graph, pChildVert, nodes );
        graph.m_edges.push_back( Graph::Edge{ szNodeIndex, childNodeIndex } );
    }

    return szNodeIndex;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Concrete Tree recursion
void recurseTree( Concrete::Node* pNode, mega::reports::Branch& tree )
{
    using namespace std::string_literals;
    using namespace mega::reports;

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
        {
            auto opt = pInterupt->get_transition_decision();
            if( opt.has_value() )
            {
                auto   pDecisionProcedure = opt.value();
                Branch decisionProcedureBranch{ { "Decision Procedure for Interupt "s, Concrete::fullTypeName( pInterupt ) } };
                decisionProcedureBranch.m_elements.push_back(
                    Table{ {},
                           { { Line{ "Concrete TypeID"s }, Line{ pInterupt->get_concrete_id()->get_type_id() } },
                             { Line{ "Common Ancestor"s },
                               Line{ Concrete::fullTypeName( pDecisionProcedure->get_common_ancestor() ) } },
                             { Line{ "Instance Divider"s },
                               Line{ std::to_string( pDecisionProcedure->get_instance_divider().getValue() ) } }

                           } } );
                decisionProcedureBranch.m_elements.push_back( makeBDDGraph( pDecisionProcedure ) );
                branch.m_elements.push_back( decisionProcedureBranch );
            }
        }
    }
    else if( auto pState = db_cast< Concrete::State >( pNode ) )
    {
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
        {
            auto opt = pState->get_transition_decision();
            if( opt.has_value() )
            {
                auto   pDecisionProcedure = opt.value();
                Branch stateDecisionProcedureBranch{ { "Decision Procedure for State "s, Concrete::fullTypeName( pState ) } };
                stateDecisionProcedureBranch.m_elements.push_back(
                    Table{ {},
                           { { Line{ "Concrete TypeID"s }, Line{ pState->get_concrete_id()->get_type_id() } },
                             { Line{ "Common Ancestor"s },
                               Line{ Concrete::fullTypeName( pDecisionProcedure->get_common_ancestor() ) } },
                             { Line{ "Instance Divider"s },
                               Line{ std::to_string( pDecisionProcedure->get_instance_divider().getValue() ) } }

                           } } );

                stateDecisionProcedureBranch.m_elements.push_back( makeBDDGraph( pDecisionProcedure ) );
                branch.m_elements.push_back( stateDecisionProcedureBranch );
            }
        }
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
    else if( auto pObject = db_cast< Concrete::Object >( pNode ) )
    {
        Branch objectTree{ { Concrete::fullTypeName( pObject ), pObject->get_concrete_id()->get_type_id() } };

        generateTruthTable( pObject, objectTree );

        {
            Graph graph;
            graph.m_rankDirection = Graph::RankDirection::TB;

            auto                       pRoot = pObject->get_automata_root();
            std::vector< std::size_t > nodes;
            recurseAndOrTree( graph, pRoot, nodes );

            graph.m_subgraphs.emplace_back( Graph::Subgraph{
                { { Concrete::fullTypeName( pObject ), pObject->get_concrete_id()->get_type_id() } }, nodes } );

            objectTree.m_elements.push_back( graph );
        }
        branch.m_elements.push_back( objectTree );
    }

    for( auto pChild : pNode->get_children() )
    {
        recurseTree( pChild, branch );
    }
    tree.m_elements.emplace_back( std::move( branch ) );
}

} // namespace

Report DecisionsReporter::generate( const report::URL& url )
{
    using namespace std::string_literals;

    Branch root{ { ID } };

    Database database( m_args.environment, m_args.environment.project_manifest(), true );

    Branch tree{ { m_args.manifest.getManifestFilePath().path() } };

    auto pRoot = database.one< Concrete::Root >( m_args.manifest.getManifestFilePath() );
    for( Concrete::Node* pNode : pRoot->get_children() )
    {
        recurseTree( pNode, tree );
    }
    root.m_elements.push_back( { tree } );

    return root;
}

} // namespace mega::reporters
