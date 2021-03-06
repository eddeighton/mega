
#include "base_task.hpp"

#include "database/model/DependencyAnalysis.hxx"
#include "database/model/DependencyAnalysisView.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"

#include "database/types/sources.hpp"
#include "utilities/glob.hpp"

namespace mega
{
namespace compiler
{

class Task_DependencyAnalysis : public BaseTask
{
    const mega::io::Manifest m_manifest;

public:
    Task_DependencyAnalysis( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
        : BaseTask( taskArguments )
        , m_manifest( m_environment, manifestFilePath )
    {
    }

    using GlobVector = std::vector< mega::utilities::Glob >;
    using PathSet    = std::set< mega::io::megaFilePath >;

    struct CalculateDependencies
    {
        const mega::io::StashEnvironment& m_environment;
        CalculateDependencies( const mega::io::StashEnvironment& env )
            : m_environment( env )
        {
        }

        void collectDependencies( DependencyAnalysis::Parser::ContextDef* pContextDef,
                                  GlobVector&                             dependencyGlobs ) const
        {
            using namespace DependencyAnalysis;

            for ( Parser::Dependency* pDependency : pContextDef->get_dependencies() )
            {
                VERIFY_PARSER( !pDependency->get_str().empty(), "Empty dependency", pContextDef->get_id() );
                boost::filesystem::path sourceFilePath = pContextDef->get_id()->get_source_file();
                VERIFY_RTE( boost::filesystem::exists( sourceFilePath ) );
                VERIFY_RTE( sourceFilePath.has_parent_path() );
                dependencyGlobs.push_back(
                    mega::utilities::Glob{ sourceFilePath.parent_path(), pDependency->get_str(), pContextDef } );
            }

            for ( Parser::ContextDef* pContext : pContextDef->get_children() )
            {
                collectDependencies( pContext, dependencyGlobs );
            }
        }

        DependencyAnalysis::Dependencies::SourceFileDependencies*
        operator()( DependencyAnalysis::Database& database,
                    const mega::io::megaFilePath& sourceFilePath,
                    const task::DeterminantHash   interfaceHash,
                    const PathSet&                sourceFiles ) const
        {
            using namespace DependencyAnalysis;
            using namespace DependencyAnalysis::Dependencies;

            GlobVector       dependencyGlobs;
            Interface::Root* pRoot = database.one< Interface::Root >( sourceFilePath );
            collectDependencies( pRoot->get_root()->get_ast(), dependencyGlobs );

            std::vector< Glob* >            globs;
            mega::utilities::FilePathVector matchedFilePaths;
            for ( const mega::utilities::Glob& glob : dependencyGlobs )
            {
                try
                {
                    mega::utilities::resolveGlob( glob, m_environment.srcDir(), matchedFilePaths );
                }
                catch ( mega::utilities::GlobException& ex )
                {
                    VERIFY_PARSER( false, "Dependency error: " << ex.what(),
                                   reinterpret_cast< Parser::ContextDef* >( glob.pDiagnostic )->get_id() );
                }
                catch ( boost::filesystem::filesystem_error& ex )
                {
                    VERIFY_PARSER( false, "Dependency error: " << ex.what(),
                                   reinterpret_cast< Parser::ContextDef* >( glob.pDiagnostic )->get_id() );
                }
                globs.push_back( database.construct< Glob >( Glob::Args{ glob.source_file, glob.glob } ) );
            }

            mega::utilities::FilePathVector resolution;
            for ( const boost::filesystem::path& filePath : matchedFilePaths )
            {
                if ( sourceFiles.count( m_environment.megaFilePath_fromPath( filePath ) ) )
                    resolution.push_back( filePath );
            }

            SourceFileDependencies* pDependencies = database.construct< SourceFileDependencies >(
                SourceFileDependencies::Args{ sourceFilePath, interfaceHash.get(), globs, resolution } );

            return pDependencies;
        }

        DependencyAnalysis::Dependencies::SourceFileDependencies*
        operator()( DependencyAnalysis::Database&                                       database,
                    const DependencyAnalysisView::Dependencies::SourceFileDependencies* pOldDependencies,
                    const PathSet&                                                      sourceFiles ) const
        {
            using namespace DependencyAnalysis;
            using namespace DependencyAnalysis::Dependencies;

            std::vector< Glob* >            globs;
            mega::utilities::FilePathVector matchedFilePaths;

            for ( DependencyAnalysisView::Dependencies::Glob* pOldGlob : pOldDependencies->get_globs() )
            {
                const mega::utilities::Glob glob{ pOldGlob->get_location(), pOldGlob->get_glob(), nullptr };
                try
                {
                    mega::utilities::resolveGlob( glob, m_environment.srcDir(), matchedFilePaths );
                }
                catch ( mega::utilities::GlobException& ex )
                {
                    VERIFY_PARSER( false, "Dependency error: " << ex.what(),
                                   reinterpret_cast< Parser::ContextDef* >( glob.pDiagnostic )->get_id() );
                }
                catch ( boost::filesystem::filesystem_error& ex )
                {
                    VERIFY_PARSER( false, "Dependency error: " << ex.what(),
                                   reinterpret_cast< Parser::ContextDef* >( glob.pDiagnostic )->get_id() );
                }
                globs.push_back( database.construct< Glob >( Glob::Args{ glob.source_file, glob.glob } ) );
            }

            mega::utilities::FilePathVector resolution;
            for ( const boost::filesystem::path& filePath : matchedFilePaths )
            {
                if ( sourceFiles.count( m_environment.megaFilePath_fromPath( filePath ) ) )
                    resolution.push_back( filePath );
            }

            SourceFileDependencies* pDependencies
                = database.construct< SourceFileDependencies >( SourceFileDependencies::Args{
                    pOldDependencies->get_source_file(), pOldDependencies->get_hash_code(), globs, resolution } );

            return pDependencies;
        }
    };

    PathSet getNewSortedSourceFiles() const
    {
        PathSet sourceFiles;
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }

    struct MegaFileDependencies
    {
        using Vector    = std::vector< mega::io::megaFilePath >;
        using Ptr       = std::shared_ptr< MegaFileDependencies >;
        using VectorPtr = std::vector< Ptr >;
        using SetPtr    = std::set< Ptr >;
        mega::io::megaFilePath m_file;
        Vector                 m_initial;
        VectorPtr              m_transitive;
        SetPtr                 m_set;
    };
    using MegaFileDependencyMap = std::map< mega::io::megaFilePath, MegaFileDependencies::Ptr >;

    void solveTransitive( MegaFileDependencies::Ptr                                 pDependencies,
                          std::set< mega::io::megaFilePath >&                       unique,
                          DependencyAnalysis::Dependencies::TransitiveDependencies* pTransitive )
    {
        for ( MegaFileDependencies::Ptr pDep : pDependencies->m_transitive )
        {
            solveTransitive( pDep, unique, pTransitive );
        }
        if ( !pDependencies->m_file.path().empty() )
        {
            if ( unique.count( pDependencies->m_file ) == 0 )
            {
                pTransitive->push_back_mega_source_files( pDependencies->m_file );
                unique.insert( pDependencies->m_file );
            }
        }
    }

    void calculateTransitiveDependencies(
        DependencyAnalysis::Database&                                                   database,
        const std::vector< DependencyAnalysis::Dependencies::SourceFileDependencies* >& dependencies )
    {
        using namespace DependencyAnalysis;
        using namespace DependencyAnalysis::Dependencies;

        MegaFileDependencyMap megaFileDependencies;

        using CPPFileDependencyMap = std::map< mega::io::cppFilePath, MegaFileDependencies::Ptr >;
        CPPFileDependencyMap cppFileDependencies;

        // collect all initial component wide dependencies first
        std::vector< Components::Component* > components
            = database.many< Components::Component >( m_environment.project_manifest() );
        for ( Components::Component* pComponent : components )
        {
            const MegaFileDependencies::Vector dependencies = pComponent->get_dependencies();
            {
                std::set< mega::io::megaFilePath > depSet;
                for ( const mega::io::megaFilePath& megaFile : dependencies )
                {
                    depSet.insert( megaFile );
                }
                VERIFY_RTE( depSet.size() == dependencies.size() );
            }

            for ( const mega::io::megaFilePath& megaFile : pComponent->get_mega_source_files() )
            {
                MegaFileDependencies::Ptr pDep( new MegaFileDependencies );
                pDep->m_file    = megaFile;
                pDep->m_initial = dependencies;
                megaFileDependencies.insert( std::make_pair( megaFile, pDep ) );
            }
            for ( const mega::io::cppFilePath cppFile : pComponent->get_cpp_source_files() )
            {
                MegaFileDependencies::Ptr pDep( new MegaFileDependencies );
                pDep->m_initial = dependencies;
                cppFileDependencies.insert( std::make_pair( cppFile, pDep ) );
            }
        }

        // collect all initial source level dependencies
        for ( SourceFileDependencies* pSourceFile : dependencies )
        {
            MegaFileDependencyMap::iterator iFind = megaFileDependencies.find( pSourceFile->get_source_file() );
            VERIFY_RTE( iFind != megaFileDependencies.end() );
            for ( const boost::filesystem::path& megaFile : pSourceFile->get_resolution() )
            {
                iFind->second->m_initial.push_back( m_environment.megaFilePath_fromPath( megaFile ) );
            }
        }

        for ( auto& [ filePath, pDependencies ] : megaFileDependencies )
        {
            for ( const mega::io::megaFilePath& megaFile : pDependencies->m_initial )
            {
                MegaFileDependencyMap::iterator iFind = megaFileDependencies.find( megaFile );
                if ( pDependencies->m_set.count( iFind->second ) == 0 )
                {
                    pDependencies->m_transitive.push_back( iFind->second );
                    pDependencies->m_set.insert( iFind->second );
                }
            }
        }

        for ( auto& [ filePath, pDependencies ] : cppFileDependencies )
        {
            for ( const mega::io::megaFilePath& megaFile : pDependencies->m_initial )
            {
                MegaFileDependencyMap::iterator iFind = megaFileDependencies.find( megaFile );
                if ( pDependencies->m_set.count( iFind->second ) == 0 )
                {
                    pDependencies->m_transitive.push_back( iFind->second );
                    pDependencies->m_set.insert( iFind->second );
                }
            }
        }

        std::map< mega::io::megaFilePath, TransitiveDependencies* > megaFileTransitiveMap;
        std::map< mega::io::cppFilePath, TransitiveDependencies* >  cppFileTransitiveMap;

        for ( auto& [ filePath, pDependencies ] : megaFileDependencies )
        {
            TransitiveDependencies* pTransitive
                = database.construct< TransitiveDependencies >( TransitiveDependencies::Args{ {} } );
            std::set< mega::io::megaFilePath > unique{ filePath };
            solveTransitive( pDependencies, unique, pTransitive );
            megaFileTransitiveMap.insert( std::make_pair( filePath, pTransitive ) );
        }

        for ( auto& [ filePath, pDependencies ] : cppFileDependencies )
        {
            TransitiveDependencies* pTransitive
                = database.construct< TransitiveDependencies >( TransitiveDependencies::Args{ {} } );
            std::set< mega::io::megaFilePath > unique;
            solveTransitive( pDependencies, unique, pTransitive );
            cppFileTransitiveMap.insert( std::make_pair( filePath, pTransitive ) );
        }

        database.construct< Analysis >( Analysis::Args{ dependencies, megaFileTransitiveMap, cppFileTransitiveMap } );
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath dependencyCompilationFilePath
            = m_environment.DependencyAnalysis_DPGraph( manifestFilePath );
        start( taskProgress, "Task_DependencyAnalysis", manifestFilePath.path(), dependencyCompilationFilePath.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if ( m_environment.restore( dependencyCompilationFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( dependencyCompilationFilePath );
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

        //
        bool bReusedOldDatabase = false;
        if ( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
        {
            try
            {
                // attempt to load previous dependency analysis
                namespace Old = DependencyAnalysisView;
                namespace New = DependencyAnalysis;

                New::Database newDatabase( m_environment, manifestFilePath );
                {
                    // load the archived database
                    io::ArchiveEnvironment archiveEnvironment( m_environment.DatabaseArchive() );
                    Old::Database          oldDatabase( archiveEnvironment, archiveEnvironment.project_manifest() );

                    const Old::Dependencies::Analysis* pOldAnalysis
                        = oldDatabase.one< Old::Dependencies::Analysis >( manifestFilePath );
                    VERIFY_RTE( pOldAnalysis );
                    using OldDependenciesVector = std::vector< Old::Dependencies::SourceFileDependencies* >;
                    const OldDependenciesVector dependencies   = pOldAnalysis->get_objects();
                    const PathSet               newSourceFiles = getNewSortedSourceFiles();

                    struct Compare
                    {
                        const mega::io::Environment& env;
                        Compare( const mega::io::Environment& env )
                            : env( env )
                        {
                        }
                        bool operator()( OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::Dependencies::SourceFileDependencies* pDependencies = *i;
                            return pDependencies->get_source_file() < *j;
                        }
                        bool opposite( OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::Dependencies::SourceFileDependencies* pDependencies = *i;
                            return *j < pDependencies->get_source_file();
                        }
                    } comparator( m_environment );

                    using NewDependenciesVector = std::vector< New::Dependencies::SourceFileDependencies* >;
                    NewDependenciesVector newDependencies;
                    {
                        generics::matchGetUpdates(
                            dependencies.begin(), dependencies.end(), newSourceFiles.begin(), newSourceFiles.end(),
                            // const Compare& cmp
                            comparator,

                            // const Update& shouldUpdate
                            [ &env = m_environment, &hashCodeGenerator, &newDependencies, &newDatabase, &newSourceFiles,
                              &taskProgress ](
                                OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) -> bool
                            {
                                const Old::Dependencies::SourceFileDependencies* pDependencies = *i;
                                const task::DeterminantHash interfaceHash = hashCodeGenerator( *j );
                                if ( interfaceHash == pDependencies->get_hash_code() )
                                {
                                    // since the code is NOT modified - can re use the globs from previous result
                                    newDependencies.push_back(
                                        CalculateDependencies( env )( newDatabase, pDependencies, newSourceFiles ) );

                                    std::ostringstream os;
                                    os << "CACHE Dependencies reused: " << j->path().string();
                                    taskProgress.onProgress( os.str() );
                                    return false;
                                }
                                else
                                {
                                    // std::ostringstream os;
                                    // os << "SUCCESS Dependencies recalculated : " << j->path().string();
                                    // taskProgress.onProgress( os.str() );
                                    return true;
                                }
                            },

                            // const Removal& rem
                            []( OldDependenciesVector::const_iterator i )
                            {
                                // a source file has been removed - can ignor this since
                                // recreating the dependency analysis and only attempting to
                                // reuse the globs
                            },

                            // const Addition& add
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newDependencies, &newSourceFiles,
                              &taskProgress ]( PathSet::const_iterator j )
                            {
                                // a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath megaFilePath  = *j;
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );
                                newDependencies.push_back( CalculateDependencies( env )(
                                    newDatabase, megaFilePath, interfaceHash, newSourceFiles ) );

                                // std::ostringstream os;
                                // os << "SUCCESS Dependencies added: " << megaFilePath.path().string();
                                // taskProgress.onProgress( os.str() );
                            },

                            // const Updated& updatesNeeded
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newDependencies,
                              &newSourceFiles ]( OldDependenciesVector::const_iterator i )
                            {
                                // since the code is modified - must re analyse ALL dependencies from the ground up
                                const Old::Dependencies::SourceFileDependencies* pDependencies = *i;
                                const mega::io::megaFilePath megaFilePath  = pDependencies->get_source_file();
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );
                                newDependencies.push_back( CalculateDependencies( env )(
                                    newDatabase, megaFilePath, interfaceHash, newSourceFiles ) );
                            } );
                    }

                    calculateTransitiveDependencies( newDatabase, newDependencies );
                }

                const task::FileHash fileHashCode = newDatabase.save_DPGraph_to_temp();
                m_environment.setBuildHashCode( dependencyCompilationFilePath, fileHashCode );
                m_environment.temp_to_real( dependencyCompilationFilePath );
                m_environment.stash( dependencyCompilationFilePath, determinant );

                succeeded( taskProgress );
                bReusedOldDatabase = true;
            }
            catch ( mega::io::DatabaseVersionException& )
            {
                bReusedOldDatabase = false;
            }
        }

        if ( !bReusedOldDatabase )
        {
            using namespace DependencyAnalysis;
            using namespace DependencyAnalysis::Dependencies;

            Database database( m_environment, manifestFilePath );
            {
                std::vector< SourceFileDependencies* > dependencies;
                {
                    const PathSet newSourceFiles = getNewSortedSourceFiles();
                    for ( const mega::io::megaFilePath& sourceFilePath : newSourceFiles )
                    {
                        const task::DeterminantHash interfaceHash = hashCodeGenerator( sourceFilePath );
                        dependencies.push_back( CalculateDependencies( m_environment )(
                            database, sourceFilePath, interfaceHash, newSourceFiles ) );
                    }
                }
                calculateTransitiveDependencies( database, dependencies );
            }

            const task::FileHash fileHashCode = database.save_DPGraph_to_temp();
            m_environment.setBuildHashCode( dependencyCompilationFilePath, fileHashCode );
            m_environment.temp_to_real( dependencyCompilationFilePath );
            m_environment.stash( dependencyCompilationFilePath, determinant );

            succeeded( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_DependencyAnalysis( const TaskArguments&              taskArguments,
                                              const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_DependencyAnalysis >( taskArguments, manifestFilePath );
}

} // namespace compiler
} // namespace mega
