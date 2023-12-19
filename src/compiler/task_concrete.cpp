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

#include "database/ConcreteStage.hxx"

#include <common/stash.hpp>

#include <optional>

namespace ConcreteStage
{
#include "compiler/interface_printer.hpp"
} // namespace ConcreteStage

namespace mega::compiler
{

using namespace ConcreteStage;

class Task_ConcreteTree : public BaseTask
{
public:
    Task_ConcreteTree( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
    {
    }

    using Inheritors = std::multimap< Interface::Node*, Concrete::Node* >;

    void inherit( Database& database, Interface::Node* pIParentNode, Concrete::Node* pCParentNode,
                  Inheritors& inheritors )
    {
        inheritors.insert( { pIParentNode, pCParentNode } );

        if( auto pIContext = db_cast< Interface::IContext >( pIParentNode ) )
        {
            if( auto inheritance = pIContext->get_inheritance_opt() )
            {
                auto pInheritance = inheritance.value();
                auto pTypePathSeq = pInheritance->get_type_path_sequence();
                for( auto pPath : pTypePathSeq->get_path_sequence() )
                {
                    if( auto pAbsolutePath = db_cast< Parser::Type::Absolute >( pPath ) )
                    {
                        auto pNode = pAbsolutePath->get_type();
                        inherit( database, pNode, pCParentNode, inheritors );
                    }
                    else
                    {
                        THROW_RTE(
                            "Non absolute path used in inheritance for: " << Interface::fullTypeName( pIContext ) );
                    }
                }
            }
        }

        for( auto pINode : pIParentNode->get_children() )
        {
            // determine if override or add
            Concrete::Node* pChildCNode = nullptr;
            {
                for( auto pExistingCNode : pCParentNode->get_children() )
                {
                    if( pINode->get_symbol()->get_token() == pExistingCNode->get_node()->get_symbol()->get_token() )
                    {
                        pChildCNode = pExistingCNode;
                        break;
                    }
                }
            }

            if( !pChildCNode )
            {
                pChildCNode = database.construct< Concrete::Node >(
                    Concrete::Node::Args{ Concrete::NodeGroup::Args{ {} }, pCParentNode, pINode } );
                pCParentNode->push_back_children( pChildCNode );
            }
            else
            {
                // override existing node with deriving interface node
                // ensure nodes are same kind
                VERIFY_RTE_MSG( pChildCNode->get_node()->get_kind() == pINode->get_kind(),
                                "Conflicting node kinds in inheritance for: "
                                    << Interface::fullTypeName( pINode ) << " of: "
                                    << pChildCNode->get_node()->get_kind() << " and: " << pINode->get_kind() );
                pChildCNode->set_node( pINode );
            }

            inherit( database, pINode, pChildCNode, inheritors );
        }
    }

    void refine( Database& database, Concrete::Node* pCNode )
    {
        if( auto pIObject = db_cast< Interface::Object >( pCNode->get_node() ) )
        {
            database.construct< Concrete::Object >( Concrete::Object::Args{ pCNode } );
        }
        else
        {
            // leave as node
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const auto projectManifestPath = m_environment.project_manifest();

        const mega::io::CompilationFilePath interfaceTreeFile
            = m_environment.InterfaceStage_Tree( projectManifestPath );
        const mega::io::CompilationFilePath concreteFile = m_environment.ConcreteStage_Concrete( projectManifestPath );
        start( taskProgress, "Task_ConcreteTree", interfaceTreeFile.path(), concreteFile.path() );

        task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceTreeFile ) } );
        for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if( m_environment.restore( concreteFile, determinant ) )
        {
            m_environment.setBuildHashCode( concreteFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, projectManifestPath );

        Interface::Root* pInterfaceRoot = database.one< Interface::Root >( projectManifestPath );

        Concrete::Root* pConcreteRoot
            = database.construct< Concrete::Root >( Concrete::Root::Args{ Concrete::NodeGroup::Args{ {} } } );

        Inheritors inheritors;
        for( auto pINode : pInterfaceRoot->get_children() )
        {
            auto pCNode = database.construct< Concrete::Node >(
                Concrete::Node::Args{ Concrete::NodeGroup::Args{ {} }, pConcreteRoot, pINode } );
            pConcreteRoot->push_back_children( pCNode );
            inherit( database, pINode, pCNode, inheritors );
        }

        // record all inheritors
        for( auto i = inheritors.begin(), iEnd = inheritors.end(); i != iEnd; )
        {
            auto                           pINode = i->first;
            std::vector< Concrete::Node* > inherits;
            for( auto iNext = inheritors.upper_bound( i->first ); i != iNext; ++i )
            {
                inherits.push_back( i->second );
            }
            database.construct< Interface::Node >( Interface::Node::Args{ pINode, inherits } );
        }

        for( auto pCNode : pConcreteRoot->get_children() )
        {
            refine( database, pCNode );
        }

        const task::FileHash buildHash = database.save_Concrete_to_temp();
        m_environment.temp_to_real( concreteFile );
        m_environment.setBuildHashCode( concreteFile, buildHash );
        m_environment.stash( concreteFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_ConcreteTree( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_ConcreteTree >( taskArguments );
}

} // namespace mega::compiler
