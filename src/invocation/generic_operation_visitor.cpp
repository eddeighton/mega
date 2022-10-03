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

#include "invocation/generic_operation_visitor.hpp"
#include "invocation/invocation.hpp"
#include "database/model/OperationsStage.hxx"

namespace mega::invocation
{

using namespace OperationsStage;
using namespace OperationsStage::Invocations;

template < typename TInstruction, typename... ConstructorArgs >
TInstruction* make_ins_group( OperationsStage::Database& database, Instructions::InstructionGroup* pParentInstruction,
                              ConstructorArgs&&... ctorArgs )
{
    // clang-format off
    TInstruction* pNewInstruction = database.construct< TInstruction >
    (
        typename TInstruction::Args
        {
            OperationsStage::Invocations::Instructions::InstructionGroup::Args
            {
                OperationsStage::Invocations::Instructions::Instruction::Args{},
                {}
            },
            ctorArgs...
        }
    );
    // clang-format on
    if ( pParentInstruction )
    {
        pParentInstruction->push_back_children( pNewInstruction );
    }

    return pNewInstruction;
}

template < typename TVariable, typename... ConstructorArgs >
TVariable* make_variable( OperationsStage::Database& database, OperationsStage::Operations::Invocation* pInvocation,
                          ConstructorArgs&&... ctorArgs )
{
    TVariable* pVariable = database.construct< TVariable >( typename TVariable::Args{ ctorArgs... } );
    pInvocation->push_back_variables( pVariable );
    return pVariable;
}

Instructions::InstructionGroup* make_elim_ins( OperationsStage::Database&      database,
                                               Instructions::InstructionGroup* pInstruction )
{
    using OperationsStage::Invocations::Instructions::Elimination;
    using OperationsStage::Invocations::Instructions::Instruction;
    using OperationsStage::Invocations::Instructions::InstructionGroup;
    Elimination* pEliminationInstruction
        = database.construct< Elimination >( Elimination::Args{ InstructionGroup::Args{ Instruction::Args{}, {} } } );
    pInstruction->push_back_children( pEliminationInstruction );
    return pEliminationInstruction;
}

Instructions::InstructionGroup* make_failure_ins( OperationsStage::Database&      database,
                                                  Instructions::InstructionGroup* pInstruction )
{
    using OperationsStage::Invocations::Instructions::Failure;
    using OperationsStage::Invocations::Instructions::Instruction;
    using OperationsStage::Invocations::Instructions::InstructionGroup;
    Failure* pFailureInstruction
        = database.construct< Failure >( Failure::Args{ InstructionGroup::Args{ Instruction::Args{}, {} } } );
    pInstruction->push_back_children( pFailureInstruction );
    return pFailureInstruction;
}

GenericOperationVisitor::GenericOperationVisitor( Database&                                database,
                                                  OperationsStage::Operations::Invocation* pInvocation )
    : m_database( database )
    , m_pInvocation( pInvocation )
{
}

void getRootPath( Concrete::ContextGroup* pContextGroup, std::list< Concrete::ContextGroup* >& path )
{
    Concrete::ContextGroup* pIter = pContextGroup;
    while ( pIter )
    {
        path.push_front( pIter );
        if ( Concrete::Context* pContext = dynamic_database_cast< Concrete::Context >( pIter ) )
        {
            pIter = pContext->get_parent();
        }
        else
        {
            pIter = nullptr;
        }
    }
}

Concrete::ContextGroup* GenericOperationVisitor::findCommonRoot( Concrete::ContextGroup* pLeft,
                                                                 Concrete::ContextGroup* pRight ) const
{
    if ( pLeft == pRight )
        return pLeft;

    std::list< Concrete::ContextGroup* > left;
    getRootPath( pLeft, left );
    std::list< Concrete::ContextGroup* > right;
    getRootPath( pRight, right );

    Concrete::ContextGroup* pCommonRoot = nullptr;
    for ( std::list< Concrete::ContextGroup* >::iterator iLeft = left.begin(), iRight = right.begin();
          iLeft != left.end() && iRight != right.end() && *iLeft == *iRight; ++iLeft, ++iRight )
        pCommonRoot = *iLeft;
    return pCommonRoot;
}

void GenericOperationVisitor::commonRootDerivation( Concrete::ContextGroup* pFrom, Concrete::ContextGroup* pTo,
                                                    Instructions::InstructionGroup*& pInstruction,
                                                    Variables::Instance*&            pVariable ) const
{
    if ( pFrom && pFrom != pTo )
    {
        Concrete::ContextGroup* pCommon = findCommonRoot( pFrom, pTo );
        VERIFY( pCommon, Exception, "Common root derivation failed" );

        if ( Concrete::Root* pRoot = dynamic_database_cast< Concrete::Root >( pCommon ) )
        {
            Concrete::Context* pFromContext = dynamic_database_cast< Concrete::Context >( pFrom );
            Concrete::Context* pToContext   = dynamic_database_cast< Concrete::Context >( pTo );
            if ( pFromContext && pToContext )
            {
                THROW_INVOCATION_EXCEPTION( "Invocation attempts to derive through root. From: "
                                            << pFromContext->get_interface()->get_identifier()
                                            << " To: " << pToContext->get_interface()->get_identifier() );
            }
            else if ( pFromContext )
            {
                THROW_INVOCATION_EXCEPTION( "Invocation attempts to derive through root. From: "
                                            << pFromContext->get_interface()->get_identifier() );
            }
            else if ( pToContext )
            {
                THROW_INVOCATION_EXCEPTION( "Invocation attempts to derive through root.  To: "
                                            << pToContext->get_interface()->get_identifier() );
            }
            else
            {
                THROW_INVOCATION_EXCEPTION( "Invocation attempts to derive through root. " );
            }
        }

        while ( pFrom != pCommon )
        {
            Concrete::Context* pFromContext = dynamic_database_cast< Concrete::Context >( pFrom );
            VERIFY( pFromContext, Exception, "" );
            Concrete::ContextGroup* pParentContextGroup = pFromContext->get_parent();
            VERIFY( pParentContextGroup, Exception, "" );
            Concrete::Context* pParent = dynamic_database_cast< Concrete::Context >( pParentContextGroup );
            VERIFY( pParent, Exception, "" );

            // generate parent derivation instruction
            Variables::Instance* pInstanceVariable = make_variable< Variables::Instance >(
                m_database, m_pInvocation,
                Variables::Instance::Args{ Variables::Variable::Args{ pVariable }, pParent } );

            pInstruction = make_ins_group< Instructions::ParentDerivation >(
                m_database, pInstruction, pVariable, pInstanceVariable );

            pVariable = pInstanceVariable;
            pFrom     = pParent;
        }

        std::vector< Concrete::ContextGroup* > path;
        while ( pTo != pCommon )
        {
            path.push_back( pTo );
            Concrete::Context* p = dynamic_database_cast< Concrete::Context >( pTo );
            VERIFY( p, Exception, "" );
            pTo = p->get_parent();
        }
        std::reverse( path.begin(), path.end() );

        for ( Concrete::ContextGroup* pConcreteContextIter : path )
        {
            // if( pConcreteContextIter->getLocalDomainSize() != 1 )
            //{
            //    pInstruction = make_failure_ins( m_database, pInstruction );
            //    return;
            //}
            Concrete::Context* pConcreteContext = dynamic_database_cast< Concrete::Context >( pConcreteContextIter );
            VERIFY( pConcreteContext, Exception, "" );

            Variables::Instance* pInstanceVariable = make_variable< Variables::Instance >(
                m_database, m_pInvocation,
                Variables::Instance::Args{ Variables::Variable::Args{ pVariable }, pConcreteContext } );

            pInstruction = make_ins_group< Instructions::ChildDerivation >(
                m_database, pInstruction, pVariable, pInstanceVariable );
            pVariable = pInstanceVariable;
        }
    }
}

void GenericOperationVisitor::buildOperation( OperationsStage::Operations::Name* prev,
                                              OperationsStage::Operations::Name& current,
                                              Instructions::InstructionGroup&    parentInstruction,
                                              Variables::Instance&               variable )
{
    auto currentConcrete = current.get_element()->get_concrete();

    Instructions::InstructionGroup* pInstruction = &parentInstruction;
    Variables::Instance*            pInstance    = &variable;

    VERIFY( prev->get_element()->get_concrete()->get_context().has_value(), Exception, "" );
    Concrete::Context* pPrevConcrete = prev->get_element()->get_concrete()->get_context().value();

    if ( currentConcrete->get_context().has_value() )
    {
        OperationsStage::Operations::InterfaceVariant* pInterfaceVar     = current.get_element()->get_interface();
        Interface::IContext*                           pCurrentInterface = pInterfaceVar->get_context().value();
        Concrete::Context* pCurrentConcrete = current.get_element()->get_concrete()->get_context().value();

        if ( Concrete::Object* pCurrentConcreteObject = dynamic_database_cast< Concrete::Object >( pCurrentConcrete ) )
        {
            // only operation on actual object is to allocate it.
            using OperationsStage::Invocations::Instructions::Instruction;
            using OperationsStage::Invocations::Operations::BasicOperation;
            using OperationsStage::Invocations::Operations::Operation;

            switch ( m_pInvocation->get_operation() )
            {
                case id_Imp_NoParams:
                {
                    // only derive the parent for the starter
                    using OperationsStage::Invocations::Operations::Allocate;
                    Allocate* pAllocate = m_database.construct< Allocate >( Allocate::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pAllocate );
                }
                break;
                default:
                    THROW_INVOCATION_EXCEPTION( "Invalid invocation on object" );
            }
        }
        else
        {
            commonRootDerivation( pPrevConcrete, pCurrentConcrete->get_parent(), pInstruction, pInstance );

            using OperationsStage::Invocations::Instructions::Instruction;
            using OperationsStage::Invocations::Operations::BasicOperation;
            using OperationsStage::Invocations::Operations::Operation;

            switch ( m_pInvocation->get_operation() )
            {
                case id_Imp_NoParams:
                case id_Imp_Params:
                {
                    // only derive the parent for the starter
                    using OperationsStage::Invocations::Operations::Call;
                    Call* pCall = m_database.construct< Call >( Call::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pCall );
                }
                break;
                case id_Start:
                {
                    using OperationsStage::Invocations::Operations::Start;
                    Start* pStart = m_database.construct< Start >( Start::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pStart );
                }
                break;
                case id_Stop:
                {
                    using OperationsStage::Invocations::Operations::Stop;
                    Stop* pStop = m_database.construct< Stop >( Stop::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pStop );
                }
                break;
                case id_Pause:
                {
                    using OperationsStage::Invocations::Operations::Pause;
                    Pause* pPause = m_database.construct< Pause >( Pause::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pPause );
                }
                break;
                case id_Resume:
                {
                    using OperationsStage::Invocations::Operations::Resume;
                    Resume* pResume = m_database.construct< Resume >( Resume::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pResume );
                }
                break;
                case id_Wait:
                {
                    using OperationsStage::Invocations::Operations::WaitAction;
                    WaitAction* pWaitAction = m_database.construct< WaitAction >( WaitAction::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pWaitAction );
                }
                break;
                case id_Get:
                {
                    using OperationsStage::Invocations::Operations::GetAction;
                    GetAction* pGetAction = m_database.construct< GetAction >( GetAction::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pGetAction );
                }
                break;
                case id_Done:
                {
                    using OperationsStage::Invocations::Operations::Done;
                    Done* pDone = m_database.construct< Done >( Done::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pDone );
                }
                break;
                default:
                    THROW_RTE( "Unreachable" );
            }
        }
    }
    else if ( currentConcrete->get_dimension().has_value() )
    {
        OperationsStage::Operations::InterfaceVariant* pInterfaceVar      = current.get_element()->get_interface();
        Concrete::Dimensions::User*                    pConcreteDimension = currentConcrete->get_dimension().value();
        Concrete::Context*                             pParent            = pConcreteDimension->get_parent();
        Interface::DimensionTrait* pInterfaceDimension = pConcreteDimension->get_interface_dimension();

        commonRootDerivation( pPrevConcrete, pParent, pInstruction, pInstance );

        using OperationsStage::Invocations::Instructions::Instruction;
        using OperationsStage::Invocations::Operations::DimensionOperation;
        using OperationsStage::Invocations::Operations::Operation;

        switch ( m_pInvocation->get_operation() )
        {
            case id_Imp_NoParams:
            {
                using OperationsStage::Invocations::Operations::Read;
                Read* pRead = m_database.construct< Read >( Read::Args{
                    DimensionOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pInterfaceDimension, pConcreteDimension } } );
                pInstruction->push_back_children( pRead );
            }
            break;
            case id_Imp_Params:
            {
                /*if ( const LinkGroup* pLinkGroup = pUserDimension->getLinkGroup() )
                {
                    WriteLinkOperation* pWrite
                        = new WriteLinkOperation( pInstanceVariable, pInterfaceDimension, pUserDimension, pLinkGroup );
                    pInstruction->append( pWrite );
                }
                else*/
                {
                    OperationsStage::Operations::InterfaceVariant* pReturnType
                        = m_database.construct< OperationsStage::Operations::InterfaceVariant >(
                            OperationsStage::Operations::InterfaceVariant::Args{
                                pInstance->get_concrete()->get_interface(), std::nullopt } );

                    using OperationsStage::Invocations::Operations::Write;
                    Write* pWrite = m_database.construct< Write >( Write::Args{ DimensionOperation::Args{
                        Operation::Args{ Instruction::Args{}, pInstance, { pReturnType, pInterfaceVar }, {} },
                        pInterfaceDimension, pConcreteDimension } } );
                    pInstruction->push_back_children( pWrite );
                }
            }
            break;
            case id_Get:
            {
                using OperationsStage::Invocations::Operations::GetDimension;
                GetDimension* pGetDimension = m_database.construct< GetDimension >( GetDimension::Args{
                    DimensionOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pInterfaceDimension, pConcreteDimension } } );
                pInstruction->push_back_children( pGetDimension );
            }
            break;
            case id_Wait:
            {
                using OperationsStage::Invocations::Operations::WaitDimension;
                WaitDimension* pWaitDimension = m_database.construct< WaitDimension >( WaitDimension::Args{
                    DimensionOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar }, {} },
                                              pInterfaceDimension, pConcreteDimension } } );
                pInstruction->push_back_children( pWaitDimension );
            }
            break;
            case id_Start:
            case id_Stop:
            case id_Pause:
            case id_Resume:
            case id_Done:
                THROW_INVOCATION_EXCEPTION( "Invalid invocation operation on dimension" );
            default:
                THROW_RTE( "Unreachable" );
        }
    }
    else
    {
        THROW_INVOCATION_EXCEPTION( "Invalid invocation target" );
    }
}

void GenericOperationVisitor::buildDimensionReference( OperationsStage::Operations::Name* prev,
                                                       OperationsStage::Operations::Name& current,
                                                       Instructions::InstructionGroup&    parentInstruction,
                                                       Variables::Instance&               variable )
{
    Instructions::InstructionGroup* pInstructionGroup = &parentInstruction;
    Variables::Instance*            pVariable         = &variable;

    auto names = current.get_children();

    VERIFY( current.get_element()->get_concrete()->get_dimension().has_value(), Exception, "" );
    Concrete::Dimensions::User* pDimension = current.get_element()->get_concrete()->get_dimension().value();

    using OperationsStage::Invocations::Variables::Dimension;
    using OperationsStage::Invocations::Variables::Reference;
    using OperationsStage::Invocations::Variables::Variable;

    Dimension* pDimensionVariable = make_variable< Dimension >(
        m_database, m_pInvocation, Dimension::Args{ Reference::Args{ Variable::Args{ pVariable }, {} } } );

    pInstructionGroup = make_ins_group< Instructions::DimensionReferenceRead >(
        m_database, pInstructionGroup, pVariable, pDimensionVariable, pDimension );

    if ( names.size() > 1U )
    {
        pInstructionGroup
            = make_ins_group< Instructions::PolyReference >( m_database, pInstructionGroup, pDimensionVariable );

        for ( OperationsStage::Operations::Name* pChildName : names )
        {
            buildPolyCase( &current, *pChildName, *pInstructionGroup, *pDimensionVariable );
        }
    }
    else
    {
        for ( OperationsStage::Operations::Name* pChildName : names )
        {
            buildMono( &current, *pChildName, *pInstructionGroup, *pDimensionVariable );
        }
    }
}

void GenericOperationVisitor::buildGenerateName( OperationsStage::Operations::Name* prev,
                                                 OperationsStage::Operations::Name& current,
                                                 Instructions::InstructionGroup&    parentInstruction,
                                                 Variables::Instance&               variable )
{
    Instructions::InstructionGroup* pInstructionGroup = &parentInstruction;
    Variables::Instance*            pVariable         = &variable;

    auto names = current.get_children();

    if ( names.empty() )
    {
        buildOperation( prev, current, *pInstructionGroup, variable );
    }
    else if ( current.get_is_reference() )
    {
        Concrete::Context*   pPrevConcrete     = prev->get_element()->get_concrete()->get_context().value();
        Concrete::Context*   pCurrentConcrete  = current.get_element()->get_concrete()->get_context().value();
        Interface::IContext* pCurrentInterface = current.get_element()->get_interface()->get_context().value();

        commonRootDerivation( pPrevConcrete, pCurrentConcrete->get_parent(), pInstructionGroup, pVariable );
        buildDimensionReference( prev, current, *pInstructionGroup, *pVariable );
    }
    else
    {
        Concrete::Context* pPrevConcrete    = prev->get_element()->get_concrete()->get_context().value();
        Concrete::Context* pCurrentConcrete = current.get_element()->get_concrete()->get_context().value();

        // commonRootDerivation( pPrevConcrete, pCurrentConcrete->get_parent(), pInstructionGroup, pVariable );
        commonRootDerivation( pPrevConcrete, pCurrentConcrete, pInstructionGroup, pVariable );

        if ( names.size() > 1U )
        {
            pInstructionGroup = make_elim_ins( m_database, pInstructionGroup );
        }

        for ( OperationsStage::Operations::Name* pChildName : names )
        {
            buildGenerateName( &current, *pChildName, *pInstructionGroup, *pVariable );
        }
    }
}

void GenericOperationVisitor::buildPolyCase( OperationsStage::Operations::Name* prev,
                                             OperationsStage::Operations::Name& current,
                                             Instructions::InstructionGroup&    parentInstruction,
                                             Variables::Reference&              variable )
{
    Instructions::InstructionGroup* pInstructionGroup = &parentInstruction;

    ASSERT( current.get_element()->get_concrete()->get_context().has_value() );
    Concrete::Context* pContext = current.get_element()->get_concrete()->get_context().value();
    VERIFY( pContext, Exception, "" );

    Variables::Instance* pInstance = make_variable< Variables::Instance >(
        m_database, m_pInvocation, Variables::Instance::Args{ Variables::Variable::Args{ &variable }, pContext } );

    {
        pInstructionGroup
            = make_ins_group< Instructions::PolyCase >( m_database, pInstructionGroup, &variable, pInstance );
    }

    auto names = current.get_children();
    if ( names.empty() )
    {
        buildOperation( prev, current, *pInstructionGroup, *pInstance );
    }
    else
    {
        if ( names.size() > 1U )
        {
            pInstructionGroup = make_elim_ins( m_database, pInstructionGroup );
        }
        ASSERT( !names.empty() );
        for ( OperationsStage::Operations::Name* pChildName : names )
        {
            buildGenerateName( &current, *pChildName, *pInstructionGroup, *pInstance );
        }
    }
}

void GenericOperationVisitor::buildMono( OperationsStage::Operations::Name* prev,
                                         OperationsStage::Operations::Name& current,
                                         Instructions::InstructionGroup&    parentInstruction,
                                         Variables::Reference&              variable )
{
    Instructions::InstructionGroup* pInstructionGroup = &parentInstruction;
    Variables::Reference*           pVariable         = &variable;
    auto                            names             = current.get_children();

    ASSERT( current.get_element()->get_concrete()->get_context().has_value() );
    Concrete::Context* pContext = current.get_element()->get_concrete()->get_context().value();
    VERIFY( pContext, Exception, "" );

    Variables::Instance* pInstance = make_variable< Variables::Instance >(
        m_database, m_pInvocation, Variables::Instance::Args{ Variables::Variable::Args{ &variable }, pContext } );

    pInstructionGroup
        = make_ins_group< Instructions::MonoReference >( m_database, pInstructionGroup, pVariable, pInstance );

    if ( names.empty() )
    {
        buildOperation( prev, current, *pInstructionGroup, *pInstance );
    }
    else
    {
        if ( names.size() > 1U )
        {
            pInstructionGroup = make_elim_ins( m_database, pInstructionGroup );
        }
        ASSERT( !names.empty() );
        for ( OperationsStage::Operations::Name* pChildName : names )
        {
            buildGenerateName( &current, *pChildName, *pInstructionGroup, *pInstance );
        }
    }
}

void GenericOperationVisitor::operator()()
{
    OperationsStage::Operations::NameResolution* pNameResolution = m_pInvocation->get_name_resolution();

    OperationsStage::Operations::NameRoot* pNameRoot = pNameResolution->get_root_name();

    std::vector< Concrete::Context* > types;

    for ( OperationsStage::Operations::Name* pName : pNameRoot->get_children() )
    {
        auto contextOpt = pName->get_element()->get_concrete()->get_context();
        VERIFY( contextOpt.has_value(), Exception, "" );
        types.push_back( contextOpt.value() );
    }

    // clang-format off
    Variables::Context* pContext = make_variable< Variables::Context >(
        m_database, m_pInvocation,
        Variables::Context::Args
        {
            Variables::Reference::Args
            { 
                Variables::Variable::Args
                { 
                    std::optional< Variables::Variable* >() 
                }, 
                types 
            } 
        } );
    // clang-format on

    Instructions::Root* pRoot = make_ins_group< Instructions::Root >( m_database, nullptr, pContext );

    m_pInvocation->set_root_instruction( pRoot );

    if ( types.size() > 1 )
    {
        Instructions::PolyReference* pPolyReference
            = make_ins_group< Instructions::PolyReference >( m_database, pRoot, pContext );

        for ( OperationsStage::Operations::Name* pName : pNameRoot->get_children() )
        {
            buildPolyCase( nullptr, *pName, *pPolyReference, *pContext );
        }
    }
    else
    {
        for ( OperationsStage::Operations::Name* pName : pNameRoot->get_children() )
        {
            buildMono( nullptr, *pName, *pRoot, *pContext );
        }
    }
}

} // namespace mega::invocation
