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
#include "mega/values/compilation/type_id.hpp"

#include "mega/mangle/traits.hpp"
#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

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
#include "compiler/concrete.hpp"
} // namespace FinalStage

namespace mega::reporters
{

mega::reports::Table legend()
{
    using namespace std::string_literals;
    using namespace mega::reports;
    mega::reports::Table legend{ { "Edge Colour"s, "Edge Type"s } };

    auto key = []( Colour colour, EdgeType edgeType ) -> ContainerVector
    {
        return ContainerVector{
            Line{ std::string{ colour.str() }, std::nullopt, std::nullopt, Colour::black, colour },
            Line{ std::string{ edgeType.str() }, std::nullopt, std::nullopt, Colour::black, colour } };
    };

    // clang-format off
    legend.m_rows.push_back( key( Colour::orange, EdgeType::eParent ) );
    legend.m_rows.push_back( key( Colour::olive, EdgeType::eChildSingular ) );
    legend.m_rows.push_back( key( Colour::purple, EdgeType::eChildNonSingular ) );
    legend.m_rows.push_back( key( Colour::red, EdgeType::eDim ) );
    legend.m_rows.push_back( key( Colour::greenyellow, EdgeType::eLink ) );
    legend.m_rows.push_back( key( Colour::darkgreen, EdgeType::ePolyParent ) );
    // clang-format on

    return legend;
}

void addEdges( mega::reports::Graph::Node::ID iPrevious, std::vector< FinalStage::Derivation::Edge* > edges,
               reports::Graph& graph )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    for( auto pEdge : edges )
    {
        auto pNextStep = pEdge->get_next();
        auto pVertex   = pNextStep->get_vertex();
        auto iNext     = graph.m_nodes.size();

        if( db_cast< Derivation::Or >( pNextStep ) )
        {
            graph.m_nodes.push_back( Graph::Node{
                { { "OR:"s, Concrete::printContextFullType( pVertex ) }, { "TypeID:"s, pVertex->get_concrete_id() } },
                Colour::lightgreen,
                std::nullopt,
                pVertex->get_concrete_id() } );
        }
        else
        {
            graph.m_nodes.push_back( Graph::Node{
                { { "AND:"s, Concrete::printContextFullType( pVertex ) }, { "TypeID:"s, pVertex->get_concrete_id() } },
                Colour::lightblue,
                std::nullopt,
                pVertex->get_concrete_id() } );
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

                    case EdgeType::eMonoSingularMandatory:
                    case EdgeType::eMonoSingularOptional:
                    case EdgeType::eMonoNonSingularMandatory:
                    case EdgeType::eMonoNonSingularOptional:

                    case EdgeType::ePolySingularMandatory:
                    case EdgeType::ePolySingularOptional:
                    case EdgeType::ePolyNonSingularMandatory:
                    case EdgeType::ePolyNonSingularOptional:
                    case EdgeType::ePolyParent:
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

void DerivationReporter::generateDerivationGraph( FinalStage::Derivation::Root* pRoot, reports::Graph& graph )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    // add root node
    graph.m_nodes.push_back( Graph::Node{ { { "Root:"s } }, Colour::lightblue } );
    addEdges( 0, pRoot->get_edges(), graph );
}

mega::reports::Container DerivationReporter::generate( const mega::reports::URL& url )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch files{ { "Files"s } };
    files.m_elements.push_back( legend() );

    std::map< TypeID, std::size_t >      objectIDs;
    std::multimap< TypeID, std::size_t > objectGroups;
    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Database database( m_args.environment, sourceFilePath );

        Branch fileBranch{ { sourceFilePath.path() } };

        for( Interface::InvocationContext* pContext : database.many< Interface::InvocationContext >( sourceFilePath ) )
        {
            Branch contextBranch{ { Interface::printIContextFullType( pContext ), " "s, pContext->get_interface_id() },
                                  {},
                                  pContext->get_interface_id() };

            for( auto pInvocationInstance : pContext->get_invocation_instances() )
            {
                auto pInvocation = pInvocationInstance->get_invocation();

                Branch invocationBranch{ { "Invocation "s, pInvocation->get_id() } };
                {
                    invocationBranch.m_bookmark = pInvocation->get_id();
                    Table table{ {},

                                 {
                                     { Line{ "Context"s }, Line{ pInvocation->get_canonical_context() } },
                                     { Line{ "Type Path"s }, Line{ pInvocation->get_canonical_type_path() } },
                                     { Line{ "Explicit Operation"s },
                                       Line{ getExplicitOperationString( pInvocation->get_explicit_operation() ) } },

                                 }

                    };
                    invocationBranch.m_elements.push_back( table );
                }
                {
                    Graph graph;
                    generateDerivationGraph( pInvocation->get_derivation(), graph );
                    invocationBranch.m_elements.push_back( graph );
                }
                contextBranch.m_elements.push_back( invocationBranch );
            }

            if( auto pState = db_cast< Interface::State >( pContext ) )
            {
                auto optTransition = pState->get_transition_trait();
                if( optTransition.has_value() )
                {
                    auto   pDerivation = optTransition.value()->get_derivation();
                    Branch successor{ { "State Successor"s } };

                    for( auto pRoot : pDerivation )
                    {
                        Graph graph;
                        generateDerivationGraph( pRoot, graph );
                        successor.m_elements.push_back( graph );
                    }

                    contextBranch.m_elements.push_back( successor );
                }
            }
            else if( auto pInterupt = db_cast< Interface::Interupt >( pContext ) )
            {
                auto optTransition = pInterupt->get_transition_trait();
                if( optTransition.has_value() )
                {
                    auto   pDerivation = optTransition.value()->get_derivation();
                    Branch successor{ { "Interupt Successor"s } };

                    for( auto pRoot : pDerivation )
                    {
                        Graph graph;
                        generateDerivationGraph( pRoot, graph );
                        successor.m_elements.push_back( graph );
                    }

                    contextBranch.m_elements.push_back( successor );
                }
            }

            fileBranch.m_elements.push_back( contextBranch );
        }

        files.m_elements.push_back( fileBranch );
    }

    return files;
}

} // namespace mega::reporters
