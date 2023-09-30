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

#include "mega/common_strings.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include <unordered_set>

namespace HyperGraphAnalysis
{
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
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
    findObjectLinkTarget( HyperGraphAnalysis::Interface::LinkTrait* pLinkTrait )
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
                            // first MUST be object or interface
                            if( auto pObject = db_cast< Interface::Object >( pContext ) )
                            {
                                found.push_back( pContext );
                            }
                            else if( auto pInterface = db_cast< Interface::Abstract >( pContext ) )
                            {
                                found.push_back( pInterface );
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
        HyperGraphAnalysis::Interface::LinkTrait* pLink          = nullptr;
        HyperGraphAnalysis::Interface::IContext*  pTargetContext = nullptr;
    };
    using ObjectLinkTargets = std::vector< ObjectLinkPair >;
    HyperGraphAnalysis::HyperGraph::Graph* constructGraph( HyperGraphAnalysis::Database& database,
                                                           const ObjectLinkTargets&      links ) const
    {
        using namespace HyperGraphAnalysis;
        using namespace HyperGraphAnalysis::HyperGraph;

        using LinkParentMap = std::unordered_map< Interface::IContext*, Interface::LinkTrait* >;
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

        // now test for disjoint link target concrete sets at each parent context
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

        std::multimap< Interface::LinkTrait*, Concrete::Dimensions::OwnershipLink* > owners;
        std::multimap< Concrete::Dimensions::OwnershipLink*, Interface::LinkTrait* > owned;
        std::map< Interface::LinkTrait*, Interface::LinkTrait* >                     nonOwningLinks;

        for( const ObjectLinkPair& link : links )
        {
            if( link.pLink->get_owning() )
            {
                for( auto pConcreteTarget : link.pTargetContext->get_concrete() )
                {
                    VERIFY_RTE( pConcreteTarget->get_concrete_object().has_value() );
                    auto pOwnershipLink = pConcreteTarget->get_concrete_object().value()->get_ownership_link();
                    owners.insert( { link.pLink, pOwnershipLink } );
                    owned.insert( { pOwnershipLink, link.pLink } );
                }
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

        // construct the ownership relation
        const mega::RelationID relationID( TypeID{}, TypeID{} );
        OwningObjectRelation*  pOwningRelation = database.construct< OwningObjectRelation >(
            OwningObjectRelation::Args{ Relation::Args{ relationID }, owners, owned } );

        std::map< Interface::LinkTrait*, NonOwningObjectRelation* > nonOwningRelations;
        for( const auto& [ pLink1, pLink2 ] : nonOwningLinks )
        {
            auto bCmp        = pLink1->get_interface_id() < pLink2->get_interface_id();
            auto pLinkSource = bCmp ? pLink1 : pLink2;
            auto pLinkTarget = bCmp ? pLink2 : pLink1;

            const mega::RelationID relationID( pLinkSource->get_interface_id(), pLinkTarget->get_interface_id() );
            auto                   pRelation = database.construct< NonOwningObjectRelation >(
                NonOwningObjectRelation::Args{ Relation::Args{ relationID }, pLinkSource, pLinkTarget } );
            VERIFY_RTE( nonOwningRelations.insert( { pLinkSource, pRelation } ).second );
            VERIFY_RTE( nonOwningRelations.insert( { pLinkTarget, pRelation } ).second );
        }

        return database.construct< Graph >( Graph::Args{ pOwningRelation, nonOwningRelations } );
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

    void calculateEdges( HyperGraphAnalysis::Database& database, const HyperGraphAnalysis::HyperGraph::Graph* pGraph )
    {
        using namespace HyperGraphAnalysis;
        using namespace HyperGraphAnalysis::HyperGraph;

        auto owners             = pGraph->get_owning_relation()->get_owners();
        auto owned              = pGraph->get_owning_relation()->get_owned();
        auto nonOwningRelations = pGraph->get_non_owning_relations();

        std::vector< Concrete::Graph::Vertex* > vertices;
        for( auto sourceFile : getSortedSourceFiles() )
        {
            for( Concrete::Graph::Vertex* pVertex : database.many< Concrete::Graph::Vertex >( sourceFile ) )
            {
                vertices.push_back( pVertex );
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
                        Concrete::Graph::Edge::Args{ mega::EdgeType::eLink, pObject, pVertex } );
                }
                {
                    database.construct< Concrete::Graph::Edge >(
                        Concrete::Graph::Edge::Args{ EdgeType::eParent, pVertex, pObject } );
                }

                // find ownership relations
                std::vector< Concrete::Dimensions::Link* > owners;
                for( auto i = owned.lower_bound( pOwnershipLink ), iEnd = owned.upper_bound( pOwnershipLink );
                     i != iEnd; ++i )
                {
                    auto pInterfaceLinktrait = i->second;
                    for( auto pOwningConcreteContext : pInterfaceLinktrait->get_concrete() )
                    {
                        owners.push_back( pOwningConcreteContext );
                    }
                }
                owners = mega::make_unique_without_reorder( owners );
                const EdgeType edgeType
                    = ( owners.size() == 1 ) ? mega::EdgeType::ePolyParent : mega::EdgeType::ePolyParent;
                for( auto pOwningConcreteContext : owners )
                {
                    database.construct< Concrete::Graph::Edge >(
                        Concrete::Graph::Edge::Args{ EdgeType::ePolyParent, pOwnershipLink, pOwningConcreteContext } );
                }
            }
            else if( auto* pUserLink = db_cast< Concrete::Dimensions::UserLink >( pVertex ) )
            {
                auto* pObjectLinkTrait = db_cast< Interface::LinkTrait >( pUserLink->get_interface_link() );
                VERIFY_RTE( pObjectLinkTrait );

                {
                    database.construct< Concrete::Graph::Edge >( Concrete::Graph::Edge::Args{
                        mega::EdgeType::eLink, pUserLink->get_parent_context(), pVertex } );
                }
                {
                    database.construct< Concrete::Graph::Edge >(
                        Concrete::Graph::Edge::Args{ EdgeType::eParent, pVertex, pUserLink->get_parent_context() } );
                }

                if( pObjectLinkTrait->get_owning() )
                {
                    auto i = owners.lower_bound( pObjectLinkTrait ), iEnd = owners.upper_bound( pObjectLinkTrait );
                    const EdgeType edgeType
                        = ( std::distance( i, iEnd ) == 1 ) ? mega::EdgeType::eMono : mega::EdgeType::ePoly;
                    for( ; i != iEnd; ++i )
                    {
                        database.construct< Concrete::Graph::Edge >(
                            Concrete::Graph::Edge::Args{ edgeType, pVertex, i->second } );
                    }
                }
                else
                {
                    auto iFind = nonOwningRelations.find( pObjectLinkTrait );
                    VERIFY_RTE( iFind != nonOwningRelations.end() );
                    auto pNonOwningObjectRelation = iFind->second;

                    if( pNonOwningObjectRelation->get_source() == pObjectLinkTrait )
                    {
                        Interface::LinkTrait* pTargetObjectLink = pNonOwningObjectRelation->get_target();

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
                        Interface::LinkTrait* pSourceObjectLink = pNonOwningObjectRelation->get_source();

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
                        THROW_RTE( "LinkTrait does not match non owning relation" );
                    }
                }
            }
            else if( auto* pContextGroup = db_cast< Concrete::ContextGroup >( pVertex ) )
            {
                if( auto* pContext = db_cast< Concrete::Context >( pContextGroup ) )
                {
                    for( auto pChild : pContext->get_children() )
                    {
                        const EdgeType edgeType = ( getLocalDomainSize( pChild ) > 1 )
                                                      ? mega::EdgeType::eChildNonSingular
                                                      : mega::EdgeType::eChildSingular;
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
                    for( auto pLink : database.many< Interface::LinkTrait >( sourceFilePath ) )
                    {
                        Interface::IContext* pTarget = findObjectLinkTarget( pLink );
                        if( pLink->get_owning() )
                        {
                            for( auto pConcrete : pTarget->get_concrete() )
                            {
                                if( !db_cast< Concrete::Object >( pConcrete ) )
                                {
                                    THROW_RTE( "Owning link NOT to object: " << printLinkTraitTypePath( pLink ) );
                                }
                            }
                        }
                        linkTargets.push_back( { pLink, pTarget } );
                    }
                }
            }
            calculateEdges( database, constructGraph( database, linkTargets ) );
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

            auto pOwnershipRelation = pHyperGraph->get_owning_relation();
            auto nonOwningRelations = pHyperGraph->get_non_owning_relations();

            for( Interface::LinkTrait* pLink : database.many< Interface::LinkTrait >( m_sourceFilePath ) )
            {
                if( pLink->get_owning() )
                {
                    database.construct< Interface::LinkTrait >(
                        Interface::LinkTrait::Args{ pLink, pOwnershipRelation } );
                }
                else
                {
                    auto iFind = nonOwningRelations.find( pLink );
                    VERIFY_RTE( iFind != nonOwningRelations.end() );
                    database.construct< Interface::LinkTrait >( Interface::LinkTrait::Args{ pLink, iFind->second } );
                }
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
                std::vector< Concrete::UserDimensionContext* > linkContexts;
                collectLinkContexts( pObject, linkContexts );

                // set the concrete link associated relation
                for( auto pLinkContext : linkContexts )
                {
                    for( auto pLink : pLinkContext->get_links() )
                    {
                        if( auto pUserLink = db_cast< Concrete::Dimensions::UserLink >( pLink ) )
                        {
                            if( pUserLink->get_interface_link()->get_owning() )
                            {
                                database.construct< Concrete::Dimensions::Link >(
                                    Concrete::Dimensions::Link::Args{ pLink, pOwnershipRelation, true, false, true } );
                            }
                            else
                            {
                                auto iFind = nonOwningRelations.find( pUserLink->get_interface_link() );
                                VERIFY_RTE( iFind != nonOwningRelations.end() );
                                auto       pRelation = iFind->second;
                                const bool bSource   = pRelation->get_source() == pUserLink->get_interface_link();
                                database.construct< Concrete::Dimensions::Link >(
                                    Concrete::Dimensions::Link::Args{ pLink, pRelation, false, false, bSource } );
                            }
                        }
                        else if( auto pOwnershipLink = db_cast< Concrete::Dimensions::OwnershipLink >( pLink ) )
                        {
                            database.construct< Concrete::Dimensions::Link >(
                                Concrete::Dimensions::Link::Args{ pLink, pOwnershipRelation, false, true, false } );
                        }
                        else
                        {
                            THROW_RTE( "Unknown link type" );
                        }
                    }
                }

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
