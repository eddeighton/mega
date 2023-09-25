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
#include "database/model/AliasAnalysis.hxx"
#include "database/model/AliasAnalysisView.hxx"
#include "database/model/AliasAnalysisRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/types/cardinality.hpp"

#include <unordered_set>

namespace AliasAnalysis
{
#include "compiler/printer.hpp"
#include "compiler/interface.hpp"
#include "compiler/derivation.hpp"
} // namespace AliasAnalysis

namespace mega::compiler
{

class Task_Alias : public BaseTask
{
    const mega::io::Manifest m_manifest;

public:
    Task_Alias( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
        : BaseTask( taskArguments )
        , m_manifest( m_environment, manifestFilePath )
    {
    }

    using GraphType = std::map< AliasAnalysis::Interface::LinkTrait*, AliasAnalysis::HyperGraph::Relation* >;


    // struct ObjectLinkPair
    // {
    //     AliasAnalysis::Interface::ObjectLinkTrait* pLink          = nullptr;
    //     AliasAnalysis::Interface::IContext*        pTargetContext = nullptr;
    // };
    // using ObjectLinkTargets = std::vector< ObjectLinkPair >;
    using RelationsMap
        = std::map< AliasAnalysis::Interface::ObjectLinkTrait*, AliasAnalysis::HyperGraph::Relation* >;

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

    using ObjectLinkContextsMap = std::multimap< AliasAnalysis::Concrete::Graph::Vertex*,
                                                 std::pair< AliasAnalysis::Concrete::Graph::Vertex*, bool > >;
    using EdgeMap
        = std::multimap< AliasAnalysis::Concrete::Graph::Vertex*, AliasAnalysis::Concrete::Graph::Edge* >;

    void recurseObjectTree( AliasAnalysis::Concrete::Graph::Vertex* pVertex, const EdgeMap& edges,
                            std::vector< AliasAnalysis::Concrete::Graph::Vertex* >& vertices )
    {
        vertices.push_back( pVertex );

        for( auto i = edges.lower_bound( pVertex ), iEnd = edges.upper_bound( pVertex ); i != iEnd; ++i )
        {
            auto pEdge = i->second;
            switch( pEdge->get_type().get() )
            {
                case EdgeType::eChildSingular:
                case EdgeType::eChildNonSingular:
                case EdgeType::eObjectLink:
                case EdgeType::eDim:
                case EdgeType::eComponentLink:
                {
                    recurseObjectTree( pEdge->get_target(), edges, vertices );
                }
                break;
                case EdgeType::eMono:
                case EdgeType::ePoly:
                case EdgeType::eParent:
                case EdgeType::ePart:
                case EdgeType::eObjectParent:
                    break;
                case EdgeType::TOTAL_EDGE_TYPES:
                default:
                    THROW_RTE( "Unknown edge type" );
                    break;
            }
        }
    }

    void calculateLinks( AliasAnalysis::Database& database, const EdgeMap& edges, ObjectLinkContextsMap& linkContexts )
    {
        using namespace AliasAnalysis;
        using namespace AliasAnalysis::HyperGraph;

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

        // solve object links
        for( auto pObject : objects )
        {
            // collect ALL vertices WITHIN the object
            std::vector< Concrete::Graph::Vertex* > vertices;
            recurseObjectTree( pObject, edges, vertices );

            // find ALL object link contexts within the object
            std::vector< Concrete::Graph::Vertex* > links;
            for( auto pVertex : vertices )
            {
                for( auto i = edges.lower_bound( pVertex ), iEnd = edges.upper_bound( pVertex ); i != iEnd; ++i )
                {
                    auto pEdge = i->second;
                    if( pEdge->get_type().get() == EdgeType::eObjectLink )
                    {
                        links.push_back( pVertex );
                        break;
                    }
                }
            }

            // construct the link context map which maps ALL vertices to
            // their associated link vertices within their associated object
            for( auto pVertex : vertices )
            {
                for( auto pLink : links )
                {
                    // object links are NOT deriving
                    linkContexts.insert( { pVertex, { pLink, false } } );
                }
            }
        }
    }

    void solveComponentLinks( AliasAnalysis::Database& database, const RelationsMap& relations,
                              const EdgeMap& edges, const ObjectLinkContextsMap& linkContexts )
    {
        using namespace AliasAnalysis;
        using namespace AliasAnalysis::HyperGraph;

        std::vector< Interface::ComponentLinkTrait* > componentLinks;
        {
            for( auto sourceFile : getSortedSourceFiles() )
            {
                for( Interface::ComponentLinkTrait* pComponentLink :
                     database.many< Interface::ComponentLinkTrait >( sourceFile ) )
                {
                    componentLinks.push_back( pComponentLink );
                }
            }
        }

        for( auto pComponentLink : componentLinks )
        {
            // attempt to solve the component link type derivation

            Derivation::VertexVariant context;
            for( auto pConcrete : pComponentLink->get_concrete() )
            {
                context.push_back( pConcrete );
            }

            std::optional< Derivation::VertexVariantVector > pathOpt;
            for( auto pTypePathVariant : pComponentLink->get_tuple() )
            {
                Derivation::VertexVariantVector path;
                for( auto pTypePath : pTypePathVariant->get_sequence() )
                {
                    for( auto pType : pTypePath->get_types() )
                    {
                        Derivation::VertexVariant vertices;
                        for( auto pContext : pType->get_contexts() )
                        {
                            for( auto pConcrete : pContext->get_concrete() )
                            {
                                vertices.push_back( pConcrete );
                            }
                        }
                        for( auto pDim : pType->get_dimensions() )
                        {
                            for( auto pConcrete : pDim->get_concrete() )
                            {
                                vertices.push_back( pConcrete );
                            }
                        }
                        for( auto pLink : pType->get_links() )
                        {
                            for( auto pConcrete : pLink->get_concrete() )
                            {
                                vertices.push_back( pConcrete );
                            }
                        }
                        path.push_back( vertices );
                    }
                }
                VERIFY_RTE_MSG( !pathOpt.has_value(), "Component link has multiple type paths" );
                pathOpt = path;
            }
            VERIFY_RTE_MSG( pathOpt.has_value(), "Component link has no type path" );

            struct ObjectLinkDerivation
            {
                const RelationsMap&          relations;
                const EdgeMap&               edges;
                const ObjectLinkContextsMap& linkContexts;

                // the link context MAY contain MULTIPLE links.  Each individual link is either deriving or not.
                // The Link context is considered to be deriving if ANY of its links are deriving
                std::vector< Concrete::Graph::Vertex* > enumerateLinkContexts( Concrete::Graph::Vertex* pVertex,
                                                                               bool bDerivingOnly ) const
                {
                    std::vector< Concrete::Graph::Vertex* > results;
                    for( auto i = linkContexts.lower_bound( pVertex ), iEnd = linkContexts.upper_bound( pVertex );
                         i != iEnd; ++i )
                    {
                        if( !bDerivingOnly || i->second.second )
                        {
                            results.push_back( i->second.first );
                        }
                    }
                    return results;
                }

                // enumerate the dimension links within the parent vertex
                using LinkDimension = std::pair< Concrete::Graph::Vertex*, Concrete::Graph::Edge* >;
                std::vector< LinkDimension > enumerateLinks( Concrete::Graph::Vertex* pParentVertex ) const
                {
                    std::vector< LinkDimension > results;
                    for( auto i = edges.lower_bound( pParentVertex ), iEnd = edges.upper_bound( pParentVertex );
                         i != iEnd; ++i )
                    {
                        auto pEdge = i->second;
                        if( pEdge->get_type().get() == EdgeType::eObjectLink )
                        {
                            results.emplace_back( pEdge->get_target(), pEdge );
                        }
                    }
                    return results;
                }

                // enumerate actual polymorphic branches of a given link
                std::vector< Concrete::Graph::Edge* > enumerateLink( Concrete::Graph::Vertex* pLink ) const
                {
                    std::vector< Concrete::Graph::Edge* > results;
                    for( auto i = edges.lower_bound( pLink ), iEnd = edges.upper_bound( pLink ); i != iEnd; ++i )
                    {
                        auto pEdge = i->second;
                        switch( pEdge->get_type().get() )
                        {
                            case EdgeType::eMono:
                            case EdgeType::ePoly:
                            case EdgeType::eObjectParent:
                            {
                                results.push_back( pEdge );
                            }
                            break;
                            case EdgeType::eObjectLink:
                            case EdgeType::eParent:
                            case EdgeType::eChildSingular:
                            case EdgeType::eChildNonSingular:
                            case EdgeType::ePart:
                            case EdgeType::eDim:
                            case EdgeType::eComponentLink:
                                break;
                            case EdgeType::TOTAL_EDGE_TYPES:
                            default:
                                THROW_RTE( "Unknown edge type" );
                                break;
                        }
                    }

                    return results;
                }

                using EdgeVector = std::vector< Concrete::Graph::Edge* >;
                Concrete::Graph::Vertex* pathToObjectRoot( Concrete::Graph::Vertex* pVertex, EdgeVector& path ) const
                {
                    while( !db_cast< Concrete::Object >( pVertex ) )
                    {
                        bool bFound = false;
                        for( auto i = edges.lower_bound( pVertex ), iEnd = edges.upper_bound( pVertex ); i != iEnd;
                             ++i )
                        {
                            auto pEdge = i->second;
                            if( pEdge->get_type().get() == EdgeType::eParent )
                            {
                                path.push_back( pEdge );
                                pVertex = pEdge->get_target();
                                bFound  = true;
                                break;
                            }
                        }
                        if( !bFound )
                        {
                            THROW_RTE( "Failed to find path to object root from vertex" );
                            return nullptr;
                        }
                    }
                    return pVertex;
                }

                bool invertObjectRootPath( const EdgeVector& path, EdgeVector& descendingPath ) const
                {
                    for( auto pEdge : path )
                    {
                        bool bFound = false;
                        for( auto i    = edges.lower_bound( pEdge->get_target() ),
                                  iEnd = edges.upper_bound( pEdge->get_target() );
                             i != iEnd;
                             ++i )
                        {
                            auto pInverseEdge = i->second;
                            if( pEdge->get_target() == pInverseEdge->get_source() )
                            {
                                switch( pInverseEdge->get_type().get() )
                                {
                                    case EdgeType::eChildSingular:
                                    case EdgeType::eObjectLink:
                                    case EdgeType::eDim:
                                    {
                                        descendingPath.push_back( pInverseEdge );
                                        bFound = true;
                                        break;
                                    }
                                    case EdgeType::eParent:
                                    case EdgeType::eChildNonSingular:
                                    case EdgeType::ePart:
                                    case EdgeType::eObjectParent:
                                    case EdgeType::eComponentLink:
                                    case EdgeType::eMono:
                                    case EdgeType::ePoly:
                                    case EdgeType::TOTAL_EDGE_TYPES:
                                        break;
                                }
                            }
                        }
                        if( !bFound )
                        {
                            return false;
                        }
                    }
                    std::reverse( descendingPath.begin(), descendingPath.end() );
                    return true;
                }

                bool commonRootDerivation( Concrete::Graph::Vertex* pSource, Concrete::Graph::Vertex* pTarget,
                                           std::vector< Concrete::Graph::Edge* >& edges ) const
                {
                    EdgeVector sourcePath, targetPath;
                    auto       pSourceObject = pathToObjectRoot( pSource, sourcePath );
                    auto       pTargetObject = pathToObjectRoot( pTarget, targetPath );

                    // if not the same object then fail
                    if( pSourceObject != pTargetObject )
                        return false;

                    // while both paths contain edges then if the edge is the same there is a lower common root
                    while( !sourcePath.empty() && !targetPath.empty() && ( sourcePath.back() == targetPath.back() ) )
                    {
                        sourcePath.pop_back();
                        targetPath.pop_back();
                    }

                    std::copy( sourcePath.begin(), sourcePath.end(), std::back_inserter( edges ) );
                    EdgeVector descendingPath;
                    if( invertObjectRootPath( targetPath, descendingPath ) )
                    {
                        std::copy( targetPath.begin(), targetPath.end(), std::back_inserter( edges ) );
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }

            } objectLinkDerivationPolicy{ relations, edges, linkContexts };

            const Derivation::Spec spec{ context, pathOpt.value() };
            auto [ result, pRoot ] = Derivation::solve( spec, objectLinkDerivationPolicy );
            switch( result )
            {
                case AliasAnalysis::Derivation::eSuccess:
                {
                    // translate the derivation result into the actual component link relation
                    // TODO.
                }
                break;
                case AliasAnalysis::Derivation::eFailure:
                {
                    THROW_RTE( "Component Link Failed: Failed to derive component link type for: "
                               << printIContextFullType( pComponentLink->get_parent() )
                               << " of: " << printLinkTraitTypePath( pComponentLink ) );
                }
                break;
                case AliasAnalysis::Derivation::eAmbiguous:
                {
                    THROW_RTE( "Component Link Ambiguous: Failed to derive component link type for: "
                               << printIContextFullType( pComponentLink->get_parent() )
                               << " of: " << printLinkTraitTypePath( pComponentLink ) );
                }
                break;
            }
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath aliasCompilationFile
            = m_environment.AliasAnalysis_Model( manifestFilePath );
        start( taskProgress, "Task_Alias", manifestFilePath.path(), aliasCompilationFile.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if( m_environment.restore( aliasCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( aliasCompilationFile );
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

        using namespace AliasAnalysis;
        using namespace AliasAnalysis::HyperGraph;

        Database database( m_environment, manifestFilePath );
        {
            // collect ALL links in program
            /*ObjectLinkTargets linkTargets;
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
            auto relations = calculateRelations( database, linkTargets );*/
            // database.construct< HyperGraph::Graph >( HyperGraph::Graph::Args{ relations } );
            /*ObjectLinkContextsMap linkContexts;
            EdgeMap               edges;
            calculateEdges( database, relations, edges, linkContexts );
            solveComponentLinks( database, relations, edges, linkContexts );*/
        }

        const task::FileHash fileHashCode = database.save_Model_to_temp();
        m_environment.setBuildHashCode( aliasCompilationFile, fileHashCode );
        m_environment.temp_to_real( aliasCompilationFile );
        m_environment.stash( aliasCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Alias( const TaskArguments&              taskArguments,
                                      const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_Alias >( taskArguments, manifestFilePath );
}

class Task_AliasRollout : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_AliasRollout( const TaskArguments& taskArguments, const mega::io::megaFilePath& megaSourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( megaSourceFilePath )
    {
    }
    /*
        void collectLinks( AliasAnalysisRollout::Concrete::Context*              pContext,
                           std::vector< AliasAnalysisRollout::Concrete::Link* >& links )
        {
            using namespace AliasAnalysisRollout;
            if( Concrete::Link* pLink = db_cast< Concrete::Link >( pContext ) )
            {
                links.push_back( pLink );
            }
            for( auto pChildContext : pContext->get_children() )
            {
                collectLinks( pChildContext, links );
            }
        }
    */
    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath aliasAnalysisCompilationFile
            = m_environment.AliasAnalysis_Model( m_environment.project_manifest() );
        const mega::io::CompilationFilePath concreteTreeCompilationFile
            = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        const mega::io::CompilationFilePath rolloutCompilationFile
            = m_environment.AliasAnalysisRollout_PerSourceModel( m_sourceFilePath );
        start( taskProgress, "Task_AliasRollout", m_sourceFilePath.path(), rolloutCompilationFile.path() );

        const task::DeterminantHash determinant
            = { m_environment.getBuildHashCode( concreteTreeCompilationFile ),
                m_environment.getBuildHashCode( aliasAnalysisCompilationFile ) };

        if( m_environment.restore( rolloutCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( rolloutCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace AliasAnalysisRollout;

        Database database( m_environment, m_sourceFilePath );

        {
            HyperGraph::Graph* pHyperGraph = database.one< HyperGraph::Graph >( m_environment.project_manifest() );
            auto               relations   = pHyperGraph->get_relations();
            /*for( Interface::ObjectLinkTrait* pLink : database.many< Interface::ObjectLinkTrait >( m_sourceFilePath ) )
            {
                auto iFind = relations.find( pLink );
                VERIFY_RTE( iFind != relations.end() );
                database.construct< Interface::ObjectLinkTrait >(
                    Interface::ObjectLinkTrait::Args{ pLink, iFind->second } );
            }

            for( Concrete::Graph::Vertex* pVertex : database.many< Concrete::Graph::Vertex >( m_sourceFilePath ) )
            {
                database.construct< Concrete::Graph::Vertex >(
                    Concrete::Graph::Vertex::Args{ pVertex, {}, {}, {}, {} } );
            }*/

            /*for( Concrete::Object* pObject : database.many< Concrete::Object >( m_sourceFilePath ) )
            {
                //std::vector< Concrete::Link* > allObjectLinks;
                //collectLinks( pObject, allObjectLinks );

                // determine the owning links for object
                std::vector< Concrete::Link* > owningLinks;
                {
                    for( Concrete::Link* pLink : allObjectLinks )
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
                    }
                }

                if( pObject->get_interface()->get_identifier() == ROOT_TYPE_NAME )
                {
                    VERIFY_RTE_MSG( owningLinks.empty(), "Hypergraph error Root has owning links" );
                }
                else
                {
                    VERIFY_RTE_MSG( !owningLinks.empty(),
                                    "Hypergraph error non-Root object: " << pObject->get_interface()->get_identifier()
                                                                         << " has NO owning links" );
                }

                database.construct< Concrete::Object >(
                    Concrete::Object::Args{ pObject, allObjectLinks, owningLinks } );
            }*/
        }

        const task::FileHash fileHashCode = database.save_PerSourceModel_to_temp();
        m_environment.setBuildHashCode( rolloutCompilationFile, fileHashCode );
        m_environment.temp_to_real( rolloutCompilationFile );
        m_environment.stash( rolloutCompilationFile, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_AliasRollout( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& megaSourceFilePath )
{
    return std::make_unique< Task_AliasRollout >( taskArguments, megaSourceFilePath );
}

} // namespace mega::compiler
