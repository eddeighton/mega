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
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }

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
            if( auto pContext = castFunctor( pContextGroup ) )
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

        using NewSymbolNames   = std::map< std::string, New::Symbols::SymbolTypeID* >;
        using NewSymbolTypeIDs = std::map< mega::TypeID, New::Symbols::SymbolTypeID* >;

        NewSymbolNames   newSymbolNames;
        NewSymbolTypeIDs newSymbolTypeIDs;

        {
            // ensure symbolIDs are sorted from -1 to -MAX
            std::set< TypeID, std::greater<> > usedTypeIDs;
            {
                // always have root be symbol -1
                auto pNewSymbol = newDatabase.construct< New::Symbols::SymbolTypeID >(
                    New::Symbols::SymbolTypeID::Args{ ROOT_TYPE_NAME, ROOT_SYMBOL_ID, {}, {} } );
                VERIFY_RTE( usedTypeIDs.insert( pNewSymbol->get_id() ).second );
                VERIFY_RTE( newSymbolNames.insert( { pNewSymbol->get_symbol(), pNewSymbol } ).second );
                VERIFY_RTE( newSymbolTypeIDs.insert( { pNewSymbol->get_id(), pNewSymbol } ).second );
            }
            // construct all symbols - reusing old symbol IDs across whole program
            for( const mega::io::megaFilePath& newSourceFile : newSourceFiles )
            {
                for( New::Interface::IContext* pContext :
                     newDatabase.many< New::Interface::IContext >( newSourceFile ) )
                {
                    const auto strSymbol = pContext->get_identifier();
                    auto       iFind     = oldSymbolTypeIDs.find( strSymbol );
                    if( iFind != oldSymbolTypeIDs.end() )
                    {
                        Old::Symbols::SymbolTypeID* pOldSymbol = iFind->second;

                        auto jFind = newSymbolNames.find( strSymbol );
                        if( jFind != newSymbolNames.end() )
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
                            VERIFY_RTE( newSymbolNames.insert( { strSymbol, pNewSymbol } ).second );
                            VERIFY_RTE( newSymbolTypeIDs.insert( { pOldSymbol->get_id(), pNewSymbol } ).second );
                        }
                    }
                    else
                    {
                        auto jFind = newSymbolNames.find( strSymbol );
                        if( jFind != newSymbolNames.end() )
                        {
                            auto pNewSymbol = jFind->second;
                            pNewSymbol->push_back_contexts( pContext );
                        }
                        else
                        {
                            auto pNewSymbol = newDatabase.construct< New::Symbols::SymbolTypeID >(
                                New::Symbols::SymbolTypeID::Args{ strSymbol, TypeID{}, { pContext }, {} } );
                            VERIFY_RTE( newSymbolNames.insert( { strSymbol, pNewSymbol } ).second );
                        }
                    }
                }

                for( New::Interface::DimensionTrait* pDimension :
                     newDatabase.many< New::Interface::DimensionTrait >( newSourceFile ) )
                {
                    const auto strSymbol = pDimension->get_id()->get_str();
                    auto       iFind     = oldSymbolTypeIDs.find( strSymbol );
                    if( iFind != oldSymbolTypeIDs.end() )
                    {
                        Old::Symbols::SymbolTypeID* pOldSymbol = iFind->second;

                        auto jFind = newSymbolNames.find( strSymbol );
                        if( jFind != newSymbolNames.end() )
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
                            VERIFY_RTE( newSymbolNames.insert( { strSymbol, pNewSymbol } ).second );
                            VERIFY_RTE( newSymbolTypeIDs.insert( { pOldSymbol->get_id(), pNewSymbol } ).second );
                        }
                    }
                    else
                    {
                        auto jFind = newSymbolNames.find( strSymbol );
                        if( jFind != newSymbolNames.end() )
                        {
                            auto pNewSymbol = jFind->second;
                            pNewSymbol->push_back_dimensions( pDimension );
                        }
                        else
                        {
                            auto pNewSymbol = newDatabase.construct< New::Symbols::SymbolTypeID >(
                                New::Symbols::SymbolTypeID::Args{ strSymbol, TypeID{}, {}, { pDimension } } );
                            VERIFY_RTE( newSymbolNames.insert( { strSymbol, pNewSymbol } ).second );
                        }
                    }
                }
            }

            // generate symbol ids - avoiding existing
            // ensure symbol ids are negative
            {
                auto              usedIter        = usedTypeIDs.begin();
                TypeID::ValueType symbolIDCounter = ROOT_SYMBOL_ID;
                for( auto [ _, pSymbolTypeID ] : newSymbolNames )
                {
                    if( pSymbolTypeID->get_id() == mega::TypeID{} )
                    {
                        while( ( usedIter != usedTypeIDs.end() ) && ( TypeID{ symbolIDCounter } == *usedIter ) )
                        {
                            ++usedIter;
                            --symbolIDCounter;
                        }
                        VERIFY_RTE_MSG(
                            symbolIDCounter > TypeID::LOWEST_SYMBOL_ID, "Exceeded lowest allowed symbol ID" );
                        const TypeID newSymbolID{ symbolIDCounter };
                        pSymbolTypeID->set_id( newSymbolID );
                        VERIFY_RTE( newSymbolTypeIDs.insert( { newSymbolID, pSymbolTypeID } ).second );
                        --symbolIDCounter;
                    }
                }
            }
        }

        std::map< TypeIDSequence, New::Symbols::InterfaceTypeID* > newInterfaceTypeIDSequences;
        std::map< TypeID, New::Symbols::InterfaceTypeID* >         newInterfaceTypeIDs;
        {
            const TypeIDSequenceGen< SymbolAnalysis::Symbols::SymbolTypeID,
                                     SymbolAnalysis::Interface::ContextGroup,
                                     SymbolAnalysis::Interface::IContext,
                                     SymbolAnalysis::Interface::DimensionTrait >
                idSequenceGen( newSymbolNames,
                               &SymbolAnalysis::db_cast< SymbolAnalysis::Interface::IContext,
                                                         SymbolAnalysis::Interface::ContextGroup > );

            std::set< TypeID > usedTypeIDs;

            {
                // always have root be type ROOT_TYPE_ID
                auto pNewInterfaceSymbol
                    = newDatabase.construct< New::Symbols::InterfaceTypeID >( New::Symbols::InterfaceTypeID::Args{
                        { ROOT_SYMBOL_ID }, ROOT_TYPE_ID, std::nullopt, std::nullopt } );

                VERIFY_RTE( usedTypeIDs.insert( pNewInterfaceSymbol->get_id() ).second );
                VERIFY_RTE( newInterfaceTypeIDSequences.insert( { { ROOT_SYMBOL_ID }, pNewInterfaceSymbol } ).second );
                VERIFY_RTE(
                    newInterfaceTypeIDs.insert( { pNewInterfaceSymbol->get_id(), pNewInterfaceSymbol } ).second );
            }

            for( const mega::io::megaFilePath& newSourceFile : newSourceFiles )
            {
                for( New::Interface::IContext* pContext :
                     newDatabase.many< New::Interface::IContext >( newSourceFile ) )
                {
                    const TypeIDSequence idSeq = idSequenceGen( pContext );
                    if( idSeq == TypeIDSequence{ ROOT_SYMBOL_ID } )
                    {
                        // special case for root
                        auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                        VERIFY_RTE( jFind != newInterfaceTypeIDSequences.end() );
                        New::Symbols::InterfaceTypeID* pRootInterfaceTypeID = jFind->second;
                        VERIFY_RTE( !pRootInterfaceTypeID->get_context().has_value() );
                        pRootInterfaceTypeID->set_context( pContext );
                    }
                    else
                    {
                        auto iFind = oldInterfaceTypeIDSequences.find( idSeq );
                        if( iFind != oldInterfaceTypeIDSequences.end() )
                        {
                            Old::Symbols::InterfaceTypeID* pOldInterfaceTypeID = iFind->second;

                            auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                            VERIFY_RTE_MSG( jFind == newInterfaceTypeIDSequences.end(),
                                            "Duplicate Interface Type ID Sequnce found: " << idSeq );

                            auto pNewInterfaceSymbol = newDatabase.construct< New::Symbols::InterfaceTypeID >(
                                New::Symbols::InterfaceTypeID::Args{
                                    idSeq, pOldInterfaceTypeID->get_id(), pContext, std::nullopt } );
                            VERIFY_RTE( usedTypeIDs.insert( pOldInterfaceTypeID->get_id() ).second );
                            VERIFY_RTE( newInterfaceTypeIDSequences.insert( { idSeq, pNewInterfaceSymbol } ).second );
                            VERIFY_RTE(
                                newInterfaceTypeIDs.insert( { pOldInterfaceTypeID->get_id(), pNewInterfaceSymbol } )
                                    .second );
                        }
                        else
                        {
                            auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                            VERIFY_RTE_MSG( jFind == newInterfaceTypeIDSequences.end(),
                                            "Duplicate Interface Type ID Sequnce found: " << idSeq );

                            auto pNewInterfaceSymbol = newDatabase.construct< New::Symbols::InterfaceTypeID >(
                                New::Symbols::InterfaceTypeID::Args{ idSeq, TypeID{}, pContext, std::nullopt } );
                            VERIFY_RTE( newInterfaceTypeIDSequences.insert( { idSeq, pNewInterfaceSymbol } ).second );
                        }
                    }
                }

                for( New::Interface::DimensionTrait* pDimension :
                     newDatabase.many< New::Interface::DimensionTrait >( newSourceFile ) )
                {
                    const TypeIDSequence idSeq = idSequenceGen( pDimension );
                    auto                 iFind = oldInterfaceTypeIDSequences.find( idSeq );
                    if( iFind != oldInterfaceTypeIDSequences.end() )
                    {
                        Old::Symbols::InterfaceTypeID* pOldInterfaceTypeID = iFind->second;

                        auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                        VERIFY_RTE_MSG( jFind == newInterfaceTypeIDSequences.end(),
                                        "Duplicate Interface Type ID Sequnce found: " << idSeq );

                        auto pNewInterfaceSymbol = newDatabase.construct< New::Symbols::InterfaceTypeID >(
                            New::Symbols::InterfaceTypeID::Args{
                                idSeq, pOldInterfaceTypeID->get_id(), std::nullopt, pDimension } );
                        VERIFY_RTE( usedTypeIDs.insert( pOldInterfaceTypeID->get_id() ).second );
                        VERIFY_RTE( newInterfaceTypeIDSequences.insert( { idSeq, pNewInterfaceSymbol } ).second );
                        VERIFY_RTE( newInterfaceTypeIDs.insert( { pOldInterfaceTypeID->get_id(), pNewInterfaceSymbol } )
                                        .second );
                    }
                    else
                    {
                        auto jFind = newInterfaceTypeIDSequences.find( idSeq );
                        VERIFY_RTE_MSG( jFind == newInterfaceTypeIDSequences.end(),
                                        "Duplicate Interface Type ID Sequnce found: " << idSeq );

                        auto pNewInterfaceSymbol = newDatabase.construct< New::Symbols::InterfaceTypeID >(
                            New::Symbols::InterfaceTypeID::Args{ idSeq, TypeID{}, std::nullopt, pDimension } );
                        VERIFY_RTE( newInterfaceTypeIDSequences.insert( { idSeq, pNewInterfaceSymbol } ).second );
                    }
                }
            }

            // generate symbol ids - avoiding existing

            std::map< New::Interface::Object*, New::Symbols::InterfaceTypeID* > objectInterfaceTypeIDs;
            {
                std::set< U8 > usedObjectIDs;
                for( auto typeID : usedTypeIDs )
                {
                    usedObjectIDs.insert( typeID.getObjectID() );
                }

                auto usedIter        = usedObjectIDs.begin();
                U8   objectIDCounter = ROOT_TYPE_ID.getObjectID();
                ASSERT( objectIDCounter == 1U );

                // set all object interface type IDs
                for( auto& [ idSequence, pInterfaceTypeID ] : newInterfaceTypeIDSequences )
                {
                    if( auto pContextOpt = pInterfaceTypeID->get_context(); pContextOpt.has_value() )
                    {
                        if( auto pObject = db_cast< New::Interface::Object >( pContextOpt.value() ) )
                        {
                            if( pInterfaceTypeID->get_id() == TypeID{} )
                            {
                                while( ( usedIter != usedObjectIDs.end() ) && ( objectIDCounter == *usedIter ) )
                                {
                                    ++usedIter;
                                    ++objectIDCounter;
                                }
                                const TypeID newTypeID = TypeID::make_context( objectIDCounter );
                                pInterfaceTypeID->set_id( newTypeID );
                                VERIFY_RTE( newInterfaceTypeIDs.insert( { newTypeID, pInterfaceTypeID } ).second );
                                ++objectIDCounter;
                            }
                            objectInterfaceTypeIDs.insert( { pObject, pInterfaceTypeID } );
                        }
                    }
                }
            }

            // establish the used subObjectIDs per objectID
            std::multimap< U8, U8 > usedSubObjectIDs;
            for( const auto typeID : usedTypeIDs )
            {
                usedSubObjectIDs.insert( { typeID.getObjectID(), typeID.getSubObjectID() } );
            }

            // set everything else
            for( auto [ _, pInterfaceTypeID ] : newInterfaceTypeIDSequences )
            {
                if( pInterfaceTypeID->get_id() == TypeID{} )
                {
                    // locate the object
                    U8 objectID = 0;
                    {
                        New::Interface::Object* pObject = nullptr;
                        {
                            New::Interface::ContextGroup* pContextGroup
                                = pInterfaceTypeID->get_context().has_value()
                                      ? pInterfaceTypeID->get_context().value()->get_parent()
                                      : pInterfaceTypeID->get_dimension().value()->get_parent();
                            VERIFY_RTE( pContextGroup );
                            while( auto pContext = db_cast< New::Interface::IContext >( pContextGroup ) )
                            {
                                if( db_cast< New::Interface::Object >( pContext ) )
                                    break;
                                else
                                    pContextGroup = pContext->get_parent();
                            }
                            pObject = db_cast< New::Interface::Object >( pContextGroup );
                        }
                        if( pObject )
                        {
                            New::Symbols::InterfaceTypeID* pObjectInterfaceTypeID = objectInterfaceTypeIDs[ pObject ];
                            VERIFY_RTE_MSG( pObjectInterfaceTypeID, "Failed locating object interface typeid" );

                            const TypeID objectTypeID = pObjectInterfaceTypeID->get_id();
                            ASSERT( objectTypeID.isContextID() && objectTypeID.getSubObjectID() == 0 );
                            objectID = objectTypeID.getObjectID();
                        }
                    }

                    // find the begining of the object TypeID range in usedTypeIDS
                    TypeID newTypeID;
                    {
                        auto i           = usedSubObjectIDs.lower_bound( objectID );
                        auto iEnd        = usedSubObjectIDs.upper_bound( objectID );
                        U8   subObjectID = 0U;
                        while( ( i != iEnd ) && ( subObjectID == i->second ) )
                        {
                            ++i;
                            ++subObjectID;
                            VERIFY_RTE_MSG( subObjectID != 0, "SubObjectID overflow" );
                        }
                        // there may not be an object in which case subObjectID 0 will not be used
                        if( subObjectID == 0U )
                        {
                            usedSubObjectIDs.insert( { objectID, subObjectID } );
                            ++subObjectID;
                        }
                        usedSubObjectIDs.insert( { objectID, subObjectID } );
                        newTypeID = TypeID::make_context( objectID, subObjectID );
                    }

                    pInterfaceTypeID->set_id( newTypeID );
                    VERIFY_RTE( newInterfaceTypeIDs.insert( { newTypeID, pInterfaceTypeID } ).second );
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
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= hashCodeGenerator( sourceFilePath );
        }

        if( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            cached( taskProgress );
            return;
        }

        bool bReusedOldDatabase = false;
        if( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
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
            catch( mega::io::DatabaseVersionException& )
            {
                bReusedOldDatabase = false;
            }
        }

        if( !bReusedOldDatabase )
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

        if( m_environment.restore( symbolRolloutFilePath, determinant ) )
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
            idSequenceGen(
                symbolNames,
                &SymbolRollout::db_cast< SymbolRollout::Interface::IContext, SymbolRollout::Interface::ContextGroup > );

        for( Interface::IContext* pContext : database.many< Interface::IContext >( m_sourceFilePath ) )
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

            VERIFY_RTE( symbolID.isSymbolID() );
            VERIFY_RTE( interfaceTypeID.isContextID() );

            database.construct< Interface::IContext >(
                Interface::IContext::Args{ pContext, symbolID, interfaceTypeID } );
        }

        for( Interface::DimensionTrait* pDimension : database.many< Interface::DimensionTrait >( m_sourceFilePath ) )
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
