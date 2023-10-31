
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

#include <common/stash.hpp>

#include <vector>
#include <sstream>
#include <algorithm>

namespace ValueSpaceStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
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

template < typename T >
std::vector< Concrete::State* > collectTargetStates( T* pContext, Derivation::Root* pDerivation )
{
    std::vector< Concrete::State* > states;
    {
        std::vector< Concrete::Graph::Vertex* > targets;
        for( auto pEdge : pDerivation->get_edges() )
        {
            collectTargets( pEdge->get_next(), targets );
        }

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
            auto pDeciderSequence = database.construct< Decision::DeciderSequence >(
                Decision::DeciderSequence::Args{ pState, deciders } );

            sequences.push_back( pDeciderSequence );
            deciders.clear();
        }
    }

public:
    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.ValueSpaceStage_ValueSpace( m_sourceFilePath );
        start( taskProgress, "Task_ValueSpace", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( m_environment.Operations( m_sourceFilePath ) ),
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
                std::vector< Automata::Vertex* > testSequence;

                using StateVector = std::vector< Concrete::State* >;
                std::vector< StateVector > stateVectors;

                for( auto pDerivation : transitions )
                {
                    std::vector< Concrete::State* > states = collectTargetStates( pInterupt, pDerivation );
                    VERIFY_RTE_MSG( !states.empty(),
                                    "Transition has empty target: " << Concrete::printContextFullType( pInterupt ) );
                    stateVectors.push_back( states );
                }

                VERIFY_RTE_MSG( stateVectors.empty(),
                                "Transition has no successors: " << Concrete::printContextFullType( pInterupt ) );

                // build the binary decision diagram
                Automata::Vertex* pVertex = stateVectors.front().front()->get_automata_vertex();

                Decision::BDDVertex* pRoot = database.construct< Decision::BDDVertex >(
                    Decision::BDDVertex::Args{ pVertex, std::nullopt, std::nullopt } );

                Decision::DecisionProcedure* pProcedure = database.construct< Decision::DecisionProcedure >(
                    Decision::DecisionProcedure::Args{ testSequence, pRoot } );

                database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, pProcedure } );
            }
            else
            {
                database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, std::nullopt } );
            }
        }
        for( Concrete::State* pState : database.many< Concrete::State >( m_sourceFilePath ) )
        {
            /*auto transitions = pState->get_transition();
            if( !transitions.empty() )
            {
                std::vector< Automata::Vertex* > testSequence;

                Decision::BDDVertex* pRoot = nullptr;

                for( auto pDerivation : transitions )
                {
                    std::vector< Concrete::State* > states = collectTargetStates( pState, pDerivation );
                }

                Decision::DecisionProcedure* pProcedure = database.construct< Decision::DecisionProcedure >(
                    Decision::DecisionProcedure::Args{ testSequence, pRoot } );

                database.construct< Concrete::State >( Concrete::State::Args{ pState, pProcedure } );
            }
            else*/
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
