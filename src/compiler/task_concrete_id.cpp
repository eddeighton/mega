
#include "base_task.hpp"

#include "database/model/ConcreteTypeAnalysis.hxx"
#include "database/model/ConcreteTypeAnalysisView.hxx"
#include "database/model/ConcreteTypeRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/types/sources.hpp"

namespace mega
{
namespace compiler
{

class Task_ConcreteTypeAnalysis : public BaseTask
{
    const mega::io::Manifest m_manifest;

public:
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
                { toolChainHash, env.getBuildHashCode( env.ConcreteStage_Concrete( megaFilePath ) ) } );
        }
    };

    Task_ConcreteTypeAnalysis( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
        : BaseTask( taskArguments )
        , m_manifest( m_environment, manifestFilePath )
    {
    }

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

    using TypeIDContextMap = std::map< int32_t, ConcreteTypeAnalysis::Concrete::Context* >;
    using TypeMap          = std::multimap< int32_t, ConcreteTypeAnalysis::Concrete::Context* >;

    struct SymbolCollector
    {
        using ConcretePath = std::vector< ConcreteTypeAnalysis::Concrete::Context* >;
        static ConcretePath getContextPath( ConcreteTypeAnalysis::Concrete::Context* pContext )
        {
            using namespace ConcreteTypeAnalysis;
            ConcretePath       result;
            Concrete::Context* pIter = pContext;
            while ( pIter )
            {
                result.push_back( pIter );
                pIter = dynamic_database_cast< Concrete::Context >( pIter->get_parent() );
            }
            std::reverse( result.begin(), result.end() );
            return result;
        }

        using SymbolSetMap       = std::map< mega::io::megaFilePath, ConcreteTypeAnalysis::Symbols::SymbolSet* >;
        using SymbolIDPath       = std::vector< std::int32_t >;
        using ConcreteSymbolMap  = std::map< SymbolIDPath, ConcreteTypeAnalysis::Symbols::ConcreteSymbol* >;
        using ConcreteContextMap = std::map< ConcreteTypeAnalysis::Concrete::Context*, int32_t >;

        ConcreteTypeAnalysis::Symbols::ConcreteSymbol*
        findOrCreateSymbol( ConcreteTypeAnalysis::Database&          database,
                            ConcreteSymbolMap&                       concreteSymbolMap,
                            ConcreteTypeAnalysis::Concrete::Context* pContext ) const
        {
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;

            SymbolIDPath symbolIDPath;
            for ( const Concrete::Context* pContext : getContextPath( pContext ) )
                symbolIDPath.push_back( pContext->get_interface()->get_symbol() );

            ConcreteSymbol* pSymbol = nullptr;
            {
                ConcreteSymbolMap::iterator iFind = concreteSymbolMap.find( symbolIDPath );
                if ( iFind != concreteSymbolMap.end() )
                {
                    pSymbol = iFind->second;
                }
            }

            if ( !pSymbol )
            {
                // create the symbol in an un-labelled state i.e. with 0 for symbol id
                pSymbol = database.construct< ConcreteSymbol >( ConcreteSymbol::Args{ symbolIDPath, 0, pContext } );
                concreteSymbolMap.insert( std::make_pair( symbolIDPath, pSymbol ) );
            }

            return pSymbol;
        }

        void operator()( ConcreteTypeAnalysis::Database& database,
                         ConcreteSymbolMap&              concreteSymbolMap,
                         const mega::io::megaFilePath&   sourceFilePath ) const
        {
            using namespace ConcreteTypeAnalysis;
            for ( Concrete::Context* pContext : database.many< Concrete::Context >( sourceFilePath ) )
            {
                Symbols::ConcreteSymbol* pSymbol = findOrCreateSymbol( database, concreteSymbolMap, pContext );
            }
        }
        void labelNewContexts( ConcreteSymbolMap& concrete_symbol_ids )
        {
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;
            // using ConcreteSymbolMap  = std::map< SymbolIDPath, ConcreteTypeAnalysis::Symbols::ConcreteSymbol* >;

            for ( ConcreteSymbolMap::iterator i = concrete_symbol_ids.begin(); i != concrete_symbol_ids.end(); ++i )
            {
                std::set< std::int32_t > symbolLabels;

                ConcreteSymbol* pSymbol = i->second;
                if ( pSymbol->get_id() != 0 ) // zero means not set
                {
                    VERIFY_RTE( !symbolLabels.count( pSymbol->get_id() ) );
                    symbolLabels.insert( pSymbol->get_id() );
                }

                std::set< std::int32_t >::iterator labelIter   = symbolLabels.begin();
                std::int32_t                       szNextLabel = 1;
                for ( ConcreteSymbolMap::iterator i = concrete_symbol_ids.begin(); i != concrete_symbol_ids.end(); ++i )
                {
                    ConcreteSymbol* pSymbol = i->second;
                    if ( pSymbol->get_id() == 0 )
                    {
                        while ( labelIter != symbolLabels.end() )
                        {
                            if ( *labelIter > szNextLabel )
                            {
                                break;
                            }
                            else
                            {
                                szNextLabel = *labelIter + 1;
                                ++labelIter;
                            }
                        }
                        pSymbol->set_id( szNextLabel );
                        ++szNextLabel;
                    }
                }
            }
        }
        void collate( const SymbolSetMap&                symbolSetMap,
                      SymbolCollector::ConcreteSymbolMap concrete_symbol_ids,
                      std::map< std::int32_t, ::ConcreteTypeAnalysis::Symbols::ConcreteSymbol* >
                          concrete_symbols,
                      std::map< std::int32_t, ::ConcreteTypeAnalysis::Concrete::Context* >
                          concrete_context_map )
        {
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;

            for ( auto& [ filePath, pSymbolSet ] : symbolSetMap )
            {
                for ( const auto& [ id, pSymbol ] : pSymbolSet->get_concrete_symbols() )
                {
                    pSymbolSet->insert_context_concrete_ids( pSymbol->get_context(), pSymbol->get_id() );
                    VERIFY_RTE( concrete_symbols.insert( { pSymbol->get_id(), pSymbol } ).second );
                    VERIFY_RTE( concrete_context_map.insert( { pSymbol->get_id(), pSymbol->get_context() } ).second );
                }
            }
        }
    };

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath symbolCompilationFile
            = m_environment.ConcreteTypeAnalysis_ConcreteTable( manifestFilePath );
        start( taskProgress, "Task_ConcreteTypeAnalysis", manifestFilePath.path(), symbolCompilationFile.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( sourceFilePath ) );
        }

        if ( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            cached( taskProgress );
            return;
        }

        InterfaceHashCodeGenerator hashCodeGenerator( m_environment, m_toolChain.toolChainHash );

        bool bReusedOldDatabase = false;
        // if ( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
        if ( bReusedOldDatabase )
        {
            /*try
            {
                // attempt to reuse previous symbol analysis
                namespace Old = ConcreteTypeAnalysisView;
                namespace New = ConcreteTypeAnalysis;

                New::Database newDatabase( m_environment, manifestFilePath );
                {
                    // load the archived database
                    io::ArchiveEnvironment archiveEnvironment( m_environment.DatabaseArchive() );
                    Old::Database          oldDatabase( archiveEnvironment, archiveEnvironment.project_manifest() );

                    const Old::Symbols::SymbolTable* pOldAnalysis
                        = oldDatabase.one< Old::Symbols::SymbolTable >( manifestFilePath );
                    VERIFY_RTE( pOldAnalysis );
                    using OldSymbolsMap                = std::map< mega::io::megaFilePath, Old::Symbols::SymbolSet* >;
                    const OldSymbolsMap oldSymbolsMaps = pOldAnalysis->get_symbol_sets();
                    const PathSet       sourceFiles    = getSortedSourceFiles();

                    TypeIDContextMap typeIDContextMap;
                    TypeMap          typeMap;

                    std::map< mega::io::megaFilePath, New::Symbols::SymbolSet* > symbolSetMap;

                    {
                        struct Compare
                        {
                            const mega::io::Environment& env;
                            Compare( const mega::io::Environment& env )
                                : env( env )
                            {
                            }
                            bool operator()( OldSymbolsMap::const_iterator i, PathSet::const_iterator j ) const
                            {
                                return i->first < *j;
                            }
                            bool opposite( OldSymbolsMap::const_iterator i, PathSet::const_iterator j ) const
                            {
                                return *j < i->first;
                            }
                        } comparator( m_environment );

                        generics::matchGetUpdates(
                            oldSymbolsMaps.begin(), oldSymbolsMaps.end(), sourceFiles.begin(), sourceFiles.end(),
                            // const Compare& cmp
                            comparator,

                            // const Update& shouldUpdate
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolSetMap,
                              &typeMap ]( OldSymbolsMap::const_iterator i, PathSet::const_iterator j ) -> bool
                            {
                                const task::DeterminantHash interfaceHash = hashCodeGenerator( *j );
                                if ( interfaceHash.get() == i->second->get_hash_code() )
                                {
                                    // since the code is NOT modified - can re use
                                    Old::Symbols::SymbolSet* pOldSymbolSet = i->second;

                                    // std::ostringstream os;
                                    // os << "\tPartially reusing symbols for: " << j->path().string();
                                    // taskProgress.msg( os.str() );
                                    return false;
                                }
                                else
                                {
                                    return true;
                                }
                            },

                            // const Removal& rem
                            []( OldSymbolsMap::const_iterator i )
                            {
                                // a source file has been removed - can ignor this since
                                // recreating the dependency analysis and only attempting to
                                // reuse the globs
                            },

                            // const Addition& add
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolSetMap,
                              &typeMap ]( PathSet::const_iterator j )
                            {
                                // a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath megaFilePath  = *j;
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );

                                // New::Symbols::SymbolSet* pSymbolSet
                                //     = newDatabase.construct< New::Symbols::SymbolSet >(
                                //     New::Symbols::SymbolSet::Args(
                                //         {}, megaFilePath, interfaceHash.get(), {}, {}, {}, {} ) );

                                // SymbolCollector()( newDatabase, pSymbolSet, typeMap );
                                // symbolSetMap.insert( std::make_pair( megaFilePath, pSymbolSet ) );

                                // std::ostringstream os;
                                // os << "\tAdded symbols for: " << megaFilePath.path().string();
                                // taskProgress.msg( os.str() );
                            },

                            // const Updated& updatesNeeded
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolSetMap,
                              &typeMap ]( OldSymbolsMap::const_iterator i )
                            {
                                // Old::Symbols::SymbolSet* pOldSymbolSet = i->second;
                                //  a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath megaFilePath  = i->first;
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );

                                // New::Symbols::SymbolSet* pSymbolSet
                                //     = newDatabase.construct< New::Symbols::SymbolSet >(
                                //     New::Symbols::SymbolSet::Args(
                                //         {}, megaFilePath, interfaceHash.get(), {}, {}, {}, {} ) );

                                // SymbolCollector()( newDatabase, pSymbolSet, symbolMap, typeMap );
                                // symbolSetMap.insert( std::make_pair( megaFilePath, pSymbolSet ) );

                                // std::ostringstream os;
                                // os << "\tUpdated symbols for: " << megaFilePath.path().string();
                                // taskProgress.msg( os.str() );
                            } );
                    }
                    // SymbolIDMap symbolIDMap;
                    // SymbolCollector().labelNewSymbols( symbolMap );
                    // SymbolCollector().labelNewTypes( typeMap );
                    // SymbolCollector().collate(
                    //    symbolSetMap, symbolMap, typeIDContextMap, typeIDDimensionTraitMap, symbolIDMap );
                    // newDatabase.construct< New::Symbols::SymbolTable >( New::Symbols::SymbolTable::Args(
                    //     symbolSetMap, symbolMap, typeIDContextMap, typeIDDimensionTraitMap, symbolIDMap ) );
                }

                const task::FileHash fileHashCode = newDatabase.save_ConcreteTable_to_temp();
                m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
                m_environment.temp_to_real( symbolCompilationFile );
                m_environment.stash( symbolCompilationFile, determinant );

                succeeded( taskProgress );
                bReusedOldDatabase = true;
            }
            catch ( mega::io::DatabaseVersionException& )
            {
                bReusedOldDatabase = false;
            }*/
        }

        if ( !bReusedOldDatabase )
        {
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;

            Database database( m_environment, manifestFilePath );
            {
                Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( manifestFilePath );
                const std::map< mega::io::megaFilePath, Symbols::SymbolSet* > symbolSetsMap
                    = pSymbolTable->get_symbol_sets();

                SymbolCollector::SymbolSetMap                                              symbolSetMap;
                SymbolCollector::ConcreteSymbolMap                                         concrete_symbol_ids;
                std::map< std::int32_t, ::ConcreteTypeAnalysis::Symbols::ConcreteSymbol* > concrete_symbols;
                std::map< std::int32_t, ::ConcreteTypeAnalysis::Concrete::Context* >       concrete_context_map;
                {
                    for ( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
                    {
                        SymbolCollector::ConcreteSymbolMap concreteSymbolMap;
                        SymbolCollector()( database, concreteSymbolMap, sourceFilePath );

                        auto iFind = symbolSetsMap.find( sourceFilePath );
                        VERIFY_RTE( iFind != symbolSetsMap.end() );
                        SymbolSet* pSymbolSet = iFind->second;

                        // recreate the symbol set
                        pSymbolSet
                            = database.construct< SymbolSet >( SymbolSet::Args{ pSymbolSet, concreteSymbolMap, {} } );
                        symbolSetMap.insert( { sourceFilePath, pSymbolSet } );
                        for ( auto& [ id, pSymbol ] : concreteSymbolMap )
                            concrete_symbol_ids.insert( std::make_pair( id, pSymbol ) );
                    }
                    SymbolCollector().labelNewContexts( concrete_symbol_ids );
                    SymbolCollector().collate(
                        symbolSetMap, concrete_symbol_ids, concrete_symbols, concrete_context_map );
                }
                // recreate the symbol table
                database.construct< SymbolTable >(
                    SymbolTable::Args( pSymbolTable, concrete_symbol_ids, concrete_symbols, concrete_context_map ) );
            }

            const task::FileHash fileHashCode = database.save_ConcreteTable_to_temp();
            m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
            m_environment.temp_to_real( symbolCompilationFile );
            m_environment.stash( symbolCompilationFile, determinant );
            succeeded( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_ConcreteTypeAnalysis( const TaskArguments&              taskArguments,
                                                const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_ConcreteTypeAnalysis >( taskArguments, manifestFilePath );
}

class Task_ConcreteTypeRollout : public BaseTask
{
public:
    Task_ConcreteTypeRollout( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath symbolAnalysisFilePath
            = m_environment.ConcreteTypeAnalysis_ConcreteTable( m_environment.project_manifest() );
        const mega::io::CompilationFilePath symbolRolloutFilePath
            = m_environment.ConcreteTypeRollout_PerSourceConcreteTable( m_sourceFilePath );
        start( taskProgress, "Task_ConcreteTypeRollout", symbolAnalysisFilePath.path(), symbolRolloutFilePath.path() );

        Task_ConcreteTypeAnalysis::InterfaceHashCodeGenerator hashGen( m_environment, m_toolChain.toolChainHash );
        const task::DeterminantHash                           determinant = hashGen( m_sourceFilePath );

        if ( m_environment.restore( symbolRolloutFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( symbolRolloutFilePath );
            cached( taskProgress );
            return;
        }

        using namespace ConcreteTypeRollout;

        Database database( m_environment, m_sourceFilePath );

        Symbols::SymbolSet* pSymbolSet = nullptr;
        {
            Symbols::SymbolTable* pSymbolTable
                = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );
            std::map< mega::io::megaFilePath, Symbols::SymbolSet* > symbolSets = pSymbolTable->get_symbol_sets();
            std::map< mega::io::megaFilePath, Symbols::SymbolSet* >::const_iterator iFind
                = symbolSets.find( m_sourceFilePath );
            VERIFY_RTE( iFind != symbolSets.end() );
            pSymbolSet = iFind->second;
        }
        // VERIFY_RTE( pSymbolSet->get_hash_code() == determinant.get() );

        for ( auto& [ pContext, id ] : pSymbolSet->get_context_concrete_ids() )
        {
            database.construct< Concrete::Context >( Concrete::Context::Args{ pContext, id } );
        }

        const task::FileHash fileHashCode = database.save_PerSourceConcreteTable_to_temp();
        m_environment.setBuildHashCode( symbolRolloutFilePath, fileHashCode );
        m_environment.temp_to_real( symbolRolloutFilePath );
        m_environment.stash( symbolRolloutFilePath, determinant );
        succeeded( taskProgress );
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_ConcreteTypeRollout( const TaskArguments&          taskArguments,
                                               const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ConcreteTypeRollout >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
