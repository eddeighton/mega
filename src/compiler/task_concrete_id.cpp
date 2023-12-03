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
#include "mega/values/compilation/symbol_table.hpp"

#include "base_task.hpp"

#include "database/ConcreteTypeAnalysis.hxx"
// #include "database/ConcreteTypeAnalysisView.hxx"
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
    requestNewSymbols( ConcreteTypeAnalysis::Database&             newDatabase,
                       ConcreteTypeAnalysis::Symbols::SymbolTable* pSymbolTable )
    {
        using namespace ConcreteTypeAnalysis;

        const auto  sourceFiles = getSortedSourceFiles();
        SymbolTable symbolTable = m_environment.getSymbolTable();

        TypeIDSequenceGen typeIDSequenceGenerator;

        // request new concrete type IDs
        {
            SymbolRequest request;
            for( const mega::io::megaFilePath& newSourceFile : sourceFiles )
            {
                for( Concrete::Graph::Vertex* pVertex : newDatabase.many< Concrete::Graph::Vertex >( newSourceFile ) )
                {
                    if( db_cast< Concrete::Root >( pVertex ) )
                        continue;

                    const SymbolTraits::TypeIDSequencePair idSeq = typeIDSequenceGenerator( pVertex );
                    if( auto* pConcrete = symbolTable.findConcreteObject( idSeq.first ) )
                    {
                        if( !idSeq.second.empty() )
                        {
                            if( TypeID{} == pConcrete->find( idSeq.second ) )
                            {
                                request.newConcreteElements.insert( idSeq );
                            }
                        }
                    }
                    else
                    {
                        request.newConcreteObjects.insert( idSeq.first );
                        if( !idSeq.second.empty() )
                        {
                            request.newConcreteElements.insert( idSeq );
                        }
                    }
                }
            }
            if( !request.newConcreteElements.empty() || !request.newConcreteObjects.empty() )
            {
                symbolTable = m_environment.newSymbols( request );
            }
        }

        using NewTypeIDSequenceMap = std::map< mega::TypeIDSequence, Symbols::ConcreteTypeID* >;
        using NewTypeIDMap         = std::map< mega::TypeID, Symbols::ConcreteTypeID* >;

        NewTypeIDSequenceMap new_concrete_type_id_sequences;
        NewTypeIDMap         new_concrete_type_ids;

        // construct concrete type IDs
        {
            for( const mega::io::megaFilePath& newSourceFile : sourceFiles )
            {
                for( Concrete::Graph::Vertex* pVertex : newDatabase.many< Concrete::Graph::Vertex >( newSourceFile ) )
                {
                    if( db_cast< Concrete::Root >( pVertex ) )
                        continue;

                    mega::TypeIDSequence typeIDSequence;
                    mega::TypeID         typeID;

                    {
                        const SymbolTraits::TypeIDSequencePair idSeq = typeIDSequenceGenerator( pVertex );
                        typeIDSequence                               = idSeq.first;

                        auto* pConcrete = symbolTable.findConcreteObject( idSeq.first );
                        VERIFY_RTE( pConcrete );
                        if( !idSeq.second.empty() )
                        {
                            std::copy( idSeq.second.begin(), idSeq.second.end(), std::back_inserter( typeIDSequence ) );
                            typeID = pConcrete->find( idSeq.second );
                        }
                        else
                        {
                            typeID = TypeID::make_object_from_objectID( pConcrete->getObjectID() );
                        }
                    }

                    VERIFY_RTE( typeID != TypeID{} );

                    auto pNewConcreteSymbol
                        = newDatabase.construct< Symbols::ConcreteTypeID >( Symbols::ConcreteTypeID::Args{ typeID } );
                    pNewConcreteSymbol->set_vertex( pVertex );

                    VERIFY_RTE(
                        new_concrete_type_id_sequences.insert( { typeIDSequence, pNewConcreteSymbol } ).second );
                    VERIFY_RTE( new_concrete_type_ids.insert( { typeID, pNewConcreteSymbol } ).second );
                }
            }
        }

        return newDatabase.construct< Symbols::SymbolTable >(
            Symbols::SymbolTable::Args{ pSymbolTable, new_concrete_type_id_sequences, new_concrete_type_ids } );
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

        using namespace ConcreteTypeAnalysis;
        Database database( m_environment, manifestFilePath );

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        requestNewSymbols( database, pSymbolTable );

        const task::FileHash fileHashCode = database.save_ConcreteTable_to_temp();
        m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
        m_environment.temp_to_real( symbolCompilationFile );
        m_environment.stash( symbolCompilationFile, determinant );
        succeeded( taskProgress );
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
                mega::TypeIDSequence typeIDSequence;
                {
                    const auto idSeqPair = typeIDSequenceGenerator( pVertex );
                    typeIDSequence       = idSeqPair.first;
                    std::copy( idSeqPair.second.begin(), idSeqPair.second.end(), std::back_inserter( typeIDSequence ) );
                }

                auto iFind = typeIDSequences.find( typeIDSequence );
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
