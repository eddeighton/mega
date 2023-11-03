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
using namespace FinalStage;

mega::reports::Graph::Node::ID recurseNodes( mega::reports::Graph& graph, Decision::Step* pStep )
{
    using namespace std::string_literals;
    using namespace mega::reports;


    const mega::reports::Graph::Node::ID nodeID = graph.m_nodes.size();
 
/*
    auto pTest    = pVertex->get_test();
    auto pContext = pTest->get_context();
    Colour bkColour = pVertex->get_ignored() ? Colour::lightgrey : Colour::lightblue;

    if( db_cast< Decision::Eliminated >( pVertex ) )
    {
        graph.m_nodes.push_back(
            Graph::Node{ { { Concrete::printContextFullType( pContext ), pContext->get_concrete_id() },
                           { "BDD Vertex Type"s, "Eliminated"s },
                           { "Ignored"s, pVertex->get_ignored() ? "true"s : "false"s }

                         },
                         Colour::red, std::nullopt, std::nullopt, bkColour } );
    }
    else if( db_cast< Decision::True >( pVertex ) )
    {
        graph.m_nodes.push_back(
            Graph::Node{ { { Concrete::printContextFullType( pContext ), pContext->get_concrete_id() },
                           { "BDD Vertex Type"s, "True"s },
                           { "Ignored"s, pVertex->get_ignored() ? "true"s : "false"s } },

                         Colour::lightgreen, std::nullopt, std::nullopt, bkColour } );
    }
    else if( db_cast< Decision::False >( pVertex ) )
    {
        graph.m_nodes.push_back(
            Graph::Node{ { { Concrete::printContextFullType( pContext ), pContext->get_concrete_id() },
                           { "BDD Vertex Type"s, "False"s },
                           { "Ignored"s, pVertex->get_ignored() ? "true"s : "false"s } },

                         Colour::lightblue, std::nullopt, std::nullopt, bkColour } );
    }
    else if( db_cast< Decision::Decideable >( pVertex ) )
    {
        graph.m_nodes.push_back(
            Graph::Node{ { { Concrete::printContextFullType( pContext ), pContext->get_concrete_id() },
                           { "BDD Vertex Type"s, "Decideable"s },
                           { "Ignored"s, pVertex->get_ignored() ? "true"s : "false"s } },

                         Colour::green, std::nullopt, std::nullopt, bkColour } );
    }
    else
    {
        THROW_RTE( "Unknown BDDVertex type" );
    }

    if( auto optFalse = pVertex->get_false_vertex(); optFalse.has_value() )
    {
        auto nestedID = recurseNodes( graph, optFalse.value() );
        graph.m_edges.push_back( Graph::Edge{ nodeID, nestedID, Colour::red } );
    }

    if( auto optTrue = pVertex->get_true_vertex(); optTrue.has_value() )
    {
        auto nestedID = recurseNodes( graph, optTrue.value() );
        graph.m_edges.push_back( Graph::Edge{ nodeID, nestedID, Colour::green } );
    }
*/
    return nodeID;
}

mega::reports::Graph BDDReporter::makeBDDGraph( Decision::DecisionProcedure* pProcedure )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Graph graph;
    graph.m_rankDirection = Graph::RankDirection::TB;

    // recurseNodes( graph, pProcedure->get_root() );

    return graph;
}

mega::reports::Container BDDReporter::generate( const mega::reports::URL& url )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { ID } };

    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Branch sourceBranch{ { sourceFilePath.path() } };

        Database database( m_args.environment, sourceFilePath );

        for( Concrete::Interupt* pInterupt : database.many< Concrete::Interupt >( sourceFilePath ) )
        {
            auto opt = pInterupt->get_transition_decision();
            if( opt.has_value() )
            {
                auto   pDecisionProcedure = opt.value();
                Branch branch{ { "Interupt "s, Concrete::printContextFullType( pInterupt ) } };
                branch.m_elements.push_back(
                    Table{ {},
                           { { Line{ "Concrete TypeID"s }, Line{ pInterupt->get_concrete_id() } },
                             { Line{ "Common Ancestor"s },
                               Line{ Concrete::printContextFullType( pDecisionProcedure->get_common_ancestor() ) } }

                           } } );
                branch.m_elements.push_back( makeBDDGraph( pDecisionProcedure ) );
                sourceBranch.m_elements.push_back( branch );
            }
        }

        for( Concrete::State* pState : database.many< Concrete::State >( sourceFilePath ) )
        {
            auto opt = pState->get_transition_decision();
            if( opt.has_value() )
            {
                auto   pDecisionProcedure = opt.value();
                Branch branch{ { "State "s, Concrete::printContextFullType( pState ) } };
                branch.m_elements.push_back(
                    Table{ {},
                           { { Line{ "Concrete TypeID"s }, Line{ pState->get_concrete_id() } },
                             { Line{ "Common Ancestor"s },
                               Line{ Concrete::printContextFullType( pDecisionProcedure->get_common_ancestor() ) } }

                           } } );

                // pState->get_transition()

                branch.m_elements.push_back( makeBDDGraph( pDecisionProcedure ) );
                sourceBranch.m_elements.push_back( branch );
            }
        }

        branch.m_elements.push_back( sourceBranch );
    }

    return branch;
}

} // namespace mega::reporters
