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

std::size_t AndOrTreeReporter::recurse( mega::reports::Graph& graph, FinalStage::Automata::Vertex* pVertex,
                                        std::vector< std::size_t >& nodes )
{
    using namespace FinalStage;
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


    Graph::Node node{ {
                          { strType, Concrete::getIdentifier( pVertex->get_context() ) },
                          { "Concrete TypeID"s, pVertex->get_context()->get_concrete_id() },
                          { "Relative Domain"s, std::to_string( pVertex->get_relative_domain() ) },
                          { "First"s, pVertex->get_is_first() ? "true"s : "false"s },
                          { "Conditional"s, pVertex->get_is_conditional() ? "true"s : "false"s },
                          { "Historical"s, pVertex->get_is_historical() ? "true"s : "false"s },
                          { "Requirement"s, pVertex->get_has_requirement() ? "true"s : "false"s },
                          { "Total Domain"s, std::to_string( pVertex->get_context()->get_total_size() ) },
                          { "Bitset Start"s, optBitsetStart.has_value() ? std::to_string( optBitsetStart.value() ) : "none"s },
                          { "Bitset Range"s, std::to_string( pVertex->get_bitset_range_size() ) },

                      },
                      pVertex->get_is_conditional() ? Colour::green : Colour::blue,
                      std::nullopt,
                      pVertex->get_context()->get_concrete_id(),
                      colour,
                      pVertex->get_is_conditional() ? 4 : 1

    };

    const std::size_t szNodeIndex = graph.m_nodes.size();

    nodes.push_back( szNodeIndex );
    graph.m_nodes.push_back( node );

    for( auto pChildVert : pVertex->get_children() )
    {
        const auto childNodeIndex = recurse( graph, pChildVert, nodes );
        graph.m_edges.push_back( Graph::Edge{ szNodeIndex, childNodeIndex } );
    }

    return szNodeIndex;
}

mega::reports::Container AndOrTreeReporter::generate( const mega::reports::URL& url )
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

            auto                       pRoot = pObject->get_automata_root();
            std::vector< std::size_t > nodes;
            recurse( graph, pRoot, nodes );

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
