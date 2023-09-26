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

#include "database/model/OperationsStage.hxx"
#include "database/types/operation.hpp"

#include "compiler/derivation.hpp"

#include "mega/operation_id.hpp"
#include "mega/common_strings.hpp"
#include "mega/invocation_io.hpp"

#include "mega/types/traits.hpp"

#include <optional>
#include <unordered_set>

namespace OperationsStage
{
#include "compiler/printer.hpp"
namespace Derivation
{
#include "compiler/disambiguate.hpp"
}
} // namespace OperationsStage

namespace mega::invocation
{
SymbolTables::SymbolTables( OperationsStage::Symbols::SymbolTable* pSymbolTable )
    : symbolIDMap( pSymbolTable->get_symbol_type_ids() )
    , interfaceIDMap( pSymbolTable->get_interface_type_ids() )
{
}

using namespace OperationsStage;

struct InvocationPolicy
{
    using GraphVertex             = OperationsStage::Concrete::Graph::Vertex;
    using GraphEdge               = OperationsStage::Concrete::Graph::Edge;
    using GraphVertexVector       = std::vector< GraphVertex* >;
    using GraphVertexSet          = std::unordered_set< GraphVertex* >;
    using GraphVertexVectorVector = std::vector< GraphVertexVector >;
    using GraphEdgeVector         = std::vector< GraphEdge* >;

    struct Spec
    {
        GraphVertexVector       context;
        GraphVertexVectorVector path;
    };

    using StepPtr     = OperationsStage::Derivation::Step*;
    using EdgePtr     = OperationsStage::Derivation::Edge*;
    using OrPtr       = OperationsStage::Derivation::Or*;
    using OrPtrVector = std::vector< OrPtr >;
    using AndPtr      = OperationsStage::Derivation::And*;
    using RootPtr     = OperationsStage::Derivation::Root*;

    EdgePtr makeEdge( StepPtr pNext, const GraphEdgeVector& edges ) const
    {
        // initially no edges are eliminated
        return m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pNext, false, edges } );
    }
    OrPtr makeOr( GraphVertex* pVertex ) const
    {
        return m_database.construct< Derivation::Or >( Derivation::Or::Args{ Derivation::Step::Args{ pVertex, {} } } );
    }
    AndPtr makeAnd( GraphVertex* pVertex ) const
    {
        return m_database.construct< Derivation::And >(
            Derivation::And::Args{ Derivation::Step::Args{ pVertex, {} } } );
    }
    RootPtr makeRoot( const GraphVertexVector& context ) const
    {
        return m_database.construct< Derivation::Root >( Derivation::Root::Args{ context, {} } );
    }
    EdgePtr makeRootEdge( OrPtr pNext ) const
    {
        return m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pNext, false, {} } );
    }

    // the link context MAY contain MULTIPLE links.  Each individual link is either deriving or not.
    // The Link context is considered to be deriving if ANY of its links are deriving
    GraphVertexVector enumerateLinkContexts( GraphVertex* pVertex, bool bDerivingOnly ) const
    {
        auto pContext = db_cast< Concrete::Context >( pVertex );
        VERIFY_RTE( pContext );
        auto optionalObject = pContext->get_concrete_object();
        VERIFY_RTE( optionalObject.has_value() );
        GraphVertexVector result;
        for( auto pLinkContext : optionalObject.value()->get_link_contexts() )
        {
            result.push_back( pLinkContext );
        }
        return result;
    }

    // enumerate actual polymorphic branches of a given link
    GraphEdgeVector enumerateLink( GraphVertex* pLink ) const
    {
        GraphEdgeVector result;
        for( auto pEdge : pLink->get_out_edges() )
        {
            switch( pEdge->get_type().get() )
            {
                case EdgeType::eMono:
                {
                    result.push_back( pEdge );
                }
                break;
                case EdgeType::ePoly:
                {
                    result.push_back( pEdge );
                }
                break;
                case EdgeType::ePolyParent:
                {
                    result.push_back( pEdge );
                }
                break;

                case EdgeType::eParent:
                case EdgeType::eChildSingular:
                case EdgeType::eChildNonSingular:
                case EdgeType::ePart:
                case EdgeType::eDim:
                case EdgeType::eObjectLink:
                case EdgeType::TOTAL_EDGE_TYPES:
                    break;
            }
        }
        return result;
    }
    // enumerate the dimension links within the parent vertex
    GraphEdgeVector enumerateLinks( GraphVertex* pParentVertex ) const
    {
        GraphEdgeVector result;

        for( auto pEdge : pParentVertex->get_out_edges() )
        {
            switch( pEdge->get_type().get() )
            {
                case EdgeType::eObjectLink:
                {
                    result.push_back( pEdge );
                }
                break;
                case EdgeType::ePolyParent:
                case EdgeType::eMono:
                case EdgeType::ePoly:
                case EdgeType::eParent:
                case EdgeType::eChildSingular:
                case EdgeType::eChildNonSingular:
                case EdgeType::ePart:
                case EdgeType::eDim:
                case EdgeType::TOTAL_EDGE_TYPES:
                    break;
            }
        }

        return result;
    }

    GraphVertex* pathToObjectRoot( GraphVertex* pVertex, GraphEdgeVector& path ) const
    {
        while( !db_cast< Concrete::Object >( pVertex ) )
        {
            bool bFound = false;
            for( auto pEdge : pVertex->get_out_edges() )
            {
                if( pEdge->get_type().get() == EdgeType::eParent )
                {
                    path.push_back( pEdge );
                    pVertex = pEdge->get_target();
                    bFound  = true;
                    break;
                }
            }
            if( !bFound )
            {
                THROW_RTE( "Failed to find path to object root from vertex" );
                return nullptr;
            }
        }
        return pVertex;
    }

    bool invertObjectRootPath( const GraphEdgeVector& path, GraphEdgeVector& descendingPath ) const
    {
        for( auto pEdge : path )
        {
            bool bFound = false;
            for( auto pInverseEdge : pEdge->get_target()->get_out_edges() )
            {
                if( pEdge->get_target() == pInverseEdge->get_source() )
                {
                    switch( pInverseEdge->get_type().get() )
                    {
                        case EdgeType::eChildSingular:
                        case EdgeType::eObjectLink:
                        case EdgeType::eDim:
                        {
                            descendingPath.push_back( pInverseEdge );
                            bFound = true;
                            break;
                        }
                        case EdgeType::eParent:
                        case EdgeType::eChildNonSingular:
                        case EdgeType::ePart:
                        case EdgeType::eMono:
                        case EdgeType::ePoly:
                        case EdgeType::ePolyParent:
                        case EdgeType::TOTAL_EDGE_TYPES:
                            break;
                    }
                }
            }
            if( !bFound )
            {
                return false;
            }
        }
        std::reverse( descendingPath.begin(), descendingPath.end() );
        return true;
    }

    bool commonRootDerivation( GraphVertex* pSource, GraphVertex* pTarget, GraphEdgeVector& edges ) const
    {
        GraphEdgeVector sourcePath, targetPath;
        auto            pSourceObject = pathToObjectRoot( pSource, sourcePath );
        auto            pTargetObject = pathToObjectRoot( pTarget, targetPath );

        // if not the same object then fail
        if( pSourceObject != pTargetObject )
            return false;

        // while both paths contain edges then if the edge is the same there is a lower common root
        while( !sourcePath.empty() && !targetPath.empty() && ( sourcePath.back() == targetPath.back() ) )
        {
            sourcePath.pop_back();
            targetPath.pop_back();
        }

        std::copy( sourcePath.begin(), sourcePath.end(), std::back_inserter( edges ) );
        GraphEdgeVector descendingPath;
        if( invertObjectRootPath( targetPath, descendingPath ) )
        {
            std::copy( targetPath.begin(), targetPath.end(), std::back_inserter( edges ) );
            return true;
        }
        else
        {
            return false;
        }
    }

    InvocationPolicy( Database& database )
        : m_database( database )
    {
    }

private:
    Database& m_database;
};

std::optional< mega::OperationID > fromInvocationID( const SymbolTables& symbolTables, const mega::InvocationID& id,
                                                     std::vector< Concrete::Context* > types,
                                                     InvocationPolicy::Spec&           spec )
{
    std::optional< mega::OperationID > operationIDOpt;

    for( auto& symbolID : id.m_context )
    {
        if( isOperationType( symbolID ) )
        {
            VERIFY_RTE_MSG( !operationIDOpt.has_value(), "Operation ID defined twice" );
            operationIDOpt = static_cast< mega::OperationID >( symbolID.getSymbolID() );
        }
        else if( symbolID.isSymbolID() )
        {
            auto iFind = symbolTables.symbolIDMap.find( symbolID );
            VERIFY_RTE( iFind != symbolTables.symbolIDMap.end() );
            auto pSymbol = iFind->second;
            for( auto pContext : pSymbol->get_contexts() )
            {
                for( auto pConcrete : pContext->get_concrete() )
                {
                    types.push_back( pConcrete );
                    spec.context.push_back( pConcrete );
                }
            }
        }
        else
        {
            auto iFind = symbolTables.interfaceIDMap.find( symbolID );
            VERIFY_RTE( iFind != symbolTables.interfaceIDMap.end() );
            auto pSymbol = iFind->second;
            VERIFY_RTE( pSymbol->get_context().has_value() );
            if( auto pInterfaceContext = pSymbol->get_context().value() )
            {
                for( auto pConcrete : pInterfaceContext->get_concrete() )
                {
                    types.push_back( pConcrete );
                    spec.context.push_back( pConcrete );
                }
            }
        }
    }

    for( auto& symbolID : id.m_type_path )
    {
        std::vector< Concrete::Graph::Vertex* > pathElement;
        if( isOperationType( symbolID ) )
        {
            VERIFY_RTE_MSG( !operationIDOpt.has_value(), "Operation ID defined twice" );
            operationIDOpt = static_cast< mega::OperationID >( symbolID.getSymbolID() );
        }
        else if( symbolID.isSymbolID() )
        {
            auto iFind = symbolTables.symbolIDMap.find( symbolID );
            VERIFY_RTE( iFind != symbolTables.symbolIDMap.end() );
            auto pSymbol = iFind->second;
            for( auto pContext : pSymbol->get_contexts() )
            {
                for( auto pConcrete : pContext->get_concrete() )
                {
                    pathElement.push_back( pConcrete );
                }
            }
            for( auto pDimension : pSymbol->get_dimensions() )
            {
                for( auto pConcrete : pDimension->get_concrete() )
                {
                    pathElement.push_back( pConcrete );
                }
            }
            for( auto pLink : pSymbol->get_links() )
            {
                for( auto pConcrete : pLink->get_concrete() )
                {
                    pathElement.push_back( pConcrete );
                }
            }
        }
        else
        {
            auto iFind = symbolTables.interfaceIDMap.find( symbolID );
            VERIFY_RTE( iFind != symbolTables.interfaceIDMap.end() );
            auto pSymbol = iFind->second;

            if( pSymbol->get_context().has_value(); auto pContext = pSymbol->get_context().value() )
            {
                for( auto pConcrete : pContext->get_concrete() )
                {
                    pathElement.push_back( pConcrete );
                }
            }
            else if( pSymbol->get_dimension().has_value(); auto pDimension = pSymbol->get_dimension().value() )
            {
                for( auto pConcrete : pDimension->get_concrete() )
                {
                    pathElement.push_back( pConcrete );
                }
            }
            else if( pSymbol->get_link().has_value(); auto pLink = pSymbol->get_link().value() )
            {
                for( auto pConcrete : pLink->get_concrete() )
                {
                    pathElement.push_back( pConcrete );
                }
            }
            else
            {
                THROW_RTE( "Unknown symbolID" );
            }
        }
        if( !pathElement.empty() )
        {
            spec.path.emplace_back( std::move( pathElement ) );
        }
    }

    return operationIDOpt;
}

// std::vector< Invocations::Variables::Variable* >;

using namespace OperationsStage::Invocations;

template < typename TVariable, typename... ConstructorArgs >
TVariable* make_variable( OperationsStage::Database& database, OperationsStage::Operations::Invocation* pInvocation,
                          ConstructorArgs&&... ctorArgs )
{
    TVariable* pVariable = database.construct< TVariable >( typename TVariable::Args{ ctorArgs... } );
    pInvocation->push_back_variables( pVariable );
    return pVariable;
}

template < typename TInstruction, typename... ConstructorArgs >
TInstruction* make_instruction( OperationsStage::Database& database, Instructions::InstructionGroup* pParentInstruction,
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

void build( OperationsStage::Database& database, OperationsStage::Operations::Invocation* pInvocation,
            Instructions::InstructionGroup* pInstruction, Variables::Variable* pVariable,
            InvocationPolicy::EdgePtr pEdge )
{
    for( auto pGraphEdge : pEdge->get_edges() )
    {
        switch( pGraphEdge->get_type().get() )
        {
            case EdgeType::eParent:
            {
            }
            break;
            case EdgeType::eChildSingular:
            {
            }
            break;
            case EdgeType::eChildNonSingular:
            {
            }
            break;
            case EdgeType::ePart:
            {
            }
            break;
            case EdgeType::eDim:
            {
            }
            break;
            case EdgeType::eObjectLink:
            {
            }
            break;
            case EdgeType::eMono:
            {
            }
            break;
            case EdgeType::ePoly:
            {
            }
            break;
            case EdgeType::ePolyParent:
            {
            }
            break;
            default:
            case EdgeType::TOTAL_EDGE_TYPES:
            {
                THROW_RTE( "Unknown hypergraph edge type" );
            }
            break;
        }
    }

    auto pNextStep = pEdge->get_next();
    if( auto pAND = db_cast< Derivation::And >( pNextStep ) )
    {
        // create polymorphic branch
        auto pRef = db_cast< Variables::Reference >( pVariable );
        VERIFY_RTE( pRef );

        auto pPolyReference = make_instruction< Instructions::PolyReference >( database, pInstruction, pRef );

        bool bFound = false;
        for( auto pEdge : pAND->get_edges() )
        {
            if( !pEdge->get_eliminated() )
            {
                auto pTargetContext = db_cast< Concrete::Context >( pEdge->get_next()->get_vertex() );
                VERIFY_RTE( pTargetContext );

                auto pInstance = make_variable< Variables::Instance >(
                    database, pInvocation, Variables::Variable::Args{ pRef }, pTargetContext );

                auto pPolyCaseInstruction
                    = make_instruction< Instructions::PolyCase >( database, pPolyReference, pRef, pInstance );

                build( database, pInvocation, pPolyCaseInstruction, pInstance, pEdge );
                bFound = true;
            }
        }
        VERIFY_RTE_MSG( bFound, "Failed to find non-eliminated edge in OR step" );
    }
    else if( auto pOR = db_cast< Derivation::Or >( pNextStep ) )
    {
        auto edges = pOR->get_edges();

        bool bFound = false;
        for( auto pEdge : edges )
        {
            if( !pEdge->get_eliminated() )
            {
                VERIFY_RTE_MSG( !bFound, "Multiple non-eliminated edges in OR step" );
                build( database, pInvocation, pInstruction, pVariable, pEdge );
                bFound = true;
            }
        }
        VERIFY_RTE_MSG( bFound, "Failed to find non-eliminated edge in OR step" );
    }
    else
    {
        THROW_RTE( "Unknown derivation step type" );
    }
}

void build( OperationsStage::Database& database, OperationsStage::Operations::Invocation* pInvocation,
            InvocationPolicy::RootPtr pStep, std::vector< Concrete::Context* > types )
{
    // clang-format off
    auto pInitialContext = make_variable< Variables::Context >(
        database, pInvocation,
            Variables::Reference::Args
            { 
                Variables::Variable::Args
                { 
                    std::optional< Variables::Variable* >() 
                }, 
                types 
            } 
         );
    // clang-format on

    auto pRootInstruction = make_instruction< Instructions::Root >( database, nullptr, pInitialContext );
    pInvocation->set_root_instruction( pRootInstruction );

    // pStep->get_context()
    for( auto pEdge : pStep->get_edges() )
    {
        build( database, pInvocation, pRootInstruction, pInitialContext, pEdge );
    }
}

OperationsStage::Operations::Invocation*
compileInvocation( OperationsStage::Database& database, const SymbolTables& symbolTables, const mega::InvocationID& id )
{
    // determine the derivation of the invocationID
    std::vector< Concrete::Context* > types;
    InvocationPolicy::Spec            derivationSpec;
    fromInvocationID( symbolTables, id, types, derivationSpec );

    // solve the context free derivation
    InvocationPolicy              policy( database );
    InvocationPolicy::OrPtrVector finalFrontier;
    InvocationPolicy::RootPtr     pRoot = DerivationSolver::solveContextFree( derivationSpec, policy, finalFrontier );

    Derivation::Disambiguation result = Derivation::disambiguate( pRoot, finalFrontier );

    // clang-format off
    auto pInvocation = database.construct< OperationsStage::Operations::Invocation >(
        OperationsStage::Operations::Invocation::Args
        {
            id,
            pRoot,
            {}
        });
    // clang-format on

    build( database, pInvocation, pRoot, types );

    return pInvocation;
}

} // namespace mega::invocation
