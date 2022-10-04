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

namespace mega::compiler
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
    PathSet getSortedSourceFiles( const io::Manifest& manifest ) const
    {
        PathSet sourceFiles;
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }
    /*
        using SymbolMap               = std::map< std::string, ::SymbolAnalysis::Symbols::Symbol* >;
        using TypeIDContextMap        = std::map< mega::TypeID, SymbolAnalysis::Interface::IContext* >;
        using TypeIDDimensionTraitMap = std::map< mega::TypeID, SymbolAnalysis::Interface::DimensionTrait* >;
        using SymbolIDMap             = std::map< mega::TypeID, ::SymbolAnalysis::Symbols::Symbol* >;

        struct ContextDimensionSymbolSet
        {
            SymbolAnalysis::Symbols::SymbolSet*        pSymbolSet = nullptr;
            SymbolAnalysis::Interface::IContext*       pContext   = nullptr;
            SymbolAnalysis::Interface::DimensionTrait* pDimension = nullptr;
        };
        using TypeMap = std::multimap< mega::TypeID, ContextDimensionSymbolSet >;

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
                    pSymbol = database.construct< Symbols::Symbol >( Symbols::Symbol::Args( strIdentifier, 0, {}, {} )
       ); symbolMap.insert( std::make_pair( strIdentifier, pSymbol ) );
                }

                // could already be there but insert anyway
                pSymbolSet->insert_symbols( strIdentifier, pSymbol );
                return pSymbol;
            }
            SymbolAnalysis::Symbols::Symbol* findOrCreateSymbol( SymbolAnalysis::Database&           database,
                                                                 SymbolAnalysis::Symbols::SymbolSet* pSymbolSet,
                                                                 const std::string&                  strIdentifier,
                                                                 SymbolMap&                          symbolMap,
                                                                 SymbolID                            symbolID ) const
            {
                using namespace SymbolAnalysis;
                Symbols::Symbol* pSymbol = findOrCreateSymbol( database, pSymbolSet, strIdentifier, symbolMap );
                VERIFY_RTE( pSymbol->get_id() == 0 );
                pSymbol->set_id( symbolID );
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

                std::set< mega::TypeID > symbolLabels;
                // std::set< mega::TypeID > paranoiCheck;
                for ( SymbolMap::iterator i = symbolMap.begin(); i != symbolMap.end(); ++i )
                {
                    Symbols::Symbol* pSymbol = i->second;
                    if ( pSymbol->get_id() != 0 ) // zero means not set
                    {
                        VERIFY_RTE( !symbolLabels.count( -pSymbol->get_id() ) );
                        symbolLabels.insert( -pSymbol->get_id() );
                    }
                }
                std::set< mega::TypeID >::iterator labelIter   = symbolLabels.begin();
                mega::TypeID                       szNextLabel = 1;
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
                                VERIFY_RTE_MSG( szNextLabel > 0, "Type ID overflowed" );
                            }
                        }
                        pSymbol->set_id( -szNextLabel );
                        ++szNextLabel;
                        VERIFY_RTE_MSG( szNextLabel > 0, "Type ID overflowed" );
                    }
                }
            }
            void labelNewTypes( const TypeMap& typeMap ) const
            {
                using namespace SymbolAnalysis;

                std::set< mega::TypeID >                 existingTypeIDs;
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

                std::set< mega::TypeID >::iterator labelIter  = existingTypeIDs.begin();
                mega::TypeID                       szNextType = 1;

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
                        VERIFY_RTE( typeIDDimensionTraitMap.insert( std::make_pair( typeID, pDimensionTrait ) ).second
       );
                    }
                }
            }
        };*/

    template < typename SymbolTypeID   = SymbolAnalysis::Symbols::SymbolTypeID,
               typename ContextGroup   = SymbolAnalysis::Interface::ContextGroup,
               typename IContext       = SymbolAnalysis::Interface::IContext,
               typename DimensionTrait = SymbolAnalysis::Interface::DimensionTrait >
    struct TypeIDSequenceGen
    {
        using NewSymbolNames = std::map< std::string, SymbolTypeID* >;
        using CastFunctor    = std::function< IContext*( ContextGroup* ) >;
        const NewSymbolNames& symbolNames;
        CastFunctor           castFunctor;
        TypeIDSequenceGen( const NewSymbolNames& symbolNames, CastFunctor castFunctor )
            : symbolNames( symbolNames )
            , castFunctor( std::move( castFunctor ) )
        {
        }

        TypeID getTypeID( const std::string& strID ) const
        {
            auto iFind = symbolNames.find( strID );
            VERIFY_RTE( iFind != symbolNames.end() );
            const SymbolTypeID* pSymbolTypeID = iFind->second;
            return pSymbolTypeID->get_id();
        }

        void recurse( ContextGroup* pContextGroup, TypeIDSequence& sequence ) const
        {
            if ( auto pContext = castFunctor( pContextGroup ) )
            {
                sequence.push_back( getTypeID( pContext->get_identifier() ) );
                recurse( pContext->get_parent(), sequence );
            }
        }

        TypeIDSequence operator()( IContext* pContext ) const
        {
            TypeIDSequence sequence{ getTypeID( pContext->get_identifier() ) };
            recurse( pContext->get_parent(), sequence );
            std::reverse( sequence.begin(), sequence.end() );
            return sequence;
        }
        TypeIDSequence operator()( DimensionTrait* pDimension ) const
        {
            TypeIDSequence sequence{ getTypeID( pDimension->get_id()->get_str() ) };
            recurse( pDimension->get_parent(), sequence );
            std::reverse( sequence.begin(), sequence.end() );
            return sequence;
        }
    };

    SymbolAnalysis::Symbols::SymbolTable* calculateSymbolTable(
        SymbolAnalysis::Database&                                                        newDatabase,
        const std::map< std::string, SymbolAnalysisView::Symbols::SymbolTypeID* >&       oldSymbolTypeIDs,
        const std::map< TypeIDSequence, SymbolAnalysisView::Symbols::InterfaceTypeID* >& oldInterfaceTypeIDSequences )
    {
        namespace Old = SymbolAnalysisView;
        namespace New = SymbolAnalysis;

        const PathSet newSourceFiles = getSortedSourceFiles( m_manifest );
        // const PathSet oldSourceFiles = getSortedSourceFiles( oldManifest );

        using NewSymbolNames   = std::map< std::string, New::Symbols::SymbolTypeID* >;
        using NewSymbolTypeIDs = std::map< mega::TypeID, New::Symbols::SymbolTypeID* >;

        NewSymbolNames   newSymbolNames;
        NewSymbolTypeIDs newSymbolTypeIDs;
        {
            // construct all symbols - reusing old symbol IDs across whole program
            std::set< TypeID > usedTypeIDs;
            for ( const mega::io::megaFilePath& newSourceFile : newSourceFiles )
            {
                for ( New::Interface::IContext* pContext :
                      newDatabase.many< New::Interface::IContext >( newSourceFile ) )
                {
                    const auto strSymbol = pContext->get_identifier();
                    auto       iFind     = oldSymbolTypeIDs.find( strSymbol );
                    if ( iFind != oldSymbolTypeIDs.end() )
                    {
                        Old::Symbols::SymbolTypeID* pOldSymbol = iFind->second;

                        auto jFind = newSymbolNames.find( strSymbol );
                        if ( jFind != newSymbolNames.end() )
                        {
                            auto pNewSymbol = jFind->second;
                            pNewSymbol->push_back_contexts( pContext );
                            VERIFY_RTE_MSG( pNewSymbol->get_id() == pOldSymbol->get_id(),
                                            "Mismatch between old and new symbol id" );
                        }
                        else
                        {
                            auto pNewSymbol = newDatabase.construct< New::Symbols::SymbolTypeID >(
                                New::Symbols::SymbolTypeID::Args{ strSymbol, pOldSymbol->get_id(), { pContext }, {} } );
                            VERIFY_RTE( usedTypeIDs.insert( pOldSymbol->get_id() ).second );
                            newSymbolNames.insert( { strSymbol, pNewSymbol } );
                            newSymbolTypeIDs.insert( { pOldSymbol->get_id(), pNewSymbol } );
                        }
                    }
                    else
                    {
                        auto jFind = newSymbolNames.find( strSymbol );
                        if ( jFind != newSymbolNames.end() )
                        {
                            auto pNewSymbol = jFind->second;
                            pNewSymbol->push_back_contexts( pContext );
                        }
                        else
                        {
                            auto pNewSymbol = newDatabase.construct< New::Symbols::SymbolTypeID >(
                                New::Symbols::SymbolTypeID::Args{ strSymbol, 0U, { pContext }, {} } );
                            newSymbolNames.insert( { strSymbol, pNewSymbol } );
                        }
                    }
                }

                for ( New::Interface::DimensionTrait* pDimension :
                      newDatabase.many< New::Interface::DimensionTrait >( newSourceFile ) )
                {
                    const auto strSymbol = pDimension->get_id()->get_str();
                    auto       iFind     = oldSymbolTypeIDs.find( strSymbol );
                    if ( iFind != oldSymbolTypeIDs.end() )
                    {
                        Old::Symbols::SymbolTypeID* pOldSymbol = iFind->second;

                        auto jFind = newSymbolNames.find( strSymbol );
                        if ( jFind != newSymbolNames.end() )
                        {
                            auto pNewSymbol = jFind->second;
                            pNewSymbol->push_back_dimensions( pDimension );
                            VERIFY_RTE_MSG( pNewSymbol->get_id() == pOldSymbol->get_id(),
                                            "Mismatch between old and new symbol id" );
                        }
                        else
                        {
                            auto pNewSymbol
                                = newDatabase.construct< New::Symbols::SymbolTypeID >( New::Symbols::SymbolTypeID::Args{
                                    strSymbol, pOldSymbol->get_id(), {}, { pDimension } } );
                            VERIFY_RTE( usedTypeIDs.insert( pOldSymbol->get_id() ).second );
                            newSymbolNames.insert( { strSymbol, pNewSymbol } );
                            newSymbolTypeIDs.insert( { pOldSymbol->get_id(), pNewSymbol } );
                        }
                    }
                    else
                    {
                        auto jFind = newSymbolNames.find( strSymbol );
                        if ( jFind != newSymbolNames.end() )
                        {
                            auto pNewSymbol = jFind->second;
                            pNewSymbol->push_back_dimensions( pDimension );
                        }
                        else
                        {
                            auto pNewSymbol = newDatabase.construct< New::Symbols::SymbolTypeID >(
                                New::Symbols::SymbolTypeID::Args{ strSymbol, 0U, {}, { pDimension } } );
                            newSymbolNames.insert( { strSymbol, pNewSymbol } );
                        }
                    }
                }
            }

            // generate symbol ids - avoiding existing
            auto   usedIter      = usedTypeIDs.begin();
            TypeID typeIDCounter = 1U;
            for ( auto& [ strSymbol, pSymbolTypeID ] : newSymbolNames )
            {
                if ( pSymbolTypeID->get_id() == 0 )
                {
                    while ( ( usedIter != usedTypeIDs.end() ) && ( typeIDCounter == *usedIter ) )
                    {
                        ++usedIter;
                        ++typeIDCounter;
                    }
                    pSymbolTypeID->set_id( typeIDCounter );
                    newSymbolTypeIDs.insert( { typeIDCounter, pSymbolTypeID } );
                    ++typeIDCounter;
                }
            }
        }

        std::map< TypeIDSequence, New::Symbols::InterfaceTypeID* > newInterfaceTypeIDSequences;
        std::map< TypeID, New::Symbols::InterfaceTypeID* >         newInterfaceTypeIDs;
        {
            TypeIDSequenceGen idSequenceGen(
                newSymbolNames,
                &SymbolAnalysis::dynamic_database_cast< SymbolAnalysis::Interface::IContext,
                                                        SymbolAnalysis::Interface::ContextGroup > );

            // std::map< TypeIDSequence, New::Symbols::InterfaceTypeID* > newInterfaceTypeIDSequences;
            // std::map< TypeID, New::Symbols::InterfaceTypeID* >         newInterfaceTypeIDs;
            std::set< TypeID > usedTypeIDs;
            for ( const mega::io::megaFilePath& newSourceFile : newSourceFiles )
            {
                for ( New::Interface::IContext* pContext :
                      newDatabase.many< New::Interface::IContext >( newSourceFile ) )
                {
                    const TypeIDSequence idSeq = idSequenceGen( pContext );
                    auto                 iFind = oldInterfaceTypeIDSequences.find( idSeq );
                    if ( iFind != oldInterfaceTypeIDSequences.end() )
                    {
                        Old::Symbols::InterfaceTypeID* pOldInterfaceTypeID = iFind->second;

                        auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                        VERIFY_RTE_MSG( jFind == newInterfaceTypeIDSequences.end(),
                                        "Duplicate Interface Type ID Sequnce found: " << idSeq );

                        auto pNewInterfaceSymbol = newDatabase.construct< New::Symbols::InterfaceTypeID >(
                            New::Symbols::InterfaceTypeID::Args{
                                idSeq, pOldInterfaceTypeID->get_id(), pContext, std::nullopt } );
                        VERIFY_RTE( usedTypeIDs.insert( pOldInterfaceTypeID->get_id() ).second );
                        newInterfaceTypeIDSequences.insert( { idSeq, pNewInterfaceSymbol } );
                        newInterfaceTypeIDs.insert( { pOldInterfaceTypeID->get_id(), pNewInterfaceSymbol } );
                    }
                    else
                    {
                        auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                        VERIFY_RTE_MSG( jFind == newInterfaceTypeIDSequences.end(),
                                        "Duplicate Interface Type ID Sequnce found: " << idSeq );

                        auto pNewInterfaceSymbol = newDatabase.construct< New::Symbols::InterfaceTypeID >(
                            New::Symbols::InterfaceTypeID::Args{ idSeq, 0U, pContext, std::nullopt } );
                        newInterfaceTypeIDSequences.insert( { idSeq, pNewInterfaceSymbol } );
                    }
                }

                for ( New::Interface::DimensionTrait* pDimension :
                      newDatabase.many< New::Interface::DimensionTrait >( newSourceFile ) )
                {
                    const TypeIDSequence idSeq = idSequenceGen( pDimension );
                    auto                 iFind = oldInterfaceTypeIDSequences.find( idSeq );
                    if ( iFind != oldInterfaceTypeIDSequences.end() )
                    {
                        Old::Symbols::InterfaceTypeID* pOldInterfaceTypeID = iFind->second;

                        auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                        VERIFY_RTE_MSG( jFind == newInterfaceTypeIDSequences.end(),
                                        "Duplicate Interface Type ID Sequnce found: " << idSeq );

                        auto pNewInterfaceSymbol = newDatabase.construct< New::Symbols::InterfaceTypeID >(
                            New::Symbols::InterfaceTypeID::Args{
                                idSeq, pOldInterfaceTypeID->get_id(), std::nullopt, pDimension } );
                        VERIFY_RTE( usedTypeIDs.insert( pOldInterfaceTypeID->get_id() ).second );
                        newInterfaceTypeIDSequences.insert( { idSeq, pNewInterfaceSymbol } );
                        newInterfaceTypeIDs.insert( { pOldInterfaceTypeID->get_id(), pNewInterfaceSymbol } );
                    }
                    else
                    {
                        auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                        VERIFY_RTE_MSG( jFind == newInterfaceTypeIDSequences.end(),
                                        "Duplicate Interface Type ID Sequnce found: " << idSeq );

                        auto pNewInterfaceSymbol = newDatabase.construct< New::Symbols::InterfaceTypeID >(
                            New::Symbols::InterfaceTypeID::Args{ idSeq, 0U, std::nullopt, pDimension } );
                        newInterfaceTypeIDSequences.insert( { idSeq, pNewInterfaceSymbol } );
                    }
                }

                // generate symbol ids - avoiding existing
                auto   usedIter      = usedTypeIDs.begin();
                TypeID typeIDCounter = 1U;
                for ( auto& [ idSequence, pSymbolTypeID ] : newInterfaceTypeIDSequences )
                {
                    if ( pSymbolTypeID->get_id() == 0 )
                    {
                        while ( ( usedIter != usedTypeIDs.end() ) && ( typeIDCounter == *usedIter ) )
                        {
                            ++usedIter;
                            ++typeIDCounter;
                        }
                        pSymbolTypeID->set_id( typeIDCounter );
                        newInterfaceTypeIDs.insert( { typeIDCounter, pSymbolTypeID } );
                        ++typeIDCounter;
                    }
                }
            }
        }

        return newDatabase.construct< New::Symbols::SymbolTable >( New::Symbols::SymbolTable::Args{
            newSymbolNames, newSymbolTypeIDs, newInterfaceTypeIDSequences, newInterfaceTypeIDs } );
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath symbolCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( manifestFilePath );
        start( taskProgress, "Task_SymbolAnalysis", manifestFilePath.path(), symbolCompilationFile.path() );

        InterfaceHashCodeGenerator hashCodeGenerator( m_environment, m_toolChain.toolChainHash );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= hashCodeGenerator( sourceFilePath );
        }

        if ( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            cached( taskProgress );
            return;
        }

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

                    const Old::Symbols::SymbolTable* pOldSymbolTable
                        = oldDatabase.one< Old::Symbols::SymbolTable >( manifestFilePath );
                    VERIFY_RTE( pOldSymbolTable );

                    const std::map< std::string, Old::Symbols::SymbolTypeID* > oldSymbolTypeIDs
                        = pOldSymbolTable->get_symbol_names();

                    const std::map< TypeIDSequence, Old::Symbols::InterfaceTypeID* > oldInterfaceTypeIDSequences
                        = pOldSymbolTable->get_interface_type_id_sequences();

                    calculateSymbolTable( newDatabase, oldSymbolTypeIDs, oldInterfaceTypeIDSequences );
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
                // pass in empty prior results
                namespace Old = SymbolAnalysisView;
                const std::map< std::string, Old::Symbols::SymbolTypeID* >       oldSymbolTypeIDs;
                const std::map< TypeIDSequence, Old::Symbols::InterfaceTypeID* > oldInterfaceTypeIDSequences;
                calculateSymbolTable( database, oldSymbolTypeIDs, oldInterfaceTypeIDSequences );
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

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );
        const auto            symbolNames  = pSymbolTable->get_symbol_names();
        const auto            interfaceTypeIDs = pSymbolTable->get_interface_type_id_sequences();

        Task_SymbolAnalysis::TypeIDSequenceGen< SymbolRollout::Symbols::SymbolTypeID,
                                                SymbolRollout::Interface::ContextGroup,
                                                SymbolRollout::Interface::IContext,
                                                SymbolRollout::Interface::DimensionTrait >
            idSequenceGen( symbolNames,
                           &SymbolRollout::dynamic_database_cast< SymbolRollout::Interface::IContext,
                                                                  SymbolRollout::Interface::ContextGroup > );

        for ( Interface::IContext* pContext : database.many< Interface::IContext >( m_sourceFilePath ) )
        {
            TypeID symbolID;
            {
                auto iFind = symbolNames.find( pContext->get_identifier() );
                VERIFY_RTE( iFind != symbolNames.end() );
                symbolID = iFind->second->get_id();
            }

            TypeID interfaceTypeID;
            {
                const auto idSeq = idSequenceGen( pContext );
                auto       iFind = interfaceTypeIDs.find( idSeq );
                VERIFY_RTE( iFind != interfaceTypeIDs.end() );
                Symbols::InterfaceTypeID* pInterfaceTypeID = iFind->second;
                interfaceTypeID                            = pInterfaceTypeID->get_id();
            }

            database.construct< Interface::IContext >(
                Interface::IContext::Args{ pContext, symbolID, interfaceTypeID } );
        }

        for ( Interface::DimensionTrait* pDimension : database.many< Interface::DimensionTrait >( m_sourceFilePath ) )
        {
            TypeID symbolID;
            {
                auto iFind = symbolNames.find( pDimension->get_id()->get_str() );
                VERIFY_RTE( iFind != symbolNames.end() );
                symbolID = iFind->second->get_id();
            }

            TypeID interfaceTypeID;
            {
                const auto idSeq = idSequenceGen( pDimension );
                auto       iFind = interfaceTypeIDs.find( idSeq );
                VERIFY_RTE( iFind != interfaceTypeIDs.end() );
                Symbols::InterfaceTypeID* pInterfaceTypeID = iFind->second;
                interfaceTypeID                            = pInterfaceTypeID->get_id();
            }

            database.construct< Interface::DimensionTrait >(
                Interface::DimensionTrait::Args{ pDimension, symbolID, interfaceTypeID } );
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

} // namespace mega::compiler
