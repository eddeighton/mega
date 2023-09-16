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

namespace
{

std::string printContextFullType( Concrete::Context* pConcrete )
{
    std::ostringstream os;
    using IContextVector = std::vector< Interface::IContext* >;
    IContextVector       path;
    Interface::IContext* pContext = pConcrete->get_interface();
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< Interface::IContext >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    for( auto i = path.begin(), iNext = path.begin(), iEnd = path.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
        {
            os << ( *i )->get_identifier();
        }
        else
        {
            os << ( *i )->get_identifier() << ".";
        }
    }
    return os.str();
}
inline std::string describe( Concrete::ContextGroup* pContextGroup )
{
    std::ostringstream os;

    if( Concrete::Context* pContext = db_cast< Concrete::Context >( pContextGroup ) )
    {
        os << printContextFullType( pContext ) << pContext->get_concrete_id();
    }
    else
    {
        os << "Unknown context group";
    }
    return os.str();
}

} // namespace

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
    if( pParentInstruction )
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
    while( pIter )
    {
        path.push_front( pIter );
        if( Concrete::Context* pContext = db_cast< Concrete::Context >( pIter ) )
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
    if( pLeft == pRight )
        return pLeft;

    std::list< Concrete::ContextGroup* > left;
    getRootPath( pLeft, left );
    std::list< Concrete::ContextGroup* > right;
    getRootPath( pRight, right );

    Concrete::ContextGroup* pCommonRoot = nullptr;
    for( std::list< Concrete::ContextGroup* >::iterator iLeft = left.begin(), iRight = right.begin();
         iLeft != left.end() && iRight != right.end() && *iLeft == *iRight; ++iLeft, ++iRight )
        pCommonRoot = *iLeft;
    return pCommonRoot;
}

bool GenericOperationVisitor::inObjectDerivation( Concrete::ContextGroup* pFrom, Concrete::ContextGroup* pTo,
                                                  Instructions::InstructionGroup*& pInstruction,
                                                  Variables::Instance*&            pVariable ) const
{
    if( pFrom && pFrom != pTo )
    {
        Concrete::ContextGroup* pCommon = findCommonRoot( pFrom, pTo );
        VERIFY( pCommon, Exception,
                "Common root derivation failed from: " << describe( pFrom ) << " to: " << describe( pTo ) );

        if( Concrete::Root* pRoot = db_cast< Concrete::Root >( pCommon ) )
        {
            pInstruction = make_failure_ins( m_database, pInstruction );
            return false;
        }

        while( pFrom != pCommon )
        {
            Concrete::Context* pFromContext = db_cast< Concrete::Context >( pFrom );
            VERIFY( pFromContext, Exception, "" );
            Concrete::ContextGroup* pParentContextGroup = pFromContext->get_parent();
            VERIFY( pParentContextGroup, Exception, "" );
            Concrete::Context* pParent = db_cast< Concrete::Context >( pParentContextGroup );
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
        while( pTo != pCommon )
        {
            path.push_back( pTo );
            Concrete::Context* p = db_cast< Concrete::Context >( pTo );
            VERIFY( p, Exception, "" );
            pTo = p->get_parent();
        }
        std::reverse( path.begin(), path.end() );

        for( Concrete::ContextGroup* pConcreteContextIter : path )
        {
            // if( pConcreteContextIter->getLocalDomainSize() != 1 )
            //{
            //    pInstruction = make_failure_ins( m_database, pInstruction );
            //    return;
            //}
            Concrete::Context* pConcreteContext = db_cast< Concrete::Context >( pConcreteContextIter );
            VERIFY( pConcreteContext, Exception, "" );

            Variables::Instance* pInstanceVariable = make_variable< Variables::Instance >(
                m_database, m_pInvocation,
                Variables::Instance::Args{ Variables::Variable::Args{ pVariable }, pConcreteContext } );

            pInstruction = make_ins_group< Instructions::ChildDerivation >(
                m_database, pInstruction, pVariable, pInstanceVariable );
            pVariable = pInstanceVariable;
        }
    }
    return true;
}

bool GenericOperationVisitor::interObjectDerivation(
    OperationsStage::Concrete::ContextGroup*                       pFromContextGroup,
    OperationsStage::Concrete::ContextGroup*                       pTo,
    OperationsStage::Invocations::Instructions::InstructionGroup*& pInstruction,
    OperationsStage::Invocations::Variables::Instance*&            pVariable ) const
{
    using namespace OperationsStage;

    auto pFrom = db_cast< Concrete::Context >( pFromContextGroup );

    

    return false;
}

void GenericOperationVisitor::buildOperation( OperationsStage::Operations::Name* prev,
                                              OperationsStage::Operations::Name& current,
                                              Instructions::InstructionGroup&    parentInstruction,
                                              Variables::Instance&               variable )
{
    auto currentConcrete = current.get_element()->get_concrete();

    Instructions::InstructionGroup* pInstruction = &parentInstruction;
    Variables::Instance*            pInstance    = &variable;

    if( currentConcrete->get_context().has_value() )
    {
        OperationsStage::Operations::InterfaceVariant* pInterfaceVar     = current.get_element()->get_interface();
        Interface::IContext*                           pCurrentInterface = pInterfaceVar->get_context().value();
        Concrete::Context* pCurrentConcrete = current.get_element()->get_concrete()->get_context().value();

        if( Concrete::Object* pCurrentConcreteObject = db_cast< Concrete::Object >( pCurrentConcrete ) )
        {
            // only operation on actual object is to allocate, save, load, files.
            using OperationsStage::Invocations::Instructions::Instruction;
            using OperationsStage::Invocations::Operations::BasicOperation;
            using OperationsStage::Invocations::Operations::Operation;

            switch( m_pInvocation->get_operation() )
            {
                case id_Imp_NoParams:
                {
                    // Allocate can ignor deriving concrete types other than matching one
                    if( pCurrentConcrete->get_interface() != pCurrentInterface )
                    {
                        pInstruction = make_failure_ins( m_database, pInstruction );
                        return;
                    }

                    // only derive the parent for the starter
                    using OperationsStage::Invocations::Operations::Allocate;
                    Allocate* pAllocate = m_database.construct< Allocate >( Allocate::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pAllocate );
                }
                break;
                case id_Move:
                {
                    using OperationsStage::Invocations::Operations::Move;
                    Move* pMove = m_database.construct< Move >( Move::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pMove );
                }
                break;
                case id_Get:
                {
                    using OperationsStage::Invocations::Operations::GetAction;
                    GetAction* pGetAction = m_database.construct< GetAction >( GetAction::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pGetAction );
                }
                break;
                default:
                    THROW_INVOCATION_EXCEPTION( "Invalid invocation on object" );
            }
        }
        /*else if( db_cast< Concrete::Link >( pCurrentConcrete ) )
        {
            using OperationsStage::Invocations::Instructions::Instruction;
            using OperationsStage::Invocations::Operations::BasicOperation;
            using OperationsStage::Invocations::Operations::Operation;

            switch( m_pInvocation->get_operation() )
            {
                case id_Imp_NoParams:
                {
                    THROW_TODO;
                    // derive directly to the concrete context
                    Concrete::Context* pPrevConcrete = prev->get_element()->get_concrete()->get_context().value();
                    if( !inObjectDerivation( pPrevConcrete, pCurrentConcrete, pInstruction, pInstance ) )
                        return;

                    using OperationsStage::Invocations::Operations::LinkOperation;
                    using OperationsStage::Invocations::Operations::ReadLink;

                    Concrete::Link*  pConcreteLink  = db_cast< Concrete::Link >( pCurrentConcrete );
                    Interface::Link* pInterfaceLink = pConcreteLink->get_link();

                    ReadLink* pReadLink = m_database.construct< ReadLink >( ReadLink::Args{
                        LinkOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                             pInterfaceLink, pConcreteLink } } );

                    pInstruction->push_back_children( pReadLink );
                }
                break;
                case id_Imp_Params:
                {
                    THROW_TODO;
                    // derive directly to the concrete context
                    Concrete::Context* pPrevConcrete = prev->get_element()->get_concrete()->get_context().value();
                    if( !inObjectDerivation( pPrevConcrete, pCurrentConcrete, pInstruction, pInstance ) )
                        return;

                    using OperationsStage::Invocations::Operations::LinkOperation;
                    using OperationsStage::Invocations::Operations::WriteLink;

                    Concrete::Link*  pConcreteLink  = db_cast< Concrete::Link >( pCurrentConcrete );
                    Interface::Link* pInterfaceLink = pConcreteLink->get_link();

                    WriteLink* pWriteLink = m_database.construct< WriteLink >( WriteLink::Args{
                        LinkOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                             pInterfaceLink, pConcreteLink } } );

                    pInstruction->push_back_children( pWriteLink );
                }
                break;
                case id_Get:
                {
                    // derive directly to the parent of the concrete context
                    if( prev->get_element()->get_concrete()->get_context().has_value() )
                    {
                        Concrete::Context* pPrevConcrete = prev->get_element()->get_concrete()->get_context().value();
                        if( !inObjectDerivation(
                                pPrevConcrete, pCurrentConcrete->get_parent(), pInstruction, pInstance ) )
                            return;
                    }

                    using OperationsStage::Invocations::Operations::GetAction;
                    GetAction* pGetAction = m_database.construct< GetAction >( GetAction::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pGetAction );
                }
                break;
                case id_Move:
                {
                    // derive directly to the concrete context
                    Concrete::Context* pPrevConcrete = prev->get_element()->get_concrete()->get_context().value();
                    if( !inObjectDerivation( pPrevConcrete, pCurrentConcrete, pInstruction, pInstance ) )
                        return;

                    using OperationsStage::Invocations::Operations::Move;
                    Move* pMove = m_database.construct< Move >( Move::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pMove );
                }
                break;
                default:
                    THROW_RTE( "Unreachable" );
            }
        }*/
        else
        {
            // derive directly to the context for now NOT THE PARENT
            if( prev )
            {
                if( auto pPrevElement = prev->get_element() )
                {
                    if( auto pPrevConcrete = pPrevElement->get_concrete() )
                    {
                        if( auto pPrevConcreteContext = pPrevConcrete->get_context() )
                        {
                            if( pPrevConcreteContext.has_value() )
                            {
                                Concrete::Context* pPrevConcrete = pPrevConcreteContext.value();
                                if( !inObjectDerivation( pPrevConcrete, pCurrentConcrete, pInstruction, pInstance ) )
                                    return;
                            }
                        }
                    }
                }
            }

            using OperationsStage::Invocations::Instructions::Instruction;
            using OperationsStage::Invocations::Operations::BasicOperation;
            using OperationsStage::Invocations::Operations::Operation;

            switch( m_pInvocation->get_operation() )
            {
                case id_Imp_NoParams:
                {
                    if( db_cast< Concrete::Action >( pCurrentConcrete ) )
                    {
                        using OperationsStage::Invocations::Operations::Start;
                        Start* pStart = m_database.construct< Start >( Start::Args{
                            BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                                  pCurrentInterface, pCurrentConcrete } } );
                        pInstruction->push_back_children( pStart );
                    }
                    else if( db_cast< Concrete::Function >( pCurrentConcrete ) )
                    {
                        using OperationsStage::Invocations::Operations::Call;
                        Call* pCall = m_database.construct< Call >( Call::Args{
                            BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                                  pCurrentInterface, pCurrentConcrete } } );
                        pInstruction->push_back_children( pCall );
                    }
                    else if( db_cast< Concrete::Object >( pCurrentConcrete ) )
                    {
                        if( pCurrentConcrete->get_interface() != pCurrentInterface )
                        {
                            pInstruction = make_failure_ins( m_database, pInstruction );
                            return;
                        }

                        using OperationsStage::Invocations::Operations::Allocate;
                        Allocate* pAllocate = m_database.construct< Allocate >( Allocate::Args{
                            BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                                  pCurrentInterface, pCurrentConcrete } } );
                        pInstruction->push_back_children( pAllocate );
                    }
                    else
                    {
                        THROW_INVOCATION_EXCEPTION( "GenericOperationVisitor:: Invalid no params implicit call" );
                    }
                }
                break;
                case id_Imp_Params:
                {
                    if( db_cast< Concrete::Function >( pCurrentConcrete ) )
                    {
                        using OperationsStage::Invocations::Operations::Call;
                        Call* pCall = m_database.construct< Call >( Call::Args{
                            BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                                  pCurrentInterface, pCurrentConcrete } } );
                        pInstruction->push_back_children( pCall );
                    }
                    else
                    {
                        THROW_INVOCATION_EXCEPTION( "GenericOperationVisitor:: Invalid params implicit call" );
                    }
                }
                break;
                case id_Start:
                {
                    using OperationsStage::Invocations::Operations::Start;
                    Start* pStart = m_database.construct< Start >( Start::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pStart );
                }
                break;
                case id_Stop:
                {
                    using OperationsStage::Invocations::Operations::Stop;
                    Stop* pStop = m_database.construct< Stop >( Stop::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pStop );
                }
                break;
                case id_Get:
                {
                    using OperationsStage::Invocations::Operations::GetAction;
                    GetAction* pGetAction = m_database.construct< GetAction >( GetAction::Args{
                        BasicOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pCurrentInterface, pCurrentConcrete } } );
                    pInstruction->push_back_children( pGetAction );
                }
                break;
                default:
                    THROW_RTE( "Unreachable" );
            }
        }
    }
    else if( currentConcrete->get_dimension().has_value() )
    {
        OperationsStage::Operations::InterfaceVariant* pInterfaceVar      = current.get_element()->get_interface();
        Concrete::Dimensions::User*                    pConcreteDimension = currentConcrete->get_dimension().value();
        Concrete::Context*                             pParent            = pConcreteDimension->get_parent();
        Interface::DimensionTrait* pInterfaceDimension = pConcreteDimension->get_interface_dimension();

        VERIFY( prev->get_element()->get_concrete()->get_context().has_value(), Exception, "" );
        Concrete::Context* pPrevConcrete = prev->get_element()->get_concrete()->get_context().value();

        if( !inObjectDerivation( pPrevConcrete, pParent, pInstruction, pInstance ) )
            return;

        using OperationsStage::Invocations::Instructions::Instruction;
        using OperationsStage::Invocations::Operations::DimensionOperation;
        using OperationsStage::Invocations::Operations::Operation;

        switch( m_pInvocation->get_operation() )
        {
            case id_Imp_NoParams:
            {
                using OperationsStage::Invocations::Operations::Read;
                Read* pRead = m_database.construct< Read >( Read::Args{
                    DimensionOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pInterfaceDimension, pConcreteDimension } } );
                pInstruction->push_back_children( pRead );
            }
            break;
            case id_Imp_Params:
            {
                OperationsStage::Operations::InterfaceVariant* pReturnType
                    = m_database.construct< OperationsStage::Operations::InterfaceVariant >(
                        OperationsStage::Operations::InterfaceVariant::Args{
                            pInstance->get_concrete()->get_interface(), std::nullopt } );

                using OperationsStage::Invocations::Operations::Write;
                Write* pWrite = m_database.construct< Write >( Write::Args{ DimensionOperation::Args{
                    Operation::Args{ Instruction::Args{}, pInstance, { pReturnType, pInterfaceVar } },
                    pInterfaceDimension, pConcreteDimension } } );
                pInstruction->push_back_children( pWrite );
            }
            break;
            case id_Get:
            {
                using OperationsStage::Invocations::Operations::GetDimension;
                GetDimension* pGetDimension = m_database.construct< GetDimension >( GetDimension::Args{
                    DimensionOperation::Args{ Operation::Args{ Instruction::Args{}, pInstance, { pInterfaceVar } },
                                              pInterfaceDimension, pConcreteDimension } } );
                pInstruction->push_back_children( pGetDimension );
            }
            break;
            case id_Start:
            case id_Stop:
            case id_Move:
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

    if( names.size() > 1U )
    {
        pInstructionGroup
            = make_ins_group< Instructions::PolyReference >( m_database, pInstructionGroup, pDimensionVariable );

        for( OperationsStage::Operations::Name* pChildName : names )
        {
            buildPolyCase( &current, *pChildName, *pInstructionGroup, *pDimensionVariable );
        }
    }
    else
    {
        for( OperationsStage::Operations::Name* pChildName : names )
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

    if( names.empty() )
    {
        buildOperation( prev, current, *pInstructionGroup, variable );
    }
    else if( current.get_is_reference() )
    {
        Concrete::Context*   pPrevConcrete     = prev->get_element()->get_concrete()->get_context().value();
        Concrete::Context*   pCurrentConcrete  = current.get_element()->get_concrete()->get_context().value();
        Interface::IContext* pCurrentInterface = current.get_element()->get_interface()->get_context().value();

        if( !inObjectDerivation( pPrevConcrete, pCurrentConcrete->get_parent(), pInstructionGroup, pVariable ) )
            return;

        buildDimensionReference( prev, current, *pInstructionGroup, *pVariable );
    }
    else
    {
        Concrete::Context* pPrevConcrete    = prev->get_element()->get_concrete()->get_context().value();
        Concrete::Context* pCurrentConcrete = current.get_element()->get_concrete()->get_context().value();

        // determine whether in-object OR inter-object derivation step required
        if( pPrevConcrete->get_concrete_id().getObjectID() == pCurrentConcrete->get_concrete_id().getObjectID() )
        {
            if( !inObjectDerivation( pPrevConcrete, pCurrentConcrete, pInstructionGroup, pVariable ) )
                return;

            if( names.size() > 1U )
            {
                pInstructionGroup = make_elim_ins( m_database, pInstructionGroup );
            }

            for( OperationsStage::Operations::Name* pChildName : names )
            {
                buildGenerateName( &current, *pChildName, *pInstructionGroup, *pVariable );
            }
        }
        else
        {
            // attempt inter object link based derivation step
            if( !interObjectDerivation( pPrevConcrete, pCurrentConcrete, pInstructionGroup, pVariable ) )
                return;

            if( names.size() > 1U )
            {
                pInstructionGroup = make_elim_ins( m_database, pInstructionGroup );
            }

            for( OperationsStage::Operations::Name* pChildName : names )
            {
                buildGenerateName( &current, *pChildName, *pInstructionGroup, *pVariable );
            }
        }
    }
}

void GenericOperationVisitor::buildNameOrOperation( OperationsStage::Operations::Name* prev,
                                                    OperationsStage::Operations::Name& current,
                                                    Instructions::InstructionGroup&    parentInstruction,
                                                    Variables::Instance&               instance )
{
    Instructions::InstructionGroup* pInstructionGroup = &parentInstruction;

    auto names = current.get_children();
    if( names.empty() )
    {
        buildOperation( prev, current, *pInstructionGroup, instance );
    }
    else
    {
        if( names.size() > 1U )
        {
            pInstructionGroup = make_elim_ins( m_database, pInstructionGroup );
        }
        for( OperationsStage::Operations::Name* pChildName : names )
        {
            buildGenerateName( &current, *pChildName, *pInstructionGroup, instance );
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

    auto pInstance = make_variable< Variables::Instance >(
        m_database, m_pInvocation, Variables::Instance::Args{ Variables::Variable::Args{ &variable }, pContext } );

    {
        pInstructionGroup
            = make_ins_group< Instructions::PolyCase >( m_database, pInstructionGroup, &variable, pInstance );
    }

    buildNameOrOperation( prev, current, *pInstructionGroup, *pInstance );
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

    auto pInstance = make_variable< Variables::Instance >(
        m_database, m_pInvocation, Variables::Instance::Args{ Variables::Variable::Args{ &variable }, pContext } );

    pInstructionGroup
        = make_ins_group< Instructions::MonoReference >( m_database, pInstructionGroup, pVariable, pInstance );

    buildNameOrOperation( prev, current, *pInstructionGroup, *pInstance );
}

void GenericOperationVisitor::operator()()
{
    OperationsStage::Operations::NameResolution* pNameResolution = m_pInvocation->get_name_resolution();

    OperationsStage::Operations::NameRoot* pNameRoot        = pNameResolution->get_root_name();
    const auto                             nameRootChildren = pNameRoot->get_children();

    Variables::Context* pInitialContext = nullptr;
    {
        std::vector< Concrete::Context* > types;

        for( OperationsStage::Operations::Name* pName : nameRootChildren )
        {
            auto contextOpt = pName->get_element()->get_concrete()->get_context();
            VERIFY( contextOpt.has_value(), Exception, "" );
            types.push_back( contextOpt.value() );
        }

        // clang-format off
        pInitialContext = make_variable< Variables::Context >(
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
    }

    auto pRoot = make_ins_group< Instructions::Root >( m_database, nullptr, pInitialContext );
    m_pInvocation->set_root_instruction( pRoot );

    if( nameRootChildren.size() > 1 )
    {
        auto pPolyReference = make_ins_group< Instructions::PolyReference >( m_database, pRoot, pInitialContext );
        for( OperationsStage::Operations::Name* pName : nameRootChildren )
        {
            buildPolyCase( nullptr, *pName, *pPolyReference, *pInitialContext );
        }
    }
    else
    {
        VERIFY( nameRootChildren.size() == 1, Exception, "Empty name resolution" );
        buildMono( nullptr, *nameRootChildren.front(), *pRoot, *pInitialContext );
    }
}

} // namespace mega::invocation
