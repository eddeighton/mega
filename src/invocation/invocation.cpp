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
#include "mega/make_unique_without_reorder.hpp"

#include "mega/types/traits.hpp"

#include <optional>
#include <unordered_set>

namespace OperationsStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
namespace Derivation
{
#include "compiler/derivation_printer.hpp"
#include "compiler/disambiguate.hpp"
} // namespace Derivation
} // namespace OperationsStage

namespace mega::invocation
{
SymbolTables::SymbolTables( OperationsStage::Symbols::SymbolTable* pSymbolTable )
    : symbolIDMap( pSymbolTable->get_symbol_type_ids() )
    , interfaceIDMap( pSymbolTable->get_interface_type_ids() )
{
}

using namespace OperationsStage;

namespace
{
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

    using StepPtr      = OperationsStage::Derivation::Step*;
    using EdgePtr      = OperationsStage::Derivation::Edge*;
    using OrPtr        = OperationsStage::Derivation::Or*;
    using OrPtrVector  = std::vector< OrPtr >;
    using AndPtr       = OperationsStage::Derivation::And*;
    using AndPtrVector = std::vector< AndPtr >;
    using RootPtr      = OperationsStage::Derivation::Root*;

    EdgePtr makeEdge( StepPtr pNext, const GraphEdgeVector& edges ) const
    {
        // initially no edges are eliminated
        return m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pNext, false, false, 0, edges } );
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
        return m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pNext, false, false, 0, {} } );
    }
    bool   isLinkDimension( GraphVertex* pVertex ) const { return db_cast< Concrete::Dimensions::Link >( pVertex ); }
    AndPtr isAndStep( StepPtr pStep ) const { return db_cast< OperationsStage::Derivation::And >( pStep ); }
    void   backtrack( EdgePtr pEdge ) const { pEdge->set_backtracked( true ); }

    // the link context MAY contain MULTIPLE links.  Each individual link is either deriving or not.
    // The Link context is considered to be deriving if ANY of its links are deriving
    GraphVertexVector enumerateLinkContexts( GraphVertex* pVertex ) const
    {
        Concrete::Context* pContext = nullptr;
        {
            if( auto pVertexContext = db_cast< Concrete::Context >( pVertex ) )
            {
                pContext = pVertexContext;
            }
            else if( auto pUserDim = db_cast< Concrete::Dimensions::User >( pVertex ) )
            {
                pContext = pUserDim->get_parent_context();
            }
            else if( auto pLinkDim = db_cast< Concrete::Dimensions::Link >( pVertex ) )
            {
                pContext = pLinkDim->get_parent_context();
            }
            else
            {
                THROW_RTE( "Unknown vertex type" );
            }
        }

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
                case EdgeType::eDim:
                case EdgeType::eLink:
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
                case EdgeType::eLink:
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
                    VERIFY_RTE( !bFound );
                    bFound = true;
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
                VERIFY_RTE( pEdge->get_target() == pInverseEdge->get_source() );
                if( pEdge->get_source() == pInverseEdge->get_target() )
                {
                    switch( pInverseEdge->get_type().get() )
                    {
                        case EdgeType::eChildSingular:
                        case EdgeType::eLink:
                        case EdgeType::eDim:
                        {
                            descendingPath.push_back( pInverseEdge );
                            VERIFY_RTE( !bFound );
                            bFound = true;
                        }
                        break;
                        case EdgeType::eChildNonSingular:
                            // do no allow non-singular
                            break;
                        case EdgeType::eParent:
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
        if( pSource == pTarget )
            return true;

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

        GraphEdgeVector descendingPath;
        if( invertObjectRootPath( targetPath, descendingPath ) )
        {
            if( !sourcePath.empty() && !descendingPath.empty() )
            {
                VERIFY_RTE( descendingPath.front()->get_source() == sourcePath.back()->get_target() );
            }

            std::copy( sourcePath.begin(), sourcePath.end(), std::back_inserter( edges ) );
            std::copy( descendingPath.begin(), descendingPath.end(), std::back_inserter( edges ) );
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

void fromInvocationID( const SymbolTables& symbolTables, const mega::InvocationID& id,
                       std::vector< Concrete::Graph::Vertex* > types, InvocationPolicy::Spec& spec )
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
    using ::operator<<;
    VERIFY_RTE_MSG( !operationIDOpt.has_value() || ( id.m_operation == operationIDOpt.value() ),
                    "Mismatching operation type in invocation: " << id );
}

// std::vector< Invocations::Variables::Variable* >;

using namespace OperationsStage::Invocations;

class InvocationBuilder
{
    Variables::Stack* make_stack_variable( Variables::Variable*                pParentVariable,
                                           OperationsStage::Concrete::Context* pTargetContext )
    {
        using namespace OperationsStage;
        auto pStack = m_database.construct< Variables::Stack >(
            Variables::Stack::Args{ Variables::Variable::Args{ pParentVariable }, pTargetContext } );
        m_pInvocation->push_back_variables( pStack );
        return pStack;
    }

    OperationsStage::Invocations::Variables::Memory*
    make_memory_variable( Variables::Variable* pParentVariable, const std::vector< Concrete::Graph::Vertex* >& types )
    {
        using namespace OperationsStage;
        auto pMemory = m_database.construct< Variables::Memory >(
            Variables::Reference::Args{ Variables::Variable::Args{ pParentVariable }, types } );
        m_pInvocation->push_back_variables( pMemory );
        return pMemory;
    }

    OperationsStage::Invocations::Variables::Parameter*
    make_parameter_variable( const std::vector< Concrete::Graph::Vertex* >& types )
    {
        using namespace OperationsStage;
        auto pParameter = m_database.construct< Variables::Parameter >( Variables::Parameter::Args{
            Variables::Reference::Args{ Variables::Variable::Args{ std::nullopt }, types } } );
        m_pInvocation->push_back_variables( pParameter );
        return pParameter;
    }

    template < typename TInstruction, typename... ConstructorArgs >
    TInstruction* make_instruction( Instructions::InstructionGroup* pParentInstruction, ConstructorArgs&&... ctorArgs )
    {
        // clang-format off
        TInstruction* pNewInstruction = m_database.construct< TInstruction >
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

private:
    OperationsStage::Database&               m_database;
    InvocationPolicy::RootPtr                m_pDerivationTreeRoot;
    OperationsStage::Operations::Invocation* m_pInvocation;
    Variables::Parameter*                    m_pParameterVariable;
    Instructions::Root*                      m_pRootInstruction;

public:
    InvocationBuilder( OperationsStage::Database& database, const mega::InvocationID& id,
                       InvocationPolicy::RootPtr                      pDerivationTreeRoot,
                       const std::vector< Concrete::Graph::Vertex* >& types )
        : m_database( database )
        , m_pDerivationTreeRoot( pDerivationTreeRoot )
        , m_pInvocation( database.construct< OperationsStage::Operations::Invocation >(
              OperationsStage::Operations::Invocation::Args{ id, pDerivationTreeRoot, {}, {} } ) )
        , m_pParameterVariable( make_parameter_variable( types ) )
        , m_pRootInstruction( make_instruction< Instructions::Root >( nullptr, m_pParameterVariable ) )
    {
        m_pInvocation->set_root_instruction( m_pRootInstruction );
    }

    void build()
    {
        auto rootOutEdges       = m_pDerivationTreeRoot->get_edges();
        U64  activeOutEdgeCount = 0U;
        for( auto pEdge : rootOutEdges )
        {
            VERIFY_RTE( !pEdge->get_backtracked() );
            if( !pEdge->get_eliminated() )
            {
                ++activeOutEdgeCount;
            }
        }

        if( activeOutEdgeCount == 1 )
        {
            for( auto pEdge : rootOutEdges )
            {
                if( !pEdge->get_eliminated() )
                {
                    build( m_pRootInstruction, m_pParameterVariable, pEdge );
                    break;
                }
            }
        }
        else if( activeOutEdgeCount > 1 )
        {
            buildAnd( m_pRootInstruction, m_pParameterVariable, m_pDerivationTreeRoot->get_edges() );
        }
        else
        {
            THROW_RTE( "Derivation tree root has no active out edges" );
        }
    }

private:
    void buildOperation( Instructions::InstructionGroup* pInstruction, Variables::Variable* pVariable,
                         Concrete::Graph::Vertex* pVertex )
    {
        Invocations::Operations::Operation* pOperation = m_database.construct< Invocations::Operations::Operation >(
            Invocations::Operations::Operation::Args{ Instructions::Instruction::Args{}, pVariable, pVertex } );
        pInstruction->push_back_children( pOperation );
        m_pInvocation->push_back_operations( pOperation );
    }

    void buildAnd( Instructions::InstructionGroup* pInstruction, Variables::Variable* pVariable,
                   const std::vector< Derivation::Edge* >& edges )
    {
        // create polymorphic branch
        auto pPolyReference = make_instruction< Instructions::PolyBranch >( pInstruction, pVariable );

        bool bFound = false;
        for( auto pEdge : edges )
        {
            if( !pEdge->get_eliminated() )
            {
                bFound = true;
                VERIFY_RTE( !pEdge->get_backtracked() );
                auto pNext = pEdge->get_next();

                auto pPolyCaseInstruction
                    = make_instruction< Instructions::PolyCase >( pPolyReference, pVariable, pNext->get_vertex() );

                // get the hypergraph object link edge
                auto hyperGraphEdges = pEdge->get_edges();
                if( !hyperGraphEdges.empty() )
                {
                    VERIFY_RTE( hyperGraphEdges.size() == 1 );
                    auto pHyperGraphObjectLinkEdge = hyperGraphEdges.front();
                    VERIFY_RTE( ( pHyperGraphObjectLinkEdge->get_type().get() == EdgeType::eMono )
                                || ( pHyperGraphObjectLinkEdge->get_type().get() == EdgeType::ePoly )
                                || ( pHyperGraphObjectLinkEdge->get_type().get() == EdgeType::ePolyParent ) );
                    VERIFY_RTE( pHyperGraphObjectLinkEdge->get_target() == pNext->get_vertex() );
                }

                auto pOR = db_cast< Derivation::Or >( pNext );
                VERIFY_RTE( pOR );
                buildOr( pPolyCaseInstruction, pVariable, pOR );
            }
        }
        VERIFY_RTE_MSG( bFound, "Failed to find non-eliminated edge in OR step" );
    }

    void buildOr( Instructions::InstructionGroup* pInstruction, Variables::Variable* pVariable, Derivation::Or* pOr )
    {
        std::vector< Derivation::Edge* > edges;
        for( auto pEdge : pOr->get_edges() )
        {
            if( !pEdge->get_eliminated() && !pEdge->get_backtracked() )
            {
                edges.push_back( pEdge );
            }
        }
        if( edges.empty() )
        {
            // add the operation
            buildOperation( pInstruction, pVariable, pOr->get_vertex() );
        }
        else
        {
            bool bFound = false;
            for( auto pEdge : edges )
            {
                VERIFY_RTE_MSG( !bFound, "Multiple non-eliminated edges in OR derivation step" );
                build( pInstruction, pVariable, pEdge );
                bFound = true;
            }
            VERIFY_RTE_MSG( bFound, "Failed to find non-eliminated edge in OR derivation step" );
        }
    }

    void build( Instructions::InstructionGroup* pInstruction, Variables::Variable* pVariable,
                InvocationPolicy::EdgePtr pEdge )
    {
        // NOTE pInstruction AND pVariable are OUT parameters taken by reference
        buildHyperGraphEdges( pInstruction, pVariable, pEdge );

        auto pNextStep = pEdge->get_next();
        if( auto pAnd = db_cast< Derivation::And >( pNextStep ) )
        {
            // is the AND out edges backtracked for link dimension
            int  iBackTrackedCount = 0;
            auto edges             = pAnd->get_edges();
            for( auto pAndEdge : edges )
            {
                if( pAndEdge->get_backtracked() )
                {
                    ++iBackTrackedCount;
                }
            }
            if( iBackTrackedCount != 0 )
            {
                VERIFY_RTE( iBackTrackedCount == edges.size() );
                buildOperation( pInstruction, pVariable, pAnd->get_vertex() );
            }
            else
            {
                buildAnd( pInstruction, pVariable, edges );
            }
        }
        else if( auto pOr = db_cast< Derivation::Or >( pNextStep ) )
        {
            buildOr( pInstruction, pVariable, pOr );
        }
        else
        {
            THROW_RTE( "Unknown derivation step type" );
        }
    }

    void buildHyperGraphEdges( Instructions::InstructionGroup*& pInstruction, Variables::Variable*& pVariable,
                               InvocationPolicy::EdgePtr pEdge )
    {
        for( auto pGraphEdge : pEdge->get_edges() )
        {
            switch( pGraphEdge->get_type().get() )
            {
                case EdgeType::eParent:
                {
                    auto pContext = db_cast< Concrete::Context >( pGraphEdge->get_target() );
                    VERIFY_RTE( pContext );
                    Variables::Stack* pTo = make_stack_variable( pVariable, pContext );
                    pInstruction = make_instruction< Instructions::ParentDerivation >( pInstruction, pVariable, pTo );
                    pVariable    = pTo;
                }
                break;
                case EdgeType::eChildSingular:
                {
                    auto pContext = db_cast< Concrete::Context >( pGraphEdge->get_target() );
                    VERIFY_RTE( pContext );
                    Variables::Stack* pTo = make_stack_variable( pVariable, pContext );
                    pInstruction = make_instruction< Instructions::ChildDerivation >( pInstruction, pVariable, pTo );
                    pVariable    = pTo;
                }
                break;
                case EdgeType::eChildNonSingular:
                {
                }
                break;
                case EdgeType::eDim:
                {
                }
                break;
                case EdgeType::eLink:
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
    }

public:
    OperationsStage::Operations::Invocation* getInvocation() const { return m_pInvocation; }
};

void buildOperation( OperationsStage::Database& database, OperationsStage::Operations::Invocation* pInvocation )
{
    using ::operator<<;

    std::vector< Concrete::Graph::Vertex* > operationContexts;
    for( auto pOperation : pInvocation->get_operations() )
    {
        operationContexts.push_back( pOperation->get_context() );
    }

    // clang-format off
    std::vector< Concrete::Object*              > objects;
    std::vector< Concrete::Component*           > components;
    std::vector< Concrete::State*               > states;
    std::vector< Concrete::Function*            > functions;
    std::vector< Concrete::Namespace*           > namespaces;
    std::vector< Concrete::Event*               > events;
    std::vector< Concrete::Interupt*            > interupts;
    std::vector< Concrete::Dimensions::User*    > userDimensions;
    std::vector< Concrete::Dimensions::Link*    > linkDimensions;
    // clang-format on

    enum TargetType
    {
        eObjects,
        eComponents,
        eStates,
        eFunctions,
        eEvents,
        eUserDimensions,
        eLinkDimensions,
        eUNSET
    } targetType
        = eUNSET;

    for( auto pVert : operationContexts )
    {
        if( auto p = db_cast< Concrete::Object >( pVert ) )
        {
            objects.push_back( p );
            VERIFY_RTE_MSG( ( targetType == eUNSET ) || ( targetType == eObjects ),
                            "Conflicting target types for invocation: " << pInvocation->get_id() );
            targetType = eObjects;
        }
        else if( auto p = db_cast< Concrete::Component >( pVert ) )
        {
            components.push_back( p );
            VERIFY_RTE_MSG( ( targetType == eUNSET ) || ( targetType == eComponents ),
                            "Conflicting target types for invocation: " << pInvocation->get_id() );
            targetType = eComponents;
        }
        else if( auto p = db_cast< Concrete::State >( pVert ) )
        {
            states.push_back( p );
            VERIFY_RTE_MSG( ( targetType == eUNSET ) || ( targetType == eStates ),
                            "Conflicting target types for invocation: " << pInvocation->get_id() );
            targetType = eStates;
        }
        else if( auto p = db_cast< Concrete::Function >( pVert ) )
        {
            functions.push_back( p );
            VERIFY_RTE_MSG( ( targetType == eUNSET ) || ( targetType == eFunctions ),
                            "Conflicting target types for invocation: " << pInvocation->get_id() );
            targetType = eFunctions;
        }
        else if( auto p = db_cast< Concrete::Namespace >( pVert ) )
        {
            namespaces.push_back( p );
        }
        else if( auto p = db_cast< Concrete::Event >( pVert ) )
        {
            events.push_back( p );
            VERIFY_RTE_MSG( ( targetType == eUNSET ) || ( targetType == eEvents ),
                            "Conflicting target types for invocation: " << pInvocation->get_id() );
            targetType = eEvents;
        }
        else if( auto p = db_cast< Concrete::Interupt >( pVert ) )
        {
            interupts.push_back( p );
        }
        else if( auto p = db_cast< Concrete::Dimensions::User >( pVert ) )
        {
            userDimensions.push_back( p );
            VERIFY_RTE_MSG( ( targetType == eUNSET ) || ( targetType == eUserDimensions ),
                            "Conflicting target types for invocation: " << pInvocation->get_id() );
            targetType = eUserDimensions;
        }
        else if( auto p = db_cast< Concrete::Dimensions::Link >( pVert ) )
        {
            linkDimensions.push_back( p );
            VERIFY_RTE_MSG( ( targetType == eUNSET ) || ( targetType == eLinkDimensions ),
                            "Conflicting target types for invocation: " << pInvocation->get_id() );
            targetType = eLinkDimensions;
        }
    }

    using ::operator<<;
    VERIFY_RTE_MSG( namespaces.empty(), "Invalid invocation on namespace: " << pInvocation->get_id() );
    VERIFY_RTE_MSG( interupts.empty(), "Invalid invocation on interupt: " << pInvocation->get_id() );
    VERIFY_RTE_MSG( targetType != eUNSET, "No targe type for invocation: " << pInvocation->get_id() );

    std::set< std::string > dimensionTypes;
    for( auto pDim : userDimensions )
    {
        dimensionTypes.insert( pDim->get_interface_dimension()->get_canonical_type() );
    }

    ExplicitOperationID explicitOperationType = HIGHEST_EXPLICIT_OPERATION_TYPE;

    using namespace OperationsStage::Operations;

    switch( pInvocation->get_id().m_operation )
    {
        case mega::id_Imp_NoParams:
        {
            switch( targetType )
            {
                case eObjects:
                    THROW_RTE( "Implicit invocation cannot be on object context" );
                    break;
                case eComponents:
                    THROW_RTE( "Implicit invocation cannot be on component context" );
                    break;
                case eStates:
                    // return type is the started state
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : states )
                        {
                            contexts.push_back( pConcrete->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< Start >( Start::Args{ pInvocation } );

                    explicitOperationType = id_exp_Start;
                    break;
                case eFunctions:
                    // return type is the function return type
                    {
                        std::vector< Interface::Function* > contexts;
                        std::set< std::string >             types;
                        for( auto pConcrete : functions )
                        {
                            contexts.push_back( pConcrete->get_interface_function() );
                            types.insert(
                                pConcrete->get_interface_function()->get_return_type_trait()->get_canonical_type() );
                        }
                        const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                        contexts               = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Function >(
                            ReturnTypes::Function::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                    }

                    database.construct< Call >( Call::Args{ pInvocation } );
                    explicitOperationType = id_exp_Call;
                    break;
                case eEvents:
                    // return type is the signaled event
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : events )
                        {
                            contexts.push_back( pConcrete->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< Signal >( Signal::Args{ pInvocation } );
                    explicitOperationType = id_exp_Signal;
                    break;
                case eUserDimensions:
                    // return type is the read dimensions
                    {
                        std::vector< Interface::DimensionTrait* > contexts;
                        std::set< std::string >                   types;
                        for( auto pConcrete : userDimensions )
                        {
                            auto pDimensionTrait = pConcrete->get_interface_dimension();
                            contexts.push_back( pDimensionTrait );
                            types.insert( pDimensionTrait->get_canonical_type() );
                        }
                        const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                        contexts               = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Dimension >(
                            ReturnTypes::Dimension::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                    }

                    database.construct< Read >( Read::Args{ pInvocation } );
                    explicitOperationType = id_exp_Read;
                    break;
                case eLinkDimensions:
                    // return type is the target of the link
                    {
                        /*std::vector< Concrete::Dimensions::Link* > targets;
                        for( auto pConcrete : linkDimensions )
                        {
                            bool bFound = false;
                            for( auto pGraphEdge : pConcrete->get_out_edges() )
                            {
                                if( pGraphEdge->get_type().get() == EdgeType::eMono
                                    || pGraphEdge->get_type().get() == EdgeType::ePoly
                                    || pGraphEdge->get_type().get() == EdgeType::ePolyParent )
                                {
                                    auto pTargetContext
                                        = db_cast< Concrete::Dimensions::Link >( pGraphEdge->get_target() );
                                    VERIFY_RTE( pTargetContext );
                                    targets.push_back( pTargetContext );
                                    bFound = true;
                                }
                            }
                            VERIFY_RTE( bFound );
                        }*/

                        linkDimensions = make_unique_without_reorder( linkDimensions );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Link >(
                            ReturnTypes::Link::Args{ ReturnTypes::ReturnType::Args{}, linkDimensions } ) );
                    }

                    database.construct< ReadLink >( ReadLink::Args{ pInvocation } );
                    explicitOperationType = id_exp_Read_Link;
                    break;
                case eUNSET:
                    break;
            }
        }
        break;
        case mega::id_Imp_Params:
        {
            switch( targetType )
            {
                case eObjects:
                    THROW_RTE( "Implicit invocation cannot be on object context" );
                    break;
                case eComponents:
                    THROW_RTE( "Implicit invocation cannot be on component context" );
                    break;
                case eStates:
                    THROW_RTE( "Start operation cannot have parameters" );
                    break;
                case eFunctions:
                    // return type is the function return type
                    {
                        std::vector< Interface::Function* > contexts;
                        std::set< std::string >             types;
                        for( auto pConcrete : functions )
                        {
                            contexts.push_back( pConcrete->get_interface_function() );
                            types.insert(
                                pConcrete->get_interface_function()->get_return_type_trait()->get_canonical_type() );
                        }
                        const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                        contexts               = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Function >(
                            ReturnTypes::Function::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                    }

                    database.construct< Call >( Call::Args{ pInvocation } );
                    explicitOperationType = id_exp_Call;
                    break;
                case eEvents:
                    THROW_RTE( "Event operation cannot have parameters" );
                    break;
                case eUserDimensions:
                    // return the context of the write
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : userDimensions )
                        {
                            contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< Write >( Write::Args{ pInvocation } );
                    explicitOperationType = id_exp_Write;
                    break;
                case eLinkDimensions:
                    // return the context of the write
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : linkDimensions )
                        {
                            contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< WriteLink >( WriteLink::Args{ pInvocation } );
                    explicitOperationType = id_exp_Write_Link;
                    break;
                case eUNSET:
                    break;
            }
        }
        break;
        case mega::id_Move:
        {
            THROW_TODO;
            database.construct< Move >( Move::Args{ pInvocation } );
            explicitOperationType = id_exp_Move;
        }
        break;
        case mega::id_Get:
        {
            switch( targetType )
            {
                case eObjects:
                    // reference context of object
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : objects )
                        {
                            contexts.push_back( pConcrete->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< GetContext >( GetContext::Args{ pInvocation } );
                    explicitOperationType = id_exp_GetContext;
                    break;
                case eComponents:
                    // reference context of component
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : components )
                        {
                            contexts.push_back( pConcrete->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< GetContext >( GetContext::Args{ pInvocation } );
                    explicitOperationType = id_exp_GetContext;
                    break;
                case eStates:
                    // reference context of state
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : states )
                        {
                            contexts.push_back( pConcrete->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< GetContext >( GetContext::Args{ pInvocation } );
                    explicitOperationType = id_exp_GetContext;
                    break;
                case eFunctions:
                    // reference context of function
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : functions )
                        {
                            contexts.push_back( pConcrete->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< GetContext >( GetContext::Args{ pInvocation } );
                    explicitOperationType = id_exp_GetContext;
                    break;
                case eEvents:
                    // reference context of events
                    {
                        std::vector< Interface::IContext* > contexts;
                        for( auto pConcrete : events )
                        {
                            contexts.push_back( pConcrete->get_interface() );
                        }
                        contexts = make_unique_without_reorder( contexts );
                        pInvocation->set_return_type( database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                    }

                    database.construct< GetContext >( GetContext::Args{ pInvocation } );
                    explicitOperationType = id_exp_GetContext;
                    break;
                case eUserDimensions:
                    THROW_RTE( "Cannot get a dimension" );
                    // database.construct< GetDimension >( GetDimension::Args{ pInvocation } );
                    // explicitOperationType = id_exp_GetDimension;
                    break;
                case eLinkDimensions:
                    THROW_RTE( "Cannot get a dimension" );
                    // database.construct< GetDimension >( GetDimension::Args{ pInvocation } );
                    // explicitOperationType = id_exp_GetDimension;
                    break;
                case eUNSET:
                    break;
            }
        }
        break;
        case mega::id_Range:
        {
            THROW_TODO;
            database.construct< Range >( Range::Args{ pInvocation } );
            explicitOperationType = id_exp_Range;
        }
        break;
        default:
        case mega::HIGHEST_OPERATION_TYPE:
        {
            THROW_RTE( "Unknown implicit operation type" );
        }
        break;
    }

    VERIFY_RTE_MSG( explicitOperationType != HIGHEST_EXPLICIT_OPERATION_TYPE,
                    "Failed to determine invocation explicit operation type: " << pInvocation->get_id() );
    pInvocation->set_explicit_operation( explicitOperationType );
}

} // namespace

OperationsStage::Operations::Invocation*
compileInvocation( OperationsStage::Database& database, const SymbolTables& symbolTables, const mega::InvocationID& id )
{
    using ::operator<<;

    // determine the derivation of the invocationID
    std::vector< Concrete::Graph::Vertex* > types;
    InvocationPolicy::Spec                  derivationSpec;
    fromInvocationID( symbolTables, id, types, derivationSpec );

    // solve the context free derivation
    InvocationPolicy              policy( database );
    InvocationPolicy::OrPtrVector finalFrontier;
    InvocationPolicy::RootPtr     pRoot = DerivationSolver::solveContextFree( derivationSpec, policy, finalFrontier );

    Derivation::precedence( pRoot );

    OperationsStage::Operations::Invocation* pInvocation = nullptr;
    try
    {
        const Derivation::Disambiguation result = Derivation::disambiguate( pRoot, finalFrontier );
        if( result != Derivation::eSuccess )
        {
            std::ostringstream os;
            using ::           operator<<;
            if( result == Derivation::eAmbiguous )
                os << "Derivation disambiguation was ambiguous for: " << id << "\n";
            else if( result == Derivation::eFailure )
                os << "Derivation disambiguation failed for: " << id << "\n";
            else
                THROW_RTE( "Unknown derivation failure type" );
            THROW_RTE( os.str() );
        }

        // is the invocation a link invocation?
        InvocationPolicy::AndPtrVector linkFrontier;
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
        }

        InvocationBuilder builder( database, id, pRoot, types );
        builder.build();
        pInvocation = builder.getInvocation();
        buildOperation( database, pInvocation );
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
