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

#include "base_task.hpp"

#include "database/DecisionsStage.hxx"

#include "compiler/clang_compilation.hpp"

#include "mega/common_strings.hpp"

#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/runtime/pointer.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <memory>

namespace DecisionsStage
{
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/interface_visitor.hpp"
#include "compiler/common_ancestor.hpp"
#include "compiler/concrete.hpp"
#include "compiler/concrete_printer.hpp"

namespace ClangTraits
{
#include "compiler/derivation_printer.hpp"
} // namespace ClangTraits

} // namespace DecisionsStage

namespace mega::compiler
{

using namespace DecisionsStage;

namespace
{
using DeciderVector     = std::vector< Concrete::Decider* >;
using StateVector       = std::vector< Concrete::State* >;
using StateVectorVector = std::vector< StateVector >;
// using StateSet          = std::unordered_set< Concrete::State* >;

Concrete::State* findCommonAncestor( Concrete::State* pState, Concrete::State* pAncestor )
{
    VERIFY_RTE( pState && pAncestor );

    CommonAncestor::GraphEdgeVector edges;
    auto pAncestorVertex = CommonAncestor::commonRootDerivation( pState, pAncestor, edges, true );
    VERIFY_RTE_MSG( pAncestorVertex,
                    "Failed to find common ancestor between: " << Concrete::fullTypeName( pState ) << " and: "
                                                               << Concrete::fullTypeName( pAncestorVertex ) );
    pAncestor = db_cast< Concrete::State >( pAncestorVertex );
    VERIFY_RTE_MSG( pAncestor,
                    "Failed to find common ancestor state between: " << Concrete::fullTypeName( pState ) << " and: "
                                                                     << Concrete::fullTypeName( pAncestor ) );
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
            if( db_cast< Automata::Or >( pVertex ) )
            {
                for( auto pChild : children )
                {
                    for( const auto& t : add( pChild, solve( pChild ) ) )
                    {
                        result.push_back( t );
                    }
                }
            }
            else if( db_cast< Automata::And >( pVertex ) )
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
        if( db_cast< Automata::Or >( pVertex ) )
        {
            for( auto pChild : pVertex->get_children() )
            {
                variables.push_back( pChild );
                collectVariables( pChild, variables );
            }
        }
        else if( db_cast< Automata::And >( pVertex ) )
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

    static void collectTargets( ClangTraits::Derivation::Step* pStep, std::vector< Concrete::Node* >& targets )
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

    static std::vector< Concrete::State* > collectTargetStates( Concrete::Context*             pContext,
                                                                ClangTraits::Derivation::Root* pDerivation )
    {
        std::vector< Concrete::State* > states;
        {
            std::vector< Concrete::Node* > targets;
            for( auto pEdge : pDerivation->get_edges() )
            {
                if( !pEdge->get_eliminated() )
                {
                    collectTargets( pEdge->get_next(), targets );
                }
            }
            VERIFY_RTE_MSG(
                !targets.empty(), "No derivation targets for derivation: " << Concrete::fullTypeName( pContext ) );
            VERIFY_RTE_MSG( targets.size() == 1U,
                            "Non-singular derivation targets for derivation: " << Concrete::fullTypeName(
                                pContext ) << "\n" << ClangTraits::printDerivationStep( pDerivation, false ) );
            for( auto pVertex : targets )
            {
                if( auto pState = db_cast< Concrete::State >( pVertex ) )
                {
                    states.push_back( pState );
                }
                else
                {
                    THROW_RTE( "Transition to non-state context: " << Concrete::fullTypeName( pContext ) );
                }
            }
            states = make_unique_without_reorder( states );
        }
        return states;
    }

    static StateVectorVector collectDerivationStates( Concrete::Context*                                   pContext,
                                                      const std::vector< ClangTraits::Derivation::Root* >& transitions )
    {
        StateVectorVector transitionStates;

        for( auto pDerivation : transitions )
        {
            std::vector< Concrete::State* > states = collectTargetStates( pContext, pDerivation );
            VERIFY_RTE_MSG( !states.empty(), "Derivation has empty target: " << Concrete::fullTypeName( pContext ) );
            transitionStates.push_back( states );
        }

        VERIFY_RTE_MSG(
            !transitionStates.empty(), "Derivation has no successors: " << Concrete::fullTypeName( pContext ) );

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
using VarSet     = std::unordered_set< Automata::Vertex* >;

class State
{
    // sorted vectors using memory address
    VariableVector true_vars, false_vars;

public:
    using Vector = std::vector< State >;

    State( const VariableVector& vertices, const VariableVector& variablesSorted )
        : true_vars( vertices )
    {
        VERIFY_RTE( !true_vars.empty() );
        std::sort( true_vars.begin(), true_vars.end() );
        std::set_difference( variablesSorted.begin(), variablesSorted.end(), true_vars.begin(), true_vars.end(),
                             std::back_inserter( false_vars ) );
    }

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

    VariableVector getCanBeTrue( const VariableVector& remainingVarsSorted ) const
    {
        VariableVector canBeTrueVarsTemp;
        std::set_intersection( true_vars.begin(), true_vars.end(), remainingVarsSorted.begin(),
                               remainingVarsSorted.end(), std::back_inserter( canBeTrueVarsTemp ) );
        return canBeTrueVarsTemp;
    }

    VariableVector getCanBeFalse( const VariableVector& remainingVarsSorted ) const
    {
        VariableVector canBeFalseVarsTemp;
        std::set_intersection( false_vars.begin(), false_vars.end(), remainingVarsSorted.begin(),
                               remainingVarsSorted.end(), std::back_inserter( canBeFalseVarsTemp ) );
        return canBeFalseVarsTemp;
    }
};

State::Vector fromTruthTable( const TruthTableSolver::VectorVector& truthTable, const VariableVector& variablesSorted )
{
    State::Vector result;
    for( const auto& row : truthTable )
    {
        result.emplace_back( row, variablesSorted );
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
                const auto total = static_cast< int >( variables.variables.size() );
                variableOrdering.resize( total );
                auto i    = variables.variablesSortedPos.begin();
                auto iEnd = variables.variablesSortedPos.end();
                auto j    = cmp.variablesSortedPos.begin();

                for( ; i != iEnd; ++i, ++j )
                {
                    const auto iDecidPos = i->second;
                    const auto iTransPos = j->second;

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
            auto states    = Collector::collectDerivationStates( pDecider, pDecider->get_events() );
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
        THROW_RTE( "Failed to resolve decider for non singular transition: " << Concrete::fullTypeName( m_pContext ) );
    }

    std::optional< Concrete::Decider* > chooseDecider( VariableVector::const_iterator iBegin,
                                                       VariableVector::const_iterator iEnd,
                                                       std::vector< U32 >&            variableOrdering ) const
    {
        auto total = std::distance( iBegin, iEnd );
        for( const DeciderInfo& deciderInfo : m_deciders )
        {
            const auto size = static_cast< int >( deciderInfo.variables.size() );
            if( total >= size )
            {
                VariableOrdering transitionVars( iBegin, iBegin + size );
                if( deciderInfo.match( transitionVars, variableOrdering ) )
                {
                    return deciderInfo.pDecider;
                }
            }
        }
        THROW_RTE( "Failed to resolve decider for non singular transition: " << Concrete::getKind( m_pContext ) << " "
                                                                             << Concrete::fullTypeName( m_pContext ) );
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

    VERIFY_RTE_MSG( !compatibleStates.empty(),
                    "Failed to find compatible truth table states for transition: "
                        << Concrete::getKind( pContext ) << " " << Concrete::fullTypeName( pContext ) );

    // determine all remaining variables that CAN be both true AND false
    VariableVector remainingDecideableVars;
    {
        VariableVector canBeTrueVars, canBeFalseVars;

        for( auto iter : compatibleStates )
        {
            const State& state = *iter;
            {
                const VariableVector canBeTrueVarsTemp = state.getCanBeTrue( remainingVarsSorted );

                VariableVector temp;
                canBeTrueVars.swap( temp );
                std::set_union( canBeTrueVarsTemp.begin(), canBeTrueVarsTemp.end(), temp.begin(), temp.end(),
                                std::back_inserter( canBeTrueVars ) );
            }

            {
                const VariableVector canBeFalseVarsTemp = state.getCanBeFalse( remainingVarsSorted );

                VariableVector temp;
                canBeFalseVars.swap( temp );
                std::set_union( canBeFalseVarsTemp.begin(), canBeFalseVarsTemp.end(), temp.begin(), temp.end(),
                                std::back_inserter( canBeFalseVars ) );
            }
        }

        // find all variables where BOTH values occur
        std::sort( canBeTrueVars.begin(), canBeTrueVars.end() );
        std::sort( canBeFalseVars.begin(), canBeFalseVars.end() );
        std::set_intersection( canBeTrueVars.begin(), canBeTrueVars.end(), canBeFalseVars.begin(), canBeFalseVars.end(),
                               std::back_inserter( remainingDecideableVars ) );
    }
    return remainingDecideableVars;
}

concrete::Instance::ValueType calculateInstanceMultiplier( Concrete::State*  pCommonAncestor,
                                                           Automata::Vertex* pVariable )
{
    concrete::Instance::ValueType multipler{ 1 };

    Concrete::Context* pIter = pVariable->get_context();

    while( pIter != pCommonAncestor )
    {
        multipler = multipler * pIter->get_icontext()->get_size();
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
            const concrete::Instance multiplier( calculateInstanceMultiplier( pCommonAncestor, pTrueVar ) );

            auto pAssignment = database.construct< Decision::Assignment >(
                Decision::Assignment::Args{ true, pTrueVar, multiplier } );
            assignments.push_back( pAssignment );
            variables.insert( pTrueVar );

            for( auto pSibling : pTrueVar->get_siblings() )
            {
                VERIFY_RTE( !variables.contains( pSibling ) );
                auto pNewAssignment = database.construct< Decision::Assignment >(
                    Decision::Assignment::Args{ false, pSibling, multiplier } );
                assignments.push_back( pNewAssignment );
                variables.insert( pSibling );
            }
        }

        for( auto pFalseVar : falseVars )
        {
            if( !variables.contains( pFalseVar ) )
            {
                concrete::Instance multiplier( calculateInstanceMultiplier( pCommonAncestor, pFalseVar ) );
                auto               pNewAssignment = database.construct< Decision::Assignment >(
                    Decision::Assignment::Args{ false, pFalseVar, multiplier } );
                assignments.push_back( pNewAssignment );
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
            std::sort( newFalseVars.begin(), newFalseVars.end() );

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

            std::sort( newTrueVars.begin(), newTrueVars.end() );
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

        for( std::size_t i = 0U; i != variableOrdering.size(); ++i )
        {
            VariableVector newAssignment, newTrueVars = trueVars, newFalseVars = falseVars;
            {
                for( std::size_t j = 0U; j != variableOrdering.size(); ++j )
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
    for( std::size_t i = 0U; i != transitionVariables.size(); ++i )
    {
        VariableVector trueVerts, falseVerts;
        {
            for( std::size_t j = 0U; j != transitionVariables.size(); ++j )
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
                                           << Concrete::fullTypeName( pContext ) );
    }

    Concrete::Object*                       pObject  = pContext->get_parent_object();
    const std::vector< Concrete::Decider* > deciders = pObject->get_deciders();

    Concrete::State* pCommonAncestor = findCommonAncestor( pContextState, transitionStates );
    pCommonAncestor                  = findCommonAncestorContextParent( pContext, pCommonAncestor );
    VERIFY_RTE_MSG(
        pCommonAncestor, "Failed to determine common ancestor for decision: " << Concrete::fullTypeName( pContext ) );

    // calculate instance divider to common ancestor
    concrete::Instance::ValueType instanceDivider = 1;
    {
        for( auto pIter = pContext; pIter != pCommonAncestor;
             pIter      = db_cast< Concrete::Context >( pIter->get_parent() ) )
        {
            VERIFY_RTE( pIter );
            instanceDivider = instanceDivider * pIter->get_icontext()->get_size();
        }
    }

    // calculate the variable sequence for the transition
    std::vector< VariableVector > variableSequence = Collector::statesToUniqueVariables( transitionStates );

    Decision::Step* pRoot = buildDecisionProcedure( database, deciders, pContext, pCommonAncestor, variableSequence );

    Decision::DecisionProcedure* pProcedure = database.construct< Decision::DecisionProcedure >(
        Decision::DecisionProcedure::Args{ pCommonAncestor, concrete::Instance( instanceDivider ), pRoot } );

    return pProcedure;
}
} // namespace

class Task_Decisions : public BaseTask
{
    const mega::io::manifestFilePath m_manifestFilePath;

public:
    Task_Decisions( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
        , m_manifestFilePath( m_environment.project_manifest() )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath decisionsDBFile
            = m_environment.DecisionsStage_Decisions( m_manifestFilePath );
        start( taskProgress, "Task_Decisions", m_manifestFilePath.path(), decisionsDBFile.path() );

        task::DeterminantHash determinant(
            m_toolChain.toolChainHash,
            m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( m_manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.HyperGraphAnalysis_Model( m_manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.ClangTraitsStage_Traits( m_manifestFilePath ) )

        );

        for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if( m_environment.restore( decisionsDBFile, determinant ) )
        {
            m_environment.setBuildHashCode( decisionsDBFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_manifestFilePath );

        // find all deciders in object and construct object truth table
        for( Concrete::Object* pObject : database.many< Concrete::Object >( m_manifestFilePath ) )
        {
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

            database.construct< Concrete::Object >( Concrete::Object::Args{ pObject, truthTable, variables } );
        }

        // determine decision procedures for each transition
        for( Concrete::Interupt* pInterupt : database.many< Concrete::Interupt >( m_manifestFilePath ) )
        {
            std::optional< Decision::DecisionProcedure* > decisionProcedureOpt;
            {
                auto transitions = pInterupt->get_transitions();
                if( !transitions.empty() )
                {
                    decisionProcedureOpt = compileDecision(
                        database, pInterupt, Collector::collectDerivationStates( pInterupt, transitions ) );
                }
            }
            database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, decisionProcedureOpt } );
        }

        for( Concrete::State* pState : database.many< Concrete::State >( m_manifestFilePath ) )
        {
            std::optional< Decision::DecisionProcedure* > decisionProcedureOpt;
            {
                auto transitions = pState->get_transitions();
                if( !transitions.empty() )
                {
                    decisionProcedureOpt = compileDecision(
                        database, pState, Collector::collectDerivationStates( pState, transitions ) );
                }
            }
            database.construct< Concrete::State >( Concrete::State::Args{ pState, decisionProcedureOpt } );
        }

        auto fileHashCode = database.save_Decisions_to_temp();
        m_environment.setBuildHashCode( decisionsDBFile, fileHashCode );
        m_environment.temp_to_real( decisionsDBFile );
        m_environment.stash( decisionsDBFile, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Decisions( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_Decisions >( taskArguments );
}

} // namespace mega::compiler
