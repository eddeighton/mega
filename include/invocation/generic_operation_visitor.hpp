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

#ifndef GENERIC_OPERATION_VISITOR_10_JUNE_2022
#define GENERIC_OPERATION_VISITOR_10_JUNE_2022

#include "database/model/OperationsStage.hxx"

namespace mega::invocation
{

class GenericOperationVisitor
{
public:
    GenericOperationVisitor( OperationsStage::Database&               database,
                             OperationsStage::Operations::Invocation* pInvocation );

private:
    OperationsStage::Concrete::ContextGroup* findCommonRoot( OperationsStage::Concrete::ContextGroup* pLeft,
                                                             OperationsStage::Concrete::ContextGroup* pRight ) const;

    bool commonRootDerivation( OperationsStage::Concrete::ContextGroup*                       pFrom,
                               OperationsStage::Concrete::ContextGroup*                       pTo,
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

} // namespace mega::invocation

#endif // GENERIC_OPERATION_VISITOR_10_JUNE_2022
