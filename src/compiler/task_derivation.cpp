
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

            std::multimap< Interface::IContext*, Interface::IContext* > inheritance;

            for ( Interface::IContext* pContext : database.many< Interface::IContext >( sourceFilePath ) )
            {
                if ( Interface::Namespace* pNamespace = dynamic_database_cast< Interface::Namespace >( pContext ) )
                {
                }
                else if ( Interface::Abstract* pAbstract = dynamic_database_cast< Interface::Abstract >( pContext ) )
                {
                    if ( Interface::InheritanceTrait* pInheritance
                         = pAbstract->get_inheritance_trait().value_or( nullptr ) )
                    {
                        for ( Interface::IContext* pInherited : pInheritance->get_contexts() )
                        {
                            inheritance.insert( { pContext, pInherited } );
                        }
                    }
                }
                else if ( Interface::Action* pAction = dynamic_database_cast< Interface::Action >( pContext ) )
                {
                    if ( Interface::InheritanceTrait* pInheritance
                         = pAction->get_inheritance_trait().value_or( nullptr ) )
                    {
                        for ( Interface::IContext* pInherited : pInheritance->get_contexts() )
                        {
                            inheritance.insert( { pContext, pInherited } );
                        }
                    }
                }
                else if ( Interface::Event* pEvent = dynamic_database_cast< Interface::Event >( pContext ) )
                {
                    if ( Interface::InheritanceTrait* pInheritance
                         = pEvent->get_inheritance_trait().value_or( nullptr ) )
                    {
                        for ( Interface::IContext* pInherited : pInheritance->get_contexts() )
                        {
                            inheritance.insert( { pContext, pInherited } );
                        }
                    }
                }
                else if ( Interface::Function* pFunction = dynamic_database_cast< Interface::Function >( pContext ) )
                {
                }
                else if ( Interface::Object* pObject = dynamic_database_cast< Interface::Object >( pContext ) )
                {
                    if ( Interface::InheritanceTrait* pInheritance
                         = pObject->get_inheritance_trait().value_or( nullptr ) )
                    {
                        for ( Interface::IContext* pInherited : pInheritance->get_contexts() )
                        {
                            inheritance.insert( { pContext, pInherited } );
                        }
                    }
                }
                else if ( Interface::LinkInterface* pLinkInterface = dynamic_database_cast< Interface::LinkInterface >( pContext ) )
                {
                    if ( Interface::LinkTrait* pLinkTrait = pLinkInterface->get_link_trait() )
                    {
                        switch ( pLinkTrait->get_derivation().get() )
                        {
                            case mega::DerivationDirection::eDeriveNone:
                            case mega::DerivationDirection::eDeriveSource:
                            case mega::DerivationDirection::eDeriveTarget:
                            case mega::DerivationDirection::TOTAL_DERIVATION_MODES:
                                break;
                        }
                    }
                }
                else if ( Interface::Link* pLink = dynamic_database_cast< Interface::Link >( pContext ) )
                {
                }
                else
                {
                    THROW_RTE( "Unknown context type" );
                }
            }

            ObjectMapping* pObjectMapping = database.construct< ObjectMapping >(
                ObjectMapping::Args{ sourceFilePath, interfaceHash.get(), inheritance } );

            return pObjectMapping;
        }

        DerivationAnalysis::Derivation::ObjectMapping*
        operator()( DerivationAnalysis::Database&                            database,
                    const DerivationAnalysisView::Derivation::ObjectMapping* pOldObjectMapping ) const
        {
            using namespace DerivationAnalysis;
            using namespace DerivationAnalysis::Derivation;

            std::multimap< Interface::IContext*, Interface::IContext* > inheritance;

            for ( auto& [ pFrom, pTo ] : pOldObjectMapping->get_inheritance() )
            {
                inheritance.insert( { database.convert< Interface::IContext >( pFrom ),
                                      database.convert< Interface::IContext >( pTo ) } );
            }

            ObjectMapping* pObjectMapping = database.construct< ObjectMapping >( ObjectMapping::Args{
                pOldObjectMapping->get_source_file(), pOldObjectMapping->get_hash_code(), inheritance } );

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

    void collate( DerivationAnalysis::Database& database, DerivationAnalysis::Derivation::Mapping* pMapping,
                  const PathSet& sourceFiles )
    {
        using namespace DerivationAnalysis;
        using namespace DerivationAnalysis::Derivation;

        using InheritanceMapping = std::multimap< Interface::IContext*, Interface::IContext* >;
        InheritanceMapping inheritance;

        for ( ObjectMapping* pObjectMapping : pMapping->get_mappings() )
        {
            for ( auto& [ pFrom, pTo ] : pObjectMapping->get_inheritance() )
            {
                inheritance.insert( { pTo, pFrom } );
            }
        }
        pMapping->set_inheritance( inheritance );
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

                    collate( newDatabase,
                             newDatabase.construct< New::Derivation::Mapping >(
                                 New::Derivation::Mapping::Args{ newObjectMappings } ),
                             sourceFiles );
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
                collate( database, database.construct< Mapping >( Mapping::Args{ mappings } ), sourceFiles );
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

        using InheritanceMapping = std::multimap< Interface::IContext*, Interface::IContext* >;

        const Derivation::Mapping* pMapping = database.one< Derivation::Mapping >( m_environment.project_manifest() );
        const InheritanceMapping   inheritance = pMapping->get_inheritance();

        for ( Interface::IContext* pContext : database.many< Interface::IContext >( m_sourceFilePath ) )
        {
            std::vector< Concrete::Context* > concreteInheritors;
            for ( InheritanceMapping::const_iterator i    = inheritance.lower_bound( pContext ),
                                                     iEnd = inheritance.upper_bound( pContext );
                  i != iEnd;
                  ++i )
            {
                std::optional< Concrete::Context* > concreteOpt = i->second->get_concrete();
                if ( concreteOpt.has_value() )
                    concreteInheritors.push_back( concreteOpt.value() );
            }
            // reconstruct the interface context with its concrete inheritors list
            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, concreteInheritors } );
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
