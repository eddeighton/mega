

#include "base_task.hpp"

#include "database/model/DerivationAnalysis.hxx"
#include "database/model/DerivationAnalysisView.hxx"
#include "database/model/DerivationAnalysisRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/types/sources.hpp"

namespace mega
{
namespace compiler
{

class Task_Derivation : public BaseTask
{
    const mega::io::Manifest m_manifest;

    using ContextMap
        = std::multimap< DerivationAnalysis::Interface::IContext*, DerivationAnalysis::Concrete::Context* >;
    using DimensionMap = std::multimap< DerivationAnalysis::Interface::DimensionTrait*,
                                        DerivationAnalysis::Concrete::Dimensions::User* >;

public:
    Task_Derivation( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
        : BaseTask( taskArguments )
        , m_manifest( m_environment, manifestFilePath )
    {
    }

    struct CalculateObjectMappings
    {
        const mega::io::StashEnvironment& m_environment;
        CalculateObjectMappings( const mega::io::StashEnvironment& env )
            : m_environment( env )
        {
        }

        DerivationAnalysis::Derivation::ObjectMapping* operator()( DerivationAnalysis::Database& database,
                                                                   const mega::io::megaFilePath& sourceFilePath,
                                                                   const task::DeterminantHash   interfaceHash ) const
        {
            using namespace DerivationAnalysis;
            using namespace DerivationAnalysis::Derivation;

            ContextMap contextInheritance;
            {
                for ( Concrete::Context* pContext : database.many< Concrete::Context >( sourceFilePath ) )
                {
                    contextInheritance.insert( { pContext->get_interface(), pContext } );
                }
            }

            DimensionMap dimensionInheritance;
            {
                for ( Concrete::Dimensions::User* pDimension :
                      database.many< Concrete::Dimensions::User >( sourceFilePath ) )
                {
                    dimensionInheritance.insert( { pDimension->get_interface_dimension(), pDimension } );
                }
            }

            ObjectMapping* pObjectMapping = database.construct< ObjectMapping >(
                ObjectMapping::Args{ sourceFilePath, interfaceHash.get(), contextInheritance, dimensionInheritance } );

            return pObjectMapping;
        }

        DerivationAnalysis::Derivation::ObjectMapping*
        operator()( DerivationAnalysis::Database&                            database,
                    const DerivationAnalysisView::Derivation::ObjectMapping* pOldObjectMapping ) const
        {
            using namespace DerivationAnalysis;
            using namespace DerivationAnalysis::Derivation;

            ContextMap contextInheritance;
            {
                for ( auto& [ pFrom, pTo ] : pOldObjectMapping->get_inheritance_contexts() )
                {
                    contextInheritance.insert( { database.convert< Interface::IContext >( pFrom ),
                                                 database.convert< Concrete::Context >( pTo ) } );
                }
            }
            DimensionMap dimensionInheritance;
            {
                for ( auto& [ pFrom, pTo ] : pOldObjectMapping->get_inheritance_dimensions() )
                {
                    dimensionInheritance.insert( { database.convert< Interface::DimensionTrait >( pFrom ),
                                                   database.convert< Concrete::Dimensions::User >( pTo ) } );
                }
            }

            ObjectMapping* pObjectMapping = database.construct< ObjectMapping >(
                ObjectMapping::Args{ pOldObjectMapping->get_source_file(), pOldObjectMapping->get_hash_code(),
                                     contextInheritance, dimensionInheritance } );

            return pObjectMapping;
        }
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

    void collate( DerivationAnalysis::Database&                                        database,
                  const std::vector< DerivationAnalysis::Derivation::ObjectMapping* >& mappings,
                  const PathSet&                                                       sourceFiles )
    {
        using namespace DerivationAnalysis;
        using namespace DerivationAnalysis::Derivation;

        ContextMap   contextInheritance;
        DimensionMap dimensionInheritance;

        for ( ObjectMapping* pObjectMapping : mappings )
        {
            for ( auto& [ pFrom, pTo ] : pObjectMapping->get_inheritance_contexts() )
            {
                contextInheritance.insert( { pFrom, pTo } );
            }
            for ( auto& [ pFrom, pTo ] : pObjectMapping->get_inheritance_dimensions() )
            {
                dimensionInheritance.insert( { pFrom, pTo } );
            }
        }
        database.construct< Mapping >( Mapping::Args{ mappings, contextInheritance, dimensionInheritance } );
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath dependencyCompilationFilePath
            = m_environment.DerivationAnalysis_Derivations( manifestFilePath );
        start( taskProgress, "Task_Derivation", manifestFilePath.path(), dependencyCompilationFilePath.path() );

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

        bool bReusedOldDatabase = false;
        if ( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
        {
            try
            {
                // attempt to load previous dependency analysis
                namespace Old = DerivationAnalysisView;
                namespace New = DerivationAnalysis;

                New::Database newDatabase( m_environment, manifestFilePath );
                {
                    // load the archived database
                    io::ArchiveEnvironment archiveEnvironment( m_environment.DatabaseArchive() );
                    Old::Database          oldDatabase( archiveEnvironment, archiveEnvironment.project_manifest() );

                    const Old::Derivation::Mapping* pOldAnalysis
                        = oldDatabase.one< Old::Derivation::Mapping >( manifestFilePath );
                    VERIFY_RTE( pOldAnalysis );
                    using OldObjectMappingVector             = std::vector< Old::Derivation::ObjectMapping* >;
                    const OldObjectMappingVector mappings    = pOldAnalysis->get_mappings();
                    const PathSet                sourceFiles = getSortedSourceFiles();

                    struct Compare
                    {
                        const mega::io::Environment& env;
                        Compare( const mega::io::Environment& env )
                            : env( env )
                        {
                        }
                        bool operator()( OldObjectMappingVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::Derivation::ObjectMapping* pDependencies = *i;
                            return pDependencies->get_source_file() < *j;
                        }
                        bool opposite( OldObjectMappingVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::Derivation::ObjectMapping* pDependencies = *i;
                            return *j < pDependencies->get_source_file();
                        }
                    } comparator( m_environment );

                    using NewObjectMappingVector = std::vector< New::Derivation::ObjectMapping* >;
                    NewObjectMappingVector newObjectMappings;
                    {
                        generics::matchGetUpdates(
                            mappings.begin(), mappings.end(), sourceFiles.begin(), sourceFiles.end(),
                            // const Compare& cmp
                            comparator,

                            // const Update& shouldUpdate
                            [ &env = m_environment, &hashCodeGenerator, &newObjectMappings, &newDatabase, &sourceFiles,
                              &taskProgress ](
                                OldObjectMappingVector::const_iterator i, PathSet::const_iterator j ) -> bool
                            {
                                const Old::Derivation::ObjectMapping* pDependencies = *i;
                                const task::DeterminantHash           interfaceHash = hashCodeGenerator( *j );
                                if ( interfaceHash == pDependencies->get_hash_code() )
                                {
                                    // since the code is NOT modified - can re use the globs from previous
                                    newObjectMappings.push_back(
                                        CalculateObjectMappings( env )( newDatabase, pDependencies ) );

                                    // std::ostringstream os;
                                    // os << "\tPartially reusing mappings for: " << j->path().string();
                                    // taskProgress.msg( os.str() );
                                    return false;
                                }
                                else
                                {
                                    // std::ostringstream os;
                                    // os << "\tRecalculating mappings for: " << j->path().string();
                                    // taskProgress.msg( os.str() );
                                    return true;
                                }
                            },

                            // const Removal& rem
                            []( OldObjectMappingVector::const_iterator i )
                            {
                                // a source file has been removed - can ignor this since
                                // recreating the dependency analysis and only attempting to
                                // reuse the globs
                            },

                            // const Addition& add
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newObjectMappings, &sourceFiles,
                              &taskProgress ]( PathSet::const_iterator j )
                            {
                                // a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath megaFilePath  = *j;
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );
                                newObjectMappings.push_back(
                                    CalculateObjectMappings( env )( newDatabase, megaFilePath, interfaceHash ) );
                                // std::ostringstream os;
                                // os << "\tAdding mappings for: " << megaFilePath.path().string();
                                // taskProgress.msg( os.str() );
                            },

                            // const Updated& updatesNeeded
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newObjectMappings,
                              &sourceFiles ]( OldObjectMappingVector::const_iterator i )
                            {
                                // since the code is modified - must re analyse ALL mappings from the ground
                                const Old::Derivation::ObjectMapping* pDependencies = *i;
                                const mega::io::megaFilePath          megaFilePath  = pDependencies->get_source_file();
                                const task::DeterminantHash           interfaceHash = hashCodeGenerator( megaFilePath );
                                newObjectMappings.push_back(
                                    CalculateObjectMappings( env )( newDatabase, megaFilePath, interfaceHash ) );
                            } );
                    }

                    collate( newDatabase, newObjectMappings, sourceFiles );
                }

                const task::FileHash fileHashCode = newDatabase.save_Derivations_to_temp();
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
            using namespace DerivationAnalysis;
            using namespace DerivationAnalysis::Derivation;

            Database database( m_environment, manifestFilePath );
            {
                std::vector< ObjectMapping* > mappings;
                const PathSet                 sourceFiles = getSortedSourceFiles();
                {
                    for ( const mega::io::megaFilePath& sourceFilePath : sourceFiles )
                    {
                        const task::DeterminantHash interfaceHash = hashCodeGenerator( sourceFilePath );
                        mappings.push_back(
                            CalculateObjectMappings( m_environment )( database, sourceFilePath, interfaceHash ) );
                    }
                }
                collate( database, mappings, sourceFiles );
            }

            const task::FileHash fileHashCode = database.save_Derivations_to_temp();
            m_environment.setBuildHashCode( dependencyCompilationFilePath, fileHashCode );
            m_environment.temp_to_real( dependencyCompilationFilePath );
            m_environment.stash( dependencyCompilationFilePath, determinant );

            succeeded( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_Derivation( const TaskArguments&              taskArguments,
                                      const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_Derivation >( taskArguments, manifestFilePath );
}

class Task_DerivationRollout : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

    using ContextMap   = std::multimap< DerivationAnalysisRollout::Interface::IContext*,
                                      DerivationAnalysisRollout::Concrete::Context* >;
    using DimensionMap = std::multimap< DerivationAnalysisRollout::Interface::DimensionTrait*,
                                        DerivationAnalysisRollout::Concrete::Dimensions::User* >;

public:
    Task_DerivationRollout( const TaskArguments& taskArguments, const mega::io::megaFilePath& megaSourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( megaSourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath analysisCompilationFilePath
            = m_environment.DerivationAnalysis_Derivations( m_environment.project_manifest() );
        const mega::io::CompilationFilePath rolloutCompilationFilePath
            = m_environment.DerivationAnalysisRollout_PerSourceDerivations( m_sourceFilePath );
        start( taskProgress, "Task_DerivationRollout", m_sourceFilePath.path(), rolloutCompilationFilePath.path() );

        const task::DeterminantHash determinant = { m_environment.getBuildHashCode( analysisCompilationFilePath ) };

        if ( m_environment.restore( rolloutCompilationFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( rolloutCompilationFilePath );
            cached( taskProgress );
            return;
        }

        using namespace DerivationAnalysisRollout;

        Database database( m_environment, m_sourceFilePath );

        const Derivation::Mapping* pMapping = database.one< Derivation::Mapping >( m_environment.project_manifest() );
        {
            const ContextMap contexts = pMapping->get_inheritance_contexts();
            for ( Interface::IContext* pContext : database.many< Interface::IContext >( m_sourceFilePath ) )
            {
                std::vector< Concrete::Context* > concreteInheritors;
                for ( ContextMap::const_iterator i    = contexts.lower_bound( pContext ),
                                                 iEnd = contexts.upper_bound( pContext );
                      i != iEnd;
                      ++i )
                {
                    concreteInheritors.push_back( i->second );
                }
                // reconstruct
                database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, concreteInheritors } );
            }
        }
        {
            const DimensionMap dimensions = pMapping->get_inheritance_dimensions();
            for ( Interface::DimensionTrait* pDimension :
                  database.many< Interface::DimensionTrait >( m_sourceFilePath ) )
            {
                std::vector< Concrete::Dimensions::User* > dimensionInheritors;
                for ( DimensionMap::const_iterator i    = dimensions.lower_bound( pDimension ),
                                                   iEnd = dimensions.upper_bound( pDimension );
                      i != iEnd;
                      ++i )
                {
                    dimensionInheritors.push_back( i->second );
                }
                // reconstruct
                database.construct< Interface::DimensionTrait >(
                    Interface::DimensionTrait::Args{ pDimension, dimensionInheritors } );
            }
        }

        const task::FileHash fileHashCode = database.save_PerSourceDerivations_to_temp();
        m_environment.setBuildHashCode( rolloutCompilationFilePath, fileHashCode );
        m_environment.temp_to_real( rolloutCompilationFilePath );
        m_environment.stash( rolloutCompilationFilePath, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_DerivationRollout( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& megaSourceFilePath )
{
    return std::make_unique< Task_DerivationRollout >( taskArguments, megaSourceFilePath );
}

} // namespace compiler
} // namespace mega
