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

#include "database/HyperGraphAnalysis.hxx"

#include "report/url.hpp"
#include "mega/values/service/project.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace HyperGraphAnalysis
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/interface.hpp"
} // namespace HyperGraphAnalysis

namespace mega::reporters
{
using namespace HyperGraphAnalysis;

Report HyperGraphReporter::generate( const URL& url )
{
    using namespace std::string_literals;

    Branch branch{ { ID } };

    Database database( m_args.environment, m_args.environment.project_manifest(), true );

    HyperGraph::Graph* pGraph = database.one< HyperGraph::Graph >( m_args.environment.project_manifest() );

    const auto nonOwningRelations = pGraph->get_non_owning_relations();

    {
        Branch nonOwningBranch{ { "Non Owning Relations"s } };
        Table  nonOwning{ { "Relation ID"s, "Source"s, "Source Type ID"s, "Target"s, "Target Type ID"s } };

        std::set< HyperGraph::Relation* > uniqueRelations;
        for( const auto [ pUserLink, pRelation ] : nonOwningRelations )
        {
            if( !uniqueRelations.contains( pRelation ) )
            {
                uniqueRelations.insert( pRelation );
                nonOwning.m_rows.push_back( { Line{ pRelation->get_id() },
                                              Line{ Interface::fullTypeName( pRelation->get_source() ) },
                                              Line{ pRelation->get_source()->get_interface_id()->get_type_id() },
                                              Line{ Interface::fullTypeName( pRelation->get_target() ) },
                                              Line{ pRelation->get_target()->get_interface_id()->get_type_id() } } );
            }
        }
        nonOwningBranch.m_elements.push_back( nonOwning );
        branch.m_elements.push_back( nonOwningBranch );
    }

    const auto* pOwningRelation = pGraph->get_owning_relation();
    {
        Branch owningBranch{ { "Owners"s } };
        Table  ownersTable{ { "Link"s, "Link Type ID"s, "Object"s, "Object Type ID"s } };

        const auto owners = pOwningRelation->get_owners();
        for( auto i = owners.begin(), iEnd = owners.end(); i != iEnd; )
        {
            for( auto j = owners.upper_bound( i->first ); i != j; ++i )
            {
                Interface::UserLink*           pLink  = i->first;
                Concrete::Data::OwnershipLink* pOwned = i->second;
                ownersTable.m_rows.push_back(
                    { Line{ Interface::fullTypeName( pLink ) }, Line{ pLink->get_interface_id()->get_type_id() },
                      Line{ Concrete::fullTypeName( pOwned ) }, Line{ pOwned->get_concrete_id()->get_type_id() } } );
            }
        }

        owningBranch.m_elements.push_back( ownersTable );
        branch.m_elements.push_back( owningBranch );
    }

    {
        Branch ownedBranch{ { "Owned"s } };
        Table  ownedTable{ { "Object"s, "Object Type ID"s, "Link"s, "Link Type ID"s } };

        const auto owned = pOwningRelation->get_owned();
        for( auto i = owned.begin(), iEnd = owned.end(); i != iEnd; )
        {
            for( auto j = owned.upper_bound( i->first ); i != j; ++i )
            {
                Concrete::Data::OwnershipLink* pLink  = i->first;
                Interface::UserLink*           pOwner = i->second;
                ownedTable.m_rows.push_back(
                    { Line{ Concrete::fullTypeName( pLink ) }, Line{ pLink->get_concrete_id()->get_type_id() },
                      Line{ Interface::fullTypeName( pOwner ) }, Line{ pOwner->get_interface_id()->get_type_id() } } );
            }
        }

        ownedBranch.m_elements.emplace_back( std::move( ownedTable ) );
        branch.m_elements.emplace_back( std::move( ownedBranch ) );
    }

    {
        Branch graphBranch{ { "Graph"s } };

        Graph graph;
        {
            std::map< Concrete::Node*, U64 > nodeIDs;

            std::multimap< Concrete::Object*, Concrete::Node* > objectNodes;

            for( auto pNode : database.many< Concrete::Node >( m_args.environment.project_manifest() ) )
            {
                nodeIDs.insert( { pNode, nodeIDs.size() } );
                graph.m_nodes.emplace_back( Graph::Node{ { { Concrete::fullTypeName( pNode ) } } } );
                objectNodes.insert( { pNode->get_parent_object(), pNode } );
            }

            for( auto pEdge : database.many< Concrete::Edge >( m_args.environment.project_manifest() ) )
            {
                Graph::Edge edge{ nodeIDs[ pEdge->get_source() ], nodeIDs[ pEdge->get_target() ] };
                if( auto pInterObjectEdge = db_cast< Concrete::InterObjectEdge >( pEdge ) )
                {
                    edge.m_label = ValueVector{ Concrete::fullTypeName( pInterObjectEdge->get_counterpart() ) };
                }
                graph.m_edges.emplace_back( std::move( edge ) );
            }

            for( auto i = objectNodes.begin(), iEnd = objectNodes.end(); i != iEnd; )
            {
                auto                           pObject = i->first;
                std::vector< Graph::Node::ID > nodes;
                for( auto iNext = objectNodes.upper_bound( i->first ); i != iNext; ++i )
                {
                    nodes.push_back( nodeIDs[ i->second ] );
                }
                graph.m_subgraphs.emplace_back( Graph::Subgraph{
                    { { Concrete::fullTypeName( pObject ), pObject->get_concrete_id()->get_type_id() } }, nodes } );
            }
        }
        graphBranch.m_elements.emplace_back( std::move( graph ) );

        branch.m_elements.emplace_back( std::move( graphBranch ) );
    }

    return branch;
}
} // namespace mega::reporters
