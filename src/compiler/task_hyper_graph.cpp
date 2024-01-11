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

#include "base_task.hpp"

#include "mega/values/compilation/interface/relation_id.hpp"
#include "mega/values/compilation/cardinality.hpp"

#include "database/HyperGraphAnalysis.hxx"
#include "database/manifest.hxx"

#include "mega/common_strings.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include <unordered_set>

namespace HyperGraphAnalysis
{
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/interface_visitor.hpp"
} // namespace HyperGraphAnalysis

namespace mega::compiler
{

using namespace HyperGraphAnalysis;

class Task_HyperGraph : public BaseTask
{
    const mega::io::manifestFilePath m_manifestFilePath;

public:
    Task_HyperGraph( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
        , m_manifestFilePath( m_environment.project_manifest() )
    {
    }

    bool findOriginal( Interface::Node* pNode, Interface::UserLink* pTargetLink )
    {
        if( auto pLink = db_cast< Interface::UserLink >( pNode ) )
        {
            if( pLink == pTargetLink )
            {
                return true;
            }
        }
        for( auto pChild : pNode->get_children() )
        {
            if( findOriginal( pChild, pTargetLink ) )
            {
                return true;
            }
        }
        return false;
    }

    Interface::UserLink* findCounterpart( Interface::Node* pNode, Interface::UserLink* pTargetLink )
    {
        if( auto pLink = db_cast< Interface::UserLink >( pNode ) )
        {
            auto pTypePath = db_cast< Parser::Type::Absolute >( pLink->get_link()->get_link_type()->get_type_path() );
            VERIFY_RTE( pTypePath );
            if( findOriginal( pTypePath->get_type(), pTargetLink ) )
            {
                return pLink;
            }
        }

        for( auto pChild : pNode->get_children() )
        {
            if( auto pResult = findCounterpart( pChild, pTargetLink ) )
            {
                return pResult;
            }
        }
        return nullptr;
    }

    void buildRelations( Database& database )
    {
        // for all non-owning links they must uniquely pair up into binary relations
        using NonOwningRelationMap = std::map< Interface::UserLink*, HyperGraph::NonOwningRelation* >;
        NonOwningRelationMap nonOwningRelationsMap;

        using UserLinkPairs = std::map< Interface::UserLink*, Interface::UserLink* >;
        UserLinkPairs userLinkPairs, userLinkPairsOpposites;

        std::multimap< Interface::OwningLink*, Concrete::Data::OwnershipLink* > owners;
        std::multimap< Concrete::Data::OwnershipLink*, Interface::OwningLink* > owned;

        for( auto pUserLink : database.many< Interface::UserLink >( m_manifestFilePath ) )
        {
            auto pTypePath
                = db_cast< Parser::Type::Absolute >( pUserLink->get_link()->get_link_type()->get_type_path() );
            VERIFY_RTE( pTypePath );
            auto pTargetType = pTypePath->get_type();

            database.construct< Interface::UserLink >( Interface::UserLink::Args{ pUserLink, pTargetType } );

            if( pUserLink->get_link()->get_owning() )
            {
                std::vector< Concrete::Data::OwnershipLink* > ownershipLinks;
                {
                    for( auto pConcreteTarget : pTargetType->get_inheritors() )
                    {
                        ownershipLinks.push_back( pConcreteTarget->get_parent_object()->get_ownership() );
                    }
                }
                auto pOwningLink = database.construct< Interface::OwningLink >(
                    Interface::OwningLink::Args{ pUserLink, ownershipLinks } );
                for( auto p : ownershipLinks )
                {
                    owners.insert( { pOwningLink, p } );
                    owned.insert( { p, pOwningLink } );
                }
            }
            else
            {
                auto pUserLinkCounterPart = findCounterpart( pTargetType, pUserLink );

                if( !userLinkPairs.contains( pUserLink ) && !userLinkPairsOpposites.contains( pUserLink ) )
                {
                    VERIFY_RTE_MSG(
                        pUserLinkCounterPart,
                        "Failed to locate link counter part for link: " << Interface::fullTypeName( pUserLink ) );
                    userLinkPairs.insert( { pUserLink, pUserLinkCounterPart } );
                    userLinkPairsOpposites.insert( { pUserLinkCounterPart, pUserLink } );
                }

                database.construct< Interface::NonOwningLink >( Interface::NonOwningLink::Args{ pUserLink } );
            }
        }

        for( auto pOwnershipLink : database.many< Concrete::Data::OwnershipLink >( m_manifestFilePath ) )
        {
            std::vector< Interface::OwningLink* > owners;
            for( auto i = owned.lower_bound( pOwnershipLink ), iEnd = owned.upper_bound( pOwnershipLink ); i != iEnd;
                 ++i )
            {
                owners.push_back( i->second );
            }
            database.construct< Concrete::Data::OwnershipLink >(
                Concrete::Data::OwnershipLink::Args{ pOwnershipLink, owners } );
        }

        for( const auto& pair : userLinkPairs )
        {
            auto pNonOwningLinkSource = db_cast< Interface::NonOwningLink >( pair.first );
            auto pNonOwningLinkTarget = db_cast< Interface::NonOwningLink >( pair.second );
            VERIFY_RTE( pNonOwningLinkSource );
            VERIFY_RTE( pNonOwningLinkTarget );
            pNonOwningLinkSource->set_counterpart( pNonOwningLinkTarget );
            pNonOwningLinkTarget->set_counterpart( pNonOwningLinkSource );

            // ensure relation ordering such that source less than target
            std::pair< Interface::UserLink*, Interface::UserLink* > relationPair = pair;
            {
                if( relationPair.second->get_interface_id()->get_type_id()
                    < relationPair.first->get_interface_id()->get_type_id() )
                {
                    auto temp           = relationPair;
                    relationPair.first  = temp.second;
                    relationPair.second = temp.first;
                }
            }
            auto pRelation = database.construct< HyperGraph::NonOwningRelation >(
                HyperGraph::NonOwningRelation::Args{ HyperGraph::Relation::Args{ interface::RelationID(
                                                         relationPair.first->get_interface_id()->get_type_id(),
                                                         relationPair.second->get_interface_id()->get_type_id() ) },
                                                     relationPair.first, relationPair.second } );
            VERIFY_RTE_MSG( nonOwningRelationsMap.insert( { relationPair.first, pRelation } ).second,
                            "Non owning link does not have unique relation to other link: "
                                << Interface::fullTypeName( relationPair.first ) );
            VERIFY_RTE_MSG( nonOwningRelationsMap.insert( { relationPair.second, pRelation } ).second,
                            "Non owning link does not have unique relation to other link: "
                                << Interface::fullTypeName( relationPair.second ) );
        }

        HyperGraph::OwningRelation* pOwningRelation
            = database.construct< HyperGraph::OwningRelation >( HyperGraph::OwningRelation::Args{
                HyperGraph::Relation::Args{ interface::OWNERSHIP_RELATION_ID }, owners, owned } );

        database.construct< HyperGraph::Graph >( HyperGraph::Graph::Args{ pOwningRelation, nonOwningRelationsMap } );
    }

    struct Visitor : public Interface::Visitor
    {
        Concrete::Node* pConcrete;

        Database& database;

        std::vector< Concrete::Edge* >& edges;

        Visitor( Database& database, HyperGraph::Graph* pGraph, std::vector< Concrete::Edge* >& edges )
            : database( database )
            , edges( edges )
        {
        }

        Concrete::Edge* createEdge( EdgeType edgeType, Concrete::Node* pSource, Concrete::Node* pTarget ) const
        {
            auto pEdge = database.construct< Concrete::Edge >( Concrete::Edge::Args{ edgeType, pSource, pTarget } );
            edges.push_back( pEdge );
            return pEdge;
        }

        virtual bool visit( Interface::UserDimension* pNode ) const
        {
            auto pConcreteParent = db_cast< Concrete::Node >( pConcrete->get_parent() );
            VERIFY_RTE( pConcreteParent );
            createEdge( EdgeType::eDim, pConcreteParent, pConcrete );
            createEdge( EdgeType::eParent, pConcrete, pConcreteParent );
            return true;
        }

        virtual bool visit( Interface::UserAlias* pNode ) const
        {
            auto pConcreteParent = db_cast< Concrete::Node >( pConcrete->get_parent() );
            VERIFY_RTE( pConcreteParent );
            createEdge( EdgeType::eDim, pConcreteParent, pConcrete );
            createEdge( EdgeType::eParent, pConcrete, pConcreteParent );
            return true;
        }

        virtual bool visit( Interface::UserUsing* pNode ) const
        {
            auto pConcreteParent = db_cast< Concrete::Node >( pConcrete->get_parent() );
            VERIFY_RTE( pConcreteParent );
            createEdge( EdgeType::eDim, pConcreteParent, pConcrete );
            createEdge( EdgeType::eParent, pConcrete, pConcreteParent );
            return true;
        }

        void findCounterPart( std::vector< Concrete::Node* >& counterParts, Concrete::Node* pTarget,
                              std::vector< Concrete::Data::Link* >& results ) const
        {
            auto iFind = std::find( counterParts.begin(), counterParts.end(), pTarget );
            if( iFind != counterParts.end() )
            {
                auto pCounterPartLink = db_cast< Concrete::Data::Link >( *iFind );
                VERIFY_RTE( pCounterPartLink );
                results.push_back( pCounterPartLink );
            }
            for( auto pChild : pTarget->get_children() )
            {
                findCounterPart( counterParts, pChild, results );
            }
        };

        virtual bool visit( Interface::NonOwningLink* pNode ) const
        {
            {
                auto pConcreteParent = db_cast< Concrete::Node >( pConcrete->get_parent() );
                VERIFY_RTE( pConcreteParent );

                createEdge( EdgeType::eLink, pConcreteParent, pConcrete );
                createEdge( EdgeType::eParent, pConcrete, pConcreteParent );
            }

            auto pCounterpart = pNode->get_counterpart();
            VERIFY_RTE( pCounterpart );

            auto pInterfaceTarget   = pNode->get_type();
            auto targets            = pInterfaceTarget->get_inheritors();
            auto counterPartTargets = pCounterpart->get_inheritors();

            // map all targets to associated counter parts
            std::map< Concrete::Node*, Concrete::Data::Link* > targetToCounterPart;
            {
                for( auto pTarget : targets )
                {
                    std::vector< Concrete::Data::Link* > results;
                    findCounterPart( counterPartTargets, pTarget, results );
                    VERIFY_RTE_MSG( results.size() == 1,
                                    "Ambiguous concrete link counterparts found for link: "
                                        << Concrete::fullTypeName( pConcrete ) );
                    targetToCounterPart.insert( { pTarget, results.front() } );
                }
            }

            for( auto pTarget : targets )
            {
                auto pEdge        = createEdge( EdgeType::eInterObjectNonOwner, pConcrete, pTarget );
                auto pCounterPart = targetToCounterPart[ pTarget ];
                VERIFY_RTE( pCounterPart );
                database.construct< Concrete::InterObjectEdge >( Concrete::InterObjectEdge::Args{
                    pEdge, pCounterPart, pNode->get_link()->get_cardinality(), targets.size() != 1 } );
            }
            return true;
        }

        virtual bool visit( Interface::OwningLink* pNode ) const
        {
            {
                auto pConcreteParent = db_cast< Concrete::Node >( pConcrete->get_parent() );
                VERIFY_RTE( pConcreteParent );

                createEdge( EdgeType::eLink, pConcreteParent, pConcrete );
                createEdge( EdgeType::eParent, pConcrete, pConcreteParent );
            }

            auto targets = pNode->get_type()->get_inheritors();
            U64  total   = targets.size();

            for( auto pConcreteTarget : targets )
            {
                auto pCounterPart = pConcreteTarget->get_parent_object()->get_ownership();
                auto pEdge        = createEdge( EdgeType::eInterObjectOwner, pConcrete, pConcreteTarget );
                database.construct< Concrete::InterObjectEdge >( Concrete::InterObjectEdge::Args{
                    pEdge, pCounterPart, pNode->get_link()->get_cardinality(), total != 1 } );
            }
            return true;
        }

        virtual bool visit( Interface::IContext* pNode ) const
        {
            for( auto pChild : pConcrete->get_children() )
            {
                if( pChild->get_node_opt().has_value() )
                {
                    if( db_cast< Interface::IContext >( pChild->get_node_opt().value() ) )
                    {
                        if( pNode->get_singular() )
                        {
                            createEdge( EdgeType::eChildSingular, pConcrete, pChild );
                            createEdge( EdgeType::eParent, pChild, pConcrete );
                        }
                        else
                        {
                            createEdge( EdgeType::eChildNonSingular, pConcrete, pChild );
                            createEdge( EdgeType::eParent, pChild, pConcrete );
                        }
                    }
                }
                else if( auto pActivationBitSet = db_cast< Concrete::Data::ActivationBitSet >( pChild ) )
                {
                    // ignor
                }
                else if( auto pOwnershipLink = db_cast< Concrete::Data::OwnershipLink >( pChild ) )
                {
                    // ignor
                }
                else
                {
                    THROW_RTE( "Unexpected missing interface node in concrete node" );
                }
            }
            return true;
        }
    };

    void buildGraph( Database& database, Concrete::Node* pNode, Visitor& visitor )
    {
        visitor.pConcrete = pNode;

        if( pNode->get_node_opt().has_value() )
        {
            bool bResult = Interface::visit( visitor, pNode->get_node_opt().value() );
            VERIFY_RTE( bResult );
        }
        else if( auto pActivationBitSet = db_cast< Concrete::Data::ActivationBitSet >( pNode ) )
        {
            auto pConcreteParent = db_cast< Concrete::Node >( pNode->get_parent() );
            VERIFY_RTE( pConcreteParent );
            visitor.createEdge( EdgeType::eDim, pConcreteParent, pActivationBitSet );
            visitor.createEdge( EdgeType::eParent, pActivationBitSet, pConcreteParent );
        }
        else if( auto pOwnershipLink = db_cast< Concrete::Data::OwnershipLink >( pNode ) )
        {
            auto pConcreteParent = db_cast< Concrete::Node >( pNode->get_parent() );
            VERIFY_RTE( pConcreteParent );

            visitor.createEdge( EdgeType::eLink, pConcreteParent, pOwnershipLink );
            visitor.createEdge( EdgeType::eParent, pOwnershipLink, pConcreteParent );

            U64 total = 0;
            {
                for( auto pOwner : pOwnershipLink->get_owners() )
                {
                    for( auto pNode : pOwner->get_inheritors() )
                    {
                        ++total;
                    }
                }
            }

            // construct inter-object edges from the ownership to to ALL owning objects
            for( auto pOwner : pOwnershipLink->get_owners() )
            {
                for( auto pCounterNode : pOwner->get_inheritors() )
                {
                    auto pCounterPart = db_cast< Concrete::Data::UserLink >( pCounterNode );
                    VERIFY_RTE( pCounterPart );
                    auto pTarget = db_cast< Concrete::Node >( pCounterPart->get_parent() );
                    auto pEdge   = visitor.createEdge( EdgeType::eInterObjectParent, pOwnershipLink, pTarget );
                    database.construct< Concrete::InterObjectEdge >(
                        Concrete::InterObjectEdge::Args{ pEdge, pCounterPart, CardinalityRange{}, total != 1 } );
                }
            }
        }
        else
        {
            THROW_RTE( "Unknown concrete node type" );
        }

        for( auto pChild : pNode->get_children() )
        {
            buildGraph( database, pChild, visitor );
        }
    }

    void buildGraph( Database& database )
    {
        using namespace HyperGraphAnalysis::Interface;
        std::vector< Concrete::Edge* > edges;
        Visitor visitor( database, database.one< HyperGraph::Graph >( m_manifestFilePath ), edges );
        for( auto pObject : database.many< Concrete::Object >( m_manifestFilePath ) )
        {
            buildGraph( database, pObject, visitor );
        }

        struct Edges
        {
            std::vector< Concrete::Edge* > outEdges;
            std::vector< Concrete::Edge* > inEdges;
        };
        std::map< Concrete::Node*, Edges > edgeMap;

        for( auto pEdge : edges )
        {
            edgeMap[ pEdge->get_source() ].outEdges.push_back( pEdge );
            edgeMap[ pEdge->get_target() ].inEdges.push_back( pEdge );
        }

        for( auto pNode : database.many< Concrete::Node >( m_manifestFilePath ) )
        {
            const auto& edges = edgeMap[ pNode ];
            database.construct< Concrete::Node >( Concrete::Node::Args{ pNode, edges.outEdges, edges.inEdges } );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath hyperGraphCompilationFile
            = m_environment.HyperGraphAnalysis_Model( m_manifestFilePath );
        start( taskProgress, "Task_HyperGraph", m_manifestFilePath.path(), hyperGraphCompilationFile.path() );

        task::DeterminantHash determinant(
            m_toolChain.toolChainHash,
            m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( m_manifestFilePath ) ) );

        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if( m_environment.restore( hyperGraphCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( hyperGraphCompilationFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_manifestFilePath );

        buildRelations( database );
        buildGraph( database );

        const task::FileHash fileHashCode = database.save_Model_to_temp();
        m_environment.setBuildHashCode( hyperGraphCompilationFile, fileHashCode );
        m_environment.temp_to_real( hyperGraphCompilationFile );
        m_environment.stash( hyperGraphCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_HyperGraph( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_HyperGraph >( taskArguments );
}

} // namespace mega::compiler
