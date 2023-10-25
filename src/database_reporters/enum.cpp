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
#include "compiler/concrete.hpp"
} // namespace FinalStage

namespace mega::reporters
{

std::size_t EnumReporter::recurse( mega::reports::Graph& graph, FinalStage::Automata::Enum* pEnum,
                                   std::vector< std::size_t >& nodes )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    std::string strType;
    Colour      colour = Colour::lightblue;
    {
        if( !pEnum->get_is_or() )
        {
            strType = "AND";
            colour  = Colour::lightgreen;
        }
        else if( pEnum->get_is_or() )
        {
            strType = "OR";
        }
        else
        {
            THROW_RTE( "Unknown automata vertex type" );
        }
    }

    std::ostringstream osIndices;
    auto               indices = pEnum->get_indices();
    common::delimit( indices.begin(), indices.end(), ",", osIndices );

    auto pVertex = pEnum->get_vertex();

    // clang-format off
    Graph::Node node{ {

        { strType, Concrete::getIdentifier( pVertex->get_context() ) },
        { "Vertex Concrete TypeID: "s, pVertex->get_context()->get_concrete_id() },
        { "Vertex Relative Domain: "s, std::to_string( pVertex->get_relative_domain() ) },
        { "Vertex Total Domain: "s, std::to_string( pVertex->get_context()->get_total_size() ) },
        { "Vertex Base: "s, std::to_string( pVertex->get_index_base() ) },
            
        { "Switch Index"s, std::to_string( pEnum->get_switch_index() ) },
        { "Bitset Index"s, std::to_string( pEnum->get_bitset_index() ) },
        { "Indices"s, osIndices.str() },

        { "Is Or"s, std::to_string( pEnum->get_is_or() ) },
        { "Has Action"s, std::to_string( pEnum->get_has_action() ) },
        { "Sub Type Instance"s, pEnum->get_sub_type_instance() },

        },
        colour };
    // clang-format on

    const std::size_t szNodeIndex = graph.m_nodes.size();

    nodes.push_back( szNodeIndex );
    graph.m_nodes.push_back( node );

    for( auto pChildVert : pEnum->get_children() )
    {
        const auto childNodeIndex = recurse( graph, pChildVert, nodes );
        graph.m_edges.push_back( Graph::Edge{ szNodeIndex, childNodeIndex } );
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

            std::vector< std::size_t > nodes;
            recurse( graph, pObject->get_automata_enum(), nodes );

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
