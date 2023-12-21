
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

#include "mega/common_strings.hpp"

#include "database/AutomataStage.hxx"

#include "database/component_type.hpp"
#include "database/sources.hpp"

#include <common/stash.hpp>

#include <vector>
#include <algorithm>

namespace AutomataStage
{
#include "compiler/concrete_printer.hpp"
}

namespace mega::compiler
{

using namespace AutomataStage;

class Task_Automata : public BaseTask
{
    const mega::io::manifestFilePath m_manifestFilePath;

public:
    Task_Automata( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
        , m_manifestFilePath( m_environment.project_manifest() )
    {
    }

    Automata::Vertex* recurseAndOrTree( Database&          database,
                                        Concrete::Context* pContext,
                                        Automata::Vertex*  pParent,
                                        std::optional< Automata::Vertex* >
                                                                          testAncestor,
                                        U32                               relative_domain,
                                        std::vector< Automata::Vertex* >& tests )
    {
        using namespace AutomataStage;

        Automata::Vertex* pResult = pParent;

        if( auto pState = db_cast< Concrete::State >( pContext ) )
        {
            // is the parent vertex an OR?
            bool bIsConditional = false;
            if( db_cast< Automata::Or >( pParent ) )
            {
                bIsConditional = true;
            }

            bool bIsHistorical = false;
            if( auto pParentState = db_cast< Concrete::State >( pParent->get_context() ) )
            {
                bIsHistorical = pParentState->get_state()->get_flags().get( IContextFlags::eHistorical );
            }

            bool bHasRequirement = false;
            // if( !pState->get_requirements().empty() )
            // {
            //     bHasRequirement = true;
            // }

            const bool bFirst     = pParent->get_children().empty();
            const bool bIsOrState = pState->get_state()->get_flags().get( IContextFlags::eOR );
            if( bIsOrState )
            {
                pResult = database.construct< Automata::Or >( { Automata::Or::Args{ Automata::Vertex::Args{

                    bFirst,
                    bIsConditional,
                    bIsHistorical,
                    bHasRequirement,
                    relative_domain,
                    pContext,
                    std::nullopt,
                    0,
                    {},
                    {} } } } );

                pParent->push_back_children( pResult );
            }
            else
            {
                pResult = database.construct< Automata::And >( { Automata::And::Args{ Automata::Vertex::Args{

                    bFirst,
                    bIsConditional,
                    bIsHistorical,
                    bHasRequirement,
                    relative_domain,
                    pContext,
                    std::nullopt,
                    0,
                    {},
                    {} } } } );

                pParent->push_back_children( pResult );
            }

            if( bIsConditional )
            {
                tests.push_back( pResult );
                testAncestor = pResult;
            }

            pResult->set_test_ancestor( testAncestor );

            relative_domain = 1;

            database.construct< Concrete::State >( Concrete::State::Args{ pState, pResult } );
        }

        for( Concrete::Node* pChild : pContext->get_children() )
        {
            if( auto pChildContext = db_cast< Concrete::Context >( pChild ) )
            {
                recurseAndOrTree( database, pChildContext, pResult, testAncestor,
                                  relative_domain * pChildContext->get_icontext()->get_size(), tests );
            }
        }

        return pResult;
    }

    void recurseSetSiblings( Automata::Vertex* pVertex )
    {
        const auto children = pVertex->get_children();
        for( auto pChild : children )
        {
            for( auto pIter : children )
            {
                if( pIter != pChild )
                {
                    pChild->push_back_siblings( pIter );
                }
            }
            recurseSetSiblings( pChild );
        }
    }

    struct Node
    {
        using Vector = std::vector< Node >;
        struct SubTypeNode
        {
            concrete::SubObjectID subtype;
            Node::Vector          instances;
        };

        concrete::Instance         instance;
        Automata::Vertex*          pVertex = nullptr;
        std::vector< SubTypeNode > subTypes;
    };

    void recurseNode( Node& node )
    {
        using namespace AutomataStage;

        for( auto pChildVertex : node.pVertex->get_children() )
        {
            auto       pContext    = pChildVertex->get_context();
            const auto localDomain = pChildVertex->get_relative_domain();

            Node::SubTypeNode subTypeNode{ pContext->get_concrete_id()->get_type_id().getSubObjectID(), {} };

            for( concrete::Instance::ValueType i = 0; i != localDomain; ++i )
            {
                const auto instance( i + ( localDomain * node.instance.getValue() ) );
                Node       subTypeInstance{ concrete::Instance( instance ), pChildVertex, {} };
                recurseNode( subTypeInstance );
                subTypeNode.instances.push_back( subTypeInstance );
            }

            node.subTypes.push_back( subTypeNode );
        }
    }

    struct CompareVertex
    {
        inline bool operator()( Automata::Vertex* pLeft, Automata::Vertex* pRight ) const
        {
            return pLeft->get_context()->get_concrete_id() < pRight->get_context()->get_concrete_id();
        }
    };
    using TestMap = std::multimap< Automata::Vertex*, Automata::Test*, CompareVertex >;

    void recurseEnum( mega::pipeline::Progress& taskProgress, Database& database, const Node& node,
                      std::vector< Automata::Enum* >& enums, Automata::Enum* pParentEnum, U32& switchIndex,
                      TestMap& tests )
    {
        using namespace AutomataStage;

        for( const auto& subType : node.subTypes )
        {
            for( const auto& instance : subType.instances )
            {
                const bool                                bIsOr = db_cast< Automata::Or >( instance.pVertex );
                const mega::concrete::SubObjectIDInstance subTypeInstance( subType.subtype, instance.instance );
                std::optional< Concrete::Action* >        pActionOpt;
                if( auto pAction = db_cast< Concrete::Action >( instance.pVertex->get_context() ) )
                {
                    pActionOpt = pAction;
                }

                Automata::Enum* pEnum = pParentEnum;
                Automata::Test* pTest = nullptr;

                const bool bHasBit = instance.pVertex->get_is_conditional() || instance.pVertex->get_has_requirement();

                if( bHasBit || pActionOpt.has_value() )
                {
                    if( bHasBit )
                    {
                        const bool bIsStartState = instance.pVertex->get_is_first();

                        pEnum = pTest = database.construct< Automata::Test >(

                            Automata::Test::Args{

                                Automata::Enum::Args{

                                    instance.pVertex,
                                    switchIndex,
                                    switchIndex + 1,
                                    {},
                                    bIsOr,
                                    pActionOpt,
                                    subTypeInstance,
                                    {} },

                                bIsStartState } );
                        tests.insert( { instance.pVertex, pTest } );
                    }
                    else
                    {
                        pEnum = database.construct< Automata::Enum >( Automata::Enum::Args{

                            instance.pVertex,
                            switchIndex,
                            switchIndex + 1,
                            {},
                            bIsOr,
                            pActionOpt,
                            subTypeInstance,
                            {} } );
                    }

                    ++switchIndex;

                    if( pParentEnum )
                    {
                        pParentEnum->push_back_children( pEnum );
                    }
                    else
                    {
                        enums.push_back( pEnum );
                    }
                }

                recurseEnum( taskProgress, database, instance, enums, pEnum, switchIndex, tests );

                if( pTest )
                {
                    pTest->set_failure_switch_index( switchIndex );
                }
            }
        }
    }

    void recurseCollectIndices( Automata::Enum* pEnum, std::vector< U32 >& indices )
    {
        std::vector< U32 > indicesSubTree;
        for( auto pChild : pEnum->get_children() )
        {
            recurseCollectIndices( pChild, indicesSubTree );
        }
        pEnum->set_child_switch_indices( indicesSubTree );
        indices.push_back( pEnum->get_switch_index() );
        std::copy( indicesSubTree.begin(), indicesSubTree.end(), std::back_inserter( indices ) );
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.AutomataStage_AutomataAnalysis( m_manifestFilePath );
        start( taskProgress, "Task_Automata", m_manifestFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_manifestFilePath ) ),
              m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( m_manifestFilePath ) ) } );

        if( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace AutomataStage;

        Database database( m_environment, m_manifestFilePath );

        for( auto pObject : database.many< Concrete::Object >( m_manifestFilePath ) )
        {
            Automata::And* pRoot = database.construct< Automata::And >( { Automata::And::Args{
                Automata::Vertex::Args{ true, false, false, false, 1, pObject, std::nullopt, 0, {}, {} } } } );
            pRoot->set_test_ancestor( std::nullopt );

            std::vector< Automata::Vertex* > tests;
            recurseAndOrTree( database, pObject, pRoot, std::nullopt, 1, tests );
            recurseSetSiblings( pRoot );

            std::vector< Automata::Enum* > enums;

            U32 switchIndex = 0;
            U32 totalBits   = 0;
            {
                Node rootNode{ concrete::INSTANCE_ZERO, pRoot };
                recurseNode( rootNode );

                TestMap tests;
                recurseEnum( taskProgress, database, rootNode, enums, nullptr, switchIndex, tests );

                // determine the bit indices
                for( TestMap::const_iterator i = tests.begin(), iEnd = tests.end(); i != iEnd; )
                {
                    auto pVertex   = i->first;
                    auto iRangeEnd = tests.upper_bound( pVertex );

                    const auto rangeStart = totalBits;
                    pVertex->set_bitset_range_start( rangeStart );

                    for( ; i != iRangeEnd; ++i )
                    {
                        auto pEnum = i->second;
                        pEnum->set_bitset_index( totalBits );
                        ++totalBits;
                    }

                    pVertex->set_bitset_range_size( totalBits - rangeStart );
                }
            }

            // set the child switch indices
            {
                std::vector< U32 > switchIndices;
                for( auto pEnum : enums )
                {
                    recurseCollectIndices( pEnum, switchIndices );
                }
            }

            database.construct< Concrete::Object >(
                { Concrete::Object::Args{ pObject, pRoot, tests, enums, totalBits, switchIndex } } );

            database.construct< Concrete::ActivationBitSet >(
                Concrete::ActivationBitSet::Args{ pObject->get_activation_bitset(), totalBits } );
        }

        const task::FileHash fileHashCode = database.save_AutomataAnalysis_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Automata( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_Automata >( taskArguments );
}

} // namespace mega::compiler
