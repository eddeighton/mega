
#ifndef ELIMINATION_11_JUNE_2022
#define ELIMINATION_11_JUNE_2022

#include "database/model/OperationsStage.hxx"

namespace mega
{
namespace invocation
{

enum EliminationResult
{
    eSuccess,
    eFailure,
    eAmbiguous
};

EliminationResult firstStageElimination( OperationsStage::Invocations::Instructions::Instruction* pInstruction );

EliminationResult
secondStageElimination( const std::vector< OperationsStage::Invocations::Operations::Operation* >& candidateOperations,
                        OperationsStage::Invocations::Instructions::Instruction*                   pInstruction );

std::vector< OperationsStage::Invocations::Operations::Operation* >
getOperations( OperationsStage::Invocations::Instructions::Instruction* pInstruction );

} // namespace invocation
} // namespace mega

#endif // ELIMINATION_11_JUNE_2022