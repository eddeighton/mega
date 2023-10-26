
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

#include "automata.hpp"

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

class Task_Automata : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Automata( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    AutomataStage::Automata::Vertex* recurseAndOrTree( AutomataStage::Database&          database,
                                                       AutomataStage::Concrete::Context* pContext,
                                                       AutomataStage::Automata::Vertex*  pParent,
                                                       U32                               relative_domain )
    {
        using namespace AutomataStage;

        Automata::Vertex* pResult = pParent;

        if( auto pState = db_cast< Concrete::State >( pContext ) )
        {
            // is the parent vertex an OR?
            bool bParentVertexIsOR = false;
            if( db_cast< Automata::Or >( pParent ) )
            {
                bParentVertexIsOR = true;
            }

            if( pState->get_interface_state()->get_is_or_state() )
            {
                pResult = database.construct< Automata::Or >( { Automata::Or::Args{
                    Automata::Vertex::Args{ bParentVertexIsOR, relative_domain, pContext, {} } } } );
                pParent->push_back_children( pResult );
            }
            else
            {
                pResult = database.construct< Automata::And >( { Automata::And::Args{
                    Automata::Vertex::Args{ bParentVertexIsOR, relative_domain, pContext, {} } } } );
                pParent->push_back_children( pResult );
            }
            relative_domain = 1;
        }

        for( auto pChildContext : pContext->get_children() )
        {
            recurseAndOrTree( database, pChildContext, pResult, relative_domain * pChildContext->get_local_size() );
        }

        return pResult;
    }

    struct Node
    {
        using Vector = std::vector< Node >;
        struct SubTypeNode
        {
            SubType      subtype;
            Node::Vector instances;
        };

        Instance                         instance;
        AutomataStage::Automata::Vertex* pVertex = nullptr;
        std::vector< SubTypeNode >       subTypes;
    };

    void recurseNode( Node& node )
    {
        using namespace AutomataStage;

        for( auto pChildVertex : node.pVertex->get_children() )
        {
            auto       pContext    = pChildVertex->get_context();
            const auto localDomain = pChildVertex->get_relative_domain();

            Node::SubTypeNode subTypeNode{ pContext->get_concrete_id().getSubObjectID(), {} };

            for( Instance i = 0; i != localDomain; ++i )
            {
                const auto instance = static_cast< Instance >( i + ( localDomain * node.instance ) );
                Node       subTypeInstance{ instance, pChildVertex, {} };
                recurseNode( subTypeInstance );
                subTypeNode.instances.push_back( subTypeInstance );
            }

            node.subTypes.push_back( subTypeNode );
        }
    }

    void recurseEnum( mega::pipeline::Progress& taskProgress, AutomataStage::Database& database, const Node& node,
                      std::vector< AutomataStage::Automata::Enum* >& enums, AutomataStage::Automata::Enum* pParentEnum,
                      U32& bitsetIndex, U32& switchIndex )
    {
        using namespace AutomataStage;

        for( const auto& subType : node.subTypes )
        {
            for( const auto& instance : subType.instances )
            {
                const bool                         bIsOr = db_cast< Automata::Or >( instance.pVertex );
                const mega::SubTypeInstance        subTypeInstance( subType.subtype, instance.instance );
                std::optional< Concrete::Action* > pActionOpt;
                if( auto pAction = db_cast< Concrete::Action >( instance.pVertex->get_context() ) )
                {
                    pActionOpt = pAction;
                }

                Automata::Enum* pEnum = pParentEnum;
                Automata::Test* pTest = nullptr;

                if( instance.pVertex->get_is_parent_or() || pActionOpt.has_value() )
                {
                    if( instance.pVertex->get_is_parent_or() )
                    {
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

                                bitsetIndex } );
                        ++bitsetIndex;
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

                recurseEnum( taskProgress, database, instance, enums, pEnum, bitsetIndex, switchIndex );

                if( pTest )
                {
                    pTest->set_failure_switch_index( switchIndex );
                }
            }
        }
    }

    void recurseCollectIndices( AutomataStage::Automata::Enum* pEnum, std::vector< U32 >& indices )
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
            = m_environment.AutomataStage_AutomataAnalysis( m_sourceFilePath );
        start( taskProgress, "Task_Automata", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.ParserStage_Body( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.MetaStage_MetaAnalysis( m_sourceFilePath ) ) } );

        if( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace AutomataStage;

        Database database( m_environment, m_sourceFilePath );

        for( auto pObject : database.many< Concrete::Object >( m_sourceFilePath ) )
        {
            AutomataStage::Automata::And* pRoot = database.construct< Automata::And >(
                { Automata::And::Args{ Automata::Vertex::Args{ false, 1, pObject, {} } } } );

            recurseAndOrTree( database, pObject, pRoot, 1 );

            std::vector< Automata::Enum* > enums;

            U32 bitsetIndex = 0;
            U32 switchIndex = 0;
            {
                Node rootNode{ 0, pRoot };
                recurseNode( rootNode );
                recurseEnum( taskProgress, database, rootNode, enums, nullptr, bitsetIndex, switchIndex );
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
                { Concrete::Object::Args{ pObject, pRoot, enums, bitsetIndex, switchIndex } } );

            for( auto pBitset : pObject->get_bitsets() )
            {
                database.construct< Concrete::Dimensions::Bitset >(
                    Concrete::Dimensions::Bitset::Args{ pBitset, bitsetIndex } );
            }
        }

        const task::FileHash fileHashCode = database.save_AutomataAnalysis_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Automata( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Automata >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
