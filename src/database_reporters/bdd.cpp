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
} // namespace FinalStage

namespace mega::reporters
{
using namespace FinalStage;

mega::reports::Graph::Node::ID recurseNodes( mega::reports::Graph& graph, Decision::Step* pStep )
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
                node.m_rows.push_back( { "SET"s, Concrete::printContextFullType( pVertex->get_context() ),
                                         pVertex->get_context()->get_concrete_id(), osMultiplier.str()

                } );
            }
            else
            {
                node.m_rows.push_back( { "UNSET"s, Concrete::printContextFullType( pVertex->get_context() ),
                                         pVertex->get_context()->get_concrete_id(), osMultiplier.str() } );
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
                { "Decider"s, Concrete::printContextFullType( pDecider ), pDecider->get_concrete_id() } );
            node.m_colour = Colour::blue;
        }
        auto pVertex = pBoolean->get_variable();
        node.m_rows.push_back(
            { Concrete::printContextFullType( pVertex->get_context() ), pVertex->get_context()->get_concrete_id() } );
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
                { "Decider"s, Concrete::printContextFullType( pDecider ), pDecider->get_concrete_id() } );
            node.m_colour = Colour::blue;
        }
        auto alternatives = pSelection->get_variable_ordering();
        auto altIter      = alternatives.begin();
        for( const auto pVar : pSelection->get_variable_alternatives() )
        {
            node.m_rows.push_back( { std::to_string( *altIter ), Concrete::printContextFullType( pVar->get_context() ),
                                     pVar->get_context()->get_concrete_id() } );
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
            node.m_rows.push_back( { "ASSIGN"s, Concrete::printContextFullType( pAssignVar->get_context() ),
                                    pAssignVar->get_context()->get_concrete_id() } );
        }
        for( auto pTrueVar : pStep->get_vars_true() )
        {
            node.m_rows.push_back( { "TRUE"s, Concrete::printContextFullType( pTrueVar->get_context() ),
                                    pTrueVar->get_context()->get_concrete_id() } );
        }
        for( auto pFalseVar : pStep->get_vars_false() )
        {
            node.m_rows.push_back( { "FALSE"s, Concrete::printContextFullType( pFalseVar->get_context() ),
                                    pFalseVar->get_context()->get_concrete_id() } );
        }
    }

    const mega::reports::Graph::Node::ID nodeID = graph.m_nodes.size();
    graph.m_nodes.push_back( node );

    for( auto pChild : pStep->get_children() )
    {
        auto nestedID = recurseNodes( graph, pChild );
        graph.m_edges.push_back( Graph::Edge{ nodeID, nestedID } );
    }

    return nodeID;
}

mega::reports::Graph BDDReporter::makeBDDGraph( Decision::DecisionProcedure* pProcedure )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Graph graph;
    graph.m_rankDirection = Graph::RankDirection::TB;

    recurseNodes( graph, pProcedure->get_root() );

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
                               Line{ Concrete::printContextFullType( pDecisionProcedure->get_common_ancestor() ) } },
                             { Line{ "Instance Divider"s },
                               Line{ std::to_string( pDecisionProcedure->get_instance_divider() ) } }

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
                               Line{ Concrete::printContextFullType( pDecisionProcedure->get_common_ancestor() ) } },
                             { Line{ "Instance Divider"s },
                               Line{ std::to_string( pDecisionProcedure->get_instance_divider() ) } }

                           } } );

                branch.m_elements.push_back( makeBDDGraph( pDecisionProcedure ) );
                sourceBranch.m_elements.push_back( branch );
            }
        }

        branch.m_elements.push_back( sourceBranch );
    }

    return branch;
}

} // namespace mega::reporters
