
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

    AutomataStage::Automata::Vertex* recurse( AutomataStage::Database&          database,
                                              AutomataStage::Concrete::Context* pContext,
                                              AutomataStage::Automata::Vertex*  pParent,
                                              U32&                              indexBase,
                                              U32                               relative_domain )
    {
        using namespace AutomataStage;

        Automata::Vertex* pResult = pParent;

        if( auto pState = db_cast< Concrete::State >( pContext ) )
        {
            if( pState->get_interface_state()->get_is_or_state() )
            {
                pResult = database.construct< Automata::Or >(
                    { Automata::Or::Args{ Automata::Vertex::Args{ indexBase, relative_domain, pContext, {} } } } );
                indexBase += pState->get_total_size();
                pParent->push_back_children( pResult );
            }
            else
            {
                pResult = database.construct< Automata::And >(
                    { Automata::And::Args{ Automata::Vertex::Args{ indexBase, relative_domain, pContext, {} } } } );
                indexBase += pState->get_total_size();
                pParent->push_back_children( pResult );
            }
            relative_domain = 1;
        }

        for( auto pChildContext : pContext->get_children() )
        {
            recurse( database, pChildContext, pResult, indexBase, relative_domain * pChildContext->get_local_size() );
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
        U32                              index;
        std::vector< SubTypeNode >       subTypes;
    };

    void recurse( Node& node, U32& index )
    {
        using namespace AutomataStage;

        for( auto pChildVertex : node.pVertex->get_children() )
        {
            // node.instance
            auto       pContext    = pChildVertex->get_context();
            const auto localDomain = pChildVertex->get_relative_domain();

            Node::SubTypeNode subTypeNode{ pContext->get_concrete_id().getSubObjectID(), {} };

            for( Instance i = 0; i != localDomain; ++i )
            {
                const auto instance = static_cast< Instance >( i + ( localDomain * node.instance ) );

                Node subTypeInstance{ instance, pChildVertex, index, {} };
                ++index;

                recurse( subTypeInstance, index );

                subTypeNode.instances.push_back( subTypeInstance );
            }

            node.subTypes.push_back( subTypeNode );
        }
    }

    void recurse( mega::pipeline::Progress& taskProgress, AutomataStage::Database& database, const Node& node,
                  AutomataStage::Automata::Enum* pParentEnum )
    {
        using namespace AutomataStage;

        for( const auto& subType : node.subTypes )
        {
            for( const auto& instance : subType.instances )
            {
                bool bIsOr = false;
                {
                    if( auto pOr = db_cast< Automata::Or >( instance.pVertex ) )
                    {
                        bIsOr = true;
                    }
                }

                const mega::SubTypeInstance subTypeInstance( subType.subtype, instance.instance );
                const bool                  bHasAction = db_cast< Concrete::Action >( instance.pVertex->get_context() );

                auto pEnum = database.construct< Automata::Enum >( Automata::Enum::Args{
                    instance.index,
                    instance.pVertex->get_index_base() + instance.instance,
                    {},
                    bIsOr,
                    bHasAction,
                    subTypeInstance,
                    {}

                } );

                pParentEnum->push_back_children( pEnum );

                recurse( taskProgress, database, instance, pEnum );
            }
        }
    }

    void collectIndices( AutomataStage::Automata::Enum* pEnum, std::vector< U32 >& indices )
    {
        indices.push_back( pEnum->get_switch_index() );
        for( auto pChild : pEnum->get_children() )
        {
            collectIndices( pChild, indices );
        }
    }

    void recurse( AutomataStage::Automata::Enum* pEnum )
    {
        {
            std::vector< U32 > indices;
            collectIndices( pEnum, indices );
            VERIFY_RTE( !indices.empty() );
            pEnum->set_indices( indices );
        }

        for( auto pChild : pEnum->get_children() )
        {
            recurse( pChild );
        }
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
                { Automata::And::Args{ Automata::Vertex::Args{ 0, 1, pObject, {} } } } );
            U32 index = 0U;
            recurse( database, pObject, pRoot, index, 1 );

            Node rootNode{ 0, pRoot, 0 };
            U32  subTypeInstanceIndex = 0U;
            recurse( rootNode, subTypeInstanceIndex );

            auto pRootEnum = database.construct< Automata::Enum >(
                Automata::Enum::Args{ 0, 0, {}, false, false, SubTypeInstance{}, {} } );

            recurse( taskProgress, database, rootNode, pRootEnum );
            recurse( pRootEnum );

            database.construct< Concrete::Object >( { Concrete::Object::Args{ pObject, pRoot, pRootEnum, index } } );
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
