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

#include "common/unreachable.hpp"

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
    /*AndPtr makeAnd( GraphVertex* pVertex ) const
    {
        return m_database.construct< Derivation::And >(
            Derivation::And::Args{ Derivation::Step::Args{ pVertex, {} } } );
    }*/
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
    /*GraphVertexVector enumerateLinkContexts( GraphVertex* pVertex ) const
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
                case EdgeType::eMonoSingularMandatory:
                case EdgeType::ePolySingularMandatory:
                case EdgeType::eMonoNonSingularMandatory:
                case EdgeType::ePolyNonSingularMandatory:
                case EdgeType::eMonoSingularOptional:
                case EdgeType::ePolySingularOptional:
                case EdgeType::eMonoNonSingularOptional:
                case EdgeType::ePolyNonSingularOptional:
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

                case EdgeType::eMonoSingularMandatory:
                case EdgeType::ePolySingularMandatory:
                case EdgeType::eMonoNonSingularMandatory:
                case EdgeType::ePolyNonSingularMandatory:
                case EdgeType::eMonoSingularOptional:
                case EdgeType::ePolySingularOptional:
                case EdgeType::eMonoNonSingularOptional:
                case EdgeType::ePolyNonSingularOptional:

                case EdgeType::eParent:
                case EdgeType::eChildSingular:
                case EdgeType::eChildNonSingular:
                case EdgeType::eDim:
                case EdgeType::TOTAL_EDGE_TYPES:
                    break;
            }
        }

        return result;
    }*/

    OrPtrVector expandLink( OrPtr pOr ) const
    {
        OrPtrVector result;

        if( auto pLink = db_cast< Concrete::Dimensions::Link >( pOr->get_vertex() ) )
        {
            GraphEdgeVector edges;
            for( auto pEdge : pLink->get_out_edges() )
            {
                switch( pEdge->get_type().get() )
                {
                    case EdgeType::eMonoSingularMandatory:
                    case EdgeType::ePolySingularMandatory:
                    case EdgeType::eMonoNonSingularMandatory:
                    case EdgeType::ePolyNonSingularMandatory:
                    case EdgeType::eMonoSingularOptional:
                    case EdgeType::ePolySingularOptional:
                    case EdgeType::eMonoNonSingularOptional:
                    case EdgeType::ePolyNonSingularOptional:
                    case EdgeType::ePolyParent:
                    {
                        edges.push_back( pEdge );
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
            VERIFY_RTE( !edges.empty() );

            auto pAnd = m_database.construct< Derivation::And >(
                Derivation::And::Args{ Derivation::Step::Args{ pLink, {} } } );

            auto pOrToAndEdge
                = m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pAnd, false, false, 0, {} } );

            pOr->push_back_edges( pOrToAndEdge );

            for( auto pGraphEdge : edges )
            {
                // determine the parent context of the link target
                GraphVertex* pParentVertex = nullptr;
                GraphEdge*   pParentEdge   = nullptr;
                {
                    auto pLinkTarget = pGraphEdge->get_target();
                    for( auto pLinkGraphEdge : pLinkTarget->get_out_edges() )
                    {
                        if( pLinkGraphEdge->get_type().get() == EdgeType::eParent )
                        {
                            VERIFY_RTE( !pParentVertex );
                            pParentEdge   = pLinkGraphEdge;
                            pParentVertex = pLinkGraphEdge->get_target();
                        }
                    }
                }
                VERIFY_RTE( pParentEdge );
                VERIFY_RTE( pParentVertex );

                auto pLinkTargetOr = m_database.construct< Derivation::Or >(
                    Derivation::Or::Args{ Derivation::Step::Args{ pParentVertex, {} } } );

                auto pDerivationEdge = m_database.construct< Derivation::Edge >(
                    Derivation::Edge::Args{ pLinkTargetOr, false, false, 0, { pGraphEdge, pParentEdge } } );

                pAnd->push_back_edges( pDerivationEdge );

                result.push_back( pLinkTargetOr );
            }
        }
        else
        {
            result.push_back( pOr );
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
                        default:
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
                       std::vector< Concrete::Graph::Vertex* >& contextTypes, InvocationPolicy::Spec& spec )
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
                    contextTypes.push_back( pConcrete );
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
                    contextTypes.push_back( pConcrete );
                    spec.context.push_back( pConcrete );
                }
            }
        }
    }

    for( auto& symbolID : id.m_type_path )
    {
        std::vector< Concrete::Graph::Vertex* > pathElement;

        std::optional< TypeID > actualSymbolID;

        if( isOperationType( symbolID ) )
        {
            VERIFY_RTE_MSG( !operationIDOpt.has_value(), "Operation ID defined twice" );
            operationIDOpt = static_cast< mega::OperationID >( symbolID.getSymbolID() );
        }
        else if( symbolID.isContextID() )
        {
            // NOTE: Always convert any interface type id BACK to its symbol ID
            auto iFind = symbolTables.interfaceIDMap.find( symbolID );
            VERIFY_RTE( iFind != symbolTables.interfaceIDMap.end() );
            auto pInterfaceSymbol = iFind->second;

            const auto& typeIDSequence = pInterfaceSymbol->get_symbol_ids();
            VERIFY_RTE( !typeIDSequence.empty() );
            actualSymbolID = typeIDSequence.back();
        }
        else if( symbolID.isSymbolID() )
        {
            actualSymbolID = symbolID;
        }

        if( actualSymbolID.has_value() )
        {
            auto iFind = symbolTables.symbolIDMap.find( actualSymbolID.value() );
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
            if( !pathElement.empty() )
            {
                spec.path.emplace_back( std::move( pathElement ) );
            }
        }
    }

    using ::operator<<;
    VERIFY_RTE_MSG( !operationIDOpt.has_value() || ( id.m_operation == operationIDOpt.value() ),
                    "Mismatching operation type in invocation: " << id );
}

// std::vector< Invocations::Variables::Variable* >;

using namespace OperationsStage::Invocations;

class DerivationBuilder
{
    Variables::Stack* make_stack_variable( Variables::Variable*                      pParentVariable,
                                           OperationsStage::Concrete::Graph::Vertex* pTargetContext )
    {
        using namespace OperationsStage;
        auto pStack = m_database.construct< Variables::Stack >(
            Variables::Stack::Args{ Variables::Variable::Args{ pParentVariable }, pTargetContext } );
        m_pInvocation->push_back_variables( pStack );
        return pStack;
    }
    Variables::LinkType* make_link_type_variable( Variables::Variable*                             pParentVariable,
                                                  OperationsStage::Concrete::Dimensions::LinkType* pLinkType )
    {
        using namespace OperationsStage;
        auto pStack = m_database.construct< Variables::LinkType >(
            Variables::LinkType::Args{ Variables::Variable::Args{ pParentVariable }, pLinkType } );
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
    DerivationBuilder( OperationsStage::Database& database, const mega::InvocationID& id,
                       InvocationPolicy::RootPtr                      pDerivationTreeRoot,
                       const std::vector< Concrete::Graph::Vertex* >& contextTypes )
        : m_database( database )
        , m_pDerivationTreeRoot( pDerivationTreeRoot )
        , m_pInvocation( database.construct< OperationsStage::Operations::Invocation >(
              OperationsStage::Operations::Invocation::Args{ id, pDerivationTreeRoot, {}, {} } ) )
        , m_pParameterVariable( make_parameter_variable( contextTypes ) )
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
            auto pPolyReference
                = make_instruction< Instructions::PolyBranch >( m_pRootInstruction, m_pParameterVariable );
            for( auto pEdge : m_pDerivationTreeRoot->get_edges() )
            {
                auto pInstruction = make_instruction< Instructions::PolyCase >(
                    pPolyReference, m_pParameterVariable, pEdge->get_next()->get_vertex() );
                auto pNext = pEdge->get_next();
                auto pOR   = db_cast< Derivation::Or >( pNext );
                VERIFY_RTE( pOR );
                buildOr( pInstruction, m_pParameterVariable, pOR );
            }
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

    void buildAnd( Instructions::InstructionGroup* pInstruction, Variables::Variable* pVariable, Derivation::And* pAnd )
    {
        // read the link
        auto pLink = db_cast< Concrete::Dimensions::Link >( pAnd->get_vertex() );

        auto pLinkReference = make_memory_variable( pVariable, {} );
        auto pLinkType      = make_link_type_variable( pVariable, pLink->get_link_type() );

        auto pDereference = make_instruction< Instructions::Dereference >(
            pInstruction, pVariable, pLinkReference, pLinkType, pLink );

        // create polymorphic branch
        auto pPolyReference = make_instruction< Instructions::LinkBranch >( pDereference, pLinkType );

        bool bFound = false;
        for( auto pEdge : pAnd->get_edges() )
        {
            if( !pEdge->get_eliminated() )
            {
                bFound = true;
                VERIFY_RTE( !pEdge->get_backtracked() );

                pInstruction                         = pPolyReference;
                Variables::Variable* pResultVariable = pLinkReference;

                auto hyperGraphEdges = pEdge->get_edges();
                VERIFY_RTE( !hyperGraphEdges.empty() );

                for( auto hyperGraphEdge : hyperGraphEdges )
                {
                    switch( hyperGraphEdge->get_type().get() )
                    {
                        case EdgeType::eMonoSingularMandatory:
                        case EdgeType::eMonoSingularOptional:
                        {
                            pInstruction = make_instruction< Instructions::PolyCase >(
                                pInstruction, pResultVariable, hyperGraphEdge->get_target() );
                        }
                        break;
                        case EdgeType::eMonoNonSingularMandatory:
                        case EdgeType::eMonoNonSingularOptional:
                        {
                            THROW_RTE( "Non singular dereference" );
                        }
                        break;
                        case EdgeType::ePolySingularMandatory:
                        case EdgeType::ePolySingularOptional:
                        {
                            pInstruction = make_instruction< Instructions::PolyCase >(
                                pInstruction, pResultVariable, hyperGraphEdge->get_target() );
                        }
                        break;
                        case EdgeType::ePolyNonSingularMandatory:
                        case EdgeType::ePolyNonSingularOptional:
                        {
                            THROW_RTE( "Non singular dereference" );
                        }
                        break;
                        case EdgeType::ePolyParent:
                        {
                            pInstruction = make_instruction< Instructions::PolyCase >(
                                pInstruction, pResultVariable, hyperGraphEdge->get_target() );
                        }
                        break;

                        case EdgeType::eParent:
                        {
                            auto pTo     = make_stack_variable( pResultVariable, hyperGraphEdge->get_target() );
                            pInstruction = make_instruction< Instructions::ParentDerivation >(
                                pInstruction, pResultVariable, pTo );
                            pResultVariable = pTo;
                        }
                        break;

                        default:
                        {
                            THROW_RTE( "Unsupported AND hypergraph edge" );
                        }
                        break;
                    }
                }

                auto pNext = pEdge->get_next();
                auto pOR   = db_cast< Derivation::Or >( pNext );
                VERIFY_RTE( pOR );
                buildOr( pInstruction, pResultVariable, pOR );
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
                buildAnd( pInstruction, pVariable, pAnd );
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
                case EdgeType::eDim:
                case EdgeType::eLink:
                case EdgeType::eChildSingular:
                {
                    Variables::Stack* pTo = make_stack_variable( pVariable, pGraphEdge->get_target() );
                    pInstruction = make_instruction< Instructions::ChildDerivation >( pInstruction, pVariable, pTo );
                    pVariable    = pTo;
                }
                break;
                case EdgeType::eChildNonSingular:
                {
                    THROW_TODO;
                }
                break;
                case EdgeType::eMonoSingularMandatory:
                case EdgeType::eMonoSingularOptional:
                case EdgeType::eMonoNonSingularMandatory:
                case EdgeType::eMonoNonSingularOptional:
                case EdgeType::ePolySingularMandatory:
                case EdgeType::ePolySingularOptional:
                case EdgeType::ePolyNonSingularMandatory:
                case EdgeType::ePolyNonSingularOptional:
                    THROW_TODO;
                    break;
                case EdgeType::ePolyParent:
                {
                    THROW_TODO;
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

using namespace OperationsStage;
using namespace OperationsStage::Operations;

class OperationBuilder
{
    Database&   m_database;
    Invocation* m_pInvocation;

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
    } m_targetType
        = eUNSET;

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

    void classifyOperations()
    {
        std::vector< Concrete::Graph::Vertex* > operationContexts;
        for( auto pOperation : m_pInvocation->get_operations() )
        {
            operationContexts.push_back( pOperation->get_context() );
        }

        for( auto pVert : operationContexts )
        {
            if( auto p = db_cast< Concrete::Object >( pVert ) )
            {
                objects.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eObjects ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eObjects;
            }
            else if( auto p = db_cast< Concrete::Component >( pVert ) )
            {
                components.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eComponents ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eComponents;
            }
            else if( auto p = db_cast< Concrete::State >( pVert ) )
            {
                states.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eStates ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eStates;
            }
            else if( auto p = db_cast< Concrete::Function >( pVert ) )
            {
                functions.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eFunctions ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eFunctions;
            }
            else if( auto p = db_cast< Concrete::Namespace >( pVert ) )
            {
                namespaces.push_back( p );
            }
            else if( auto p = db_cast< Concrete::Event >( pVert ) )
            {
                events.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eEvents ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eEvents;
            }
            else if( auto p = db_cast< Concrete::Interupt >( pVert ) )
            {
                interupts.push_back( p );
            }
            else if( auto p = db_cast< Concrete::Dimensions::User >( pVert ) )
            {
                userDimensions.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eUserDimensions ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eUserDimensions;
            }
            else if( auto p = db_cast< Concrete::Dimensions::Link >( pVert ) )
            {
                linkDimensions.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eLinkDimensions ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eLinkDimensions;
            }
        }

        using ::operator<<;
        VERIFY_RTE_MSG( namespaces.empty(), "Invalid invocation on namespace: " << m_pInvocation->get_id() );
        VERIFY_RTE_MSG( interupts.empty(), "Invalid invocation on interupt: " << m_pInvocation->get_id() );
        VERIFY_RTE_MSG( m_targetType != eUNSET, "No targe type for invocation: " << m_pInvocation->get_id() );

        std::set< std::string > dimensionTypes;
        for( auto pDim : userDimensions )
        {
            dimensionTypes.insert( pDim->get_interface_dimension()->get_canonical_type() );
        }
    }

    void buildNoParams()
    {
        switch( m_targetType )
        {
            case eObjects:
                THROW_RTE( "Implicit invocation cannot be on object context" );
                break;
            case eComponents:
                THROW_RTE( "Implicit invocation cannot be on component context" );
                break;
            case eStates:
            {
                // return type is the started state
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

                m_database.construct< Start >( Start::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Start );
            }
            break;
            case eFunctions:
            {
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
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Function >(
                        ReturnTypes::Function::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Call >( Call::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Call );
            }
            break;
            case eEvents:
            {
                // return type is the signaled event
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

                m_database.construct< Signal >( Signal::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Signal );
            }
            break;
            case eUserDimensions:
            {
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
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Dimension >(
                        ReturnTypes::Dimension::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Read >( Read::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Read );
            }
            break;
            case eLinkDimensions:
            {
                // return type is the target of the link
                {
                    std::vector< Interface::IContext* > targets;
                    for( auto pConcrete : linkDimensions )
                    {
                        bool bFound = false;
                        for( auto pGraphEdge : pConcrete->get_out_edges() )
                        {
                            switch( pGraphEdge->get_type().get() )
                            {
                                case EdgeType::eMonoSingularMandatory:
                                case EdgeType::ePolySingularMandatory:
                                case EdgeType::eMonoSingularOptional:
                                case EdgeType::ePolySingularOptional:
                                case EdgeType::ePolyParent:

                                case EdgeType::eMonoNonSingularMandatory:
                                case EdgeType::ePolyNonSingularMandatory:
                                case EdgeType::eMonoNonSingularOptional:
                                case EdgeType::ePolyNonSingularOptional:
                                {
                                    VERIFY_RTE( !bFound );
                                    auto pTargetContext
                                        = db_cast< Concrete::Dimensions::Link >( pGraphEdge->get_target() );
                                    VERIFY_RTE( pTargetContext );
                                    auto pParentContext = pTargetContext->get_parent_context();
                                    targets.push_back( pParentContext->get_interface() );
                                    bFound = true;
                                }
                                break;
                            }
                        }
                        VERIFY_RTE( bFound );
                    }

                    targets = make_unique_without_reorder( targets );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, targets } ) );
                }

                m_database.construct< LinkRead >( LinkRead::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Read );
            }
            break;
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
    }

    void buildParams()
    {
        switch( m_targetType )
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
            {
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
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Function >(
                        ReturnTypes::Function::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Call >( Call::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Call );
            }
            break;
            case eEvents:
            {
                THROW_RTE( "Event operation cannot have parameters" );
            }
            break;
            case eUserDimensions:
            {
                // return the context of the write
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : userDimensions )
                    {
                        contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }
                // parameter type
                ReturnTypes::Dimension* pParameterType = nullptr;
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
                    pParameterType         = m_database.construct< ReturnTypes::Dimension >(
                        ReturnTypes::Dimension::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } );
                }

                m_database.construct< Write >( Write::Args{ m_pInvocation, pParameterType } );
                m_pInvocation->set_explicit_operation( id_exp_Write );
            }
            break;
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

                m_database.construct< LinkAdd >( LinkAdd::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Add );
            }
            break;
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
    }

    void buildGet()
    {
        switch( m_targetType )
        {
            case eObjects:
            {
                // reference context of object
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
                // reference context of component
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
                // reference context of state
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
            case eFunctions:
            {
                // reference context of function
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
                // reference context of events
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
                UNREACHABLE;
                break;
        }
    }

    void buildRemove()
    {
        switch( m_targetType )
        {
            case eObjects:
                THROW_RTE( "Cannot remove an object" );
                break;
            case eComponents:
                THROW_RTE( "Cannot remove a component" );
                break;
            case eStates:
                THROW_RTE( "Cannot remove a state" );
                break;
            case eFunctions:
                THROW_RTE( "Cannot remove a function" );
                break;
            case eEvents:
                THROW_RTE( "Cannot remove an event" );
                break;
            case eUserDimensions:
                THROW_RTE( "Cannot remove a dimension" );
                break;
            case eLinkDimensions:
            {
                // parameter type
                /*ReturnTypes::Context* pParameterType = nullptr;
                {
                    std::vector< Interface::DimensionTrait* > contexts;
                    std::set< std::string >                   types;
                    for( auto pConcrete : linkDimensions )
                    {
                        auto pDimensionTrait = pConcrete->get_interface_dimension();
                        contexts.push_back( pDimensionTrait );
                        types.insert( pDimensionTrait->get_canonical_type() );
                    }
                    contexts               = make_unique_without_reorder( contexts );
                    pParameterType         = m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } );
                }*/
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
                UNREACHABLE;
                break;
        }
    }

    void buildClear()
    {
        switch( m_targetType )
        {
            case eObjects:
                THROW_RTE( "Cannot clear an object" );
                break;
            case eComponents:
                THROW_RTE( "Cannot clear a component" );
                break;
            case eStates:
                THROW_RTE( "Cannot clear a state" );
                break;
            case eFunctions:
                THROW_RTE( "Cannot clear a function" );
                break;
            case eEvents:
                THROW_RTE( "Cannot clear an event" );
                break;
            case eUserDimensions:
                THROW_RTE( "Cannot clear a dimension" );
                break;
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
                UNREACHABLE;
                break;
        }
    }

    void buildMove()
    {
        THROW_TODO;
        switch( m_targetType )
        {
            case eObjects:
            case eComponents:
            case eStates:
            case eFunctions:
            case eEvents:
            case eUserDimensions:
            case eLinkDimensions:
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
        // m_database.construct< Move >( Move::Args{ m_pInvocation } );
        // m_pInvocation->set_explicit_operation( id_exp_Move );
    }

    void buildRange()
    {
        THROW_TODO;
        switch( m_targetType )
        {
            case eObjects:
            case eComponents:
            case eStates:
            case eFunctions:
            case eEvents:
            case eUserDimensions:
            case eLinkDimensions:
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
        // m_database.construct< Range >( Range::Args{ m_pInvocation } );
        // m_pInvocation->set_explicit_operation( id_exp_Range );
    }

public:
    OperationBuilder( OperationsStage::Database& database, OperationsStage::Operations::Invocation* pInvocation )
        : m_database( database )
        , m_pInvocation( pInvocation )
    {
    }

    void build()
    {
        classifyOperations();

        m_pInvocation->set_explicit_operation( HIGHEST_EXPLICIT_OPERATION_TYPE );

        switch( m_pInvocation->get_id().m_operation )
        {
            case mega::id_Imp_NoParams:
            {
                buildNoParams();
            }
            break;
            case mega::id_Imp_Params:
            {
                buildParams();
            }
            break;
            case mega::id_Move:
            {
                buildMove();
            }
            break;
            case mega::id_Get:
            {
                buildGet();
            }
            break;
            case mega::id_Range:
            {
                buildRange();
            }
            break;
            case mega::id_Remove:
            {
                buildRemove();
            }
            break;
            case mega::id_Clear:
            {
                buildClear();
            }
            break;
            default:
            case mega::HIGHEST_OPERATION_TYPE:
            {
                THROW_RTE( "Unknown implicit operation type" );
            }
            break;
        }

        VERIFY_RTE_MSG( m_pInvocation->get_explicit_operation() != HIGHEST_EXPLICIT_OPERATION_TYPE,
                        "Failed to determine invocation explicit operation type: " << m_pInvocation->get_id() );
    }
};

} // namespace

OperationsStage::Operations::Invocation*
compileInvocation( OperationsStage::Database& database, const SymbolTables& symbolTables, const mega::InvocationID& id )
{
    using ::operator<<;

    // determine the derivation of the invocationID
    std::vector< Concrete::Graph::Vertex* > contextTypes;
    InvocationPolicy::Spec                  derivationSpec;
    fromInvocationID( symbolTables, id, contextTypes, derivationSpec );

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
            DerivationBuilder builder( database, id, pRoot, contextTypes );
            builder.build();
            pInvocation = builder.getInvocation();
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
