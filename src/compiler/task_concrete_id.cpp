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

#include <utility>

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
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            srcFiles.insert( sourceFilePath );
        }
        return srcFiles;
    }

    // clang-format off
    template
    <
        typename ConcreteTypeID = ConcreteTypeAnalysis::Symbols::ConcreteTypeID,
        typename IContext       = ConcreteTypeAnalysis::Interface::IContext,
        typename IDim           = ConcreteTypeAnalysis::Interface::DimensionTrait,
        typename ILink          = ConcreteTypeAnalysis::Interface::LinkTrait,
        typename Context        = ConcreteTypeAnalysis::Concrete::Context,
        typename ContextGroup   = ConcreteTypeAnalysis::Concrete::ContextGroup,
        typename DimUser        = ConcreteTypeAnalysis::Concrete::Dimensions::User,
        typename DimLink        = ConcreteTypeAnalysis::Concrete::Dimensions::Link,
        typename DimAlloc       = ConcreteTypeAnalysis::Concrete::Dimensions::Allocation,
        typename DimAllocator   = ConcreteTypeAnalysis::Concrete::Dimensions::Allocator
    >
    // clang-format on
    struct TypeIDSequenceGen
    {
        using CastFunctor = std::function< Context*( ContextGroup* ) >;
        CastFunctor castFunctor;

        using DimCastFunctor = std::function< DimAllocator*( DimAlloc* ) >;
        DimCastFunctor dimCastFunctor;

        TypeIDSequenceGen( CastFunctor castFunctor, DimCastFunctor dimCastFunctor )
            : castFunctor( std::move( castFunctor ) )
            , dimCastFunctor( std::move( dimCastFunctor ) )
        {
        }

        TypeID getTypeID( const IContext* pIContext ) const { return pIContext->get_interface_id(); }
        TypeID getTypeID( const IDim* pIDim ) const { return pIDim->get_interface_id(); }
        TypeID getTypeID( const ILink* pILink ) const { return pILink->get_interface_id(); }

        void recurse( ContextGroup* pContextGroup, TypeIDSequence& sequence ) const
        {
            if( auto pContext = castFunctor( pContextGroup ) )
            {
                sequence.push_back( getTypeID( pContext->get_interface() ) );
                recurse( pContext->get_parent(), sequence );
            }
        }

        TypeIDSequence operator()( Context* pContext ) const
        {
            TypeIDSequence sequence{ getTypeID( pContext->get_interface() ) };
            recurse( pContext->get_parent(), sequence );
            return sequence;
        }
        TypeIDSequence operator()( DimUser* pDimUser ) const
        {
            TypeIDSequence sequence{ getTypeID( pDimUser->get_interface_dimension() ) };
            recurse( pDimUser->get_parent(), sequence );
            return sequence;
        }
        TypeIDSequence operator()( DimLink* pDimLink ) const
        {
            TypeIDSequence sequence{ getTypeID( pDimLink->get_interface_link() ) };
            recurse( pDimLink->get_parent(), sequence );
            return sequence;
        }
        TypeIDSequence operator()( DimAlloc* pDimAlloc ) const
        {
            if( auto pDimAllocator = dimCastFunctor( pDimAlloc ) )
            {
                auto pAllocator        = pDimAllocator->get_allocator();
                auto pAllocatedContext = pAllocator->get_allocated_context();

                TypeIDSequence sequence{ getTypeID( pAllocatedContext->get_interface() ) };
                recurse( pAllocatedContext->get_parent(), sequence );

                return sequence;
            }
            else
            {
                THROW_RTE( "Unknown allocator dimension type" );
            }
        }
    };

    ConcreteTypeAnalysis::Symbols::SymbolTable* calculateSymbolTable(
        ConcreteTypeAnalysis::Database&             newDatabase,
        ConcreteTypeAnalysis::Symbols::SymbolTable* pSymbolTable,
        const std::map< mega::TypeIDSequence, ConcreteTypeAnalysisView::Symbols::ConcreteTypeID* >&
            oldConcreteTypeIDSequences,
        const std::map< mega::TypeIDSequence, ConcreteTypeAnalysisView::Symbols::ConcreteTypeID* >& oldAllocIDSequences,
        const std::map< mega::TypeIDSequence, ConcreteTypeAnalysisView::Symbols::ConcreteTypeID* >& oldLinkIDSequences )
    {
        namespace New = ConcreteTypeAnalysis;

        using NewTypeIDSequenceMap = std::map< mega::TypeIDSequence, New::Symbols::ConcreteTypeID* >;
        using NewTypeIDMap         = std::map< mega::TypeID, New::Symbols::ConcreteTypeID* >;

        NewTypeIDSequenceMap                                            new_concrete_type_id_sequences;
        std::map< mega::TypeIDSequence, New::Symbols::ConcreteTypeID* > new_concrete_type_id_seq_alloc;
        std::map< mega::TypeIDSequence, New::Symbols::ConcreteTypeID* > new_concrete_type_id_set_link;
        NewTypeIDMap                                                    new_concrete_type_ids;
        std::set< TypeID >                                              usedTypeIDs;

        TypeIDSequenceGen< New::Symbols::ConcreteTypeID,
                           New::Interface::IContext,
                           New::Interface::DimensionTrait,
                           New::Interface::LinkTrait,
                           New::Concrete::Context,
                           New::Concrete::ContextGroup,
                           New::Concrete::Dimensions::User,
                           New::Concrete::Dimensions::Link,
                           New::Concrete::Dimensions::Allocation,
                           New::Concrete::Dimensions::Allocator >
            idgen( &New::db_cast< New::Concrete::Context, New::Concrete::ContextGroup >,
                   &New::db_cast< New::Concrete::Dimensions::Allocator, New::Concrete::Dimensions::Allocation > );

        {
            auto pNewConcreteSymbol
                = newDatabase.construct< New::Symbols::ConcreteTypeID >( New::Symbols::ConcreteTypeID::Args{
                    ROOT_TYPE_ID, std::nullopt, std::nullopt, std::nullopt, std::nullopt } );

            usedTypeIDs.insert( ROOT_TYPE_ID );
            new_concrete_type_ids.insert( { ROOT_TYPE_ID, pNewConcreteSymbol } );
            new_concrete_type_id_sequences.insert( { TypeIDSequence{ ROOT_TYPE_ID }, pNewConcreteSymbol } );
        }

        for( const mega::io::megaFilePath& newSourceFile : getSortedSourceFiles() )
        {
            for( New::Concrete::Context* pContext : newDatabase.many< New::Concrete::Context >( newSourceFile ) )
            {
                const TypeIDSequence idSeq = idgen( pContext );

                if( idSeq == TypeIDSequence{ ROOT_TYPE_ID } )
                {
                    // special case for root
                    auto jFind = new_concrete_type_id_sequences.find( idSeq );
                    VERIFY_RTE( jFind != new_concrete_type_id_sequences.end() );
                    New::Symbols::ConcreteTypeID* pRootInterfaceTypeID = jFind->second;
                    VERIFY_RTE( !pRootInterfaceTypeID->get_context().has_value() );
                    pRootInterfaceTypeID->set_context( pContext );
                }
                else
                {
                    New::Symbols::ConcreteTypeID* pNewConcreteSymbol = nullptr;
                    {
                        auto iFind = oldConcreteTypeIDSequences.find( idSeq );
                        if( iFind != oldConcreteTypeIDSequences.end() )
                        {
                            auto         pOldConcreteTypeID = iFind->second;
                            const TypeID oldTypeID          = pOldConcreteTypeID->get_id();
                            pNewConcreteSymbol              = newDatabase.construct< New::Symbols::ConcreteTypeID >(
                                New::Symbols::ConcreteTypeID::Args{
                                    oldTypeID, pContext, std::nullopt, std::nullopt, std::nullopt } );
                            VERIFY_RTE( new_concrete_type_ids.insert( { oldTypeID, pNewConcreteSymbol } ).second );
                            VERIFY_RTE( usedTypeIDs.insert( oldTypeID ).second );
                        }
                        else
                        {
                            pNewConcreteSymbol = newDatabase.construct< New::Symbols::ConcreteTypeID >(
                                New::Symbols::ConcreteTypeID::Args{
                                    TypeID{}, pContext, std::nullopt, std::nullopt, std::nullopt } );
                        }
                    }
                    VERIFY_RTE( new_concrete_type_id_sequences.insert( { idSeq, pNewConcreteSymbol } ).second );
                }
            }
            for( New::Concrete::Dimensions::User* pUserDimension :
                 newDatabase.many< New::Concrete::Dimensions::User >( newSourceFile ) )
            {
                const TypeIDSequence          idSeq              = idgen( pUserDimension );
                New::Symbols::ConcreteTypeID* pNewConcreteSymbol = nullptr;
                {
                    auto iFind = oldConcreteTypeIDSequences.find( idSeq );
                    if( iFind != oldConcreteTypeIDSequences.end() )
                    {
                        auto         pOldConcreteTypeID = iFind->second;
                        const TypeID oldTypeID          = pOldConcreteTypeID->get_id();
                        pNewConcreteSymbol
                            = newDatabase.construct< New::Symbols::ConcreteTypeID >( New::Symbols::ConcreteTypeID::Args{
                                oldTypeID, std::nullopt, pUserDimension, std::nullopt, std::nullopt } );
                        VERIFY_RTE( new_concrete_type_ids.insert( { oldTypeID, pNewConcreteSymbol } ).second );
                        VERIFY_RTE( usedTypeIDs.insert( oldTypeID ).second );
                    }
                    else
                    {
                        pNewConcreteSymbol
                            = newDatabase.construct< New::Symbols::ConcreteTypeID >( New::Symbols::ConcreteTypeID::Args{
                                TypeID{}, std::nullopt, pUserDimension, std::nullopt, std::nullopt } );
                    }
                }
                VERIFY_RTE( new_concrete_type_id_sequences.insert( { idSeq, pNewConcreteSymbol } ).second );
            }
            for( New::Concrete::Dimensions::Link* pLinkDimension :
                 newDatabase.many< New::Concrete::Dimensions::Link >( newSourceFile ) )
            {
                const TypeIDSequence          idSeq              = idgen( pLinkDimension );
                New::Symbols::ConcreteTypeID* pNewConcreteSymbol = nullptr;
                {
                    auto iFind = oldLinkIDSequences.find( idSeq );
                    if( iFind != oldLinkIDSequences.end() )
                    {
                        auto         pOldConcreteTypeID = iFind->second;
                        const TypeID oldTypeID          = pOldConcreteTypeID->get_id();
                        pNewConcreteSymbol
                            = newDatabase.construct< New::Symbols::ConcreteTypeID >( New::Symbols::ConcreteTypeID::Args{
                                oldTypeID, std::nullopt, std::nullopt, pLinkDimension, std::nullopt } );
                        VERIFY_RTE( new_concrete_type_ids.insert( { oldTypeID, pNewConcreteSymbol } ).second );
                        VERIFY_RTE( usedTypeIDs.insert( oldTypeID ).second );
                    }
                    else
                    {
                        pNewConcreteSymbol
                            = newDatabase.construct< New::Symbols::ConcreteTypeID >( New::Symbols::ConcreteTypeID::Args{
                                TypeID{}, std::nullopt, std::nullopt, pLinkDimension, std::nullopt } );
                    }
                }
                VERIFY_RTE( new_concrete_type_id_set_link.insert( { idSeq, pNewConcreteSymbol } ).second );
            }
            for( New::Concrete::Dimensions::Allocation* pAllocationDimension :
                 newDatabase.many< New::Concrete::Dimensions::Allocation >( newSourceFile ) )
            {
                const TypeIDSequence          idSeq              = idgen( pAllocationDimension );
                New::Symbols::ConcreteTypeID* pNewConcreteSymbol = nullptr;
                {
                    auto iFind = oldAllocIDSequences.find( idSeq );
                    if( iFind != oldAllocIDSequences.end() )
                    {
                        auto         pOldConcreteTypeID = iFind->second;
                        const TypeID oldTypeID          = pOldConcreteTypeID->get_id();
                        pNewConcreteSymbol
                            = newDatabase.construct< New::Symbols::ConcreteTypeID >( New::Symbols::ConcreteTypeID::Args{
                                oldTypeID, std::nullopt, std::nullopt, std::nullopt, pAllocationDimension } );
                        VERIFY_RTE( new_concrete_type_ids.insert( { oldTypeID, pNewConcreteSymbol } ).second );
                        VERIFY_RTE( usedTypeIDs.insert( oldTypeID ).second );
                    }
                    else
                    {
                        pNewConcreteSymbol
                            = newDatabase.construct< New::Symbols::ConcreteTypeID >( New::Symbols::ConcreteTypeID::Args{
                                TypeID{}, std::nullopt, std::nullopt, std::nullopt, pAllocationDimension } );
                    }
                }
                VERIFY_RTE( new_concrete_type_id_seq_alloc.insert( { idSeq, pNewConcreteSymbol } ).second );
            }
        }

        // generate symbol ids - avoiding existing

        // set object ID first
        std::map< New::Concrete::Object*, New::Symbols::ConcreteTypeID* > objectConcreteTypeIDs;
        {
            std::set< TypeID::SubValueType > usedObjectIDs;
            for( auto typeID : usedTypeIDs )
            {
                usedObjectIDs.insert( typeID.getObjectID() );
            }
            auto                 usedIter        = usedObjectIDs.begin();
            TypeID::SubValueType objectIDCounter = ROOT_TYPE_ID.getObjectID();

            for( auto [ _, pSymbolTypeID ] : new_concrete_type_id_sequences )
            {
                if( auto pContextOpt = pSymbolTypeID->get_context(); pContextOpt.has_value() )
                {
                    if( auto pObject = New::db_cast< New::Concrete::Object >( pContextOpt.value() ) )
                    {
                        if( pSymbolTypeID->get_id() == TypeID{} )
                        {
                            while( ( usedIter != usedObjectIDs.end() ) && ( objectIDCounter == *usedIter ) )
                            {
                                ++usedIter;
                                ++objectIDCounter;
                            }
                            const TypeID newTypeID = TypeID::make_context( objectIDCounter );
                            pSymbolTypeID->set_id( newTypeID );
                            new_concrete_type_ids.insert( { newTypeID, pSymbolTypeID } );
                            ++objectIDCounter;
                        }
                        objectConcreteTypeIDs.insert( { pObject, pSymbolTypeID } );
                    }
                }
            }
        }

        // establish the used subObjectIDs per objectID
        std::map< TypeID::SubValueType, std::set< TypeID::SubValueType > > usedSubObjectIDs;
        for( const auto typeID : usedTypeIDs )
        {
            usedSubObjectIDs[ typeID.getObjectID() ].insert( typeID.getSubObjectID() );
        }

        auto genSubObject
            = [ &objectConcreteTypeIDs, &usedSubObjectIDs, &new_concrete_type_ids ](
                  New::Symbols::ConcreteTypeID* pConcreteTypeID, New::Concrete::ContextGroup* pContextGroup )
        {
            // locate the object
            TypeID::SubValueType objectID = 0;
            {
                New::Concrete::Object* pObject = nullptr;
                {
                    VERIFY_RTE( pContextGroup );
                    while( auto pContext = db_cast< New::Concrete::Context >( pContextGroup ) )
                    {
                        if( db_cast< New::Concrete::Object >( pContext ) )
                            break;
                        else
                            pContextGroup = pContext->get_parent();
                    }
                    pObject = db_cast< New::Concrete::Object >( pContextGroup );
                }
                if( pObject )
                {
                    New::Symbols::ConcreteTypeID* pObjectInterfaceTypeID = objectConcreteTypeIDs[ pObject ];
                    VERIFY_RTE_MSG( pObjectInterfaceTypeID, "Failed locating object concrete typeid" );

                    const TypeID objectTypeID = pObjectInterfaceTypeID->get_id();
                    ASSERT( objectTypeID.isContextID() && objectTypeID.getSubObjectID() == 0 );
                    objectID = objectTypeID.getObjectID();
                }
            }

            // find the begining of the object TypeID range in usedTypeIDS
            TypeID newTypeID;
            {
                std::set< TypeID::SubValueType >& subobjectIDs = usedSubObjectIDs[ objectID ];
                TypeID::SubValueType              subObjectID  = 0U;
                for( auto used : subobjectIDs )
                {
                    if( subObjectID != used )
                    {
                        break;
                    }
                    else
                    {
                        ++subObjectID;
                        VERIFY_RTE_MSG( subObjectID != 0, "SubObjectID overflow" );
                    }
                }

                // there may not be an object in which case subObjectID 0 will not be used
                if( subObjectID == 0U )
                {
                    subobjectIDs.insert( subObjectID );
                    ++subObjectID;
                    VERIFY_RTE_MSG( subObjectID != 0, "SubObjectID overflow" );
                }
                subobjectIDs.insert( subObjectID );
                newTypeID = TypeID::make_context( objectID, subObjectID );
            }

            pConcreteTypeID->set_id( newTypeID );
            VERIFY_RTE( new_concrete_type_ids.insert( { newTypeID, pConcreteTypeID } ).second );
        };

        auto   usedIter      = usedTypeIDs.begin();
        TypeID typeIDCounter = TypeID{ 1U };
        for( auto& [ idSequence, pSymbolTypeID ] : new_concrete_type_id_sequences )
        {
            if( pSymbolTypeID->get_id() == 0 )
            {
                if( pSymbolTypeID->get_context().has_value() )
                {
                    genSubObject( pSymbolTypeID, pSymbolTypeID->get_context().value() );
                }
                else if( pSymbolTypeID->get_dim_user().has_value() )
                {
                    genSubObject( pSymbolTypeID, pSymbolTypeID->get_dim_user().value()->get_parent() );
                }
                else
                {
                    THROW_RTE( "Unexpected symbol type" );
                }
            }
        }
        for( auto& [ idSequence, pSymbolTypeID ] : new_concrete_type_id_set_link )
        {
            if( pSymbolTypeID->get_id() == 0 )
            {
                genSubObject( pSymbolTypeID, pSymbolTypeID->get_dim_link().value()->get_parent() );
            }
        }
        for( auto& [ idSequence, pSymbolTypeID ] : new_concrete_type_id_seq_alloc )
        {
            if( pSymbolTypeID->get_id() == 0 )
            {
                genSubObject( pSymbolTypeID, pSymbolTypeID->get_dim_allocation().value()->get_parent() );
            }
        }

        return newDatabase.construct< New::Symbols::SymbolTable >( New::Symbols::SymbolTable::Args{
            pSymbolTable, new_concrete_type_id_sequences, new_concrete_type_id_seq_alloc, new_concrete_type_id_set_link,
            new_concrete_type_ids } );
    }

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
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            const task::DeterminantHash hashCode(
                m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( sourceFilePath ) ),
                m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) ),
                m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( sourceFilePath ) ),
                m_environment.getBuildHashCode( m_environment.InterfaceAnalysisStage_Clang( sourceFilePath ) ) );
            determinant ^= hashCode;
        }

        if( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            cached( taskProgress );
            return;
        }

        ConcreteHashCodeGenerator hashCodeGenerator( m_environment, m_toolChain.toolChainHash );

        bool bReusedOldDatabase = false;
        if( boost::filesystem::exists( m_environment.DatabaseArchive() ) )
        {
            try
            {
                // attempt to reuse previous symbol analysis
                namespace Old = ConcreteTypeAnalysisView;
                namespace New = ConcreteTypeAnalysis;

                New::Database newDatabase( m_environment, manifestFilePath );

                New::Symbols::SymbolTable* pSymbolTable
                    = newDatabase.one< New::Symbols::SymbolTable >( manifestFilePath );

                {
                    io::ArchiveEnvironment archiveEnvironment( m_environment.DatabaseArchive() );
                    Old::Database          oldDatabase( archiveEnvironment, archiveEnvironment.project_manifest() );

                    Old::Symbols::SymbolTable* pOldSymbolTable
                        = oldDatabase.one< Old::Symbols::SymbolTable >( manifestFilePath );

                    const std::map< mega::TypeIDSequence, Old::Symbols::ConcreteTypeID* > oldConcreteTypeIDSequences
                        = pOldSymbolTable->get_concrete_type_id_sequences();
                    const std::map< mega::TypeIDSequence, Old::Symbols::ConcreteTypeID* > oldAllocIDSequences
                        = pOldSymbolTable->get_concrete_type_id_seq_alloc();
                    const std::map< mega::TypeIDSequence, Old::Symbols::ConcreteTypeID* > oldLinkIDSequences
                        = pOldSymbolTable->get_concrete_type_id_set_link();

                    calculateSymbolTable( newDatabase,
                                          pSymbolTable,
                                          oldConcreteTypeIDSequences,
                                          oldAllocIDSequences,
                                          oldLinkIDSequences );
                }

                const task::FileHash fileHashCode = newDatabase.save_ConcreteTable_to_temp();
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
            using namespace ConcreteTypeAnalysis;
            using namespace ConcreteTypeAnalysis::Symbols;

            Database database( m_environment, manifestFilePath );
            {
                Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( manifestFilePath );
                namespace Old                      = ConcreteTypeAnalysisView;
                const std::map< mega::TypeIDSequence, Old::Symbols::ConcreteTypeID* > oldConcreteTypeIDSequences;
                const std::map< mega::TypeIDSequence, Old::Symbols::ConcreteTypeID* > oldAllocIDSequences;
                const std::map< mega::TypeIDSequence, Old::Symbols::ConcreteTypeID* > oldLinkIDSequences;

                calculateSymbolTable(
                    database, pSymbolTable, oldConcreteTypeIDSequences, oldAllocIDSequences, oldLinkIDSequences );
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

        if( m_environment.restore( symbolRolloutFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( symbolRolloutFilePath );
            cached( taskProgress );
            return;
        }

        using namespace ConcreteTypeRollout;

        Database database( m_environment, m_sourceFilePath );

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        const std::map< mega::TypeIDSequence, Symbols::ConcreteTypeID* > oldConcreteTypeIDSequences
            = pSymbolTable->get_concrete_type_id_sequences();
        const std::map< mega::TypeIDSequence, Symbols::ConcreteTypeID* > oldAllocIDSequences
            = pSymbolTable->get_concrete_type_id_seq_alloc();
        const std::map< mega::TypeIDSequence, Symbols::ConcreteTypeID* > oldLinkIDSequences
            = pSymbolTable->get_concrete_type_id_set_link();

        Task_ConcreteTypeAnalysis::TypeIDSequenceGen< Symbols::ConcreteTypeID,
                                                      Interface::IContext,
                                                      Interface::DimensionTrait,
                                                      Interface::LinkTrait,
                                                      Concrete::Context,
                                                      Concrete::ContextGroup,
                                                      Concrete::Dimensions::User,
                                                      Concrete::Dimensions::Link,
                                                      Concrete::Dimensions::Allocation,
                                                      Concrete::Dimensions::Allocator >
            idgen( &db_cast< Concrete::Context, Concrete::ContextGroup >,
                   &db_cast< Concrete::Dimensions::Allocator, Concrete::Dimensions::Allocation > );

        for( auto pContext : database.many< Concrete::Context >( m_sourceFilePath ) )
        {
            auto iFind = oldConcreteTypeIDSequences.find( idgen( pContext ) );
            VERIFY_RTE( iFind != oldConcreteTypeIDSequences.end() );
            database.construct< Concrete::Context >( Concrete::Context::Args{ pContext, iFind->second->get_id() } );
        }
        for( auto pUserDimension : database.many< Concrete::Dimensions::User >( m_sourceFilePath ) )
        {
            auto iFind = oldConcreteTypeIDSequences.find( idgen( pUserDimension ) );
            VERIFY_RTE( iFind != oldConcreteTypeIDSequences.end() );
            database.construct< Concrete::Dimensions::User >(
                Concrete::Dimensions::User::Args{ pUserDimension, iFind->second->get_id() } );
        }
        for( auto pLinkDimension : database.many< Concrete::Dimensions::Link >( m_sourceFilePath ) )
        {
            const TypeIDSequence idSeq = idgen( pLinkDimension );
            auto                 iFind = oldLinkIDSequences.find( idSeq );
            VERIFY_RTE( iFind != oldLinkIDSequences.end() );
            database.construct< Concrete::Dimensions::Link >(
                Concrete::Dimensions::Link::Args{ pLinkDimension, iFind->second->get_id() } );
        }
        for( auto pAllocationDimension : database.many< Concrete::Dimensions::Allocation >( m_sourceFilePath ) )
        {
            const TypeIDSequence idSeq = idgen( pAllocationDimension );
            auto                 iFind = oldAllocIDSequences.find( idSeq );
            VERIFY_RTE( iFind != oldAllocIDSequences.end() );
            database.construct< Concrete::Dimensions::Allocation >(
                Concrete::Dimensions::Allocation::Args{ pAllocationDimension, iFind->second->get_id() } );
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
