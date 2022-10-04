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

#include "database/model/ConcreteTypeAnalysis.hxx"
#include "database/model/ConcreteTypeAnalysisView.hxx"
#include "database/model/ConcreteTypeRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/types/sources.hpp"

namespace mega::compiler
{

class Task_ConcreteTypeAnalysis : public BaseTask
{
    const mega::io::Manifest m_manifest;

public:
    struct ConcreteHashCodeGenerator
    {
        const mega::io::StashEnvironment& env;
        task::DeterminantHash             toolChainHash;
        ConcreteHashCodeGenerator( const mega::io::StashEnvironment& env, task::DeterminantHash toolChainHash )
            : env( env )
            , toolChainHash( toolChainHash )
        {
        }
        inline task::DeterminantHash operator()( const mega::io::megaFilePath& sourceFilePath ) const
        {
            return task::DeterminantHash(
                toolChainHash,
                env.getBuildHashCode( env.ConcreteStage_Concrete( sourceFilePath ) ),
                env.getBuildHashCode( env.ParserStage_AST( sourceFilePath ) ),
                env.getBuildHashCode( env.InterfaceStage_Tree( sourceFilePath ) ),
                env.getBuildHashCode( env.InterfaceAnalysisStage_Clang( sourceFilePath ) ),
                env.getBuildHashCode( env.SymbolRollout_PerSourceSymbols( sourceFilePath ) ) );
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
        PathSet srcFiles;
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            srcFiles.insert( sourceFilePath );
        }
        return srcFiles;
    }

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

        using SymbolSetMap      = std::map< mega::io::megaFilePath, ConcreteTypeAnalysis::Symbols::SymbolSet* >;
        using SymbolIDPath      = std::vector< mega::TypeID >;
        using IDPathToSymbolMap = std::map< SymbolIDPath, ConcreteTypeAnalysis::Symbols::ConcreteSymbol* >;
        using IDToSymbolMap     = std::map< mega::TypeID, ::ConcreteTypeAnalysis::Symbols::ConcreteSymbol* >;
        using IDToContextMap    = std::map< mega::TypeID, ::ConcreteTypeAnalysis::Concrete::Context* >;

        ConcreteTypeAnalysis::Symbols::ConcreteSymbol*
        findOrCreateSymbol( ConcreteTypeAnalysis::Database&          database,
                            IDPathToSymbolMap&                       idPathToSymbolMap,
                            ConcreteTypeAnalysis::Concrete::Context* pContext ) const
        {
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;

            SymbolIDPath symbolIDPath;
            for ( const Concrete::Context* pContext : getContextPath( pContext ) )
                symbolIDPath.push_back( pContext->get_interface()->get_symbol() );

            ConcreteSymbol* pSymbol = nullptr;
            {
                IDPathToSymbolMap::iterator iFind = idPathToSymbolMap.find( symbolIDPath );
                VERIFY_RTE( iFind == idPathToSymbolMap.end() );
                /*if ( iFind != idPathToSymbolMap.end() )
                {
                    pSymbol = iFind->second;
                }*/
            }

            if ( !pSymbol )
            {
                // create the symbol in an un-labelled state i.e. with 0 for symbol id
                pSymbol = database.construct< ConcreteSymbol >(
                    ConcreteSymbol::Args{ symbolIDPath, 0, std::optional< Concrete::Context* >{ pContext },
                                          std::optional< Concrete::Dimensions::User* >{},
                                          std::optional< Concrete::Dimensions::LinkReference* >{},
                                          std::optional< Concrete::Dimensions::Allocation* >{} } );

                idPathToSymbolMap.insert( std::make_pair( symbolIDPath, pSymbol ) );
            }

            return pSymbol;
        }

        void operator()( ConcreteTypeAnalysis::Database& database,
                         IDPathToSymbolMap&              idPathToSymbolMap,
                         const mega::io::megaFilePath&   sourceFilePath ) const
        {
            using namespace ConcreteTypeAnalysis;
            for ( Concrete::Context* pContext : database.many< Concrete::Context >( sourceFilePath ) )
            {
                Symbols::ConcreteSymbol* pSymbol = findOrCreateSymbol( database, idPathToSymbolMap, pContext );
            }
        }
        void labelNewContexts( IDPathToSymbolMap& globalIDPathToSymbolMap )
        {
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;

            bool bFoundRoot = false;

            std::set< mega::TypeID > symbolLabels;
            {
                for ( IDPathToSymbolMap::iterator i = globalIDPathToSymbolMap.begin();
                      i != globalIDPathToSymbolMap.end();
                      ++i )
                {
                    ConcreteSymbol* pSymbol = i->second;

                    // pSymbol->get_context()->get_interface()
                    //  if the symbol is the root then set to 1

                    if ( pSymbol->get_context().has_value() )
                    {
                        if ( dynamic_database_cast< Interface::Root >(
                                 pSymbol->get_context().value()->get_interface()->get_parent() ) )
                        {
                            if ( pSymbol->get_context().value()->get_interface()->get_identifier() == ROOT_TYPE_NAME )
                            {
                                VERIFY_RTE_MSG( !bFoundRoot, "Found duplicate Roots" );
                                pSymbol->set_id( mega::ROOT_TYPE_ID );
                                bFoundRoot = true;
                            }
                        }
                    }

                    if ( pSymbol->get_id() != 0 ) // zero means not set
                    {
                        VERIFY_RTE( !symbolLabels.count( pSymbol->get_id() ) );
                        symbolLabels.insert( pSymbol->get_id() );
                    }
                }
            }
            VERIFY_RTE_MSG( bFoundRoot, "Failed to find Root symbol" );

            std::set< mega::TypeID >::iterator labelIter   = symbolLabels.begin();
            mega::TypeID                       szNextLabel = 2;
            for ( IDPathToSymbolMap::iterator i = globalIDPathToSymbolMap.begin(); i != globalIDPathToSymbolMap.end();
                  ++i )
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
                        VERIFY_RTE_MSG( szNextLabel > 0, "Concrete Type ID overflowed" );
                    }
                    pSymbol->set_id( szNextLabel );
                    ++szNextLabel;
                    VERIFY_RTE_MSG( szNextLabel > 0, "Concrete Type ID overflowed" );
                }
            }
        }
        void collate( const SymbolSetMap&              symbolSetMap,
                      SymbolCollector::IDToSymbolMap&  idToSymbolMap,
                      SymbolCollector::IDToContextMap& idToContextMap )
        {
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;

            for ( auto& [ filePath, pSymbolSet ] : symbolSetMap )
            {
                for ( const auto& [ id, pSymbol ] : pSymbolSet->get_concrete_symbols() )
                {
                    VERIFY_RTE( pSymbol->get_id() != 0 );
                    if ( pSymbol->get_context().has_value() )
                    {
                        pSymbolSet->insert_context_concrete_ids( pSymbol->get_context().value(), pSymbol->get_id() );
                        VERIFY_RTE(
                            idToContextMap.insert( { pSymbol->get_id(), pSymbol->get_context().value() } ).second );
                    }
                    VERIFY_RTE( idToSymbolMap.insert( { pSymbol->get_id(), pSymbol } ).second );
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

        // std::ostringstream os;
        task::DeterminantHash determinant(
            m_toolChain.toolChainHash,
            m_environment.getBuildHashCode( m_environment.SymbolAnalysis_SymbolTable( manifestFilePath ) ) );
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            const task::DeterminantHash hashCode(
                m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( sourceFilePath ) ),
                m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) ),
                m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( sourceFilePath ) ),
                m_environment.getBuildHashCode( m_environment.InterfaceAnalysisStage_Clang( sourceFilePath ) ) );

            // os << sourceFilePath.path() << " : " << hashCode.toHexString() << std::endl;
            determinant ^= hashCode;
        }
        //{
        //    os << "determinant : " << symbolCompilationFile.path() << " : " << determinant.toHexString();
        //    msg( taskProgress, os.str() );
        //}

        if ( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            cached( taskProgress );
            return;
        }

        ConcreteHashCodeGenerator hashCodeGenerator( m_environment, m_toolChain.toolChainHash );

        bool bReusedOldDatabase = false;
        if ( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
        {
            try
            {
                // attempt to reuse previous symbol analysis
                namespace Old = ConcreteTypeAnalysisView;
                namespace New = ConcreteTypeAnalysis;

                New::Database newDatabase( m_environment, manifestFilePath );
                {
                    New::Symbols::SymbolTable* pSymbolTable
                        = newDatabase.one< New::Symbols::SymbolTable >( manifestFilePath );

                    // load the archived database
                    io::ArchiveEnvironment archiveEnvironment( m_environment.DatabaseArchive() );
                    Old::Database          oldDatabase( archiveEnvironment, archiveEnvironment.project_manifest() );

                    const Old::Symbols::SymbolTable* pOldAnalysis
                        = oldDatabase.one< Old::Symbols::SymbolTable >( manifestFilePath );
                    VERIFY_RTE( pOldAnalysis );
                    using OldSymbolsMap                = std::map< mega::io::megaFilePath, Old::Symbols::SymbolSet* >;
                    const OldSymbolsMap oldSymbolsMaps = pOldAnalysis->get_symbol_sets();
                    const PathSet       sourceFiles    = getSortedSourceFiles();

                    // oldSymbolSetsMap is really the new_old - i.e. the one from the new database but that
                    // needs to be reconstructed in to the later stage type
                    const SymbolCollector::SymbolSetMap oldSymbolSetsMap = pSymbolTable->get_symbol_sets();
                    SymbolCollector::SymbolSetMap       newSymbolSetsMap;
                    SymbolCollector::IDPathToSymbolMap  globalIDPathToSymbolMap;

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
                            [ &hashCodeGenerator, &newDatabase, &taskProgress, &oldSymbolSetsMap, &newSymbolSetsMap,
                              &globalIDPathToSymbolMap ](
                                OldSymbolsMap::const_iterator i, PathSet::const_iterator j ) -> bool
                            {
                                const task::DeterminantHash concreteHash = hashCodeGenerator( *j );
                                if ( concreteHash.get() == i->second->get_concrete_hash_code() )
                                {
                                    // since the code is NOT modified - can re use
                                    const mega::io::megaFilePath sourceFilePath = *j;
                                    Old::Symbols::SymbolSet*     pOldSymbolSet  = i->second;

                                    SymbolCollector::IDPathToSymbolMap localSymbolMap;
                                    for ( auto& [ id, pSymbol ] : pOldSymbolSet->get_concrete_symbols() )
                                    {
                                        if ( pSymbol->get_context().has_value() )
                                        {
                                            New::Concrete::Context* pNewContext
                                                = newDatabase.convert< New::Concrete::Context >(
                                                    pSymbol->get_context().value() );

                                            // converted symbol will retain old concrete id
                                            VERIFY_RTE( pSymbol->get_id() != 0 );
                                            New::Symbols::ConcreteSymbol* pNewSymbol
                                                = newDatabase.construct< New::Symbols::ConcreteSymbol >(
                                                    New::Symbols::ConcreteSymbol::Args{
                                                        pSymbol->get_id_sequence(), pSymbol->get_id(), pNewContext,
                                                        std::nullopt, std::nullopt, std::nullopt } );

                                            VERIFY_RTE( localSymbolMap.insert( { id, pNewSymbol } ).second );
                                            VERIFY_RTE( globalIDPathToSymbolMap.insert( { id, pNewSymbol } ).second );
                                        }
                                    }

                                    auto iFind = oldSymbolSetsMap.find( sourceFilePath );
                                    VERIFY_RTE( iFind != oldSymbolSetsMap.end() );
                                    New::Symbols::SymbolSet* pSymbolSet = iFind->second;

                                    pSymbolSet = newDatabase.construct< New::Symbols::SymbolSet >(
                                        New::Symbols::SymbolSet::Args{
                                            pSymbolSet, concreteHash.get(), localSymbolMap, {}, {}, {}, {} } );
                                    VERIFY_RTE( newSymbolSetsMap.insert( { sourceFilePath, pSymbolSet } ).second );

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
                                // a source file has been removed
                            },

                            // const Addition& add
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &oldSymbolSetsMap,
                              &newSymbolSetsMap, &globalIDPathToSymbolMap ]( PathSet::const_iterator j )
                            {
                                // a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath sourceFilePath = *j;
                                const task::DeterminantHash  concreteHash   = hashCodeGenerator( sourceFilePath );

                                SymbolCollector::IDPathToSymbolMap idPathToSymbolMap;
                                SymbolCollector()( newDatabase, idPathToSymbolMap, sourceFilePath );

                                auto iFind = oldSymbolSetsMap.find( sourceFilePath );
                                VERIFY_RTE( iFind != oldSymbolSetsMap.end() );
                                New::Symbols::SymbolSet* pSymbolSet = iFind->second;

                                pSymbolSet
                                    = newDatabase.construct< New::Symbols::SymbolSet >( New::Symbols::SymbolSet::Args{
                                        pSymbolSet, concreteHash.get(), idPathToSymbolMap, {}, {}, {}, {} } );
                                VERIFY_RTE( newSymbolSetsMap.insert( { sourceFilePath, pSymbolSet } ).second );

                                for ( auto& [ id, pSymbol ] : idPathToSymbolMap )
                                {
                                    VERIFY_RTE(
                                        globalIDPathToSymbolMap.insert( std::make_pair( id, pSymbol ) ).second );
                                }
                            },

                            // const Updated& updatesNeeded
                            [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &oldSymbolSetsMap,
                              &newSymbolSetsMap, &globalIDPathToSymbolMap ]( OldSymbolsMap::const_iterator i )
                            {
                                // Old::Symbols::SymbolSet* pOldSymbolSet = i->second;
                                //  a new source file is added so must analysis from the ground up
                                const mega::io::megaFilePath sourceFilePath = i->first;
                                const task::DeterminantHash  concreteHash   = hashCodeGenerator( sourceFilePath );

                                SymbolCollector::IDPathToSymbolMap idPathToSymbolMap;
                                SymbolCollector()( newDatabase, idPathToSymbolMap, sourceFilePath );

                                auto iFind = oldSymbolSetsMap.find( sourceFilePath );
                                VERIFY_RTE( iFind != oldSymbolSetsMap.end() );
                                New::Symbols::SymbolSet* pSymbolSet = iFind->second;

                                pSymbolSet
                                    = newDatabase.construct< New::Symbols::SymbolSet >( New::Symbols::SymbolSet::Args{
                                        pSymbolSet, concreteHash.get(), idPathToSymbolMap, {}, {}, {}, {} } );
                                VERIFY_RTE( newSymbolSetsMap.insert( { sourceFilePath, pSymbolSet } ).second );

                                for ( auto& [ id, pSymbol ] : idPathToSymbolMap )
                                {
                                    VERIFY_RTE(
                                        globalIDPathToSymbolMap.insert( std::make_pair( id, pSymbol ) ).second );
                                }
                            } );
                    }

                    SymbolCollector::IDToSymbolMap  idToSymbolMap;
                    SymbolCollector::IDToContextMap idToContextMap;

                    SymbolCollector().labelNewContexts( globalIDPathToSymbolMap );
                    SymbolCollector().collate( newSymbolSetsMap, idToSymbolMap, idToContextMap );

                    newDatabase.construct< New::Symbols::SymbolTable >( New::Symbols::SymbolTable::Args(
                        pSymbolTable, globalIDPathToSymbolMap, idToSymbolMap, idToContextMap ) );
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
            }
        }

        if ( !bReusedOldDatabase )
        {
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;

            Database database( m_environment, manifestFilePath );
            {
                Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( manifestFilePath );
                const SymbolCollector::SymbolSetMap oldSymbolSetsMap = pSymbolTable->get_symbol_sets();

                SymbolCollector::SymbolSetMap      newSymbolSetsMap;
                SymbolCollector::IDPathToSymbolMap globalIDPathToSymbolMap;
                SymbolCollector::IDToSymbolMap     idToSymbolMap;
                SymbolCollector::IDToContextMap    idToContextMap;
                {
                    for ( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
                    {
                        const task::DeterminantHash concreteHash = hashCodeGenerator( sourceFilePath );

                        SymbolCollector::IDPathToSymbolMap idPathToSymbolMap;
                        SymbolCollector()( database, idPathToSymbolMap, sourceFilePath );

                        auto iFind = oldSymbolSetsMap.find( sourceFilePath );
                        VERIFY_RTE( iFind != oldSymbolSetsMap.end() );
                        SymbolSet* pSymbolSet = iFind->second;

                        // recreate the symbol set
                        pSymbolSet = database.construct< SymbolSet >(
                            SymbolSet::Args{ pSymbolSet, concreteHash.get(), idPathToSymbolMap, {}, {}, {}, {} } );
                        newSymbolSetsMap.insert( { sourceFilePath, pSymbolSet } );
                        for ( auto& [ id, pSymbol ] : idPathToSymbolMap )
                        {
                            const bool bResult = globalIDPathToSymbolMap.insert( std::make_pair( id, pSymbol ) ).second;
                            if ( !bResult )
                            {
                                if( pSymbol->get_context().has_value() )
                                {
                                    const auto         path = SymbolCollector::getContextPath( pSymbol->get_context().value() );
                                    std::ostringstream os;
                                    bool               bFirst = true;
                                    for ( const auto p : path )
                                    {
                                        if ( bFirst )
                                            bFirst = false;
                                        else
                                            os << "::";
                                        os << p->get_interface()->get_identifier();
                                    }
                                    THROW_RTE( "Duplicate symbols found: " << os.str() );
                                }
                            }
                        }
                    }
                    SymbolCollector().labelNewContexts( globalIDPathToSymbolMap );
                    SymbolCollector().collate( newSymbolSetsMap, idToSymbolMap, idToContextMap );
                }
                // recreate the symbol table
                database.construct< SymbolTable >(
                    SymbolTable::Args( pSymbolTable, globalIDPathToSymbolMap, idToSymbolMap, idToContextMap ) );
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
        start( taskProgress, "Task_ConcreteTypeRollout", m_sourceFilePath.path(), symbolRolloutFilePath.path() );

        Task_ConcreteTypeAnalysis::ConcreteHashCodeGenerator hashGen( m_environment, m_toolChain.toolChainHash );
        const task::DeterminantHash                          determinant = hashGen( m_sourceFilePath );

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

} // namespace mega::compiler
