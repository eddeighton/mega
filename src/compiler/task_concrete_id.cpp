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

#include "mega/common_strings.hpp"

#include "base_task.hpp"

#include "database/ConcreteTypeAnalysis.hxx"
#include "database/ConcreteTypeAnalysisView.hxx"
#include "database/ConcreteTypeRollout.hxx"
#include "database/manifest.hxx"

#include "environment/environment_archive.hpp"
#include "database/exception.hpp"
#include "database/sources.hpp"

namespace ConcreteTypeAnalysis
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/concrete_typeid_sequence.hpp"
} // namespace ConcreteTypeAnalysis
namespace ConcreteTypeRollout
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_typeid_sequence.hpp"
} // namespace ConcreteTypeRollout

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

    ConcreteTypeAnalysis::Symbols::SymbolTable*
    calculateSymbolTable( ConcreteTypeAnalysis::Database&             newDatabase,
                          ConcreteTypeAnalysis::Symbols::SymbolTable* pSymbolTable,
                          const std::map< mega::TypeIDSequence, ConcreteTypeAnalysisView::Symbols::ConcreteTypeID* >&
                              oldConcreteTypeIDSequences )
    {
        namespace New = ConcreteTypeAnalysis;

        using NewTypeIDSequenceMap = std::map< mega::TypeIDSequence, New::Symbols::ConcreteTypeID* >;
        using NewTypeIDMap         = std::map< mega::TypeID, New::Symbols::ConcreteTypeID* >;

        NewTypeIDSequenceMap new_concrete_type_id_sequences;
        NewTypeIDMap         new_concrete_type_ids;
        std::set< TypeID >   usedTypeIDs;

        New::TypeIDSequenceGen typeIDSequenceGenerator;

        {
            auto pNewConcreteSymbol = newDatabase.construct< New::Symbols::ConcreteTypeID >(
                New::Symbols::ConcreteTypeID::Args{ ROOT_TYPE_ID } );

            usedTypeIDs.insert( ROOT_TYPE_ID );
            new_concrete_type_ids.insert( { ROOT_TYPE_ID, pNewConcreteSymbol } );
            new_concrete_type_id_sequences.insert( { TypeIDSequence{ ROOT_TYPE_ID }, pNewConcreteSymbol } );
        }

        for( const mega::io::megaFilePath& newSourceFile : getSortedSourceFiles() )
        {
            for( New::Concrete::Graph::Vertex* pVertex :
                 newDatabase.many< New::Concrete::Graph::Vertex >( newSourceFile ) )
            {
                // ignore the concrete Root - is vertex due to contextGroup
                if( auto pRoot = db_cast< New::Concrete::Root >( pVertex ) )
                {
                    continue;
                }

                const TypeIDSequence idSeq = typeIDSequenceGenerator( pVertex );

                if( idSeq == TypeIDSequence{ ROOT_TYPE_ID } )
                {
                    // special case for root
                    auto jFind = new_concrete_type_id_sequences.find( idSeq );
                    VERIFY_RTE( jFind != new_concrete_type_id_sequences.end() );
                    New::Symbols::ConcreteTypeID* pRootInterfaceTypeID = jFind->second;
                    pRootInterfaceTypeID->set_vertex( pVertex );
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
                                New::Symbols::ConcreteTypeID::Args{ oldTypeID } );
                            pNewConcreteSymbol->set_vertex( pVertex );
                            VERIFY_RTE( new_concrete_type_ids.insert( { oldTypeID, pNewConcreteSymbol } ).second );
                            VERIFY_RTE( usedTypeIDs.insert( oldTypeID ).second );
                        }
                        else
                        {
                            pNewConcreteSymbol = newDatabase.construct< New::Symbols::ConcreteTypeID >(
                                New::Symbols::ConcreteTypeID::Args{ TypeID{} } );
                            pNewConcreteSymbol->set_vertex( pVertex );
                        }
                    }
                    const bool bSuccess = new_concrete_type_id_sequences.insert( { idSeq, pNewConcreteSymbol } ).second;
                    if( !bSuccess )
                    {
                        auto iFind = new_concrete_type_id_sequences.find( idSeq );
                        std::ostringstream os;
                        for( const auto& i : idSeq )
                        {
                            using ::operator<<;
                            os << i << " ";
                        }
                        THROW_RTE( "Failed to insert concrete symbol for: "
                                   << New::Concrete::printContextFullType( pNewConcreteSymbol->get_vertex() )
                                   << " existing: " << New::Concrete::printContextFullType( iFind->second->get_vertex() )
                                   << " with id seq of: " << os.str() );
                    }
                }
            }
        }

        // generate symbol ids - avoiding existing

        // set object ID first
        std::map< New::Concrete::Object*, New::Symbols::ConcreteTypeID* > objectConcreteTypeIDs;
        {
            std::set< SubType > usedObjectIDs;
            for( auto typeID : usedTypeIDs )
            {
                usedObjectIDs.insert( typeID.getObjectID() );
            }
            auto    usedIter        = usedObjectIDs.begin();
            SubType objectIDCounter = ROOT_TYPE_ID.getObjectID();

            for( auto [ _, pConcreteSymbolTypeID ] : new_concrete_type_id_sequences )
            {
                if( auto pObject = New::db_cast< New::Concrete::Object >( pConcreteSymbolTypeID->get_vertex() ) )
                {
                    if( pConcreteSymbolTypeID->get_id() == TypeID{} )
                    {
                        while( ( usedIter != usedObjectIDs.end() ) && ( objectIDCounter == *usedIter ) )
                        {
                            ++usedIter;
                            ++objectIDCounter;
                        }
                        const TypeID newTypeID = TypeID::make_context( objectIDCounter );
                        pConcreteSymbolTypeID->set_id( newTypeID );
                        new_concrete_type_ids.insert( { newTypeID, pConcreteSymbolTypeID } );
                        ++objectIDCounter;
                    }
                    objectConcreteTypeIDs.insert( { pObject, pConcreteSymbolTypeID } );
                }
            }
        }

        // establish the used subObjectIDs per objectID
        std::map< SubType, std::set< SubType > > usedSubObjectIDs;
        for( const auto typeID : usedTypeIDs )
        {
            usedSubObjectIDs[ typeID.getObjectID() ].insert( typeID.getSubObjectID() );
        }

        auto genSubObject = [ &objectConcreteTypeIDs, &usedSubObjectIDs, &new_concrete_type_ids ](
                                New::Symbols::ConcreteTypeID* pConcreteTypeID )
        {
            // locate the object
            SubType objectID = 0;
            {
                New::Concrete::Object* pObject = nullptr;
                {
                    New::Concrete::Graph::Vertex* pVertex       = pConcreteTypeID->get_vertex();
                    New::Concrete::ContextGroup*  pContextGroup = nullptr;
                    if( auto pActualContext = db_cast< New::Concrete::Context >( pVertex ) )
                    {
                        pContextGroup = pActualContext;
                    }
                    else if( auto pUser = db_cast< New::Concrete::Dimensions::User >( pVertex ) )
                    {
                        pContextGroup = pUser->get_parent_context();
                    }
                    else if( auto pLink = db_cast< New::Concrete::Dimensions::Link >( pVertex ) )
                    {
                        pContextGroup = pLink->get_parent_context();
                    }
                    else if( auto pBitset = db_cast< New::Concrete::Dimensions::Bitset >( pVertex ) )
                    {
                        pContextGroup = pBitset->get_parent_object();
                    }
                    else
                    {
                        THROW_RTE( "Unknown vertex type" );
                    }

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
                std::set< SubType >& subobjectIDs = usedSubObjectIDs[ objectID ];
                SubType              subObjectID  = 0U;
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
                genSubObject( pSymbolTypeID );
            }
        }

        return newDatabase.construct< New::Symbols::SymbolTable >(
            New::Symbols::SymbolTable::Args{ pSymbolTable, new_concrete_type_id_sequences, new_concrete_type_ids } );
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

                    calculateSymbolTable( newDatabase, pSymbolTable, oldConcreteTypeIDSequences );
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

                calculateSymbolTable( database, pSymbolTable, oldConcreteTypeIDSequences );
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

        const std::map< mega::TypeIDSequence, Symbols::ConcreteTypeID* > typeIDSequences
            = pSymbolTable->get_concrete_type_id_sequences();

        ConcreteTypeRollout::TypeIDSequenceGen typeIDSequenceGenerator;

        for( auto pVertex : database.many< Concrete::Graph::Vertex >( m_sourceFilePath ) )
        {
            if( auto pRoot = db_cast< Concrete::Root >( pVertex ) )
            {
                // special case for concrete tree root
                database.construct< Concrete::Graph::Vertex >( Concrete::Graph::Vertex::Args{ pVertex, TypeID{} } );
            }
            else
            {
                auto iFind = typeIDSequences.find( typeIDSequenceGenerator( pVertex ) );
                VERIFY_RTE( iFind != typeIDSequences.end() );
                database.construct< Concrete::Graph::Vertex >(
                    Concrete::Graph::Vertex::Args{ pVertex, iFind->second->get_id() } );
            }
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
