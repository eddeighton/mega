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

#include "invocation/elimination.hpp"
#include "database/model/OperationsStage.hxx"

namespace mega::invocation
{

using namespace OperationsStage;
using namespace OperationsStage::Operations;

namespace
{

template < typename TFunctor >
EliminationResult elimination( OperationsStage::Invocations::Instructions::Instruction* pInstruction,
                               TFunctor&                                                functor )
{
    EliminationResult eliminationResult = eSuccess;

    if ( functor( pInstruction ) )
    {
        return eFailure;
    }

    using OperationsStage::Invocations::Instructions::InstructionGroup;

    if ( InstructionGroup* pInstructionGroup = db_cast< InstructionGroup >( pInstruction ) )
    {
        using OperationsStage::Invocations::Instructions::Elimination;
        using OperationsStage::Invocations::Instructions::Failure;
        using OperationsStage::Invocations::Instructions::Instruction;
        using OperationsStage::Invocations::Instructions::Prune;
        using Vector = std::vector< Instruction* >;

        Vector children = pInstructionGroup->get_children();
        for ( Instruction* pChildInstruction : pInstructionGroup->get_children() )
        {
            if ( Elimination* pElimination = db_cast< Elimination >( pChildInstruction ) )
            {
                Vector success, failure, ambiguous;
                for ( Instruction* pEliminationChild : pElimination->get_children() )
                {
                    VERIFY_RTE( !db_cast< Elimination >( pEliminationChild ) );

                    const EliminationResult nestedResult = elimination( pEliminationChild, functor );
                    switch ( nestedResult )
                    {
                        case eSuccess:
                            success.push_back( pEliminationChild );
                            break;
                        case eFailure:
                            failure.push_back( pEliminationChild );
                            break;
                        case eAmbiguous:
                            ambiguous.push_back( pEliminationChild );
                            break;
                    }
                }

                pElimination->set_children( {} );

                if ( !ambiguous.empty() || ( success.size() > 1U ) )
                {
                    Vector nested = std::move( ambiguous );
                    std::copy( success.begin(), success.end(), std::back_inserter( nested ) );
                    eliminationResult = eAmbiguous;

                    // keep the elimination instruction but remove the failures
                    pElimination->set_children( nested );
                    children.push_back( pElimination );
                }
                else if ( success.size() == 1U )
                {
                    // replace the elimination instruction with the unambiguous success
                    children.push_back( success.front() );
                }
                else if ( success.empty() )
                {
                    if ( eliminationResult != eAmbiguous )
                        eliminationResult = eFailure;

                    children.push_back( pElimination );
                }
            }
            else if ( auto pFailure = db_cast< Failure >( pChildInstruction ) )
            {
                if ( eliminationResult != eAmbiguous )
                    eliminationResult = eFailure;
                children.push_back( pFailure );
            }
            else if ( auto pPrune = db_cast< Prune >( pChildInstruction ) )
            {
                Vector success, failure;

                for ( Instruction* pPruneChild : pPrune->get_children() )
                {
                    VERIFY_RTE( !db_cast< Prune >( pPruneChild ) );

                    const EliminationResult nestedResult = elimination( pPruneChild, functor );
                    switch ( nestedResult )
                    {
                        case eSuccess:
                            success.push_back( pPruneChild );
                            break;
                        case eFailure:
                            failure.push_back( pPruneChild );
                            break;
                        case eAmbiguous:
                            THROW_RTE( "Unreachable" );
                    }
                }

                pPrune->set_children( {} );

                if ( !success.empty() )
                {
                    // successful
                    VERIFY_RTE( children.size() == 1 );
                    children = std::move( success );
                }
                else if ( success.empty() )
                {
                    eliminationResult = eFailure;
                }
            }
            else
            {
                const EliminationResult nestedResult = elimination( pChildInstruction, functor );
                switch ( nestedResult )
                {
                    case eSuccess:
                        break;
                    case eFailure:
                        if ( eliminationResult != eAmbiguous )
                            eliminationResult = eFailure;
                        break;
                    case eAmbiguous:
                        eliminationResult = eAmbiguous;
                        break;
                }
            }
        }
        pInstructionGroup->set_children( children );
    }

    return eliminationResult;
}
} // namespace

EliminationResult firstStageElimination( OperationsStage::Invocations::Instructions::Instruction* pInstruction )
{
    using OperationsStage::Invocations::Instructions::Instruction;
    using OperationsStage::Invocations::Instructions::Failure;
    auto functor = []( Instruction* _pInstruction ) -> bool { return db_cast< Failure >( _pInstruction ); };
    return elimination( pInstruction, functor );
}

EliminationResult
secondStageElimination( const std::vector< OperationsStage::Invocations::Operations::Operation* >& candidateOperations,
                        OperationsStage::Invocations::Instructions::Instruction*                   pInstruction )
{
    EliminationResult eliminationResult = eSuccess;

    using OperationsStage::Invocations::Instructions::Instruction;
    using OperationsStage::Invocations::Operations::Operation;

    auto functor = [ &candidateOperations ]( Instruction* pInstruction ) -> bool
    {
        if ( Operation* pOperation = db_cast< Operation >( pInstruction ) )
        {
            auto iFind = std::find( candidateOperations.begin(), candidateOperations.end(), pOperation );
            if ( iFind == candidateOperations.end() )
            {
                return false;
            }
        }
        return true;
    };
    return elimination( pInstruction, functor );
}

std::vector< OperationsStage::Invocations::Operations::Operation* >
getOperations( OperationsStage::Invocations::Instructions::Instruction* pInstruction )
{
    using OperationsStage::Invocations::Instructions::Instruction;
    using OperationsStage::Invocations::Instructions::InstructionGroup;
    using OperationsStage::Invocations::Operations::Operation;

    std::vector< Operation* > operations;

    if ( Operation* pOperation = db_cast< Operation >( pInstruction ) )
    {
        operations.push_back( pOperation );
    }

    if ( InstructionGroup* pInstructionGroup = db_cast< InstructionGroup >( pInstruction ) )
    {
        for ( Instruction* pChildInstruction : pInstructionGroup->get_children() )
        {
            const std::vector< Operation* > temp = getOperations( pChildInstruction );
            std::copy( temp.begin(), temp.end(), std::back_inserter( operations ) );
        }
    }
    return operations;
}

} // namespace mega::invocation
