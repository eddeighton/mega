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

#include "database/model/SymbolAnalysis.hxx"
#include "database/model/SymbolAnalysisView.hxx"
#include "database/model/SymbolRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"

namespace mega
{
namespace compiler
{

class Task_SymbolAnalysis : public BaseTask
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
            return task::DeterminantHash( { toolChainHash, env.getBuildHashCode( env.ParserStage_AST( megaFilePath ) ),
                                            env.getBuildHashCode( env.InterfaceStage_Tree( megaFilePath ) ) } );
        }
    };

    Task_SymbolAnalysis( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
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

    using SymbolMap               = std::map< std::string, ::SymbolAnalysis::Symbols::Symbol* >;
    using TypeIDContextMap        = std::map< int32_t, SymbolAnalysis::Interface::IContext* >;
    using TypeIDDimensionTraitMap = std::map< int32_t, SymbolAnalysis::Interface::DimensionTrait* >;
    using SymbolIDMap             = std::map< int32_t, ::SymbolAnalysis::Symbols::Symbol* >;

    struct ContextDimensionSymbolSet
    {
        SymbolAnalysis::Symbols::SymbolSet*        pSymbolSet = nullptr;
        SymbolAnalysis::Interface::IContext*       pContext   = nullptr;
        SymbolAnalysis::Interface::DimensionTrait* pDimension = nullptr;
    };
    using TypeMap = std::multimap< int32_t, ContextDimensionSymbolSet >;

    struct SymbolCollector
    {
        SymbolAnalysis::Symbols::Symbol* findOrCreateSymbol( SymbolAnalysis::Database&           database,
                                                             SymbolAnalysis::Symbols::SymbolSet* pSymbolSet,
                                                             const std::string&                  strIdentifier,
                                                             SymbolMap&                          symbolMap ) const
        {
            using namespace SymbolAnalysis;

            Symbols::Symbol* pSymbol = nullptr;
            {
                SymbolMap::iterator iFind = symbolMap.find( strIdentifier );
                if ( iFind != symbolMap.end() )
                {
                    pSymbol = iFind->second;
                }
            }

            if ( !pSymbol )
            {
                // create the symbol in an un-labelled state i.e. with 0 for symbol id
                pSymbol = database.construct< Symbols::Symbol >( Symbols::Symbol::Args( strIdentifier, 0, {}, {} ) );
                symbolMap.insert( std::make_pair( strIdentifier, pSymbol ) );
            }

            // could already be there but insert anyway
            pSymbolSet->insert_symbols( strIdentifier, pSymbol );
            return pSymbol;
        }

        void operator()( SymbolAnalysis::Database& database, SymbolAnalysis::Symbols::SymbolSet* pSymbolSet,
                         SymbolMap& symbolMap, TypeMap& typeMap ) const
        {
            using namespace SymbolAnalysis;
            for ( Interface::IContext* pContext :
                  database.many< Interface::IContext >( pSymbolSet->get_source_file() ) )
            {
                Symbols::Symbol* pSymbol
                    = findOrCreateSymbol( database, pSymbolSet, pContext->get_identifier(), symbolMap );
                pSymbolSet->insert_context_symbols( pContext, pSymbol );
                // create type id in un-labelled state
                typeMap.insert( std::make_pair( 0, ContextDimensionSymbolSet{ pSymbolSet, pContext, nullptr } ) );
            }
            for ( Interface::DimensionTrait* pDimension :
                  database.many< Interface::DimensionTrait >( pSymbolSet->get_source_file() ) )
            {
                Symbols::Symbol* pSymbol
                    = findOrCreateSymbol( database, pSymbolSet, pDimension->get_id()->get_str(), symbolMap );
                pSymbolSet->insert_dimension_symbols( pDimension, pSymbol );
                // create type id in un-labelled state
                typeMap.insert( std::make_pair( 0, ContextDimensionSymbolSet{ pSymbolSet, nullptr, pDimension } ) );
            }
        }

        void labelNewSymbols( SymbolMap& symbolMap ) const
        {
            using namespace SymbolAnalysis;
            // symbol id are negative
            // type id are positive

            std::set< mega::I32 > symbolLabels;
            // std::set< mega::I32 > paranoiCheck;
            for ( SymbolMap::iterator i = symbolMap.begin(); i != symbolMap.end(); ++i )
            {
                Symbols::Symbol* pSymbol = i->second;
                if ( pSymbol->get_id() != 0 ) // zero means not set
                {
                    VERIFY_RTE( !symbolLabels.count( -pSymbol->get_id() ) );
                    symbolLabels.insert( -pSymbol->get_id() );
                }
            }
            std::set< mega::I32 >::iterator labelIter   = symbolLabels.begin();
            mega::I32                       szNextLabel = 1;
            for ( SymbolMap::iterator i = symbolMap.begin(); i != symbolMap.end(); ++i )
            {
                Symbols::Symbol* pSymbol = i->second;
                if ( pSymbol->get_id() == 0 )
                {
                    while ( labelIter != symbolLabels.end() )
                    {
                        if ( ( -*labelIter ) > szNextLabel )
                        {
                            break;
                        }
                        else
                        {
                            szNextLabel = *labelIter + 1;
                            ++labelIter;
                        }
                    }
                    pSymbol->set_id( -szNextLabel );
                    ++szNextLabel;
                }
            }
        }
        void labelNewTypes( const TypeMap& typeMap ) const
        {
            using namespace SymbolAnalysis;

            std::set< mega::I32 >                 existingTypeIDs;
            std::vector< ContextDimensionSymbolSet > unTyped;

            for ( TypeMap::const_iterator i = typeMap.begin(), iEnd = typeMap.end(); i != iEnd; ++i )
            {
                if ( i->first == 0 )
                {
                    unTyped.push_back( i->second );
                }
                else
                {
                    existingTypeIDs.insert( i->first );
                }
            }

            std::set< mega::I32 >::iterator labelIter  = existingTypeIDs.begin();
            mega::I32                       szNextType = 1;

            for ( std::vector< ContextDimensionSymbolSet >::iterator i = unTyped.begin(); i != unTyped.end(); ++i )
            {
                while ( labelIter != existingTypeIDs.end() )
                {
                    if ( *labelIter > szNextType )
                    {
                        break;
                    }
                    else
                    {
                        szNextType = *labelIter + 1;
                        ++labelIter;
                    }
                }
                if ( i->pContext )
                    i->pSymbolSet->insert_context_type_ids( i->pContext, szNextType );
                else if ( i->pDimension )
                    i->pSymbolSet->insert_dimension_type_ids( i->pDimension, szNextType );
                else
                    THROW_RTE( "Invalid type for type id" );
                ++szNextType;
            }
        }

        void collate( std::map< mega::io::megaFilePath, SymbolAnalysis::Symbols::SymbolSet* >& symbolSetMap,
                      SymbolMap& symbolMap, TypeIDContextMap& typeIDContextMap,
                      TypeIDDimensionTraitMap& typeIDDimensionTraitMap, SymbolIDMap& symbolIDMap ) const
        {
            using namespace SymbolAnalysis;
            for ( auto& [ filePath, pSymbolSet ] : symbolSetMap )
            {
                for ( auto& [ pContext, pSymbol ] : pSymbolSet->get_context_symbols() )
                {
                    pSymbol->push_back_contexts( pContext );
                    symbolIDMap.insert( std::make_pair( pSymbol->get_id(), pSymbol ) );
                }
                for ( auto& [ pDimension, pSymbol ] : pSymbolSet->get_dimension_symbols() )
                {
                    pSymbol->push_back_dimensions( pDimension );
                    symbolIDMap.insert( std::make_pair( pSymbol->get_id(), pSymbol ) );
                }
                for ( auto& [ pContext, typeID ] : pSymbolSet->get_context_type_ids() )
                {
                    VERIFY_RTE( typeIDContextMap.insert( std::make_pair( typeID, pContext ) ).second );
                }
                for ( auto& [ pDimensionTrait, typeID ] : pSymbolSet->get_dimension_type_ids() )
                {
                    VERIFY_RTE( typeIDDimensionTraitMap.insert( std::make_pair( typeID, pDimensionTrait ) ).second );
                }
            }
        }
    };

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath symbolCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( manifestFilePath );
        start( taskProgress, "Task_SymbolAnalysis", manifestFilePath.path(), symbolCompilationFile.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
            determinant ^= m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( sourceFilePath ) );
        }

        if ( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            cached( taskProgress );
            return;
        }

        InterfaceHashCodeGenerator hashCodeGenerator( m_environment, m_toolChain.toolChainHash );

        bool bReusedOldDatabase = false;
        if ( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
        {
            try
            {
                // attempt to reuse previous symbol analysis
                namespace Old = SymbolAnalysisView;
                namespace New = SymbolAnalysis;

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

                    // using SymbolMap = std::map< std::string, ::SymbolAnalysis::Symbols::Symbol* >;
                    SymbolMap               symbolMap;
                    TypeIDContextMap        typeIDContextMap;
                    TypeIDDimensionTraitMap typeIDDimensionTraitMap;
                    TypeMap                 typeMap;

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
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolMap,
                              &symbolSetMap,
                              &typeMap ]( OldSymbolsMap::const_iterator i, PathSet::const_iterator j ) -> bool
                            {
                                const task::DeterminantHash interfaceHash = hashCodeGenerator( *j );
                                if ( interfaceHash.get() == i->second->get_hash_code() )
                                {
                                    // since the code is NOT modified - can re use
                                    Old::Symbols::SymbolSet* pOldSymbolSet = i->second;
                                    {
                                        // load the old symbol set maps - which only contain pointers to stuff from the
                                        // same compilation file - so they are safe to use

                                        New::Symbols::SymbolSet* pNewSymbolSet
                                            = newDatabase.construct< New::Symbols::SymbolSet >(
                                                New::Symbols::SymbolSet::Args( {}, pOldSymbolSet->get_source_file(),
                                                                               pOldSymbolSet->get_hash_code(), {}, {},
                                                                               {}, {} ) );
                                        {
                                            for ( const auto& [ strSymbol, pOldSymbol ] : pOldSymbolSet->get_symbols() )
                                            {
                                                New::Symbols::Symbol* pNewSymbol = nullptr;
                                                {
                                                    SymbolMap::iterator iFind = symbolMap.find( strSymbol );
                                                    if ( iFind == symbolMap.end() )
                                                    {
                                                        pNewSymbol = newDatabase.construct< New::Symbols::Symbol >(
                                                            New::Symbols::Symbol::Args(
                                                                strSymbol, pOldSymbol->get_id(), {}, {} ) );
                                                        symbolMap.insert( std::make_pair( strSymbol, pNewSymbol ) );
                                                    }
                                                    else
                                                    {
                                                        pNewSymbol = iFind->second;
                                                        // could have already encountered the symbol in modified file
                                                        // but then it could already exist in previous file
                                                        // so set the symbol id
                                                        if ( pNewSymbol->get_id() == 0 )
                                                            pNewSymbol->set_id( pOldSymbol->get_id() );
                                                        VERIFY_RTE( pNewSymbol->get_id() == pOldSymbol->get_id() );
                                                    }
                                                }
                                                pNewSymbolSet->insert_symbols( strSymbol, pNewSymbol );
                                            }
                                        }
                                        {
                                            for ( const auto& [ pOldContext, typeID ] :
                                                  pOldSymbolSet->get_context_type_ids() )
                                            {
                                                New::Interface::IContext* pNewContext
                                                    = newDatabase.convert< New::Interface::IContext >( pOldContext );
                                                pNewSymbolSet->insert_context_type_ids( pNewContext, typeID );
                                                typeMap.insert(
                                                    std::make_pair( typeID,
                                                                    ContextDimensionSymbolSet{
                                                                        pNewSymbolSet, pNewContext, nullptr } ) );
                                            }
                                            for ( const auto& [ pOldDimension, typeID ] :
                                                  pOldSymbolSet->get_dimension_type_ids() )
                                            {
                                                New::Interface::DimensionTrait* pNewDimension
                                                    = newDatabase.convert< New::Interface::DimensionTrait >(
                                                        pOldDimension );
                                                pNewSymbolSet->insert_dimension_type_ids( pNewDimension, typeID );
                                                typeMap.insert(
                                                    std::make_pair( typeID,
                                                                    ContextDimensionSymbolSet{
                                                                        pNewSymbolSet, nullptr, pNewDimension } ) );
                                            }
                                        }
                                        for ( const auto& [ pOldContext, pOldSymbol ] :
                                              pOldSymbolSet->get_context_symbols() )
                                        {
                                            SymbolMap::iterator iFind = symbolMap.find( pOldSymbol->get_symbol() );
                                            VERIFY_RTE( iFind != symbolMap.end() );
                                            pNewSymbolSet->insert_context_symbols(
                                                newDatabase.convert< New::Interface::IContext >( pOldContext ),
                                                iFind->second );
                                        }
                                        for ( const auto& [ pOldDimension, pOldSymbol ] :
                                              pOldSymbolSet->get_dimension_symbols() )
                                        {
                                            SymbolMap::iterator iFind = symbolMap.find( pOldSymbol->get_symbol() );
                                            VERIFY_RTE( iFind != symbolMap.end() );
                                            pNewSymbolSet->insert_dimension_symbols(
                                                newDatabase.convert< New::Interface::DimensionTrait >( pOldDimension ),
                                                iFind->second );
                                        }
                                        symbolSetMap.insert( std::make_pair( *j, pNewSymbolSet ) );
                                    }

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
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolMap,
                              &symbolSetMap, &typeMap ]( PathSet::const_iterator j )
                            {
                                // a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath megaFilePath  = *j;
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );

                                New::Symbols::SymbolSet* pSymbolSet
                                    = newDatabase.construct< New::Symbols::SymbolSet >( New::Symbols::SymbolSet::Args(
                                        {}, megaFilePath, interfaceHash.get(), {}, {}, {}, {} ) );

                                SymbolCollector()( newDatabase, pSymbolSet, symbolMap, typeMap );
                                symbolSetMap.insert( std::make_pair( megaFilePath, pSymbolSet ) );

                                // std::ostringstream os;
                                // os << "\tAdded symbols for: " << megaFilePath.path().string();
                                // taskProgress.msg( os.str() );
                            },

                            // const Updated& updatesNeeded
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolMap,
                              &symbolSetMap, &typeMap ]( OldSymbolsMap::const_iterator i )
                            {
                                // Old::Symbols::SymbolSet* pOldSymbolSet = i->second;
                                //  a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath megaFilePath  = i->first;
                                const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );

                                New::Symbols::SymbolSet* pSymbolSet
                                    = newDatabase.construct< New::Symbols::SymbolSet >( New::Symbols::SymbolSet::Args(
                                        {}, megaFilePath, interfaceHash.get(), {}, {}, {}, {} ) );

                                SymbolCollector()( newDatabase, pSymbolSet, symbolMap, typeMap );
                                symbolSetMap.insert( std::make_pair( megaFilePath, pSymbolSet ) );

                                // std::ostringstream os;
                                // os << "\tUpdated symbols for: " << megaFilePath.path().string();
                                // taskProgress.msg( os.str() );
                            } );
                    }
                    SymbolIDMap symbolIDMap;
                    SymbolCollector().labelNewSymbols( symbolMap );
                    SymbolCollector().labelNewTypes( typeMap );
                    SymbolCollector().collate(
                        symbolSetMap, symbolMap, typeIDContextMap, typeIDDimensionTraitMap, symbolIDMap );
                    newDatabase.construct< New::Symbols::SymbolTable >( New::Symbols::SymbolTable::Args(
                        symbolSetMap, symbolMap, typeIDContextMap, typeIDDimensionTraitMap, symbolIDMap ) );
                }

                const task::FileHash fileHashCode = newDatabase.save_SymbolTable_to_temp();
                m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
                m_environment.temp_to_real( symbolCompilationFile );
                m_environment.stash( symbolCompilationFile, determinant );

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
            using namespace SymbolAnalysis;
            using namespace SymbolAnalysis::Symbols;

            Database database( m_environment, manifestFilePath );
            {
                SymbolMap               symbolMap;
                TypeIDContextMap        typeIDContextMap;
                TypeIDDimensionTraitMap typeIDDimensionTraitMap;
                SymbolIDMap             symbolIDMap;

                TypeMap                                                 typeMap;
                std::map< mega::io::megaFilePath, Symbols::SymbolSet* > symbolSetMap;
                {
                    for ( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
                    {
                        const task::DeterminantHash interfaceHash = hashCodeGenerator( sourceFilePath );
                        using OldSymbolsMap            = std::map< mega::io::megaFilePath, Symbols::SymbolSet* >;
                        Symbols::SymbolSet* pSymbolSet = database.construct< Symbols::SymbolSet >(
                            Symbols::SymbolSet::Args( {}, sourceFilePath, interfaceHash.get(), {}, {}, {}, {} ) );
                        SymbolCollector()( database, pSymbolSet, symbolMap, typeMap );
                        symbolSetMap.insert( std::make_pair( sourceFilePath, pSymbolSet ) );
                    }
                    SymbolCollector().labelNewSymbols( symbolMap );
                    SymbolCollector().labelNewTypes( typeMap );
                    SymbolCollector().collate(
                        symbolSetMap, symbolMap, typeIDContextMap, typeIDDimensionTraitMap, symbolIDMap );
                }
                database.construct< SymbolTable >( SymbolTable::Args(
                    symbolSetMap, symbolMap, typeIDContextMap, typeIDDimensionTraitMap, symbolIDMap ) );
            }

            const task::FileHash fileHashCode = database.save_SymbolTable_to_temp();
            m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
            m_environment.temp_to_real( symbolCompilationFile );
            m_environment.stash( symbolCompilationFile, determinant );
            succeeded( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_SymbolAnalysis( const TaskArguments&              taskArguments,
                                          const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_SymbolAnalysis >( taskArguments, manifestFilePath );
}

class Task_SymbolRollout : public BaseTask
{
public:
    Task_SymbolRollout( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath symbolAnalysisFilePath
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );
        const mega::io::CompilationFilePath symbolRolloutFilePath
            = m_environment.SymbolRollout_PerSourceSymbols( m_sourceFilePath );
        start( taskProgress, "Task_SymbolRollout", m_sourceFilePath.path(), symbolRolloutFilePath.path() );

        Task_SymbolAnalysis::InterfaceHashCodeGenerator hashGen( m_environment, m_toolChain.toolChainHash );
        const task::DeterminantHash                     determinant = hashGen( m_sourceFilePath );

        if ( m_environment.restore( symbolRolloutFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( symbolRolloutFilePath );
            cached( taskProgress );
            return;
        }

        using namespace SymbolRollout;

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
        VERIFY_RTE( pSymbolSet->get_hash_code() == determinant.get() );

        for ( auto& [ pContext, pSymbol ] : pSymbolSet->get_context_symbols() )
        {
            database.construct< Interface::IContext >( Interface::IContext::Args( pContext, pSymbol->get_id() ) );
        }
        for ( auto& [ pDimension, pSymbol ] : pSymbolSet->get_dimension_symbols() )
        {
            database.construct< Interface::DimensionTrait >(
                Interface::DimensionTrait::Args( pDimension, pSymbol->get_id() ) );
        }
        for ( auto& [ pContext, typeID ] : pSymbolSet->get_context_type_ids() )
        {
            pContext->set_type_id( typeID );
        }
        for ( auto& [ pDimension, typeID ] : pSymbolSet->get_dimension_type_ids() )
        {
            pDimension->set_type_id( typeID );
        }

        const task::FileHash fileHashCode = database.save_PerSourceSymbols_to_temp();
        m_environment.setBuildHashCode( symbolRolloutFilePath, fileHashCode );
        m_environment.temp_to_real( symbolRolloutFilePath );
        m_environment.stash( symbolRolloutFilePath, determinant );
        succeeded( taskProgress );
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_SymbolRollout( const TaskArguments&          taskArguments,
                                         const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_SymbolRollout >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
