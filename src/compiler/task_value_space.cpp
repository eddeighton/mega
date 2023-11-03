
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

        // find all deciders in object
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
            /*if( !transitions.empty() )
            {
                Decision::DeciderSequence* pDeciderSequence = chooseDeciderSequence( pInterupt );
                StateVectorVector transitionStates = collectDerivationStates( pInterupt, transitions );

                Decision::DecisionProcedure* pProcedure
                    = compileTransition( database, pInterupt, pDeciderSequence, transitionStates );

                database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, pProcedure } );
            }
            else*/
            {
                database.construct< Concrete::Interupt >( Concrete::Interupt::Args{ pInterupt, std::nullopt } );
            }
        }
        for( Concrete::State* pState : database.many< Concrete::State >( m_sourceFilePath ) )
        {
            auto transitions = pState->get_transition();
            /*if( !transitions.empty() )
            {
                Decision::DeciderSequence* pDeciderSequence = chooseDeciderSequence( pState );

                StateVectorVector transitionStates = collectDerivationStates( pState, transitions );

                Decision::DecisionProcedure* pProcedure
                    = compileTransition( database, pState, pDeciderSequence, transitionStates );

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
