
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

#include "database/ValueSpaceStage.hxx"

#include "database/sources.hpp"

#include "mega/make_unique_without_reorder.hpp"
#include "mega/common_strings.hpp"

#include "mega/values/compilation/source_location.hpp"
#include "mega/values/compilation/hyper_graph.hpp"

#include <common/stash.hpp>

#include <vector>
#include <sstream>
#include <algorithm>
#include <unordered_set>

namespace ValueSpaceStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/common_ancestor.hpp"
} // namespace ValueSpaceStage

using namespace ValueSpaceStage;

namespace mega::compiler
{
class Task_ValueSpace : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_ValueSpace( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

private:
    using DeciderVector = std::vector< Concrete::Decider* >;

    void findDeciders( Concrete::Context* pContext, DeciderVector& deciders )
    {
        using namespace ValueSpaceStage;

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
                        "Failed to find common ancestor between: " << Concrete::printContextFullType(
                            pState ) << " and: " << Concrete::printContextFullType( pAncestorVertex ) );
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

        while( pCommonAncestor )
        {
            if( db_cast< Automata::Or >( pCommonAncestor->get_automata_vertex() ) )
            {
                break;
            }
            pCommonAncestor = db_cast< Concrete::State >( pCommonAncestor->get_parent() );
        }

        return pCommonAncestor;
    }

    struct TruthAssignment
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
    };

    TruthAssignment::VectorVector solveSatisfiability( Automata::Vertex* pVertex )
    {
        TruthAssignment::VectorVector result;

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
                    for( const auto& t : TruthAssignment::add( pChild, solveSatisfiability( pChild ) ) )
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
                        result = solveSatisfiability( pChild );
                    }
                    else
                    {
                        result = TruthAssignment::multiply( result, solveSatisfiability( pChild ) );
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

    void collectVariables( Automata::Vertex* pVertex, std::vector< Automata::Vertex* >& variables )
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

    void collectTargets( Derivation::Step* pStep, std::vector< Concrete::Graph::Vertex* >& targets )
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

    std::vector< Concrete::State* > collectTargetStates( Concrete::Context* pContext, Derivation::Root* pDerivation )
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

    StateVectorVector collectDerivationStates( Concrete::Context*                      pContext,
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

    using VariableVector = std::vector< Automata::Vertex* >;
    using TruthTable     = std::vector< Automata::TruthAssignment* >;

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
    State::Vector fromTruthTable( const TruthAssignment::VectorVector& truthTable,
                                  const VariableVector&                variablesSorted )
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

    Decision::Step* buildDecisionProcedure( Database& database, Concrete::Context* pContext,
                                            Concrete::State*                     pCommonAncestor,
                                            const std::vector< VariableVector >& transitionVariables )
    {
        VariableVector variablesSorted;
        {
            collectVariables( pCommonAncestor->get_automata_vertex(), variablesSorted );
            std::sort( variablesSorted.begin(), variablesSorted.end() );
        }

        State::Vector truthTable
            = fromTruthTable( solveSatisfiability( pCommonAncestor->get_automata_vertex() ), variablesSorted );

        // std::vector< Decision::BDDVertex* > frontier;
        std::vector< Decision::VariableSet* > transitionSelectionVariables;
        VariableVector                        transitionVarsSorted;
        {
            for( const VariableVector& vars : transitionVariables )
            {
                auto pVariableSet = database.construct< Decision::VariableSet >( Decision::VariableSet::Args{ vars } );
                transitionSelectionVariables.push_back( pVariableSet );
                std::copy( vars.begin(), vars.end(), std::back_inserter( transitionVarsSorted ) );
            }
            std::sort( transitionVarsSorted.begin(), transitionVarsSorted.end() );
        }

        VariableVector remainingVarsSorted;
        {
            std::set_difference( variablesSorted.begin(), variablesSorted.end(), transitionVarsSorted.begin(),
                                 transitionVarsSorted.end(), std::back_inserter( remainingVarsSorted ) );
        }

        Concrete::Decider* pDecider = nullptr;

        std::vector< U32 > variableOrdering;

        Decision::Selection* pSelection = database.construct< Decision::Selection >( Decision::Selection::Args{
            Decision::Step::Args{ pDecider, {} }, transitionSelectionVariables, variableOrdering } );

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

            std::vector< State::Vector::const_iterator > compatibleStates;
            for( auto i = truthTable.begin(), iEnd = truthTable.end(); i != iEnd; ++i )
            {
                const State& state = *i;
                if( state.match( trueVerts, falseVerts ) )
                {
                    compatibleStates.push_back( i );
                }
            }

            VERIFY_RTE_MSG( !compatibleStates.empty(),
                            "Failed to find compatible truth table states for transition: "
                                << Concrete::printContextFullType( pContext ) );

            // determine all remaining variables that CAN be both true AND false
            VariableVector remainingDecideableVars;
            {
                VariableVector canBeTrueVars, canBeFalseVars;

                for( auto iter : compatibleStates )
                {
                    const State& state = *iter;
                    {
                        VariableVector canBeTrueVarsTemp;
                        std::set_intersection( state.true_vars.begin(), state.true_vars.end(),
                                               remainingVarsSorted.begin(), remainingVarsSorted.end(),
                                               std::back_inserter( canBeTrueVarsTemp ) );

                        VariableVector temp;
                        canBeTrueVars.swap( temp );
                        std::set_union( canBeTrueVarsTemp.begin(), canBeTrueVarsTemp.end(), temp.begin(), temp.end(),
                                        std::back_inserter( canBeTrueVars ) );
                    }

                    {
                        VariableVector canBeFalseVarsTemp;
                        std::set_intersection( state.false_vars.begin(), state.false_vars.end(),
                                               remainingVarsSorted.begin(), remainingVarsSorted.end(),
                                               std::back_inserter( canBeFalseVarsTemp ) );

                        VariableVector temp;
                        canBeFalseVars.swap( temp );
                        std::set_union( canBeFalseVarsTemp.begin(), canBeFalseVarsTemp.end(), temp.begin(), temp.end(),
                                        std::back_inserter( canBeFalseVars ) );
                    }
                }

                // find all variables where BOTH values occur
                std::set_intersection( canBeTrueVars.begin(), canBeTrueVars.end(), canBeFalseVars.begin(),
                                       canBeFalseVars.end(), std::back_inserter( remainingDecideableVars ) );
            }

            // solve decider calls for remainingDecideableVars
        }

        return pSelection;
    }

    Decision::DecisionProcedure* compileTransition( Database& database, Concrete::Context* pContext,
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

        // calculate the variable sequence for the transition
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

        Decision::Step* pRoot = buildDecisionProcedure( database, pContext, pCommonAncestor, variableSequence );

        Decision::DecisionProcedure* pProcedure = database.construct< Decision::DecisionProcedure >(
            Decision::DecisionProcedure::Args{ pCommonAncestor, pRoot } );

        return pProcedure;
    }

public:
    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.ValueSpaceStage_ValueSpace( m_sourceFilePath );
        start( taskProgress, "Task_ValueSpace", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.InterfacePCH( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.AutomataStage_AutomataAnalysis( m_sourceFilePath ) ) } );

        if( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_sourceFilePath );

        // find all deciders in object and construct object truth table
        for( Concrete::Object* pObject : database.many< Concrete::Object >( m_sourceFilePath ) )
        {
            DeciderVector deciders;
            findDeciders( pObject, deciders );

            std::vector< Automata::TruthAssignment* > truthTable;
            {
                for( const auto& truthAssignments : solveSatisfiability( pObject->get_automata_root() ) )
                {
                    auto pTruthAssignment = database.construct< Automata::TruthAssignment >(
                        Automata::TruthAssignment::Args{ truthAssignments } );
                    truthTable.push_back( pTruthAssignment );
                };
            }

            std::vector< Automata::Vertex* > variables;
            collectVariables( pObject->get_automata_root(), variables );

            database.construct< Concrete::Object >(
                Concrete::Object::Args{ pObject, deciders, truthTable, variables } );
        }

        // determine decision procedures for each transition
        for( Concrete::Interupt* pInterupt : database.many< Concrete::Interupt >( m_sourceFilePath ) )
        {
            auto transitions = pInterupt->get_transition();
            if( !transitions.empty() )
            {
                Decision::DecisionProcedure* pProcedure
                    = compileTransition( database, pInterupt, collectDerivationStates( pInterupt, transitions ) );
                database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, pProcedure } );
            }
            else
            {
                database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, std::nullopt } );
            }
        }
        for( Concrete::State* pState : database.many< Concrete::State >( m_sourceFilePath ) )
        {
            auto transitions = pState->get_transition();
            if( !transitions.empty() )
            {
                Decision::DecisionProcedure* pProcedure
                    = compileTransition( database, pState, collectDerivationStates( pState, transitions ) );
                database.construct< Concrete::State >( Concrete::State::Args{ pState, pProcedure } );
            }
            else
            {
                database.construct< Concrete::State >( Concrete::State::Args{ pState, std::nullopt } );
            }
        }

        const task::FileHash fileHashCode = database.save_ValueSpace_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_ValueSpace( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ValueSpace >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
