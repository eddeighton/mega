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

#include "database/InheritanceAnalysis.hxx"
#include "database/InheritanceAnalysisView.hxx"
#include "database/InheritanceAnalysisRollout.hxx"
#include "database/manifest.hxx"

#include "environment/environment_archive.hpp"
#include "database/exception.hpp"
#include "database/sources.hpp"

namespace mega::compiler
{

class Task_Inheritance : public BaseTask
{
    const mega::io::Manifest m_manifest;

    using ContextMap
        = std::multimap< InheritanceAnalysis::Interface::IContext*, InheritanceAnalysis::Concrete::Context* >;
    using DimensionMap = std::multimap< InheritanceAnalysis::Interface::DimensionTrait*,
                                        InheritanceAnalysis::Concrete::Dimensions::User* >;
    using LinkMap      = std::multimap< InheritanceAnalysis::Interface::LinkTrait*,
                                   InheritanceAnalysis::Concrete::Dimensions::Link* >;

public:
    Task_Inheritance( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
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

        void addInheritance( ContextMap& contextInheritance, InheritanceAnalysis::Interface::IContext* pIContext,
                             InheritanceAnalysis::Concrete::Context* pContext ) const
        {
            using namespace InheritanceAnalysis;
            using namespace InheritanceAnalysis::Inheritance;

            contextInheritance.insert( { pIContext, pContext } );

            if( auto pAbstract = db_cast< Interface::Abstract >( pIContext ) )
            {
                if( auto opt = pAbstract->get_inheritance_trait_opt() )
                {
                    for( auto pInheritedIContext : opt.value()->get_contexts() )
                    {
                        addInheritance( contextInheritance, pInheritedIContext, pContext );
                    }
                }
            }
            else if( auto pState = db_cast< Interface::State >( pIContext ) )
            {
                if( auto opt = pState->get_inheritance_trait_opt() )
                {
                    for( auto pInheritedIContext : opt.value()->get_contexts() )
                    {
                        addInheritance( contextInheritance, pInheritedIContext, pContext );
                    }
                }
            }
            else if( auto pEvent = db_cast< Interface::Event >( pIContext ) )
            {
                if( auto opt = pEvent->get_inheritance_trait_opt() )
                {
                    for( auto pInheritedIContext : opt.value()->get_contexts() )
                    {
                        addInheritance( contextInheritance, pInheritedIContext, pContext );
                    }
                }
            }
            else if( auto pObject = db_cast< Interface::Object >( pIContext ) )
            {
                if( auto opt = pObject->get_inheritance_trait_opt() )
                {
                    for( auto pInheritedIContext : opt.value()->get_contexts() )
                    {
                        addInheritance( contextInheritance, pInheritedIContext, pContext );
                    }
                }
            }
        }

        InheritanceAnalysis::Inheritance::ObjectMapping* operator()( InheritanceAnalysis::Database& database,
                                                                     const mega::io::megaFilePath&  sourceFilePath,
                                                                     const task::DeterminantHash interfaceHash ) const
        {
            using namespace InheritanceAnalysis;
            using namespace InheritanceAnalysis::Inheritance;

            ContextMap contextInheritance;
            {
                for( Concrete::Context* pContext : database.many< Concrete::Context >( sourceFilePath ) )
                {
                    Interface::IContext* pIContext = pContext->get_interface();
                    addInheritance( contextInheritance, pIContext, pContext );
                }
            }

            DimensionMap dimensionInheritance;
            {
                for( Concrete::Dimensions::User* pDimension :
                     database.many< Concrete::Dimensions::User >( sourceFilePath ) )
                {
                    dimensionInheritance.insert( { pDimension->get_interface_dimension(), pDimension } );
                }
            }
            LinkMap linkInheritance;
            {
                for( Concrete::Dimensions::Link* pLink :
                     database.many< Concrete::Dimensions::Link >( sourceFilePath ) )
                {
                    linkInheritance.insert( { pLink->get_interface_link(), pLink } );
                }
            }

            ObjectMapping* pObjectMapping = database.construct< ObjectMapping >( ObjectMapping::Args{
                sourceFilePath, interfaceHash.get(), contextInheritance, dimensionInheritance, linkInheritance } );

            return pObjectMapping;
        }

        InheritanceAnalysis::Inheritance::ObjectMapping*
        operator()( InheritanceAnalysis::Database&                             database,
                    const InheritanceAnalysisView::Inheritance::ObjectMapping* pOldObjectMapping ) const
        {
            using namespace InheritanceAnalysis;
            using namespace InheritanceAnalysis::Inheritance;

            ContextMap contextInheritance;
            {
                for( auto& [ pFrom, pTo ] : pOldObjectMapping->get_inheritance_contexts() )
                {
                    contextInheritance.insert( { database.convert< Interface::IContext >( pFrom ),
                                                 database.convert< Concrete::Context >( pTo ) } );
                }
            }
            DimensionMap dimensionInheritance;
            {
                for( auto& [ pFrom, pTo ] : pOldObjectMapping->get_inheritance_dimensions() )
                {
                    dimensionInheritance.insert( { database.convert< Interface::DimensionTrait >( pFrom ),
                                                   database.convert< Concrete::Dimensions::User >( pTo ) } );
                }
            }
            LinkMap linkInheritance;
            {
                for( auto& [ pFrom, pTo ] : pOldObjectMapping->get_inheritance_links() )
                {
                    linkInheritance.insert( { database.convert< Interface::LinkTrait >( pFrom ),
                                              database.convert< Concrete::Dimensions::UserLink >( pTo ) } );
                }
            }

            ObjectMapping* pObjectMapping = database.construct< ObjectMapping >(
                ObjectMapping::Args{ pOldObjectMapping->get_source_file(), pOldObjectMapping->get_hash_code(),
                                     contextInheritance, dimensionInheritance, linkInheritance } );

            return pObjectMapping;
        }
    };

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

    void collate( InheritanceAnalysis::Database&                                         database,
                  const std::vector< InheritanceAnalysis::Inheritance::ObjectMapping* >& mappings,
                  const PathSet&                                                         sourceFiles )
    {
        using namespace InheritanceAnalysis;
        using namespace InheritanceAnalysis::Inheritance;

        ContextMap   contextInheritance;
        DimensionMap dimensionInheritance;
        LinkMap      linkInheritance;

        for( ObjectMapping* pObjectMapping : mappings )
        {
            for( auto& [ pFrom, pTo ] : pObjectMapping->get_inheritance_contexts() )
            {
                contextInheritance.insert( { pFrom, pTo } );
            }
            for( auto& [ pFrom, pTo ] : pObjectMapping->get_inheritance_dimensions() )
            {
                dimensionInheritance.insert( { pFrom, pTo } );
            }
            for( auto& [ pFrom, pTo ] : pObjectMapping->get_inheritance_links() )
            {
                linkInheritance.insert( { pFrom, pTo } );
            }
        }
        database.construct< Mapping >(
            Mapping::Args{ mappings, contextInheritance, dimensionInheritance, linkInheritance } );
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath dependencyCompilationFilePath
            = m_environment.InheritanceAnalysis_Derivations( manifestFilePath );
        start( taskProgress, "Task_Inheritance", manifestFilePath.path(), dependencyCompilationFilePath.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if( m_environment.restore( dependencyCompilationFilePath, determinant ) )
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
        /*if( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
        {
            try
            {
                // attempt to load previous dependency analysis
                namespace Old = InheritanceAnalysisView;
                namespace New = InheritanceAnalysis;

                New::Database newDatabase( m_environment, manifestFilePath );
                {
                    // load the archived database
                    io::ArchiveEnvironment archiveEnvironment( m_environment.DatabaseArchive() );
                    Old::Database          oldDatabase( archiveEnvironment, archiveEnvironment.project_manifest() );

                    const Old::Inheritance::Mapping* pOldAnalysis
                        = oldDatabase.one< Old::Inheritance::Mapping >( manifestFilePath );
                    VERIFY_RTE( pOldAnalysis );
                    using OldObjectMappingVector             = std::vector< Old::Inheritance::ObjectMapping* >;
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
                            const Old::Inheritance::ObjectMapping* pDependencies = *i;
                            return pDependencies->get_source_file() < *j;
                        }
                        bool opposite( OldObjectMappingVector::const_iterator i, PathSet::const_iterator j ) const
                        {
                            const Old::Inheritance::ObjectMapping* pDependencies = *i;
                            return *j < pDependencies->get_source_file();
                        }
                    } comparator( m_environment );

                    using NewObjectMappingVector = std::vector< New::Inheritance::ObjectMapping* >;
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
                                const Old::Inheritance::ObjectMapping* pDependencies = *i;
                                const task::DeterminantHash            interfaceHash = hashCodeGenerator( *j );
                                if( interfaceHash == pDependencies->get_hash_code() )
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
                                const Old::Inheritance::ObjectMapping* pDependencies = *i;
                                const mega::io::megaFilePath           megaFilePath  = pDependencies->get_source_file();
                                const task::DeterminantHash interfaceHash = hashCodeGenerator( megaFilePath );
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
            catch( mega::io::DatabaseVersionException& )
            {
                bReusedOldDatabase = false;
            }
        }*/

        if( !bReusedOldDatabase )
        {
            using namespace InheritanceAnalysis;
            using namespace InheritanceAnalysis::Inheritance;

            Database database( m_environment, manifestFilePath );
            {
                std::vector< ObjectMapping* > mappings;
                const PathSet                 sourceFiles = getSortedSourceFiles();
                {
                    for( const mega::io::megaFilePath& sourceFilePath : sourceFiles )
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

BaseTask::Ptr create_Task_Inheritance( const TaskArguments&              taskArguments,
                                       const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_Inheritance >( taskArguments, manifestFilePath );
}

class Task_InheritanceRollout : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

    using ContextMap   = std::multimap< InheritanceAnalysisRollout::Interface::IContext*,
                                      InheritanceAnalysisRollout::Concrete::Context* >;
    using DimensionMap = std::multimap< InheritanceAnalysisRollout::Interface::DimensionTrait*,
                                        InheritanceAnalysisRollout::Concrete::Dimensions::User* >;
    using LinkMap      = std::multimap< InheritanceAnalysisRollout::Interface::LinkTrait*,
                                   InheritanceAnalysisRollout::Concrete::Dimensions::Link* >;

public:
    Task_InheritanceRollout( const TaskArguments& taskArguments, const mega::io::megaFilePath& megaSourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( megaSourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath analysisCompilationFilePath
            = m_environment.InheritanceAnalysis_Derivations( m_environment.project_manifest() );
        const mega::io::CompilationFilePath rolloutCompilationFilePath
            = m_environment.InheritanceAnalysisRollout_PerSourceDerivations( m_sourceFilePath );
        start( taskProgress, "Task_InheritanceRollout", m_sourceFilePath.path(), rolloutCompilationFilePath.path() );

        const task::DeterminantHash determinant = { m_environment.getBuildHashCode( analysisCompilationFilePath ) };

        if( m_environment.restore( rolloutCompilationFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( rolloutCompilationFilePath );
            cached( taskProgress );
            return;
        }

        using namespace InheritanceAnalysisRollout;

        Database database( m_environment, m_sourceFilePath );

        const Inheritance::Mapping* pMapping = database.one< Inheritance::Mapping >( m_environment.project_manifest() );
        {
            const ContextMap contexts = pMapping->get_inheritance_contexts();
            for( Interface::IContext* pContext : database.many< Interface::IContext >( m_sourceFilePath ) )
            {
                std::vector< Concrete::Context* > concreteInheritors;
                for( auto i = contexts.lower_bound( pContext ), iEnd = contexts.upper_bound( pContext ); i != iEnd;
                     ++i )
                {
                    concreteInheritors.push_back( i->second );
                }
                database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, concreteInheritors } );
            }
        }
        {
            const DimensionMap dimensions = pMapping->get_inheritance_dimensions();
            for( Interface::DimensionTrait* pDimension :
                 database.many< Interface::DimensionTrait >( m_sourceFilePath ) )
            {
                std::vector< Concrete::Dimensions::User* > dimensionInheritors;
                for( auto i = dimensions.lower_bound( pDimension ), iEnd = dimensions.upper_bound( pDimension );
                     i != iEnd; ++i )
                {
                    dimensionInheritors.push_back( i->second );
                }
                database.construct< Interface::DimensionTrait >(
                    Interface::DimensionTrait::Args{ pDimension, dimensionInheritors } );
            }
        }
        {
            const LinkMap links = pMapping->get_inheritance_links();
            for( Interface::LinkTrait* pLink : database.many< Interface::LinkTrait >( m_sourceFilePath ) )
            {
                std::vector< Concrete::Dimensions::Link* > linkInheritors;
                for( auto i = links.lower_bound( pLink ), iEnd = links.upper_bound( pLink ); i != iEnd; ++i )
                {
                    linkInheritors.push_back( i->second );
                }
                database.construct< Interface::LinkTrait >( Interface::LinkTrait::Args{ pLink, linkInheritors } );
            }
        }

        const task::FileHash fileHashCode = database.save_PerSourceDerivations_to_temp();
        m_environment.setBuildHashCode( rolloutCompilationFilePath, fileHashCode );
        m_environment.temp_to_real( rolloutCompilationFilePath );
        m_environment.stash( rolloutCompilationFilePath, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_InheritanceRollout( const TaskArguments&          taskArguments,
                                              const mega::io::megaFilePath& megaSourceFilePath )
{
    return std::make_unique< Task_InheritanceRollout >( taskArguments, megaSourceFilePath );
}

} // namespace mega::compiler
