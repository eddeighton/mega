
#include "base_task.hpp"

#include "database/model/DerivationAnalysis.hxx"
#include "database/model/HyperGraphAnalysis.hxx"
#include "database/model/HyperGraphAnalysisView.hxx"
#include "database/model/HyperGraphAnalysisRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/types/cardinality.hpp"

namespace mega
{
namespace compiler
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

    struct CalculateGraph
    {
        const mega::io::StashEnvironment& m_environment;
        CalculateGraph( const mega::io::StashEnvironment& env )
            : m_environment( env )
        {
        }

        HyperGraphAnalysis::Interface::IContext* getLinkTarget( HyperGraphAnalysis::Interface::Link* pLink ) const
        {
            const auto targets = pLink->get_link_target()->get_contexts();
            VERIFY_RTE( targets.size() == 1 );
            return targets.front();
        }

        HyperGraphAnalysis::Interface::LinkInterface*
        findLinkInterface( HyperGraphAnalysis::Interface::Link* pLink ) const
        {
            using namespace HyperGraphAnalysis;
            using namespace HyperGraphAnalysis::HyperGraph;

            // if pLink is an interface link then it IS the pSourceLinkInterface
            if ( Interface::LinkInterface* pSourceInterfaceLink
                 = dynamic_database_cast< Interface::LinkInterface >( pLink ) )
            {
                return pSourceInterfaceLink;
            }
            else
            {
                Interface::IContext* pInterfaceTarget = getLinkTarget( pLink );
                // otherwise the links target will define its interface link
                // either it targets a link
                if ( Interface::Link* pTargetLink = dynamic_database_cast< Interface::Link >( pInterfaceTarget ) )
                {
                    if ( Interface::LinkInterface* pTargetLinkInterface
                         = dynamic_database_cast< Interface::LinkInterface >( pInterfaceTarget ) )
                    {
                        // if the target is a link interface than that IS the pSourceLinkInterface
                        return pTargetLinkInterface;
                    }
                    else
                    {
                        // recurse
                        return findLinkInterface( pTargetLink );
                    }
                }
                // or it targets an object
                else if ( Interface::Object* pTargetObject
                          = dynamic_database_cast< Interface::Object >( pInterfaceTarget ) )
                {
                    THROW_RTE( "Link targetss to objects unsupported" );
                }
            }
        }

        HyperGraphAnalysis::HyperGraph::Relations* operator()( HyperGraphAnalysis::Database& database,
                                                               const mega::io::megaFilePath& sourceFilePath,
                                                               const task::DeterminantHash   interfaceHash ) const
        {
            using namespace HyperGraphAnalysis;
            using namespace HyperGraphAnalysis::HyperGraph;

            std::map< Interface::Link*, Relation* > relations;

            for ( Interface::Link* pSourceLink : database.many< Interface::Link >( sourceFilePath ) )
            {
                Interface::LinkInterface* pSourceLinkInterface = findLinkInterface( pSourceLink );
                Interface::Link*          pTargetLink
                    = dynamic_database_cast< Interface::Link >( getLinkTarget( pSourceLinkInterface ) );
                VERIFY_RTE_MSG( pTargetLink, "Invalid link target" );
                Interface::LinkInterface* pTargetLinkInterface = findLinkInterface( pTargetLink );
                Relation* pRelation = database.construct< Relation >(
                    Relation::Args{ pSourceLink, pTargetLink, pSourceLinkInterface, pTargetLinkInterface } );
                relations.insert( { pSourceLink, pRelation } );
            }

            Relations* pDependencies
                = database.construct< Relations >( Relations::Args{ sourceFilePath, interfaceHash.get(), relations } );
            return pDependencies;
        }

        /*HyperGraphAnalysis::HyperGraph::Relations*
        operator()( HyperGraphAnalysis::Database&                        database,
                    const HyperGraphAnalysisView::HyperGraph::Relations* pOldObjectGraph ) const
        {
            using namespace HyperGraphAnalysis;
            using namespace HyperGraphAnalysis::HyperGraph;

            std::multimap< HyperGraphAnalysisView::Interface::IContext*, HyperGraphAnalysisView::HyperGraph::Relation* >
                oldRelations = pOldObjectGraph->get_relations();

            std::multimap< Interface::IContext*, Relation* > relations;

            for ( const auto& [ pOldContext, pOldRelation ] : oldRelations )
            {
                Interface::IContext* pContext  = database.convert< Interface::IContext >( pOldContext );
                Relation*            pRelation = nullptr;
                if ( HyperGraphAnalysisView::HyperGraph::SingularRelation* pSingular
                     = HyperGraphAnalysisView::dynamic_database_cast<
                         HyperGraphAnalysisView::HyperGraph::SingularRelation >( pOldRelation ) )
                {
                    // pRelation = database.construct< SingularRelation >(
                    //     SingularRelation::Args{ Relation::Args{ pLink, pTarget } } );
                }
                else if ( HyperGraphAnalysisView::HyperGraph::NonSingularRelation* pNonSingular
                          = HyperGraphAnalysisView::dynamic_database_cast<
                              HyperGraphAnalysisView::HyperGraph::NonSingularRelation >( pOldRelation ) )
                {
                }
                else
                {
                    THROW_RTE( "Unknown relation type" );
                }
            }
            //
            // database.convert<typename T>(const TFrom *pFrom)

            Relations* pDependencies = database.construct< Relations >(
                Relations::Args{ pOldObjectGraph->get_source_file(), pOldObjectGraph->get_hash_code(), relations } );
            return pDependencies;
        }*/
    };

    using PathSet = std::set< mega::io::megaFilePath >;
    PathSet getSortedSourceFiles() const
    {
        PathSet sourceFiles;
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath hyperGraphCompilationFile
            = m_environment.HyperGraphAnalysis_Model( manifestFilePath );
        start( taskProgress, "Task_HyperGraph", manifestFilePath.path(), hyperGraphCompilationFile.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if ( m_environment.restore( hyperGraphCompilationFile, determinant ) )
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

        bool bReusedOldDatabase = false;
        /*if ( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
        {
            try
            {
                // attempt to load previous dependency analysis
                namespace Old = HyperGraphAnalysisView;
                namespace New = HyperGraphAnalysis;

                New::Database newDatabase( m_environment, manifestFilePath );
                {
                    // load the archived database
                    io::ArchiveEnvironment archiveEnvironment( m_environment.DatabaseArchive() );
                    Old::Database          oldDatabase( archiveEnvironment, archiveEnvironment.project_manifest() );

                    const Old::HyperGraph::Graph* pOldAnalysis
                        = oldDatabase.one< Old::HyperGraph::Graph >( manifestFilePath );
                    VERIFY_RTE( pOldAnalysis );
                    using OldObjectGraphVector              = std::vector< Old::HyperGraph::Relations* >;
                    const OldObjectGraphVector dependencies = pOldAnalysis->get_relations();
                    const PathSet              sourceFiles  = getSortedSourceFiles();

                    struct Compare
                    {
                        const mega::io::Environment& env;
                        Compare( const mega::io::Environment& env )
                            : env( env )
                        {
                        }
                        bool operator()( OldObjectGraphVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::HyperGraph::Relations* pDependencies = *i;
                            return pDependencies->get_source_file() < *j;
                        }
                        bool opposite( OldObjectGraphVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::HyperGraph::Relations* pDependencies = *i;
                            return *j < pDependencies->get_source_file();
                        }
                    } comparator( m_environment );

                    using NewDependenciesVector = std::vector< New::HyperGraph::Relations* >;
                    NewDependenciesVector newDependencies;
                    {
                        generics::matchGetUpdates(
                            dependencies.begin(), dependencies.end(), sourceFiles.begin(), sourceFiles.end(),
                            // const Compare& cmp
                            comparator,

                            // const Update& shouldUpdate
                            [ &env = m_environment, &hashCodeGenerator, &newDependencies, &newDatabase, &taskProgress ](
                                OldObjectGraphVector::const_iterator i, PathSet::const_iterator j ) -> bool
                            {
                                const Old::HyperGraph::Relations* pDependencies = *i;
                                const task::DeterminantHash       interfaceHash = hashCodeGenerator( *j );
                                if ( interfaceHash == pDependencies->get_hash_code() )
                                {
                                    // since the code is NOT modified - can re use the globs from previous result
                                    newDependencies.push_back( CalculateGraph( env )( newDatabase, pDependencies ) );

                                    // std::ostringstream os;
                                    // os << "\tPartially reusing dependencies for: " << j->path().string();
                                    // taskProgress.msg( os.str() );
                                    return false;
                                }
                                else
                                {
                                    // std::ostringstream os;
                                    // os << "\tRecalculating dependencies for: " << j->path().string();
                                    // taskProgress.msg( os.str() );
                                    return true;
                                }
                            },

                            // const Removal& rem
                            []( OldObjectGraphVector::const_iterator i )
                            {
                                // a source file has been removed - can ignor this since
                                // recreating the dependency analysis and only attempting to
                                // reuse the globs
                            },

                            // const Addition& add
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newDependencies,
                              &taskProgress ]( PathSet::const_iterator j )
                            {
                                // a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath megaFilePath  = *j;
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );
                                newDependencies.push_back(
                                    CalculateGraph( env )( newDatabase, megaFilePath, interfaceHash ) );
                                // std::ostringstream os;
                                // os << "\tAdding dependencies for: " << megaFilePath.path().string();
                                // taskProgress.msg( os.str() );
                            },

                            // const Updated& updatesNeeded
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase,
                              &newDependencies ]( OldObjectGraphVector::const_iterator i )
                            {
                                // since the code is modified - must re analyse ALL dependencies from the ground up
                                const Old::HyperGraph::Relations* pDependencies = *i;
                                const mega::io::megaFilePath      megaFilePath  = pDependencies->get_source_file();
                                const task::DeterminantHash       interfaceHash = hashCodeGenerator( megaFilePath );
                                newDependencies.push_back(
                                    CalculateGraph( env )( newDatabase, megaFilePath, interfaceHash ) );
                            } );
                    }

                    newDatabase.construct< New::HyperGraph::Graph >( New::HyperGraph::Graph::Args{ newDependencies } );
                }

                const task::FileHash fileHashCode = newDatabase.save_Model_to_temp();
                m_environment.setBuildHashCode( hyperGraphCompilationFile, fileHashCode );
                m_environment.temp_to_real( hyperGraphCompilationFile );
                m_environment.stash( hyperGraphCompilationFile, determinant );

                succeeded( taskProgress );
                bReusedOldDatabase = true;
            }
            catch ( mega::io::DatabaseVersionException& )
            {
                bReusedOldDatabase = false;
            }
        }*/

        if ( !bReusedOldDatabase )
        {
            using namespace HyperGraphAnalysis;
            using namespace HyperGraphAnalysis::HyperGraph;

            Database database( m_environment, manifestFilePath );
            {
                CalculateGraph functor( m_environment );

                std::vector< Relations* > dependencies;
                {
                    const PathSet sourceFiles = getSortedSourceFiles();
                    for ( const mega::io::megaFilePath& sourceFilePath : sourceFiles )
                    {
                        const task::DeterminantHash interfaceHash = hashCodeGenerator( sourceFilePath );
                        dependencies.push_back( functor( database, sourceFilePath, interfaceHash ) );
                    }
                    /*for ( const mega::io::megaFilePath& sourceFilePath : sourceFiles )
                    {
                        functor.generateConcreteLinkDimensions( database, sourceFilePath );
                    }*/
                }
                database.construct< Graph >( Graph::Args{ dependencies } );
            }

            const task::FileHash fileHashCode = database.save_Model_to_temp();
            m_environment.setBuildHashCode( hyperGraphCompilationFile, fileHashCode );
            m_environment.temp_to_real( hyperGraphCompilationFile );
            m_environment.stash( hyperGraphCompilationFile, determinant );

            succeeded( taskProgress );
        }
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

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath hyperGraphAnalysisCompilationFile
            = m_environment.HyperGraphAnalysis_Model( m_environment.project_manifest() );
        const mega::io::CompilationFilePath rolloutCompilationFile
            = m_environment.HyperGraphAnalysisRollout_PerSourceModel( m_sourceFilePath );
        start( taskProgress, "Task_ConcreteTypeRollout", m_sourceFilePath.path(), rolloutCompilationFile.path() );

        const task::DeterminantHash determinant
            = { m_environment.getBuildHashCode( hyperGraphAnalysisCompilationFile ) };

        if ( m_environment.restore( rolloutCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( rolloutCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace HyperGraphAnalysisRollout;

        Database database( m_environment, m_sourceFilePath );

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

} // namespace compiler
} // namespace mega
