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
#include "database/manifest.hxx"

#include "database/exception.hpp"
#include "database/sources.hpp"

namespace ConcreteTypeAnalysis
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/concrete_typeid_sequence.hpp"
} // namespace ConcreteTypeAnalysis

namespace mega::compiler
{
using namespace ConcreteTypeAnalysis;

class Task_ConcreteTypeAnalysis : public BaseTask
{
public:
    Task_ConcreteTypeAnalysis( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
    {
    }

    Symbols::SymbolTable*
    requestNewSymbols( Database&             database,
                       Symbols::SymbolTable* pSymbolTable )
    {
        using namespace ConcreteTypeAnalysis;

        const auto manifestPath = m_environment.project_manifest();

        SymbolTable symbolTable = m_environment.getSymbolTable();

        TypeIDSequenceGen typeIDSequenceGenerator;

        // request new concrete type IDs
        {
            SymbolRequest request;
            for( Concrete::Node* pNode : database.many< Concrete::Node >( manifestPath ) )
            {
                const SymbolTraits::TypeIDSequencePair idSeq = typeIDSequenceGenerator( pNode );
                if( auto* pConcrete = symbolTable.findConcreteObject( idSeq.first ) )
                {
                    if( !idSeq.second.empty() )
                    {
                        if( concrete::NULL_TYPE_ID == pConcrete->find( idSeq.second ) )
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
            if( !request.newConcreteElements.empty() || !request.newConcreteObjects.empty() )
            {
                symbolTable = m_environment.newSymbols( request );
            }
        }

        using NewTypeIDSequenceMap = std::map< mega::interface::TypeIDSequence, Symbols::ConcreteID* >;
        using NewTypeIDMap         = std::map< mega::concrete::TypeID, Symbols::ConcreteID* >;

        NewTypeIDSequenceMap new_concrete_type_id_sequences;
        NewTypeIDMap         new_concrete_type_ids;

        // construct concrete type IDs
        {
            for( Concrete::Node* pNode : database.many< Concrete::Node >( manifestPath ) )
            {
                mega::interface::TypeIDSequence typeIDSequence;
                mega::concrete::TypeID          concreteTypeID;

                {
                    const SymbolTraits::TypeIDSequencePair idSeq = typeIDSequenceGenerator( pNode );
                    typeIDSequence                               = idSeq.first;

                    auto* pConcrete = symbolTable.findConcreteObject( idSeq.first );
                    VERIFY_RTE( pConcrete );
                    if( !idSeq.second.empty() )
                    {
                        std::copy( idSeq.second.begin(), idSeq.second.end(), std::back_inserter( typeIDSequence ) );
                        concreteTypeID = pConcrete->find( idSeq.second );
                    }
                    else
                    {
                        concreteTypeID = concrete::TypeID( pConcrete->getObjectID(), concrete::NULL_SUB_OBJECT_ID );
                    }
                }

                VERIFY_RTE( concreteTypeID != concrete::NULL_TYPE_ID );

                auto pNewConcreteSymbol
                    = database.construct< Symbols::ConcreteID >( Symbols::ConcreteID::Args{ concreteTypeID, pNode } );

                VERIFY_RTE( new_concrete_type_id_sequences.insert( { typeIDSequence, pNewConcreteSymbol } ).second );
                VERIFY_RTE( new_concrete_type_ids.insert( { concreteTypeID, pNewConcreteSymbol } ).second );

                database.construct< Concrete::Node >( Concrete::Node::Args
                {
                    pNode, pNewConcreteSymbol
                });
            }
        }

        return database.construct< Symbols::SymbolTable >(
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
            m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.SymbolAnalysis_SymbolTable( manifestFilePath ) )

        );

        if( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace ConcreteTypeAnalysis;
        Database database( m_environment, manifestFilePath );

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( manifestFilePath );

        requestNewSymbols( database, pSymbolTable );

        const task::FileHash fileHashCode = database.save_ConcreteTable_to_temp();
        m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
        m_environment.temp_to_real( symbolCompilationFile );
        m_environment.stash( symbolCompilationFile, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_ConcreteTypeAnalysis( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_ConcreteTypeAnalysis >( taskArguments );
}

} // namespace mega::compiler
