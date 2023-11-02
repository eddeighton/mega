
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
namespace
{

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
            collectTargets( pEdge->get_next(), targets );
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
            collectTargets( pEdge->get_next(), targets );
        }
        VERIFY_RTE_MSG(
            !targets.empty(), "No derivation targets for derivation: " << Concrete::printContextFullType( pContext ) );
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

} // namespace

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
    Decision::DeciderSequence* chooseDeciderSequence( Concrete::Context* pContext )
    {
        VERIFY_RTE( pContext->get_concrete_object().has_value() );
        auto pObject = pContext->get_concrete_object().value();

        Decision::DeciderSequence* pDeciderSequence = nullptr;
        {
            auto deciders = pObject->get_decider_sequences();
            VERIFY_RTE_MSG(
                deciders.size() <= 1,
                "Non singular number of decider sequencers in: " << Concrete::printContextFullType( pObject ) );
            if( !deciders.empty() )
            {
                pDeciderSequence = deciders.front();
            }
        }
        // VERIFY_RTE_MSG(
        //     pDeciderSequence, "Failed to choose decider sequence for: " << Concrete::printContextFullType( pContext )
        //     );
        return pDeciderSequence;
    }

    using StateVector       = std::vector< Concrete::State* >;
    using StateVectorVector = std::vector< StateVector >;
    using StateSet          = std::unordered_set< Concrete::State* >;

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

    StateSet fromStateVectorVector( const StateVectorVector& states )
    {
        StateSet stateSet;
        for( const auto& s : states )
        {
            for( auto pState : s )
            {
                stateSet.insert( pState );
            }
        }
        return stateSet;
    }

    using DeciderSequenceVector = std::vector< Decision::DeciderSequence* >;
    using DeciderVector         = std::vector< Concrete::Decider* >;

    void findDeciderStates( Database& database, Concrete::Context* pContext, DeciderSequenceVector& sequences,
                            DeciderVector& deciders )
    {
        using namespace ValueSpaceStage;

        for( auto pChild : pContext->get_children() )
        {
            findDeciderStates( database, pChild, sequences, deciders );
        }

        if( auto pDecider = db_cast< Concrete::Decider >( pContext ) )
        {
            deciders.push_back( pDecider );
        }
        else if( auto pState = db_cast< Concrete::State >( pContext ) )
        {
            if( !deciders.empty() )
            {
                auto pDeciderSequence = database.construct< Decision::DeciderSequence >(
                    Decision::DeciderSequence::Args{ pState, deciders } );
                sequences.push_back( pDeciderSequence );
                deciders.clear();
            }
        }
    }

    Concrete::State* findCommonAncestor( Concrete::State* pState, Concrete::State* pAncestor )
    {
        if( pAncestor == nullptr )
        {
            return pState;
        }
        else
        {
            CommonAncestor::GraphEdgeVector edges;
            auto pAncestorVertex = CommonAncestor::commonRootDerivation( pState, pAncestor, edges );
            pAncestor            = db_cast< Concrete::State >( pAncestorVertex );
            VERIFY_RTE_MSG( pAncestor,
                            "Failed to find common ancestor state between: " << Concrete::printContextFullType(
                                pState ) << " and: " << Concrete::printContextFullType( pAncestor ) );
            return pAncestor;
        }
    }

    Concrete::State* findCommonAncestor( const StateSet& transitionStatesSet )
    {
        Concrete::State* pCommonAncestor = nullptr;

        for( auto pState : transitionStatesSet )
        {
            pCommonAncestor = findCommonAncestor( pState, pCommonAncestor );
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

    bool recurseEliminatedStates( Concrete::State* pState, const StateSet& transitionStates, StateSet& decidedStates )
    {
        bool bIsStateInTransition = transitionStates.contains( pState );
        if( !bIsStateInTransition )
        {
            for( auto pChild : pState->get_children() )
            {
                if( auto pChildState = db_cast< Concrete::State >( pChild ) )
                {
                    if( recurseEliminatedStates( pChildState, transitionStates, decidedStates ) )
                    {
                        bIsStateInTransition = true;
                    }
                }
            }
            if( !bIsStateInTransition )
            {
                decidedStates.insert( pState );
            }
        }
        return bIsStateInTransition;
    }

    void recurseDecidedStates( Concrete::State* pState, const StateSet& transitionStates,
                               const StateSet& eliminatedStates, StateSet& decidedStates )
    {
        if( !transitionStates.contains( pState ) && !eliminatedStates.contains( pState ) )
        {
            if( pState->get_automata_vertex()->get_is_conditional() )
            {
                if( !pState->get_interface_state()->get_is_historical() )
                {
                    decidedStates.insert( pState );
                }
            }
        }

        for( auto pChild : pState->get_children() )
        {
            if( auto pChildState = db_cast< Concrete::State >( pChild ) )
            {
                recurseDecidedStates( pChildState, transitionStates, eliminatedStates, decidedStates );
            }
        }
    }

    void findDecidedStates( Concrete::State* pCommonAncestor, const StateSet& transitionStatesSet,
                            StateSet& eliminatedStates, StateSet& decidedStates )
    {
        // ignore the common ancestor if it is NOT in the transitionSet
        if( !transitionStatesSet.contains( pCommonAncestor ) )
        {
            eliminatedStates.insert( pCommonAncestor );
        }

        // pCommonAncestor is the common ancestor of transitionStates#
        recurseEliminatedStates( pCommonAncestor, transitionStatesSet, eliminatedStates );

        // given the pDeciderSequence
        // determine ALL decided states within the common ancestor that ARE NOT already in transitionStates
        recurseDecidedStates( pCommonAncestor, transitionStatesSet, eliminatedStates, decidedStates );
    }

    struct DeciderInvocation
    {
        using Vector = std::vector< DeciderInvocation >;

        Concrete::Decider*                          pDecider;
        std::vector< std::set< Concrete::State* > > arguments;
    };

    std::optional< DeciderInvocation > matchDecider( StateVectorVector::const_iterator i,
                                                     StateVectorVector::const_iterator iEnd,
                                                     Concrete::Decider*                pDecider,
                                                     const StateVectorVector&          deciderParams )
    {
        const auto size      = deciderParams.size();
        const auto available = std::distance( i, iEnd );

        if( size <= available )
        {
            // allow matching in any order
            using SetOfStates       = std::set< Concrete::State* >;
            using SetOfSetsOfStates = std::set< SetOfStates >;

            SetOfSetsOfStates deciderSets;
            for( const auto& states : deciderParams )
            {
                deciderSets.insert( SetOfStates( states.begin(), states.end() ) );
            }

            SetOfSetsOfStates transitionStates;
            for( auto j = i; j != ( i + size ); ++j )
            {
                const auto& states = *j;
                transitionStates.insert( SetOfStates( states.begin(), states.end() ) );
            }

            if( deciderSets == transitionStates )
            {
                // TODO - work out the ordering...
                DeciderInvocation deciderInvocation{ pDecider };
                return deciderInvocation;
            }
        }

        return {};
    }

    std::optional< DeciderInvocation > matchDecider( const StateSet& remaining, Concrete::Decider* pDecider,
                                                     const StateVectorVector& deciderParams )
    {
        DeciderInvocation deciderInvocation{ pDecider };

        StateSet remainingStates = remaining;
        for( const auto& states : deciderParams )
        {
            for( auto pState : states )
            {
                if( remainingStates.contains( pState ) )
                {
                    remainingStates.erase( pState );
                }
                else
                {
                    return {};
                }
            }
        }

        return deciderInvocation;
    }

    DeciderInvocation::Vector selectDeciders( Concrete::Context* pContext, Decision::DeciderSequence* pDeciderSequence,
                                              const StateVectorVector& transitionStates, const StateSet& decidedStates )
    {
        DeciderInvocation::Vector result;

        using DeciderParams = std::pair< Concrete::Decider*, StateVectorVector >;
        std::vector< DeciderParams > deciderParams;

        if( pDeciderSequence )
        {
            for( auto pDecider : pDeciderSequence->get_deciders() )
            {
                StateVectorVector transitionStates = collectDerivationStates( pDecider, pDecider->get_variables() );
                deciderParams.emplace_back( pDecider, transitionStates );
            }
        }

        // first solve the transitionStates sequence;
        {
            auto i    = transitionStates.begin();
            auto iEnd = transitionStates.end();
            // no decider required if singular
            if( std::distance( i, iEnd ) > 1 )
            {
                while( i != iEnd )
                {
                    bool bMadeProgress = false;
                    for( const auto& [ pDecider, deciderParams ] : deciderParams )
                    {
                        // attempt to match the decider params to the transition states

                        // the deciderParams sequence of sets must match the sequence of transition states but
                        // in ANY order
                        if( auto match = matchDecider( i, iEnd, pDecider, deciderParams ); match.has_value() )
                        {
                            i += deciderParams.size();
                            bMadeProgress = true;
                            result.push_back( match.value() );
                            break;
                        }
                    }

                    VERIFY_RTE_MSG( bMadeProgress,
                                    "Failed to make progress matching deciders to transition states for: "
                                        << Concrete::printContextFullType( pContext ) );
                }
            }
        }

        // now solve the decidedStates
        {
            StateSet remaining = decidedStates;
            while( !remaining.empty() )
            {
                bool bMadeProgress = false;
                for( const auto& [ pDecider, deciderParams ] : deciderParams )
                {
                    // attempt to match the decider params to the transition states

                    // the deciderParams sequence of sets must match the sequence of transition states but
                    // in ANY order
                    if( auto match = matchDecider( remaining, pDecider, deciderParams ); match.has_value() )
                    {
                        for( auto pState : fromStateVectorVector( deciderParams ) )
                        {
                            remaining.erase( pState );
                        }
                        bMadeProgress = true;
                        result.push_back( match.value() );
                    }
                }

                if( !bMadeProgress )
                {
                    std::ostringstream os;
                    for( auto p : remaining )
                    {
                        os << Concrete::printContextFullType( p ) << " ";
                    }
                    THROW_RTE( "Failed to make progress matching deciders to decided states for: "
                               << Concrete::printContextFullType( pContext ) << " remaining: " << os.str() );
                }
            }
        }

        return result;
    }

    using VariableSequence = std::vector< Automata::Vertex* >;
    void getVariableSequence( const StateVectorVector& transitionStates, const StateSet& decidedStates,
                              const StateSet& eliminatedStates, VariableSequence& decidedVariables,
                              VariableSequence& ignoredVariables )
    {
        // find common ancestor
        std::unordered_set< Automata::Vertex* > used;
        {
            for( const auto& states : transitionStates )
            {
                for( auto pState : states )
                {
                    if( auto opt = pState->get_automata_vertex()->get_test_ancestor(); opt.has_value() )
                    {
                        auto pVertex = opt.value();
                        if( !used.contains( pVertex ) )
                        {
                            decidedVariables.push_back( pVertex );
                            used.insert( pVertex );
                        }
                    }
                }
            }
        }
        {
            for( auto pState : decidedStates )
            {
                if( auto opt = pState->get_automata_vertex()->get_test_ancestor(); opt.has_value() )
                {
                    auto pVertex = opt.value();
                    if( !used.contains( pVertex ) )
                    {
                        decidedVariables.push_back( pVertex );
                        used.insert( pVertex );
                    }
                }
            }
        }
        {
            for( auto pState : eliminatedStates )
            {
                if( auto opt = pState->get_automata_vertex()->get_test_ancestor(); opt.has_value() )
                {
                    auto pVertex = opt.value();
                    if( !used.contains( pVertex ) )
                    {
                        ignoredVariables.push_back( pVertex );
                        used.insert( pVertex );
                    }
                }
            }
        }
    }

    Decision::BDDVertex* buildBDDrecurse( Database& database, VariableSequence::const_iterator i,
                                          VariableSequence::const_iterator iEnd,
                                          VariableSequence::const_iterator iIgnor,
                                          VariableSequence::const_iterator iIgnorEnd )
    {
        std::optional< Decision::BDDVertex* > left, right;

        if( i != iEnd )
        {
            auto iNext = i + 1;
            if( iNext != iEnd )
            {
                left  = buildBDDrecurse( database, iNext, iEnd, iIgnor, iIgnorEnd );
                right = buildBDDrecurse( database, iNext, iEnd, iIgnor, iIgnorEnd );
            }
            else if( iIgnor != iIgnorEnd )
            {
                left  = buildBDDrecurse( database, iNext, iEnd, iIgnor, iIgnorEnd );
                right = buildBDDrecurse( database, iNext, iEnd, iIgnor, iIgnorEnd );
            }
            return database.construct< Decision::BDDVertex >( Decision::BDDVertex::Args{ false, *i, left, right } );
        }
        else // if ( iIgnor != iIgnorEnd )
        {
            auto iNext = iIgnor + 1;
            if( iNext != iIgnorEnd )
            {
                left  = buildBDDrecurse( database, i, iEnd, iNext, iIgnorEnd );
                right = buildBDDrecurse( database, i, iEnd, iNext, iIgnorEnd );
            }
            return database.construct< Decision::BDDVertex >( Decision::BDDVertex::Args{ true, *iIgnor, left, right } );
        }
    }

    Decision::BDDVertex* buildBDD( Database& database, const VariableSequence& variableSequence,
                                   const VariableSequence& ignoredVariables )
    {
        VERIFY_RTE( !variableSequence.empty() );
        Decision::BDDVertex* pRoot = buildBDDrecurse( database, variableSequence.begin(), variableSequence.end(),
                                                      ignoredVariables.begin(), ignoredVariables.end() );

        return pRoot;
    }

    struct VariableAssignment
    {
        std::map< Automata::Vertex*, bool > assignments;
    };

    enum TriBool
    {
        eTrue,
        eFalse,
        eInvalid
    };

    TriBool recurseEvaluateBooleanExpression( Automata::Vertex* pVertex, const VariableAssignment& assignment )
    {
        std::vector< TriBool > results;
        for( auto pChild : pVertex->get_children() )
        {
            const TriBool b = recurseEvaluateBooleanExpression( pChild, assignment );
            if( b == eInvalid )
            {
                return eInvalid;
            }
            results.push_back( b );
        }

        if( auto pOr = db_cast< Automata::Or >( pVertex ) )
        {
            int iCounter = 0;
            for( auto b : results )
            {
                if( b == eTrue )
                {
                    ++iCounter;
                }
            }
            // NOTE: allow empty OR
            if( ( iCounter == 1 ) || results.empty() )
            {
                if( pVertex->get_is_conditional() )
                {
                    auto iFind = assignment.assignments.find( pVertex );
                    if( iFind == assignment.assignments.end() )
                    {
                        return eFalse;
                    }
                    else if( !iFind->second )
                    {
                        return eFalse;
                    }
                }
                return eTrue;
            }
            else
            {
                return eInvalid;
            }
        }
        else if( auto pAnd = db_cast< Automata::And >( pVertex ) )
        {
            int iCounter = 0;
            for( auto b : results )
            {
                if( b == eTrue )
                {
                    ++iCounter;
                }
            }
            if( iCounter == results.size() )
            {
                if( pVertex->get_is_conditional() )
                {
                    auto iFind = assignment.assignments.find( pVertex );
                    if( iFind == assignment.assignments.end() )
                    {
                        return eFalse;
                    }
                    else if( !iFind->second )
                    {
                        return eFalse;
                    }
                }
                return eTrue;
            }
            else
            {
                return eInvalid;
            }
        }
        else
        {
            THROW_RTE( "Unknown automata vertex type" );
        }
    }

    TriBool evaluateBooleanExpression( Concrete::State* pCommonAncestor, const VariableAssignment& assignment )
    {
        return recurseEvaluateBooleanExpression( pCommonAncestor->get_automata_vertex(), assignment );
    }

    enum Classification
    {
        eEliminated,
        eOnlyTrue,
        eOnlyFalse,
        eDecideable
    };

    void classifyVertex( Database& database, Decision::BDDVertex* pVertex, Classification classification )
    {
        switch( classification )
        {
            case eEliminated:
            {
                database.construct< Decision::Eliminated >( Decision::Eliminated::Args{ pVertex } );
            }
            break;
            case eOnlyTrue:
            {
                database.construct< Decision::True >( Decision::True::Args{ pVertex } );
            }
            break;
            case eOnlyFalse:
            {
                database.construct< Decision::False >( Decision::False::Args{ pVertex } );
            }
            break;
            case eDecideable:
            {
                database.construct< Decision::Decideable >( Decision::Decideable::Args{ pVertex } );
            }
            break;
        }
    }

    bool classifyBDDrecurse( Database& database, Concrete::State* pCommonAncestor, Decision::BDDVertex* pVertex,
                             VariableAssignment& assignment )
    {
        Classification result;
        // is this vertex a leaf node ?
        if( !pVertex->get_false_vertex().has_value() )
        {
            // if so evaluate the boolean expression for both true and false
            assignment.assignments[ pVertex->get_test() ] = false;
            const TriBool falseResult                     = evaluateBooleanExpression( pCommonAncestor, assignment );
            assignment.assignments[ pVertex->get_test() ] = true;
            const TriBool trueResult                      = evaluateBooleanExpression( pCommonAncestor, assignment );

            if( falseResult == eTrue )
            {
                if( trueResult == eTrue )
                {
                    result = eDecideable;
                }
                else
                {
                    result = eOnlyFalse;
                }
            }
            else
            {
                if( trueResult == eTrue )
                {
                    result = eOnlyTrue;
                }
                else
                {
                    result = eEliminated;
                }
            }
        }
        else
        {
            assignment.assignments[ pVertex->get_test() ] = false;
            const bool falseResult
                = classifyBDDrecurse( database, pCommonAncestor, pVertex->get_false_vertex().value(), assignment );
            assignment.assignments[ pVertex->get_test() ] = true;
            const bool trueResult
                = classifyBDDrecurse( database, pCommonAncestor, pVertex->get_true_vertex().value(), assignment );

            if( falseResult )
            {
                if( trueResult )
                {
                    result = eDecideable;
                }
                else
                {
                    result = eOnlyFalse;
                }
            }
            else
            {
                if( trueResult )
                {
                    result = eOnlyTrue;
                }
                else
                {
                    result = eEliminated;
                }
            }
        }

        classifyVertex( database, pVertex, result );
        return result != eEliminated;
    }

    void classifyBDD( Database& database, Concrete::State* pCommonAncestor, Decision::BDDVertex* pVertex )
    {
        VariableAssignment assignment;
        classifyBDDrecurse( database, pCommonAncestor, pVertex, assignment );
    }

    void validateBDD( Decision::BDDVertex* pVertex, const DeciderInvocation::Vector& deciderSequence )
    {
        // for the sequences of sets ensure ignoring requirements that ALL exclusive and inclusive
        // semantics are correctly possible given the BDD
    }

    Decision::DecisionProcedure* buildDecisionProcedure( Database& database, Decision::BDDVertex* pRoot,
                                                         Concrete::State*                 pCommonAncestor,
                                                         const DeciderInvocation::Vector& deciders )
    {
        // for each step in reduced binary tree

        // 1. Determine associated decider callback if required

        // 2. Determine how to call the decider callback - i.e. derivation from variables to states and
        // dealing with parameter orders for decider

        // 3. For all possible decider return values determine consequence including

        // 4. How to actually set the activation state bits

        Decision::DecisionProcedure* pProcedure = database.construct< Decision::DecisionProcedure >(
            Decision::DecisionProcedure::Args{ pCommonAncestor, pRoot } );

        return pProcedure;
    }

    Decision::DecisionProcedure* compileTransition( Database& database, Concrete::Context* pContext,
                                                    Decision::DeciderSequence* pDeciderSequence,
                                                    const StateVectorVector&   transitionStates )
    {
        StateSet transitionStatesSet = fromStateVectorVector( transitionStates );

        Concrete::State* pCommonAncestor = findCommonAncestor( transitionStatesSet );
        VERIFY_RTE_MSG( pCommonAncestor && db_cast< Automata::Or >( pCommonAncestor->get_automata_vertex() ),
                        "Common ancestor should be OR vertex for transition: " << Concrete::printContextFullType(
                            pContext ) << " ancestor: " << Concrete::printContextFullType( pCommonAncestor ) );

        StateSet eliminatedStates;
        StateSet decidedStates;
        findDecidedStates( pCommonAncestor, transitionStatesSet, eliminatedStates, decidedStates );

        const DeciderInvocation::Vector deciders
            = selectDeciders( pContext, pDeciderSequence, transitionStates, decidedStates );

        VariableSequence decidedVariables;
        VariableSequence ignoredVariables;
        getVariableSequence( transitionStates, decidedStates, eliminatedStates, decidedVariables, ignoredVariables );

        Decision::BDDVertex* pRoot = buildBDD( database, decidedVariables, ignoredVariables );

        classifyBDD( database, pCommonAncestor, pRoot );

        validateBDD( pRoot, deciders );

        Decision::DecisionProcedure* pProcedure = buildDecisionProcedure( database, pRoot, pCommonAncestor, deciders );

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

        // determine the decider sequences
        for( Concrete::Object* pObject : database.many< Concrete::Object >( m_sourceFilePath ) )
        {
            DeciderSequenceVector sequences;
            DeciderVector         deciders;
            findDeciderStates( database, pObject, sequences, deciders );

            database.construct< Concrete::Object >( Concrete::Object::Args{ pObject, sequences } );
        }

        // determine decision procedures for each transition
        for( Concrete::Interupt* pInterupt : database.many< Concrete::Interupt >( m_sourceFilePath ) )
        {
            auto transitions = pInterupt->get_transition();
            if( !transitions.empty() )
            {
                Decision::DeciderSequence* pDeciderSequence = chooseDeciderSequence( pInterupt );

                StateVectorVector transitionStates = collectDerivationStates( pInterupt, transitions );

                Decision::DecisionProcedure* pProcedure
                    = compileTransition( database, pInterupt, pDeciderSequence, transitionStates );

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
                Decision::DeciderSequence* pDeciderSequence = chooseDeciderSequence( pState );

                StateVectorVector transitionStates = collectDerivationStates( pState, transitions );

                Decision::DecisionProcedure* pProcedure
                    = compileTransition( database, pState, pDeciderSequence, transitionStates );

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
