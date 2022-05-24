#ifndef TASK_DEPENDENCIES_10_MAY_2022
#define TASK_DEPENDENCIES_10_MAY_2022

#include "base_task.hpp"

#include "database/common/exception.hpp"

#include "database/model/DependencyAnalysis.hxx"
#include "database/model/DependencyAnalysisView.hxx"

#include "utilities/glob.hpp"

namespace driver
{
namespace interface
{

class Task_DependencyAnalysis : public BaseTask
{
    const mega::io::Manifest& m_manifest;

public:
    Task_DependencyAnalysis( const TaskArguments& taskArguments, const mega::io::Manifest& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    using GlobVector = std::vector< mega::utilities::Glob >;
    using PathSet    = std::set< mega::io::megaFilePath >;

    struct CalculateDependencies
    {
        const mega::io::BuildEnvironment& m_environment;
        CalculateDependencies( const mega::io::BuildEnvironment& env )
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

        DependencyAnalysis::Dependencies::ObjectDependencies* operator()( DependencyAnalysis::Database& database,
                                                                          const mega::io::megaFilePath& sourceFilePath,
                                                                          const task::DeterminantHash   interfaceHash,
                                                                          const PathSet& sourceFiles ) const
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
                    mega::utilities::resolveGlob( glob, m_environment.rootSourceDir(), matchedFilePaths );
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

            ObjectDependencies* pDependencies = database.construct< ObjectDependencies >(
                ObjectDependencies::Args{ sourceFilePath, interfaceHash.get(), globs, resolution } );

            return pDependencies;
        }

        DependencyAnalysis::Dependencies::ObjectDependencies*
        operator()( DependencyAnalysis::Database&                                   database,
                    const DependencyAnalysisView::Dependencies::ObjectDependencies* pOldDependencies,
                    const PathSet&                                                  sourceFiles ) const
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
                    mega::utilities::resolveGlob( glob, m_environment.rootSourceDir(), matchedFilePaths );
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

            ObjectDependencies* pDependencies = database.construct< ObjectDependencies >( ObjectDependencies::Args{
                pOldDependencies->get_source_file(), pOldDependencies->get_hash_code(), globs, resolution } );

            return pDependencies;
        }
    };

    PathSet getSortedSourceFiles() const
    {
        PathSet sourceFiles;
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }

    virtual void run( task::Progress& taskProgress )
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
            const mega::io::BuildEnvironment& env;
            task::DeterminantHash             toolChainHash;
            InterfaceHashCodeGenerator( const mega::io::BuildEnvironment& env, task::DeterminantHash toolChainHash )
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
        try
        {
            // try loading previous one...
            if ( m_environment.exists( dependencyCompilationFilePath ) )
            {
                // attempt to load previous dependency analysis
                namespace Old = DependencyAnalysisView;
                namespace New = DependencyAnalysis;

                New::Database newDatabase( m_environment, manifestFilePath );
                {
                    Old::Database oldDatabase( m_environment, manifestFilePath );

                    const Old::Dependencies::Analysis* pOldAnalysis
                        = oldDatabase.one< Old::Dependencies::Analysis >( manifestFilePath );
                    VERIFY_RTE( pOldAnalysis );
                    using OldDependenciesVector              = std::vector< Old::Dependencies::ObjectDependencies* >;
                    const OldDependenciesVector dependencies = pOldAnalysis->get_objects();
                    const PathSet               sourceFiles  = getSortedSourceFiles();

                    struct Compare
                    {
                        const mega::io::Environment& env;
                        Compare( const mega::io::Environment& env )
                            : env( env )
                        {
                        }
                        bool operator()( OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                            return pDependencies->get_source_file() < *j;
                        }
                        bool opposite( OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                            return *j < pDependencies->get_source_file();
                        }
                    } comparator( m_environment );

                    using NewDependenciesVector = std::vector< New::Dependencies::ObjectDependencies* >;
                    NewDependenciesVector newDependencies;
                    {
                        generics::matchGetUpdates(
                            dependencies.begin(), dependencies.end(), sourceFiles.begin(), sourceFiles.end(),
                            // const Compare& cmp
                            comparator,

                            // const Update& shouldUpdate
                            [ &env = m_environment, &hashCodeGenerator, &newDependencies, &newDatabase, &sourceFiles,
                              &taskProgress ](
                                OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) -> bool
                            {
                                const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                                const task::DeterminantHash                  interfaceHash = hashCodeGenerator( *j );
                                if ( interfaceHash == pDependencies->get_hash_code() )
                                {
                                    // since the code is NOT modified - can re use the globs from previous result
                                    newDependencies.push_back(
                                        CalculateDependencies( env )( newDatabase, pDependencies, sourceFiles ) );

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
                            []( OldDependenciesVector::const_iterator i )
                            {
                                // a source file has been removed - can ignor this since
                                // recreating the dependency analysis and only attempting to
                                // reuse the globs
                            },

                            // const Addition& add
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newDependencies, &sourceFiles,
                              &taskProgress ]( PathSet::const_iterator j )
                            {
                                // a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath megaFilePath  = *j;
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );
                                newDependencies.push_back( CalculateDependencies( env )(
                                    newDatabase, megaFilePath, interfaceHash, sourceFiles ) );
                                // std::ostringstream os;
                                // os << "\tAdding dependencies for: " << megaFilePath.path().string();
                                // taskProgress.msg( os.str() );
                            },

                            // const Updated& updatesNeeded
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newDependencies,
                              &sourceFiles ]( OldDependenciesVector::const_iterator i )
                            {
                                // since the code is modified - must re analyse ALL dependencies from the ground up
                                const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                                const mega::io::megaFilePath megaFilePath  = pDependencies->get_source_file();
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );
                                newDependencies.push_back( CalculateDependencies( env )(
                                    newDatabase, megaFilePath, interfaceHash, sourceFiles ) );
                            } );
                    }

                    newDatabase.construct< New::Dependencies::Analysis >(
                        New::Dependencies::Analysis::Args{ newDependencies } );
                }

                const task::FileHash fileHashCode = newDatabase.save_DPGraph_to_temp();
                m_environment.setBuildHashCode( dependencyCompilationFilePath, fileHashCode );
                m_environment.temp_to_real( dependencyCompilationFilePath );
                m_environment.stash( dependencyCompilationFilePath, determinant );

                succeeded( taskProgress );
                bReusedOldDatabase = true;
            }
        }
        catch ( mega::io::DatabaseVersionException& )
        {
            bReusedOldDatabase = false;
        }

        if ( !bReusedOldDatabase )
        {
            using namespace DependencyAnalysis;
            using namespace DependencyAnalysis::Dependencies;

            Database database( m_environment, manifestFilePath );
            {
                std::vector< ObjectDependencies* > dependencies;
                {
                    const PathSet sourceFiles = getSortedSourceFiles();
                    for ( const mega::io::megaFilePath& sourceFilePath : sourceFiles )
                    {
                        const task::DeterminantHash interfaceHash = hashCodeGenerator( sourceFilePath );
                        dependencies.push_back( CalculateDependencies( m_environment )(
                            database, sourceFilePath, interfaceHash, sourceFiles ) );
                    }
                }
                database.construct< Analysis >( Analysis::Args{ dependencies } );
            }

            const task::FileHash fileHashCode = database.save_DPGraph_to_temp();
            m_environment.setBuildHashCode( dependencyCompilationFilePath, fileHashCode );
            m_environment.temp_to_real( dependencyCompilationFilePath );
            m_environment.stash( dependencyCompilationFilePath, determinant );

            succeeded( taskProgress );
        }
    }
};

} // namespace interface
} // namespace driver

#endif // TASK_DEPENDENCIES_10_MAY_2022