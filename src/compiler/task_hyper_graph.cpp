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

#include "mega/relation_id.hpp"

#include "database/model/InheritanceAnalysis.hxx"
#include "database/model/HyperGraphAnalysis.hxx"
#include "database/model/HyperGraphAnalysisView.hxx"
#include "database/model/HyperGraphAnalysisRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/types/cardinality.hpp"

#include <unordered_set>

namespace HyperGraphAnalysis
{
#include "compiler/printer.hpp"
#include "compiler/interface.hpp"
#include "compiler/derivation.hpp"
} // namespace HyperGraphAnalysis

namespace mega::compiler
{

class Task_HyperGraph : public BaseTask
{
    const mega::io::Manifest m_manifest;

public:
    Task_HyperGraph( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
        : BaseTask( taskArguments )
        , m_manifest( m_environment, manifestFilePath )
    {
    }

    using GraphType = std::map< HyperGraphAnalysis::Interface::LinkTrait*, HyperGraphAnalysis::HyperGraph::Relation* >;

    HyperGraphAnalysis::Interface::IContext*
    findObjectLinkTarget( HyperGraphAnalysis::Interface::ObjectLinkTrait* pLinkTrait )
    {
        using namespace HyperGraphAnalysis;
        using namespace HyperGraphAnalysis::HyperGraph;

        std::vector< Interface::IContext* > variantResults;
        for( auto pTypePathVariant : pLinkTrait->get_tuple() )
        {
            for( Interface::TypePath* pTypePath : pTypePathVariant->get_sequence() )
            {
                std::vector< Interface::IContext* > candidates;

                for( auto pSymbol : pTypePath->get_types() )
                {
                    std::vector< Interface::IContext* > found;
                    const bool                          bFirst = candidates.empty();
                    for( auto pContext : pSymbol->get_contexts() )
                    {
                        if( bFirst )
                        {
                            // first MUST be object
                            if( auto pObject = db_cast< Interface::Object >( pContext ) )
                            {
                                found.push_back( pContext );
                            }
                        }
                        else
                        {
                            // next must be child or previous
                            for( auto pIter = pContext; pIter;
                                 pIter      = db_cast< Interface::IContext >( pIter->get_parent() ) )
                            {
                                if( std::find( candidates.begin(), candidates.end(), pIter ) != candidates.end() )
                                {
                                    found.push_back( pContext );
                                    break;
                                }
                            }
                        }
                    }
                    VERIFY_RTE_MSG( !found.empty(),
                                    "Failed to resolve object link type: " << printLinkTraitTypePath( pLinkTrait ) );
                    candidates.swap( found );
                }

                VERIFY_RTE_MSG( !candidates.empty(),
                                "Failed to resolve object link type: " << printLinkTraitTypePath( pLinkTrait ) );
                VERIFY_RTE_MSG( 1 == candidates.size(),
                                "Failed to disambiguate object link type: " << printLinkTraitTypePath( pLinkTrait ) );
                variantResults.push_back( candidates.front() );
            }
        }
        VERIFY_RTE_MSG( 1 == variantResults.size(),
                        "Failed to disambiguate variant object link type: " << printLinkTraitTypePath( pLinkTrait ) );

        return variantResults.front();
    }

    struct ObjectLinkPair
    {
        HyperGraphAnalysis::Interface::ObjectLinkTrait* pLink          = nullptr;
        HyperGraphAnalysis::Interface::IContext*        pTargetContext = nullptr;
    };
    using ObjectLinkTargets = std::vector< ObjectLinkPair >;
    using RelationsMap
        = std::map< HyperGraphAnalysis::Interface::ObjectLinkTrait*, HyperGraphAnalysis::HyperGraph::Relation* >;
    RelationsMap calculateRelations( HyperGraphAnalysis::Database& database, const ObjectLinkTargets& links ) const
    {
        using namespace HyperGraphAnalysis;
        using namespace HyperGraphAnalysis::HyperGraph;

        using LinkParentMap = std::unordered_map< Interface::IContext*, Interface::ObjectLinkTrait* >;
        LinkParentMap linkParentMap;
        {
            for( const ObjectLinkPair& link : links )
            {
                linkParentMap.insert( { link.pLink->get_parent(), link.pLink } );
            }
        }

        // build a graph of all object link relationships in BOTH directions
        std::multimap< Interface::IContext*, Interface::IContext* > linkMap;
        {
            for( const ObjectLinkPair& link : links )
            {
                if( !link.pLink->get_owning() )
                {
                    auto iFind = linkParentMap.find( link.pTargetContext );
                    VERIFY_RTE_MSG( iFind != linkParentMap.end(),
                                    "Failed to locate object link counterpart for non-owning link: "
                                        << printLinkTraitTypePath( link.pLink ) );

                    linkMap.insert( { link.pLink->get_parent(), link.pTargetContext } );
                    linkMap.insert( { link.pTargetContext, link.pLink->get_parent() } );
                }
                else
                {
                    linkMap.insert( { link.pLink->get_parent(), link.pTargetContext } );
                    linkMap.insert( { link.pTargetContext, link.pLink->get_parent() } );
                }
            }
        }

        // now test for disjoint link target concrete sets at each context
        for( auto i = linkMap.begin(), iEnd = linkMap.end(); i != iEnd; )
        {
            std::vector< Concrete::Context* > concrete;
            for( auto iNext = linkMap.upper_bound( i->first ); i != iNext; ++i )
            {
                auto pTargetContext = i->second;
                auto targetConcrete = pTargetContext->get_concrete();
                std::copy( targetConcrete.begin(), targetConcrete.end(), std::back_inserter( concrete ) );
                std::sort( concrete.begin(), concrete.end() );
                const bool bUnique = std::unique( concrete.begin(), concrete.end() ) == concrete.end();
                if( !bUnique )
                {
                    Interface::IContext* pContext = i->first;
                    THROW_RTE( "Non unique target concrete sets in objects links from: " << printIContextFullType(
                                   pContext ) << " to " << printIContextFullType( pTargetContext ) );
                }
            }
        }

        std::map< Interface::IContext*, std::vector< Interface::ObjectLinkTrait* > > owningLinks;
        std::map< Interface::ObjectLinkTrait*, Interface::ObjectLinkTrait* >         nonOwningLinks;

        for( const ObjectLinkPair& link : links )
        {
            if( link.pLink->get_owning() )
            {
                owningLinks[ link.pTargetContext ].push_back( link.pLink );
            }
            else
            {
                auto iFind = nonOwningLinks.find( link.pLink );
                if( iFind == nonOwningLinks.end() )
                {
                    auto iFindOther = linkParentMap.find( link.pTargetContext );
                    VERIFY_RTE( iFindOther != linkParentMap.end() );
                    auto pOtherLink = iFindOther->second;

                    nonOwningLinks.insert( { link.pLink, pOtherLink } );
                    nonOwningLinks.insert( { pOtherLink, link.pLink } );
                }
            }
        }

        std::map< Interface::ObjectLinkTrait*, Relation* > relations;

        // construct the relations
        for( const auto& [ pContext, owners ] : owningLinks )
        {
            {
                // check unique
                auto temp = owners;
                std::sort( temp.begin(), temp.end() );
                VERIFY_RTE( std::unique( temp.begin(), temp.end() ) == temp.end() );
            }
            const mega::RelationID relationID( pContext->get_interface_id(), TypeID{} );
            // clang-format off
            auto pRelation =
                database.construct< OwningObjectRelation >(
                    OwningObjectRelation::Args
                    {
                        ObjectRelation::Args
                        {
                            Relation::Args
                            {
                                relationID
                            }
                        },
                        owners,
                        pContext
                    }
                );
            // clang-format on
            for( auto pLink : owners )
            {
                VERIFY_RTE( relations.insert( { pLink, pRelation } ).second );
            }
        }

        for( const auto& [ pLink1, pLink2 ] : nonOwningLinks )
        {
            auto bCmp        = pLink1->get_interface_id() < pLink2->get_interface_id();
            auto pLinkSource = bCmp ? pLink1 : pLink2;
            auto pLinkTarget = bCmp ? pLink2 : pLink1;

            const mega::RelationID relationID( pLinkSource->get_interface_id(), pLinkTarget->get_interface_id() );
            // clang-format off
            auto pRelation =
                database.construct< NonOwningObjectRelation >(
                    NonOwningObjectRelation::Args
                    {
                        ObjectRelation::Args
                        {
                            Relation::Args
                            {
                                relationID
                            }
                        },
                        pLinkSource,
                        pLinkTarget
                    }
                );
            // clang-format on
            VERIFY_RTE( relations.insert( { pLinkSource, pRelation } ).second );
            VERIFY_RTE( relations.insert( { pLinkTarget, pRelation } ).second );
        }

        return relations;
    }

    using PathSet = std::set< mega::io::megaFilePath >;
    PathSet getSortedSourceFiles() const
    {
        PathSet sourceFiles;
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }

    void calculateEdges( HyperGraphAnalysis::Database& database, const RelationsMap& relations )
    {
        using namespace HyperGraphAnalysis;
        using namespace HyperGraphAnalysis::HyperGraph;

        std::vector< Concrete::Graph::Vertex* > vertices;
        std::vector< Concrete::Graph::Vertex* > objects;
        for( auto sourceFile : getSortedSourceFiles() )
        {
            for( Concrete::Graph::Vertex* pVertex : database.many< Concrete::Graph::Vertex >( sourceFile ) )
            {
                vertices.push_back( pVertex );
                if( db_cast< Concrete::Object >( pVertex ) )
                {
                    objects.push_back( pVertex );
                }
            }
        }

        for( auto pVertex : vertices )
        {
            if( auto* pUserDim = db_cast< Concrete::Dimensions::User >( pVertex ) )
            {
                {
                    database.construct< Concrete::Graph::Edge >(
                        Concrete::Graph::Edge::Args{ mega::EdgeType::eDim, pUserDim->get_parent_context(), pVertex } );
                }
                {
                    database.construct< Concrete::Graph::Edge >(
                        Concrete::Graph::Edge::Args{ EdgeType::eParent, pVertex, pUserDim->get_parent_context() } );
                }
            }
            else if( auto* pOwnershipLink = db_cast< Concrete::Dimensions::OwnershipLink >( pVertex ) )
            {
                auto pObject = db_cast< Concrete::Object >( pOwnershipLink->get_parent_context() );
                VERIFY_RTE( pObject );

                // create edge from object to the ownership link
                {
                    database.construct< Concrete::Graph::Edge >(
                        Concrete::Graph::Edge::Args{ mega::EdgeType::eObjectLink, pObject, pVertex } );
                }
                {
                    database.construct< Concrete::Graph::Edge >(
                        Concrete::Graph::Edge::Args{ EdgeType::eParent, pVertex, pObject } );
                }

                // find ownership relations
                std::vector< OwningObjectRelation* > owningRelations;
                {
                    std::set< Relation* > uniqueRelations;
                    for( auto [ _, pRelation ] : relations )
                    {
                        if( !uniqueRelations.contains( pRelation ) )
                        {
                            uniqueRelations.insert( pRelation );
                            if( auto pOwningRelation = db_cast< OwningObjectRelation >( pRelation ) )
                            {
                                for( auto pOwnedConcreteContext : pOwningRelation->get_owned()->get_concrete() )
                                {
                                    auto pOwnedConcreteObjectOpt = pOwnedConcreteContext->get_concrete_object();
                                    if( pOwnedConcreteObjectOpt.has_value() )
                                    {
                                        if( pObject == pOwnedConcreteObjectOpt.value() )
                                        {
                                            owningRelations.push_back( pOwningRelation );
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // create edges describing the ownership
                for( auto pOwningObjectRelation : owningRelations )
                {
                    for( auto pObjectLinkTrait : pOwningObjectRelation->get_owners() )
                    {
                        for( auto pOwningConcreteContext : pObjectLinkTrait->get_concrete() )
                        {
                            database.construct< Concrete::Graph::Edge >( Concrete::Graph::Edge::Args{
                                EdgeType::ePolyParent, pOwnershipLink, pOwningConcreteContext } );
                        }
                    }
                }
            }
            else if( auto* pUserLink = db_cast< Concrete::Dimensions::UserLink >( pVertex ) )
            {
                auto pLinkTrait = pUserLink->get_interface_link();
                if( auto* pObjectLinkTrait = db_cast< Interface::ObjectLinkTrait >( pLinkTrait ) )
                {
                    {
                        database.construct< Concrete::Graph::Edge >( Concrete::Graph::Edge::Args{
                            mega::EdgeType::eObjectLink, pUserLink->get_parent_context(), pVertex } );
                    }
                    {
                        database.construct< Concrete::Graph::Edge >( Concrete::Graph::Edge::Args{
                            EdgeType::eParent, pVertex, pUserLink->get_parent_context() } );
                    }

                    auto iFind = relations.find( pObjectLinkTrait );
                    VERIFY_RTE( iFind != relations.end() );
                    Relation* pRelation = iFind->second;

                    if( auto* pOwningObjectRelation = db_cast< OwningObjectRelation >( pRelation ) )
                    {
                        auto           concreteTargets = pOwningObjectRelation->get_owned()->get_concrete();
                        const EdgeType edgeType
                            = ( concreteTargets.size() == 1 ) ? mega::EdgeType::eMono : mega::EdgeType::ePoly;
                        for( auto pConcreteTarget : concreteTargets )
                        {
                            database.construct< Concrete::Graph::Edge >(
                                Concrete::Graph::Edge::Args{ edgeType, pVertex, pConcreteTarget } );
                        }
                    }
                    else if( auto* pNonOwningObjectRelation = db_cast< NonOwningObjectRelation >( pRelation ) )
                    {
                        if( pNonOwningObjectRelation->get_source() == pObjectLinkTrait )
                        {
                            Interface::ObjectLinkTrait* pTargetObjectLink = pNonOwningObjectRelation->get_target();

                            auto           concreteTargets = pTargetObjectLink->get_concrete();
                            const EdgeType edgeType
                                = ( concreteTargets.size() == 1 ) ? mega::EdgeType::eMono : mega::EdgeType::ePoly;
                            for( auto pConcreteTarget : concreteTargets )
                            {
                                database.construct< Concrete::Graph::Edge >(
                                    Concrete::Graph::Edge::Args{ edgeType, pVertex, pConcreteTarget } );
                            }
                        }
                        else if( pNonOwningObjectRelation->get_target() == pObjectLinkTrait )
                        {
                            Interface::ObjectLinkTrait* pSourceObjectLink = pNonOwningObjectRelation->get_source();

                            auto           concreteTargets = pSourceObjectLink->get_concrete();
                            const EdgeType edgeType
                                = ( concreteTargets.size() == 1 ) ? mega::EdgeType::eMono : mega::EdgeType::ePoly;
                            for( auto pConcreteTarget : concreteTargets )
                            {
                                database.construct< Concrete::Graph::Edge >(
                                    Concrete::Graph::Edge::Args{ edgeType, pVertex, pConcreteTarget } );
                            }
                        }
                        else
                        {
                            THROW_RTE( "ObjectLinkTrait does not match non owning relation" );
                        }
                    }
                    else
                    {
                        THROW_RTE( "Unknown object type" );
                    }
                }
                else if( auto* pComponentLinkTrait = db_cast< Interface::ComponentLinkTrait >( pLinkTrait ) )
                {
                }
                else
                {
                    THROW_RTE( "Unknown link trait type" );
                }
            }
            else if( auto* pContextGroup = db_cast< Concrete::ContextGroup >( pVertex ) )
            {
                if( auto* pContext = db_cast< Concrete::Context >( pContextGroup ) )
                {
                    for( auto pChild : pContext->get_children() )
                    {
                        const EdgeType edgeType = ( getLocalDomainSize( pChild ) > 1 )
                                                      ? mega::EdgeType::eChildSingular
                                                      : mega::EdgeType::eChildNonSingular;
                        {
                            database.construct< Concrete::Graph::Edge >(
                                Concrete::Graph::Edge::Args{ edgeType, pContext, pChild } );
                        }
                        {
                            database.construct< Concrete::Graph::Edge >(
                                Concrete::Graph::Edge::Args{ EdgeType::eParent, pChild, pContext } );
                        }
                    }
                }
                else
                {
                    // do nothing
                }
            }
            else
            {
                THROW_RTE( "Unknown vertex type" );
            }
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath hyperGraphCompilationFile
            = m_environment.HyperGraphAnalysis_Model( manifestFilePath );
        start( taskProgress, "Task_HyperGraph", manifestFilePath.path(), hyperGraphCompilationFile.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
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

        struct InterfaceHashCodeGenerator
        {
            const mega::io::StashEnvironment& env;
            task::DeterminantHash             toolChainHash;
            InterfaceHashCodeGenerator( const mega::io::StashEnvironment& env, task::DeterminantHash toolChainHash )
                : env( env )
                , toolChainHash( toolChainHash )
            {
            }
            inline task::DeterminantHash operator()( const mega::io::megaFilePath& megaFilePath ) const
            {
                return task::DeterminantHash(
                    { toolChainHash, env.getBuildHashCode( env.ParserStage_AST( megaFilePath ) ) } );
            }

        } hashCodeGenerator( m_environment, m_toolChain.toolChainHash );

        using namespace HyperGraphAnalysis;
        using namespace HyperGraphAnalysis::HyperGraph;

        Database database( m_environment, manifestFilePath );
        {
            // collect ALL links in program
            ObjectLinkTargets linkTargets;
            {
                for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
                {
                    for( auto pLink : database.many< Interface::ObjectLinkTrait >( sourceFilePath ) )
                    {
                        Interface::IContext* pTarget = findObjectLinkTarget( pLink );
                        linkTargets.push_back( { pLink, pTarget } );
                    }
                }
            }
            auto relations = calculateRelations( database, linkTargets );
            database.construct< HyperGraph::Graph >( HyperGraph::Graph::Args{ relations } );
            calculateEdges( database, relations );
        }

        const task::FileHash fileHashCode = database.save_Model_to_temp();
        m_environment.setBuildHashCode( hyperGraphCompilationFile, fileHashCode );
        m_environment.temp_to_real( hyperGraphCompilationFile );
        m_environment.stash( hyperGraphCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_HyperGraph( const TaskArguments&              taskArguments,
                                      const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_HyperGraph >( taskArguments, manifestFilePath );
}

class Task_HyperGraphRollout : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_HyperGraphRollout( const TaskArguments& taskArguments, const mega::io::megaFilePath& megaSourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( megaSourceFilePath )
    {
    }

    void collectLinkContexts( HyperGraphAnalysisRollout::Concrete::Context*                              pContext,
                              std::vector< HyperGraphAnalysisRollout::Concrete::UserDimensionContext* >& linkContexts )
    {
        using namespace HyperGraphAnalysisRollout;

        if( auto pDimensionContext = db_cast< Concrete::UserDimensionContext >( pContext ) )
        {
            if( !pDimensionContext->get_links().empty() )
            {
                linkContexts.push_back( pDimensionContext );
            }
        }
        for( auto pChildContext : pContext->get_children() )
        {
            collectLinkContexts( pChildContext, linkContexts );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath hyperGraphAnalysisCompilationFile
            = m_environment.HyperGraphAnalysis_Model( m_environment.project_manifest() );
        const mega::io::CompilationFilePath concreteTreeCompilationFile
            = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        const mega::io::CompilationFilePath rolloutCompilationFile
            = m_environment.HyperGraphAnalysisRollout_PerSourceModel( m_sourceFilePath );
        start( taskProgress, "Task_HyperGraphRollout", m_sourceFilePath.path(), rolloutCompilationFile.path() );

        const task::DeterminantHash determinant
            = { m_environment.getBuildHashCode( concreteTreeCompilationFile ),
                m_environment.getBuildHashCode( hyperGraphAnalysisCompilationFile ) };

        if( m_environment.restore( rolloutCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( rolloutCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace HyperGraphAnalysisRollout;

        Database database( m_environment, m_sourceFilePath );

        {
            HyperGraph::Graph* pHyperGraph = database.one< HyperGraph::Graph >( m_environment.project_manifest() );
            auto               relations   = pHyperGraph->get_relations();
            for( Interface::ObjectLinkTrait* pLink : database.many< Interface::ObjectLinkTrait >( m_sourceFilePath ) )
            {
                auto iFind = relations.find( pLink );
                VERIFY_RTE( iFind != relations.end() );
                database.construct< Interface::ObjectLinkTrait >(
                    Interface::ObjectLinkTrait::Args{ pLink, iFind->second } );
            }

            auto edges = database.many< Concrete::Graph::Edge >( m_environment.project_manifest() );

            for( Concrete::Graph::Vertex* pVertex : database.many< Concrete::Graph::Vertex >( m_sourceFilePath ) )
            {
                std::vector< Concrete::Graph::Edge* > inEdges, outEdges;
                for( auto pEdge : edges )
                {
                    if( pEdge->get_source() == pVertex )
                    {
                        outEdges.push_back( pEdge );
                    }
                    else if( pEdge->get_target() == pVertex )
                    {
                        inEdges.push_back( pEdge );
                    }
                }

                database.construct< Concrete::Graph::Vertex >(
                    Concrete::Graph::Vertex::Args{ pVertex, outEdges, inEdges } );
            }

            for( Concrete::Object* pObject : database.many< Concrete::Object >( m_sourceFilePath ) )
            {
                // std::vector< Concrete::Link* > allObjectLinks;
                std::vector< HyperGraphAnalysisRollout::Concrete::UserDimensionContext* > linkContexts;
                collectLinkContexts( pObject, linkContexts );

                // determine the owning links for object
                std::vector< Concrete::Dimensions::Link* > links;
                {
                    /*for( Concrete::Link* pLink : allObjectLinks )
                    {
                        // NOTE: pLink->get_link_interface()->get_relation(); will not work since being set above in
                        // this stage
                        auto iFind = relations.find( pLink->get_link() );
                        VERIFY_RTE( iFind != relations.end() );
                        HyperGraph::Relation* pRelation = iFind->second;
                        const mega::Ownership ownership = pRelation->get_ownership();

                        if( pRelation->get_source_interface() == pLink->get_link_interface() )
                        {
                            if( mega::Ownership::eOwnSource == ownership.get() )
                            {
                                owningLinks.push_back( pLink );
                            }
                        }
                        else if( pRelation->get_target_interface() == pLink->get_link_interface() )
                        {
                            if( mega::Ownership::eOwnTarget == ownership.get() )
                            {
                                owningLinks.push_back( pLink );
                            }
                        }
                        else
                        {
                            THROW_RTE( "Relation error" );
                        }
                    }*/
                }

                /*if( pObject->get_interface()->get_identifier() == ROOT_TYPE_NAME )
                {
                    VERIFY_RTE_MSG( owningLinks.empty(), "Hypergraph error Root has owning links" );
                }
                else
                {
                    VERIFY_RTE_MSG( !owningLinks.empty(),
                                    "Hypergraph error non-Root object: " << pObject->get_interface()->get_identifier()
                                                                         << " has NO owning links" );
                }*/

                database.construct< Concrete::Object >( Concrete::Object::Args{ pObject, linkContexts } );
            }
        }

        const task::FileHash fileHashCode = database.save_PerSourceModel_to_temp();
        m_environment.setBuildHashCode( rolloutCompilationFile, fileHashCode );
        m_environment.temp_to_real( rolloutCompilationFile );
        m_environment.stash( rolloutCompilationFile, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_HyperGraphRollout( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& megaSourceFilePath )
{
    return std::make_unique< Task_HyperGraphRollout >( taskArguments, megaSourceFilePath );
}

} // namespace mega::compiler
