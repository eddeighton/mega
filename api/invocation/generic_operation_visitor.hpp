
#ifndef GENERIC_OPERATION_VISITOR_10_JUNE_2022
#define GENERIC_OPERATION_VISITOR_10_JUNE_2022

#include "database/model/OperationsStage.hxx"

namespace mega
{
namespace invocation
{

class GenericOperationVisitor
{
public:
    GenericOperationVisitor( OperationsStage::Database&               database,
                             OperationsStage::Operations::Invocation* pInvocation );

private:
    OperationsStage::Concrete::Context* findCommonRoot( OperationsStage::Concrete::Context* pLeft,
                                                        OperationsStage::Concrete::Context* pRight ) const;

    void commonRootDerivation( OperationsStage::Concrete::Context* pFrom, OperationsStage::Concrete::Context* pTo,
                               OperationsStage::Invocations::Instructions::InstructionGroup*& pInstruction,
                               OperationsStage::Invocations::Variables::Instance*&            pVariable ) const;

    void buildOperation( OperationsStage::Operations::Name*                            prev,
                         OperationsStage::Operations::Name&                            current,
                         OperationsStage::Invocations::Instructions::InstructionGroup& parentInstruction,
                         OperationsStage::Invocations::Variables::Instance&            variable );

    void buildDimensionReference( OperationsStage::Operations::Name*                            prev,
                                  OperationsStage::Operations::Name&                            current,
                                  OperationsStage::Invocations::Instructions::InstructionGroup& parentInstruction,
                                  OperationsStage::Invocations::Variables::Instance&            variable );

    void buildGenerateName( OperationsStage::Operations::Name*                            prev,
                            OperationsStage::Operations::Name&                            current,
                            OperationsStage::Invocations::Instructions::InstructionGroup& parentInstruction,
                            OperationsStage::Invocations::Variables::Instance&            variable );

    void buildPolyCase( OperationsStage::Operations::Name*                            prev,
                        OperationsStage::Operations::Name&                            current,
                        OperationsStage::Invocations::Instructions::InstructionGroup& parentInstruction,
                        OperationsStage::Invocations::Variables::Reference&           variable );

    void buildMono( OperationsStage::Operations::Name*                            prev,
                    OperationsStage::Operations::Name&                            current,
                    OperationsStage::Invocations::Instructions::InstructionGroup& parentInstruction,
                    OperationsStage::Invocations::Variables::Reference&           variable );

public:
    void operator()();

private:
    OperationsStage::Database&               m_database;
    OperationsStage::Operations::Invocation* m_pInvocation;
};

} // namespace invocation
} // namespace mega

#endif // GENERIC_OPERATION_VISITOR_10_JUNE_2022
