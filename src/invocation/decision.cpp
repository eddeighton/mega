
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

#include "database/OperationsStage.hxx"

#include "compiler/derivation.hpp"

#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/compilation/operation_id.hpp"
#include "mega/values/compilation/hyper_graph.hpp"

#include "mega/common_strings.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "common/unreachable.hpp"

#include <optional>
#include <unordered_set>

namespace OperationsStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/common_ancestor.hpp"
namespace Derivation
{
#include "compiler/derivation_printer.hpp"
#include "compiler/disambiguate.hpp"
} // namespace Derivation
} // namespace OperationsStage

using namespace OperationsStage;

namespace mega::invocation
{
namespace
{
using DeciderVector = std::vector< Concrete::Decider* >;

void findDeciders( Concrete::Context* pContext, DeciderVector& deciders )
{
    if( auto pDecider = db_cast< Concrete::Decider >( pContext ) )
    {
        deciders.push_back( pDecider );
    }

    for( auto pChild : pContext->get_children() )
    {
        findDeciders( pChild, deciders );
    }
}

using StateVector       = std::vector< Concrete::State* >;
using StateVectorVector = std::vector< StateVector >;
using StateSet          = std::unordered_set< Concrete::State* >;

Concrete::State* findCommonAncestor( Concrete::State* pState, Concrete::State* pAncestor )
{
    VERIFY_RTE( pState && pAncestor );

    CommonAncestor::GraphEdgeVector edges;
    auto pAncestorVertex = CommonAncestor::commonRootDerivation( pState, pAncestor, edges, true );
    VERIFY_RTE_MSG( pAncestorVertex,
                    "Failed to find common ancestor between: " << Concrete::printContextFullType( pState ) << " and: "
                                                               << Concrete::printContextFullType( pAncestorVertex ) );
    pAncestor = db_cast< Concrete::State >( pAncestorVertex );
    VERIFY_RTE_MSG( pAncestor,
                    "Failed to find common ancestor state between: " << Concrete::printContextFullType(
                        pState ) << " and: " << Concrete::printContextFullType( pAncestor ) );
    return pAncestor;
}

Concrete::State* findCommonAncestor( Concrete::State* pContext, const StateVectorVector& transitionStatesSet )
{
    Concrete::State* pCommonAncestor = pContext;

    for( const auto& states : transitionStatesSet )
    {
        for( auto pState : states )
        {
            pCommonAncestor = findCommonAncestor( pState, pCommonAncestor );
        }
    }

    return pCommonAncestor;
}

Concrete::State* findCommonAncestorContextParent( Concrete::Context* pContext, Concrete::State* pCommonAncestor )
{
    // Common Ancestor MUST NOT be the actual context i.e. action foo > foo
    while( pCommonAncestor )
    {
        if( pCommonAncestor != pContext )
        {
            if( db_cast< Automata::Or >( pCommonAncestor->get_automata_vertex() ) )
            {
                break;
            }
        }
        pCommonAncestor = db_cast< Concrete::State >( pCommonAncestor->get_parent() );
    }

    return pCommonAncestor;
}

struct TruthTableSolver
{
    using Vector       = std::vector< Automata::Vertex* >;
    using VectorVector = std::vector< Vector >;

    static VectorVector add( Automata::Vertex* pVertex, const VectorVector& right )
    {
        VectorVector result;
        result.reserve( right.size() );
        for( const Vector& r : right )
        {
            Vector t;
            {
                t.reserve( 1 + r.size() );
                t.push_back( pVertex );
                std::copy( r.begin(), r.end(), std::back_inserter( t ) );
            }
            result.push_back( t );
        }
        return result;
    }

    static VectorVector multiply( const VectorVector& left, const VectorVector& right )
    {
        VectorVector result;
        result.reserve( left.size() * right.size() );
        for( const Vector& l : left )
        {
            for( const Vector& r : right )
            {
                Vector t;
                {
                    t.reserve( l.size() + r.size() );
                    std::copy( l.begin(), l.end(), std::back_inserter( t ) );
                    std::copy( r.begin(), r.end(), std::back_inserter( t ) );
                }
                result.push_back( t );
            }
        }
        return result;
    }

    static VectorVector solve( Automata::Vertex* pVertex )
    {
        VectorVector result;

        auto children = pVertex->get_children();

        if( children.empty() )
        {
            result.push_back( { pVertex } );
        }
        else
        {
            if( auto pOr = db_cast< Automata::Or >( pVertex ) )
            {
                for( auto pChild : children )
                {
                    for( const auto& t : add( pChild, solve( pChild ) ) )
                    {
                        result.push_back( t );
                    }
                }
            }
            else if( auto pAnd = db_cast< Automata::And >( pVertex ) )
            {
                bool bFirst = true;
                for( auto pChild : children )
                {
                    if( bFirst )
                    {
                        bFirst = false;
                        result = solve( pChild );
                    }
                    else
                    {
                        result = multiply( result, solve( pChild ) );
                    }
                }
            }
            else
            {
                THROW_RTE( "Unknown automata vertex type" );
            }
        }

        return result;
    }
};

using VariableVector = std::vector< Automata::Vertex* >;

struct Collector
{
    static void collectVariables( Automata::Vertex* pVertex, std::vector< Automata::Vertex* >& variables )
    {
        if( auto pOr = db_cast< Automata::Or >( pVertex ) )
        {
            for( auto pChild : pVertex->get_children() )
            {
                variables.push_back( pChild );
                collectVariables( pChild, variables );
            }
        }
        else if( auto pAnd = db_cast< Automata::And >( pVertex ) )
        {
            for( auto pChild : pVertex->get_children() )
            {
                collectVariables( pChild, variables );
            }
        }
        else
        {
            THROW_RTE( "Unknown automata vertex type" );
        }
    }

    static void collectTargets( Derivation::Step* pStep, std::vector< Concrete::Graph::Vertex* >& targets )
    {
        auto edges = pStep->get_edges();
        if( edges.empty() )
        {
            targets.push_back( pStep->get_vertex() );
        }
        else
        {
            for( auto pEdge : edges )
            {
                if( !pEdge->get_eliminated() )
                {
                    collectTargets( pEdge->get_next(), targets );
                }
            }
        }
    }

    static std::vector< Concrete::State* > collectTargetStates( Concrete::Context* pContext,
                                                                Derivation::Root*  pDerivation )
    {
        std::vector< Concrete::State* > states;
        {
            std::vector< Concrete::Graph::Vertex* > targets;
            for( auto pEdge : pDerivation->get_edges() )
            {
                if( !pEdge->get_eliminated() )
                {
                    collectTargets( pEdge->get_next(), targets );
                }
            }
            VERIFY_RTE_MSG( !targets.empty(),
                            "No derivation targets for derivation: " << Concrete::printContextFullType( pContext ) );
            VERIFY_RTE_MSG( targets.size() == 1U,
                            "Non-singular derivation targets for derivation: " << Concrete::printContextFullType(
                                pContext ) << "\n" << Derivation::printDerivationStep( pDerivation, false ) );
            for( auto pVertex : targets )
            {
                if( auto pState = db_cast< Concrete::State >( pVertex ) )
                {
                    states.push_back( pState );
                }
                else
                {
                    THROW_RTE( "Transition to non-state context: " << Concrete::printContextFullType( pContext ) );
                }
            }
            states = make_unique_without_reorder( states );
        }
        return states;
    }

    static StateVectorVector collectDerivationStates( Concrete::Context*                      pContext,
                                                      const std::vector< Derivation::Root* >& transitions )
    {
        StateVectorVector transitionStates;

        for( auto pDerivation : transitions )
        {
            std::vector< Concrete::State* > states = collectTargetStates( pContext, pDerivation );
            VERIFY_RTE_MSG(
                !states.empty(), "Derivation has empty target: " << Concrete::printContextFullType( pContext ) );
            transitionStates.push_back( states );
        }

        VERIFY_RTE_MSG(
            !transitionStates.empty(), "Derivation has no successors: " << Concrete::printContextFullType( pContext ) );

        return transitionStates;
    }

    static std::vector< VariableVector > statesToUniqueVariables( const StateVectorVector& transitionStates )
    {
        std::vector< VariableVector >           variableSequence;
        std::unordered_set< Automata::Vertex* > transitionVariables;
        {
            for( const auto& transition : transitionStates )
            {
                VariableVector variables;
                for( auto pState : transition )
                {
                    if( auto opt = pState->get_automata_vertex()->get_test_ancestor(); opt.has_value() )
                    {
                        auto pVariable = opt.value();
                        if( !transitionVariables.contains( pVariable ) )
                        {
                            transitionVariables.insert( pVariable );
                            variables.push_back( opt.value() );
                        }
                    }
                }
                if( !variables.empty() )
                {
                    variableSequence.push_back( variables );
                }
            }
        }
        return variableSequence;
    }
};

using TruthTable = std::vector< Automata::TruthAssignment* >;

using VarSet = std::unordered_set< Automata::Vertex* >;
struct State
{
    // sorted vectors using memory address
    VariableVector true_vars, false_vars;
    using Vector = std::vector< State >;

    bool match( const VariableVector& trueVerts, const VariableVector& falseVerts ) const
    {
        if( std::includes( true_vars.begin(), true_vars.end(), trueVerts.begin(), trueVerts.end() )
            && std::includes( false_vars.begin(), false_vars.end(), falseVerts.begin(), falseVerts.end() ) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};
State::Vector fromTruthTable( const TruthTableSolver::VectorVector& truthTable, const VariableVector& variablesSorted )
{
    State::Vector result;
    for( const auto& row : truthTable )
    {
        State state{ row, {} };
        std::sort( state.true_vars.begin(), state.true_vars.end() );
        std::set_difference( variablesSorted.begin(), variablesSorted.end(), state.true_vars.begin(),
                             state.true_vars.end(), std::back_inserter( state.false_vars ) );
        result.push_back( state );
    }
    return result;
}

struct DeciderSelector
{
    using DeciderVariables     = std::unordered_map< Concrete::Decider*, std::vector< VariableVector > >;
    using VariableVectorVector = std::vector< VariableVector >;

    struct VariableOrdering
    {
        using VarPos = std::pair< VariableVector, int >;
        std::vector< VariableVector > variables;
        std::vector< VarPos >         variablesSortedPos;
        std::vector< VariableVector > variablesSorted;

        void commonCtor()
        {
            int iPos = 0;
            for( const auto& v : variables )
            {
                VarPos pos{ v, iPos };
                std::sort( pos.first.begin(), pos.first.end() );
                variablesSortedPos.push_back( pos );
                variablesSorted.push_back( v );
                ++iPos;
            }
            std::sort( variablesSorted.begin(), variablesSorted.end() );
            std::sort( variablesSortedPos.begin(), variablesSortedPos.end(),
                       []( const VarPos& left, const VarPos& right ) { return left.first < right.first; } );
        }

        VariableOrdering( VariableVectorVector::const_iterator iBegin, VariableVectorVector::const_iterator iEnd )
            : variables( iBegin, iEnd )
        {
            commonCtor();
        }

        VariableOrdering( VariableVector::const_iterator iBegin, VariableVector::const_iterator iEnd )
        {
            for( auto i = iBegin; i != iEnd; ++i )
            {
                variables.push_back( { *i } );
            }
            commonCtor();
        }

        auto size() const { return variables.size(); }
        bool match( const VariableOrdering& cmp ) const { return variablesSorted == cmp.variablesSorted; }
    };

    struct DeciderInfo
    {
        using Vector = std::vector< DeciderInfo >;

        Concrete::Decider* pDecider;
        VariableOrdering   variables;

        auto size() const { return variables.size(); }

        bool match( const VariableOrdering& cmp, std::vector< U32 >& variableOrdering ) const
        {
            if( variables.match( cmp ) )
            {
                // work out relative variable ordering...
                const auto total = variables.variables.size();
                variableOrdering.resize( total );
                auto i    = variables.variablesSortedPos.begin();
                auto iEnd = variables.variablesSortedPos.end();
                auto j    = cmp.variablesSortedPos.begin();

                for( ; i != iEnd; ++i, ++j )
                {
                    int iDecidPos = i->second;
                    int iTransPos = j->second;

                    VERIFY_RTE( iDecidPos < total );
                    VERIFY_RTE( iTransPos < total );

                    variableOrdering[ iDecidPos ] = iTransPos;
                }

                return true;
            }
            else
            {
                return false;
            }
        }
    };
    Concrete::Context*  m_pContext;
    DeciderInfo::Vector m_deciders;

    DeciderSelector( Concrete::Context* pContext, const std::vector< Concrete::Decider* >& deciders )
        : m_pContext( pContext )
    {
        for( auto pDecider : deciders )
        {
            auto states    = Collector::collectDerivationStates( pDecider, pDecider->get_variables() );
            auto variables = Collector::statesToUniqueVariables( states );
            m_deciders.push_back( DeciderInfo{ pDecider, { variables.begin(), variables.end() } } );
        }
    }

    std::optional< Concrete::Decider* >
    chooseTransitionDecider( const std::vector< VariableVector >& transitionVariables,
                             std::vector< U32 >&                  variableOrdering ) const
    {
        if( transitionVariables.size() == 1 )
        {
            variableOrdering = { 0 };
            return {};
        }
        else
        {
            VariableOrdering transitionVars( transitionVariables.begin(), transitionVariables.end() );
            for( const DeciderInfo& deciderInfo : m_deciders )
            {
                if( deciderInfo.match( transitionVars, variableOrdering ) )
                {
                    return deciderInfo.pDecider;
                }
            }
        }
        THROW_RTE(
            "Failed to resolve decider for non singular transition: " << Concrete::printContextFullType( m_pContext ) );
    }

    std::optional< Concrete::Decider* > chooseDecider( VariableVector::const_iterator iBegin,
                                                       VariableVector::const_iterator iEnd,
                                                       std::vector< U32 >&            variableOrdering ) const
    {
        auto total = std::distance( iBegin, iEnd );
        for( const DeciderInfo& deciderInfo : m_deciders )
        {
            auto size = deciderInfo.variables.size();
            if( total >= size )
            {
                VariableOrdering transitionVars( iBegin, iBegin + size );
                if( deciderInfo.match( transitionVars, variableOrdering ) )
                {
                    return deciderInfo.pDecider;
                }
            }
        }
        THROW_RTE(
            "Failed to resolve decider for non singular transition: " << Concrete::printContextFullType( m_pContext ) );
    }
};

VariableVector calculateRemainingDecideableVariables( Concrete::Context* pContext, const State::Vector& truthTable,
                                                      const VariableVector& trueVars, const VariableVector& falseVars,
                                                      const VariableVector& remainingVarsSorted )
{
    // find ALL compatible states given the current true and false variables
    std::vector< State::Vector::const_iterator > compatibleStates;
    for( auto i = truthTable.begin(), iEnd = truthTable.end(); i != iEnd; ++i )
    {
        const State& state = *i;
        if( state.match( trueVars, falseVars ) )
        {
            compatibleStates.push_back( i );
        }
    }

    VERIFY_RTE_MSG(
        !compatibleStates.empty(),
        "Failed to find compatible truth table states for transition: " << Concrete::printContextFullType( pContext ) );

    // determine all remaining variables that CAN be both true AND false
    VariableVector remainingDecideableVars;
    {
        VariableVector canBeTrueVars, canBeFalseVars;

        for( auto iter : compatibleStates )
        {
            const State& state = *iter;
            {
                VariableVector canBeTrueVarsTemp;
                std::set_intersection( state.true_vars.begin(), state.true_vars.end(), remainingVarsSorted.begin(),
                                       remainingVarsSorted.end(), std::back_inserter( canBeTrueVarsTemp ) );

                VariableVector temp;
                canBeTrueVars.swap( temp );
                std::set_union( canBeTrueVarsTemp.begin(), canBeTrueVarsTemp.end(), temp.begin(), temp.end(),
                                std::back_inserter( canBeTrueVars ) );
            }

            {
                VariableVector canBeFalseVarsTemp;
                std::set_intersection( state.false_vars.begin(), state.false_vars.end(), remainingVarsSorted.begin(),
                                       remainingVarsSorted.end(), std::back_inserter( canBeFalseVarsTemp ) );

                VariableVector temp;
                canBeFalseVars.swap( temp );
                std::set_union( canBeFalseVarsTemp.begin(), canBeFalseVarsTemp.end(), temp.begin(), temp.end(),
                                std::back_inserter( canBeFalseVars ) );
            }
        }

        // find all variables where BOTH values occur
        std::set_intersection( canBeTrueVars.begin(), canBeTrueVars.end(), canBeFalseVars.begin(), canBeFalseVars.end(),
                               std::back_inserter( remainingDecideableVars ) );
    }
    return remainingDecideableVars;
}

Instance calculateInstanceMultiplier( Concrete::State* pCommonAncestor, Automata::Vertex* pVariable )
{
    Instance multipler = 1;

    Concrete::Context* pIter = pVariable->get_context();

    while( pIter != pCommonAncestor )
    {
        multipler = multipler * pIter->get_local_size();
        pIter     = db_cast< Concrete::Context >( pIter->get_parent() );
        VERIFY_RTE( pIter );
    }

    return multipler;
}

Decision::Assignments* buildAssignments( Database& database, Concrete::State* pCommonAncestor,
                                         const VariableVector& assignment, const VariableVector& trueVars,
                                         const VariableVector& falseVars )
{
    std::vector< Decision::Assignment* > assignments;
    {
        std::unordered_set< Automata::Vertex* > variables;
        for( auto pTrueVar : trueVars )
        {
            const Instance multiplier = calculateInstanceMultiplier( pCommonAncestor, pTrueVar );

            auto pAssignment = database.construct< Decision::Assignment >(
                Decision::Assignment::Args{ true, pTrueVar, multiplier } );
            assignments.push_back( pAssignment );
            variables.insert( pTrueVar );

            for( auto pSibling : pTrueVar->get_siblings() )
            {
                VERIFY_RTE( !variables.contains( pSibling ) );
                auto pAssignment = database.construct< Decision::Assignment >(
                    Decision::Assignment::Args{ false, pSibling, multiplier } );
                assignments.push_back( pAssignment );
                variables.insert( pSibling );
            }
        }

        for( auto pFalseVar : falseVars )
        {
            if( !variables.contains( pFalseVar ) )
            {
                Instance multiplier  = calculateInstanceMultiplier( pCommonAncestor, pFalseVar );
                auto     pAssignment = database.construct< Decision::Assignment >(
                    Decision::Assignment::Args{ false, pFalseVar, multiplier } );
                assignments.push_back( pAssignment );
                variables.insert( pFalseVar );
            }
        }
    }

    auto pAssignments = database.construct< Decision::Assignments >( Decision::Assignments::Args{
        Decision::Step::Args{ std::nullopt, assignment, trueVars, falseVars, {} }, assignments } );

    return pAssignments;
}

Decision::Step* buildRecurse( Database& database, Concrete::State* pCommonAncestor, Concrete::Context* pContext,
                              const DeciderSelector& deciderSelector, const State::Vector& truthTable,
                              VariableVector assignment, VariableVector trueVars, VariableVector falseVars,
                              VariableVector remainingDecideableVars )
{
    std::vector< U32 >                  variableOrdering;
    std::optional< Concrete::Decider* > deciderOpt = deciderSelector.chooseDecider(
        remainingDecideableVars.begin(), remainingDecideableVars.end(), variableOrdering );

    VERIFY_RTE( variableOrdering.size() );
    if( variableOrdering.size() == 1 )
    {
        auto pBooleanVar = remainingDecideableVars.front();
        // Boolean
        Decision::Boolean* pBoolean = database.construct< Decision::Boolean >( Decision::Boolean::Args{
            Decision::Step::Args{ deciderOpt, assignment, trueVars, falseVars, {} }, pBooleanVar } );

        // true
        {
            VariableVector newTrueVars = trueVars, newFalseVars = falseVars;
            newTrueVars.push_back( pBooleanVar );
            std::sort( newTrueVars.begin(), newTrueVars.end() );
            VariableVector newRemainingDecideableVars = calculateRemainingDecideableVariables(
                pContext, truthTable, newTrueVars, newFalseVars, remainingDecideableVars );

            VariableVector newAssignment{ pBooleanVar };

            if( !newRemainingDecideableVars.empty() )
            {
                auto pStep = buildRecurse( database, pCommonAncestor, pContext, deciderSelector, truthTable,
                                           newAssignment, newTrueVars, newFalseVars, newRemainingDecideableVars );
                VERIFY_RTE( pStep );
                pBoolean->push_back_children( pStep );
            }
            else
            {
                pBoolean->push_back_children(
                    buildAssignments( database, pCommonAncestor, newAssignment, newTrueVars, newFalseVars ) );
            }
        }
        // false
        {
            VariableVector newTrueVars = trueVars, newFalseVars = falseVars;
            newFalseVars.push_back( pBooleanVar );
            std::sort( newFalseVars.begin(), newFalseVars.end() );
            VariableVector newRemainingDecideableVars = calculateRemainingDecideableVariables(
                pContext, truthTable, newTrueVars, newFalseVars, remainingDecideableVars );

            VariableVector newAssignment{};

            if( !newRemainingDecideableVars.empty() )
            {
                auto pStep = buildRecurse( database, pCommonAncestor, pContext, deciderSelector, truthTable,
                                           newAssignment, newTrueVars, newFalseVars, newRemainingDecideableVars );
                VERIFY_RTE( pStep );
                pBoolean->push_back_children( pStep );
            }
            else
            {
                pBoolean->push_back_children(
                    buildAssignments( database, pCommonAncestor, newAssignment, newTrueVars, newFalseVars ) );
            }
        }

        return pBoolean;
    }
    else
    {
        // selection
        Decision::Selection* pSelection = database.construct< Decision::Selection >( Decision::Selection::Args{
            Decision::Step::Args{ deciderOpt, assignment, trueVars, falseVars, {} },
            { remainingDecideableVars.begin(), remainingDecideableVars.begin() + variableOrdering.size() },
            variableOrdering } );

        for( int i = 0; i != variableOrdering.size(); ++i )
        {
            VariableVector newAssignment, newTrueVars = trueVars, newFalseVars = falseVars;
            {
                for( int j = 0; j != variableOrdering.size(); ++j )
                {
                    if( i == j )
                    {
                        newAssignment.push_back( remainingDecideableVars[ j ] );
                        newTrueVars.push_back( remainingDecideableVars[ j ] );
                    }
                    else
                    {
                        newFalseVars.push_back( remainingDecideableVars[ j ] );
                    }
                }
                std::sort( newTrueVars.begin(), newTrueVars.end() );
                std::sort( newFalseVars.begin(), newFalseVars.end() );
            }

            VariableVector newRemainingDecideableVars = calculateRemainingDecideableVariables(
                pContext, truthTable, newTrueVars, newFalseVars, remainingDecideableVars );
            if( !newRemainingDecideableVars.empty() )
            {
                auto pStep = buildRecurse( database, pCommonAncestor, pContext, deciderSelector, truthTable,
                                           newAssignment, newTrueVars, newFalseVars, newRemainingDecideableVars );
                VERIFY_RTE( pStep );
                pSelection->push_back_children( pStep );
            }
            else
            {
                pSelection->push_back_children(
                    buildAssignments( database, pCommonAncestor, newAssignment, newTrueVars, newFalseVars ) );
            }
        }

        return pSelection;
    }
}

Decision::Step* buildDecisionProcedure( Database&                                database,
                                        const std::vector< Concrete::Decider* >& deciders,
                                        Concrete::Context*                       pContext,
                                        Concrete::State*                         pCommonAncestor,
                                        const std::vector< VariableVector >&     transitionVariables )
{
    DeciderSelector deciderSelector( pContext, deciders );

    VariableVector variablesSorted;
    {
        Collector::collectVariables( pCommonAncestor->get_automata_vertex(), variablesSorted );
        std::sort( variablesSorted.begin(), variablesSorted.end() );
    }

    State::Vector truthTable
        = fromTruthTable( TruthTableSolver::solve( pCommonAncestor->get_automata_vertex() ), variablesSorted );

    VariableVector transitionSelectionVariables, transitionVarsSorted;
    {
        for( const VariableVector& vars : transitionVariables )
        {
            VERIFY_RTE( vars.size() == 1 );
            transitionSelectionVariables.push_back( vars.front() );
        }
        transitionVarsSorted = transitionSelectionVariables;
        std::sort( transitionVarsSorted.begin(), transitionVarsSorted.end() );
    }

    VariableVector remainingVarsSorted;
    {
        std::set_difference( variablesSorted.begin(), variablesSorted.end(), transitionVarsSorted.begin(),
                             transitionVarsSorted.end(), std::back_inserter( remainingVarsSorted ) );
    }

    std::vector< U32 >                  variableOrdering;
    std::optional< Concrete::Decider* > decider
        = deciderSelector.chooseTransitionDecider( transitionVariables, variableOrdering );

    Decision::Selection* pSelection = database.construct< Decision::Selection >( Decision::Selection::Args{
        Decision::Step::Args{ decider, {}, {}, {}, {} }, transitionSelectionVariables, variableOrdering } );

    // enumerate variable assignments for each selection
    for( int i = 0; i != transitionVariables.size(); ++i )
    {
        VariableVector trueVerts, falseVerts;
        {
            for( int j = 0; j != transitionVariables.size(); ++j )
            {
                const auto& verts = transitionVariables[ j ];
                if( i == j )
                {
                    trueVerts = verts;
                }
                else
                {
                    std::copy( verts.begin(), verts.end(), std::back_inserter( falseVerts ) );
                }
            }
            std::sort( trueVerts.begin(), trueVerts.end() );
            std::sort( falseVerts.begin(), falseVerts.end() );
        }
        // initially trueVerts IS the assignment

        VariableVector remainingDecideableVariables
            = calculateRemainingDecideableVariables( pContext, truthTable, trueVerts, falseVerts, remainingVarsSorted );
        if( !remainingDecideableVariables.empty() )
        {
            auto pStep = buildRecurse( database, pCommonAncestor, pContext, deciderSelector, truthTable, trueVerts,
                                       trueVerts, falseVerts, remainingDecideableVariables );
            VERIFY_RTE( pStep );
            pSelection->push_back_children( pStep );
        }
        else
        {
            pSelection->push_back_children(
                buildAssignments( database, pCommonAncestor, trueVerts, trueVerts, falseVerts ) );
        }
    }

    return pSelection;
}

Decision::DecisionProcedure* compileDecision( Database& database, Concrete::Context* pContext,
                                              const StateVectorVector& transitionStates )
{
    // determine the Concrete::State associated with the pContext
    Concrete::State* pContextState = nullptr;
    {
        for( auto pIter = pContext; pIter; pIter = db_cast< Concrete::Context >( pIter->get_parent() ) )
        {
            if( ( pContextState = db_cast< Concrete::State >( pIter ) ) )
            {
                break;
            }
        }
        VERIFY_RTE_MSG( pContextState, "Failed to determine state assocaited with transition context : "
                                           << Concrete::printContextFullType( pContext ) );
    }

    auto opt = pContext->get_concrete_object();
    VERIFY_RTE( opt.has_value() );
    Concrete::Object* pObject = opt.value();

    // const TruthTable                        truthTable = pObject->get_truth_table();
    // const std::vector< Automata::Vertex* >  variables  = pObject->get_variable_vertices();
    const std::vector< Concrete::Decider* > deciders = pObject->get_deciders();

    Concrete::State* pCommonAncestor = findCommonAncestor( pContextState, transitionStates );
    pCommonAncestor                  = findCommonAncestorContextParent( pContext, pCommonAncestor );
    VERIFY_RTE_MSG( pCommonAncestor, "Failed to determine common ancestor for decision: "
                                         << Concrete::printContextFullType( pContext ) );

    // calculate instance divider to common ancestor
    Instance instanceDivider = 1;
    {
        for( auto pIter = pContext; pIter != pCommonAncestor;
             pIter      = db_cast< Concrete::Context >( pIter->get_parent() ) )
        {
            VERIFY_RTE( pIter );
            instanceDivider = instanceDivider * pIter->get_local_size();
        }
    }

    // calculate the variable sequence for the transition
    std::vector< VariableVector > variableSequence = Collector::statesToUniqueVariables( transitionStates );

    Decision::Step* pRoot = buildDecisionProcedure( database, deciders, pContext, pCommonAncestor, variableSequence );

    Decision::DecisionProcedure* pProcedure = database.construct< Decision::DecisionProcedure >(
        Decision::DecisionProcedure::Args{ pCommonAncestor, instanceDivider, pRoot } );

    return pProcedure;
}

} // namespace

void compileDecisions( Database& database, const mega::io::megaFilePath& sourceFile )
{
    // find all deciders in object and construct object truth table
    for( Concrete::Object* pObject : database.many< Concrete::Object >( sourceFile ) )
    {
        DeciderVector deciders;
        findDeciders( pObject, deciders );

        std::vector< Automata::TruthAssignment* > truthTable;
        {
            for( const auto& truthAssignments : TruthTableSolver::solve( pObject->get_automata_root() ) )
            {
                auto pTruthAssignment = database.construct< Automata::TruthAssignment >(
                    Automata::TruthAssignment::Args{ truthAssignments } );
                truthTable.push_back( pTruthAssignment );
            };
        }

        std::vector< Automata::Vertex* > variables;
        Collector::collectVariables( pObject->get_automata_root(), variables );

        database.construct< Concrete::Object >( Concrete::Object::Args{ pObject, deciders, truthTable, variables } );
    }

    // determine decision procedures for each transition
    for( Concrete::Interupt* pInterupt : database.many< Concrete::Interupt >( sourceFile ) )
    {
        auto transitions = pInterupt->get_transition();
        if( !transitions.empty() )
        {
            Decision::DecisionProcedure* pProcedure
                = compileDecision( database, pInterupt, Collector::collectDerivationStates( pInterupt, transitions ) );
            pInterupt->set_transition_decision( std::optional< Decision::DecisionProcedure* >( pProcedure ) );
        }
        else
        {
            pInterupt->set_transition_decision( std::nullopt );
        }
    }
    for( Concrete::State* pState : database.many< Concrete::State >( sourceFile ) )
    {
        auto transitions = pState->get_transition();
        if( !transitions.empty() )
        {
            Decision::DecisionProcedure* pProcedure
                = compileDecision( database, pState, Collector::collectDerivationStates( pState, transitions ) );
            pState->set_transition_decision( std::optional< Decision::DecisionProcedure* >( pProcedure ) );
        }
        else
        {
            pState->set_transition_decision( std::nullopt );
        }
    }
}

} // namespace mega::invocation
