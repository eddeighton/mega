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

#include "invocation/invocation.hpp"

#include "database/ObjectStage.hxx"

#include "mega/values/compilation/invocation_id.hpp"
#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/compilation/reserved_symbols.hpp"

#include "mega/common_strings.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "common/unreachable.hpp"

#include <optional>
#include <unordered_set>

namespace ObjectStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/common_ancestor.hpp"

namespace Invocation
{
#include "compiler/derivation.hpp"
#include "compiler/derivation_printer.hpp"
#include "compiler/disambiguate.hpp"
} // namespace Invocation

} // namespace ObjectStage

using namespace ObjectStage;
using namespace ObjectStage::Invocation;

namespace mega::invocation
{

namespace
{

using InvocationPolicy = ObjectStage::Invocation::InterObjectDerivationPolicy;

std::optional< mega::interface::SymbolID > fromInvocationID( const SymbolTables&             symbolTables,
                                                             const mega::InvocationID&       id,
                                                             std::vector< Concrete::Node* >& contextTypes,
                                                             InvocationPolicy::Spec&         spec )
{
    for( const interface::TypeID& interfaceTypeID : id.m_context )
    {
        if( interfaceTypeID.valid() )
        {
            auto iFind = symbolTables.interfaceIDMap.find( interfaceTypeID );
            VERIFY_RTE( iFind != symbolTables.interfaceIDMap.end() );
            for( auto pConcrete : iFind->second->get_node()->get_inheritors() )
            {
                if( std::find( contextTypes.begin(), contextTypes.end(), pConcrete ) == contextTypes.end() )
                {
                    contextTypes.push_back( pConcrete );
                    spec.context.push_back( pConcrete );
                }
            }
        }
        else
        {
            break;
        }
    }
    VERIFY_RTE_MSG( !contextTypes.empty(), "Invocation has no context" );

    std::optional< mega::interface::SymbolID > operationIDOpt;
    for( const interface::SymbolID& symbolID : id.m_symbols )
    {
        std::vector< Concrete::Node* > pathElement;
        if( symbolID != interface::NULL_SYMBOL_ID )
        {
            if( interface::isOperationType( symbolID ) )
            {
                VERIFY_RTE_MSG( !operationIDOpt.has_value(), "Operation ID defined twice in invocation: " << id );
                operationIDOpt = symbolID;
            }
            else
            {
                auto iFind = symbolTables.symbolIDMap.find( symbolID );
                VERIFY_RTE_MSG( iFind != symbolTables.symbolIDMap.end(), "Failed to locate symbol: " << symbolID );
                auto pSymbol = iFind->second;

                for( auto pInterfaceID : pSymbol->get_interfaceIDs() )
                {
                    for( auto pConcrete : pInterfaceID->get_node()->get_inheritors() )
                    {
                        if( std::find( pathElement.begin(), pathElement.end(), pConcrete ) == pathElement.end() )
                        {
                            pathElement.push_back( pConcrete );
                        }
                    }
                }
            }
        }
        if( !pathElement.empty() )
        {
            spec.path.emplace_back( std::move( pathElement ) );
        }
    }
    return operationIDOpt;
}

class OperationBuilder
{
    Database&              m_database;
    Functions::Invocation* m_pInvocation;

    enum TargetType
    {
#define OPERATION_CONTEXT( CONCRETE_TYPE, NAME ) e_##NAME,
#include "operation_contexts.hxx"
#undef OPERATION_CONTEXT
        eUNSET
    } m_targetType
        = eUNSET;

#define OPERATION_CONTEXT( CONCRETE_TYPE, NAME ) std::vector< Concrete::CONCRETE_TYPE* > NAME;
#include "operation_contexts.hxx"
#undef OPERATION_CONTEXT

    void findOperationVertices( Derivation::Node* pNode, std::vector< Concrete::Node* >& vertices )
    {
        auto edges = pNode->get_edges();
        if( edges.empty() )
        {
            auto pStep = db_cast< Derivation::Step >( pNode );
            VERIFY_RTE( pStep );
            vertices.push_back( pStep->get_vertex() );
        }
        else
        {
            for( auto pEdge : edges )
            {
                if( !pEdge->get_eliminated() )
                {
                    findOperationVertices( pEdge->get_next(), vertices );
                }
            }
        }
    }

    void classifyOperations()
    {
        std::vector< Concrete::Node* > operationContexts;
        findOperationVertices( m_pInvocation->get_derivation(), operationContexts );

        for( auto pVert : operationContexts )
        {
            if( false )
            {
            }

#define OPERATION_CONTEXT( CONCRETE_TYPE, NAME )                                                  \
    else if( auto p##NAME = db_cast< Concrete::CONCRETE_TYPE >( pVert ) )                         \
    {                                                                                             \
        NAME.push_back( p##NAME );                                                                \
        VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == e_##NAME ),               \
                        "Conflicting target types for invocation: " << m_pInvocation->get_id() ); \
        m_targetType = e_##NAME;                                                                  \
    }
#include "operation_contexts.hxx"
#undef OPERATION_CONTEXT
            else
            {
                THROW_RTE( "Unsupported invocation target type: " << Concrete::fullTypeName( pVert )
                                                                  << " in invocation: " << m_pInvocation->get_id() );
            }
        }

        VERIFY_RTE_MSG( m_targetType != eUNSET, "No targe type for invocation: " << m_pInvocation->get_id() );
    }

    void buildNoParams()
    {
        using namespace ObjectStage::Functions;

        switch( m_targetType )
        {
            case e_objects:
                THROW_RTE( "No params invocation cannot be on object context" );
                break;
            case e_states:
            {
                // return type is the started state
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : states )
                    {
                        contexts.push_back( pConcrete->get_state() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< Start >( Start::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Start );
            }
            break;
            case e_functions:
            {
                // return type is the function return type
                {
                    std::vector< Interface::Function* >   contexts;
                    std::set< Interface::CPP::TypeInfo* > types;
                    for( auto pConcrete : functions )
                    {
                        contexts.push_back( pConcrete->get_function() );
                        types.insert( pConcrete->get_function()->get_cpp_function_type()->get_return_type_info() );
                    }
                    const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                    contexts               = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Function >(
                        ReturnTypes::Function::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Call >( Call::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Call );
            }
            break;
            case e_events:
            {
                // return type is the signaled event
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : events )
                    {
                        contexts.push_back( pConcrete->get_event() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< Signal >( Signal::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Signal );
            }
            break;
            case e_dimensions:
            {
                // return type is the read dimensions
                {
                    std::vector< Interface::UserDimension* > contexts;
                    std::set< Interface::CPP::TypeInfo* >    types;
                    for( auto pConcrete : dimensions )
                    {
                        auto pUserDimension = pConcrete->get_dimension();
                        contexts.push_back( pUserDimension );
                        types.insert( pUserDimension->get_cpp_data_type()->get_type_info() );
                    }
                    const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                    contexts               = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Dimension >(
                        ReturnTypes::Dimension::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Read >( Read::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Read );
            }
            break;
            case e_links:
            {
                // return type is the target of the link
                {
                    std::vector< Interface::IContext* > targets;
                    bool                                bSingular = true;
                    for( auto pConcrete : links )
                    {
                        bool bFound = false;
                        for( auto pGraphEdge : pConcrete->get_out_edges() )
                        {
                            if( auto pInterObjectEdge = db_cast< Concrete::InterObjectEdge >( pGraphEdge ) )
                            {
                                if( pInterObjectEdge->get_cardinality().isNonSingular() )
                                {
                                    bSingular = false;
                                }
                                Concrete::Node* pTargetContext   = pGraphEdge->get_target();
                                auto            interfaceNodeOpt = pTargetContext->get_node_opt();
                                VERIFY_RTE( interfaceNodeOpt.has_value() );
                                auto pIContext = db_cast< Interface::IContext >( interfaceNodeOpt.value() );
                                VERIFY_RTE( pIContext );
                                targets.push_back( pIContext );
                                bFound = true;
                            }
                        }
                        VERIFY_RTE( bFound );
                    }

                    targets = make_unique_without_reorder( targets );
                    if( bSingular )
                    {
                        m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, targets } ) );
                    }
                    else
                    {
                        m_pInvocation->set_return_type(
                            m_database.construct< ReturnTypes::Range >( ReturnTypes::Range::Args{
                                ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, targets } } ) );
                    }
                }

                m_database.construct< LinkRead >( LinkRead::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Read );
            }
            break;
            default:
                THROW_RTE( "Unsupported invocation target type in invocation: " << m_pInvocation->get_id() );
                break;
        }
    }

    void buildParams()
    {
        using namespace ObjectStage::Functions;

        switch( m_targetType )
        {
            case e_functions:
            {
                // return type is the function return type
                {
                    std::vector< Interface::Function* >   contexts;
                    std::set< Interface::CPP::TypeInfo* > types;
                    for( auto pConcrete : functions )
                    {
                        contexts.push_back( pConcrete->get_function() );
                        types.insert( pConcrete->get_function()->get_cpp_function_type()->get_return_type_info() );
                    }
                    const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                    contexts               = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Function >(
                        ReturnTypes::Function::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Call >( Call::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Call );
            }
            break;
            case e_dimensions:
            {
                // return the context of the write
                std::set< Interface::CPP::TypeInfo* >    types;
                std::vector< Interface::UserDimension* > userDimensions;
                std::vector< Interface::IContext* >      contexts;
                {
                    for( auto pConcrete : dimensions )
                    {
                        auto pUserDimension = pConcrete->get_dimension();
                        userDimensions.push_back( pUserDimension );
                        auto pParent = db_cast< Interface::IContext >( pUserDimension->get_parent() );
                        contexts.push_back( pParent );
                        types.insert( pUserDimension->get_cpp_data_type()->get_type_info() );
                    }
                    userDimensions = make_unique_without_reorder( userDimensions );
                    contexts       = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }
                // parameter type
                ReturnTypes::Dimension* pParameterType = nullptr;
                {
                    const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                    pParameterType         = m_database.construct< ReturnTypes::Dimension >(
                        ReturnTypes::Dimension::Args{ ReturnTypes::ReturnType::Args{}, userDimensions, bHomogenous } );
                }

                m_database.construct< Write >( Write::Args{ m_pInvocation, pParameterType } );
                m_pInvocation->set_explicit_operation( id_exp_Write );
            }
            break;
            case e_links:
            {
                // return the context of the write
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : links )
                    {
                        auto interfaceNodeOpt = pConcrete->get_node_opt();
                        VERIFY_RTE( interfaceNodeOpt.has_value() );
                        auto pContext = db_cast< Interface::IContext >( interfaceNodeOpt.value()->get_parent() );
                        contexts.push_back( pContext );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< LinkAdd >( LinkAdd::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Add );
            }
            break;
            default:
                THROW_RTE( "Unsupported invocation target type in invocation: " << m_pInvocation->get_id() );
                break;
        }
    }

    void buildGet()
    {
        THROW_TODO;
        /*switch( m_targetType )
        {
            case eObjects:
            {
                // Pointer context of object
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : objects )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eComponents:
            {
                // Pointer context of component
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : components )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eStates:
            {
                // Pointer context of state
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : states )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eDeciders:
            {
                THROW_RTE( "Cannot get a decider" );
            }
            break;
            case eFunctions:
            {
                // Pointer context of function
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : functions )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eEvents:
            {
                // Pointer context of events
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : events )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eUserDimensions:
                THROW_RTE( "Cannot get a dimension" );
                break;
            case eLinkDimensions:
                THROW_RTE( "Cannot get a link" );
                break;
            case eUNSET:
            default:
                THROW_RTE( "Unsupported invocation target type in invocation: " << m_pInvocation->get_id() );
                break;
        }*/
    }

    void buildRemove()
    {
        THROW_TODO;
        /*switch( m_targetType )
        {
            case eLinkDimensions:
            {
                // parameter type
                // ReturnTypes::Context* pParameterType = nullptr;
                // {
                //     std::vector< Interface::DimensionTrait* > contexts;
                //     std::set< std::string >                   types;
                //     for( auto pConcrete : linkDimensions )
                //     {
                //         auto pDimensionTrait = pConcrete->get_interface_dimension();
                //         contexts.push_back( pDimensionTrait );
                //         types.insert( pDimensionTrait->get_canonical_type() );
                //     }
                //     contexts               = make_unique_without_reorder( contexts );
                //     pParameterType         = m_database.construct< ReturnTypes::Context >(
                //         ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } );
                // }
                // return the context of the write
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : linkDimensions )
                    {
                        contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< LinkRemove >( LinkRemove::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Remove );
            }
            break;
            case eUNSET:
            default:
                THROW_RTE( "Unsupported invocation target type in invocation: " << m_pInvocation->get_id() );
                break;
        }*/
    }

    void buildClear()
    {
        THROW_TODO;
        /*switch( m_targetType )
        {
            case eLinkDimensions:
            {
                // return the context of the write
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : linkDimensions )
                    {
                        contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< LinkClear >( LinkClear::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Clear );
            }
            break;
            case eUNSET:
            default:
                THROW_RTE( "Unsupported invocation target type in invocation: " << m_pInvocation->get_id() );
                break;
        }*/
    }

    void buildMove()
    {
        THROW_TODO;
        // m_database.construct< Move >( Move::Args{ m_pInvocation } );
        // m_pInvocation->set_explicit_operation( id_exp_Move );
    }

    void buildRange()
    {
        THROW_TODO;
        // m_database.construct< Range >( Range::Args{ m_pInvocation } );
        // m_pInvocation->set_explicit_operation( id_exp_Range );
    }

public:
    OperationBuilder( Database& database, Functions::Invocation* pInvocation )
        : m_database( database )
        , m_pInvocation( pInvocation )
    {
    }

    void build()
    {
        classifyOperations();

        m_pInvocation->set_explicit_operation( HIGHEST_EXPLICIT_OPERATION_TYPE );

        if( m_pInvocation->get_id().m_bHasParams )
        {
            buildParams();
        }
        else
        {
            buildNoParams();
        }

        VERIFY_RTE_MSG( m_pInvocation->get_explicit_operation() != HIGHEST_EXPLICIT_OPERATION_TYPE,
                        "Failed to determine invocation explicit operation type: " << m_pInvocation->get_id() );
    }
};

} // namespace

Functions::Invocation* compileInvocation( Database& database, const SymbolTables& symbolTables,
                                          const mega::InvocationID& id )
{
    // determine the derivation of the invocationID
    std::vector< Concrete::Node* > contextTypes;
    InvocationPolicy::Spec         derivationSpec;
    // std::optional< mega::interface::SymbolID > operationSymbolOpt  =
    fromInvocationID( symbolTables, id, contextTypes, derivationSpec );

    // solve the context free derivation
    InvocationPolicy               policy( database );
    std::vector< Derivation::Or* > finalFrontier;
    Derivation::Root*              pRoot = solveContextFree( derivationSpec, policy, finalFrontier );

    precedence( pRoot );

    Functions::Invocation* pInvocation = nullptr;
    try
    {
        const Disambiguation result = disambiguate( pRoot, finalFrontier );
        if( result != eSuccess )
        {
            std::ostringstream os;
            if( result == eAmbiguous )
                os << "Derivation disambiguation was ambiguous for: " << id << "\n";
            else if( result == eFailure )
                os << "Derivation disambiguation failed for: " << id << "\n";
            else
                THROW_RTE( "Unknown derivation failure type" );
            THROW_RTE( os.str() );
        }

        // is the invocation a link invocation?
        /*InvocationPolicy::AndPtrVector linkFrontier;
        if( ( id.m_operation == id_Imp_NoParams ) || ( id.m_operation == id_Imp_Params ) )
        {
            bool bTargetsObjectOrComponent = false;
            bool bTargetsOtherContextType  = false;
            for( auto pOr : finalFrontier )
            {
                if( db_cast< Concrete::Object >( pOr->get_vertex() )
                    || db_cast< Concrete::Component >( pOr->get_vertex() ) )
                {
                    bTargetsObjectOrComponent = true;
                }
                else
                {
                    bTargetsOtherContextType = true;
                }
            }
            VERIFY_RTE_MSG( !bTargetsObjectOrComponent || !bTargetsOtherContextType,
                            "Conficting target types for invocation: " << id );
            if( bTargetsObjectOrComponent )
            {
                // invocation IS a link read or write operation
                DerivationSolver::backtrackToLinkDimensions( pRoot, policy, linkFrontier );
            }
        }*/

        {
            pInvocation = database.construct< Functions::Invocation >( Functions::Invocation::Args{ pRoot, id } );
        }

        {
            OperationBuilder builder( database, pInvocation );
            builder.build();
        }
    }
    catch( std::exception& ex )
    {
        std::ostringstream os;
        os << "Exception while compiling invocation: " << id << "\n";
        printDerivationStep( pRoot, true, os );
        os << "\nError: " << ex.what();
        THROW_RTE( os.str() );
    }

    return pInvocation;
}

} // namespace mega::invocation
