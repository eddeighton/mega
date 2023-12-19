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
#include "database/FinalStage.hxx"

#include "reports/renderer_html.hpp"

#include "mega/values/service/url.hpp"
#include "mega/values/service/project.hpp"

#include "mega/mangle/traits.hpp"
#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"
#include "common/string.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/interface.hpp"
} // namespace FinalStage

namespace mega::reporters
{

reports::Graph::Node::ID EnumReporter::recurse( mega::reports::Graph& graph, FinalStage::Automata::Enum* pEnum,
                                                std::vector< reports::Graph::Node::ID >& nodes,
                                                SwitchIDToNodeMap& switchIDToNodeMap, Edges& edges )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    auto pVertex = pEnum->get_vertex();

    std::string strVertexType;
    {
        if( !pEnum->get_is_or() )
        {
            strVertexType = "AND";
        }
        else if( pEnum->get_is_or() )
        {
            strVertexType = "OR";
        }
        else
        {
            THROW_RTE( "Unknown automata vertex type" );
        }
    }

    std::string strEnumType;
    Colour      colour;
    {
        if( auto pTest = db_cast< Automata::Test >( pEnum ) )
        {
            if( pTest->get_start_state() )
            {
                if( pEnum->get_action().has_value() )
                {
                    strEnumType = "Start && Test && Action";
                    colour      = Colour::lightsalmon;
                }
                else
                {
                    strEnumType = "Start && Test";
                    colour      = Colour::lightgoldenrodyellow;
                }
            }
            else
            {
                if( pEnum->get_action().has_value() )
                {
                    strEnumType = "Test && Action";
                    colour      = Colour::lightyellow;
                }
                else
                {
                    strEnumType = "Test";
                    colour      = Colour::lightgreen;
                }
            }
        }
        else
        {
            VERIFY_RTE( pEnum->get_action().has_value() );
            strEnumType = "Action";
            colour      = Colour::lightblue;
        }
    }

    std::ostringstream osChildSwitchIndices;
    {
        auto indices = pEnum->get_child_switch_indices();
        common::delimit( indices.begin(), indices.end(), ",", osChildSwitchIndices );
    }

    const std::size_t szNodeIndex = graph.m_nodes.size();
    nodes.push_back( szNodeIndex );

    // clang-format off
    Graph::Node node{ {

        { "Switch Index"s, std::to_string( pEnum->get_switch_index() ) },
        { "Next Switch Index"s, std::to_string( pEnum->get_next_switch_index() ) }

        },
        colour, std::nullopt, pVertex->get_context()->get_concrete_id() };
    // clang-format on

    if( auto pTest = db_cast< Automata::Test >( pEnum ) )
    {
        node.m_rows.push_back( { "Failure Switch Index"s, std::to_string( pTest->get_failure_switch_index() ) } );
        node.m_rows.push_back( { "Bitset Index"s, std::to_string( pTest->get_bitset_index() ) } );

        edges.failureEdges.insert( { pTest->get_switch_index(), pTest->get_failure_switch_index() } );
    }

    // general stuff
    node.m_rows.push_back( { strEnumType, Concrete::getIdentifier( pVertex->get_context() ) } );
    node.m_rows.push_back( { "Vertex Concrete TypeID: "s, pVertex->get_context()->get_concrete_id() } );
    node.m_rows.push_back(
        { "Vertex Interface TypeID: "s, pVertex->get_context()->get_interface()->get_interface_id() } );
    node.m_rows.push_back( { "Sub Type Instance"s, pEnum->get_sub_type_instance() } );

    edges.siblingEdges.insert( { pEnum->get_switch_index(), pEnum->get_next_switch_index() } );

    graph.m_nodes.push_back( node );

    VERIFY_RTE( switchIDToNodeMap.insert( { pEnum->get_switch_index(), szNodeIndex } ).second );

    for( auto pChildEnum : pEnum->get_children() )
    {
        const auto childNodeIndex = recurse( graph, pChildEnum, nodes, switchIDToNodeMap, edges );
        edges.treeEdges.insert( { pEnum->get_switch_index(), pChildEnum->get_switch_index() } );
    }

    return szNodeIndex;
}

mega::reports::Container EnumReporter::generate( const mega::reports::URL& url )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { ID } };

    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Branch sourceBranch{ { sourceFilePath.path() } };

        Database database( m_args.environment, sourceFilePath );

        for( Concrete::Object* pObject : database.many< Concrete::Object >( sourceFilePath ) )
        {
            Graph graph;
            graph.m_rankDirection = Graph::RankDirection::TB;

            std::vector< reports::Graph::Node::ID > nodes;
            std::vector< reports::Graph::Node::ID > rootNodes;
            SwitchIDToNodeMap                       switchIDToNodeMap;
            Edges                                   edges;
            for( auto pEnum : pObject->get_automata_enums() )
            {
                reports::Graph::Node::ID rootNodeIndex = recurse( graph, pEnum, nodes, switchIDToNodeMap, edges );
                rootNodes.push_back( rootNodeIndex );
            }

            // create a root node for tree
            {
                Graph::Node           node{ { { "Start"s } } };
                const Graph::Node::ID szNodeIndex = graph.m_nodes.size();
                nodes.push_back( szNodeIndex );
                graph.m_nodes.push_back( node );

                bool bFirst = true;
                for( auto rootNodeIndex : rootNodes )
                {
                    if( bFirst )
                    {
                        bFirst = false;
                        graph.m_edges.push_back(
                            Graph::Edge{ szNodeIndex, rootNodeIndex, Colour::green, Graph::Edge::Style::solid } );
                    }
                    else
                    {
                        graph.m_edges.push_back(
                            Graph::Edge{ szNodeIndex, rootNodeIndex, Colour{}, Graph::Edge::Style::invis } );
                    }
                }
            }

            // add node index for final state edge
            switchIDToNodeMap.insert( { pObject->get_total_switch_index(), graph.m_nodes.size() } );

            // NOTE: non-tree edges have m_bIgnoreInLayout == true
            for( auto& edge : edges.siblingEdges )
            {
                VERIFY_RTE( switchIDToNodeMap.contains( edge.first ) );
                VERIFY_RTE( switchIDToNodeMap.contains( edge.second ) );
                graph.m_edges.push_back( Graph::Edge{ switchIDToNodeMap[ edge.first ], switchIDToNodeMap[ edge.second ],
                                                      Colour::green, Graph::Edge::Style::solid, true } );
            }

            for( auto& edge : edges.failureEdges )
            {
                VERIFY_RTE( switchIDToNodeMap.contains( edge.first ) );
                VERIFY_RTE( switchIDToNodeMap.contains( edge.second ) );
                graph.m_edges.push_back( Graph::Edge{ switchIDToNodeMap[ edge.first ], switchIDToNodeMap[ edge.second ],
                                                      Colour::red, Graph::Edge::Style::dashed, true } );
            }

            for( auto& edge : edges.treeEdges )
            {
                VERIFY_RTE( switchIDToNodeMap.contains( edge.first ) );
                VERIFY_RTE( switchIDToNodeMap.contains( edge.second ) );
                graph.m_edges.push_back( Graph::Edge{ switchIDToNodeMap[ edge.first ], switchIDToNodeMap[ edge.second ],
                                                      Colour{}, Graph::Edge::Style::invis } );
            }

            // create a final state node - ensuring its invisible edges are added last
            {
                Graph::Node node{ { { "Final State"s, std::to_string( pObject->get_total_switch_index() ) } } };
                const Graph::Node::ID szNodeIndex = graph.m_nodes.size();
                graph.m_nodes.push_back( node );

                // attempt to make final state node on lowest rank
                for( auto nodeIndex : nodes )
                {
                    graph.m_edges.push_back(
                        Graph::Edge{ nodeIndex, szNodeIndex, Colour{}, Graph::Edge::Style::invis } );
                }

                // final switch index + 1 can be final state
                nodes.push_back( szNodeIndex );
            }

            Graph::Subgraph subgraph{ { { Concrete::printContextFullType( pObject ), pObject->get_concrete_id() } } };
            subgraph.m_nodes = nodes;
            graph.m_subgraphs.push_back( subgraph );

            sourceBranch.m_elements.push_back( graph );
        }

        branch.m_elements.push_back( sourceBranch );
    }

    return branch;
}

} // namespace mega::reporters
