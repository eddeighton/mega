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
#include "compiler/concrete_printer.hpp"
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

    using Inheritors       = std::multimap< Interface::Node*, Concrete::Node* >;
    using DirectInheritors = std::multimap< Interface::Node*, Interface::Node* >;

    void inherit( Database& database, Concrete::Root* pConcreteRoot, Interface::Node* pIParentNode,
                  Concrete::Node* pCParentNode, std::vector< Concrete::Node* >& objects, Inheritors& inheritors,
                  DirectInheritors& directInheritors )
    {
        if( !pCParentNode )
        {
            // see if can start object
            if( auto pIObject = db_cast< Interface::Object >( pIParentNode ) )
            {
                pCParentNode = database.construct< Concrete::Node >(
                    Concrete::Node::Args{ Concrete::NodeGroup::Args{ {} }, pConcreteRoot, pIParentNode } );
                objects.push_back( pCParentNode );
            }
        }

        if( pCParentNode )
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
                            directInheritors.insert( { pNode, pIContext } );
                            inherit(
                                database, pConcreteRoot, pNode, pCParentNode, objects, inheritors, directInheritors );
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
                        if( pINode->get_symbol()->get_token() == Concrete::getIdentifier( pExistingCNode ) )
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
                    if( pChildCNode->get_node_opt().has_value() )
                    {
                        VERIFY_RTE_MSG( pChildCNode->get_node_opt().value()->get_kind() == pINode->get_kind(),
                                        "Conflicting node kinds in inheritance for: "
                                            << Interface::fullTypeName( pINode )
                                            << " of: " << pChildCNode->get_node_opt().value()->get_kind()
                                            << " and: " << pINode->get_kind() );
                    }
                    pChildCNode->set_node_opt( pINode );
                }

                inherit( database, pConcreteRoot, pINode, pChildCNode, objects, inheritors, directInheritors );
            }
        }
        else
        {
            // recurse through the interface looking for objects
            for( auto pINode : pIParentNode->get_children() )
            {
                inherit( database, pConcreteRoot, pINode, nullptr, objects, inheritors, directInheritors );
            }
        }
    }

    void refine( Database& database, Concrete::Node* pCNode, Concrete::Object* pParentObject )
    {
        if( pParentObject )
        {
            pCNode->set_parent_object( pParentObject );
        }

        if( !pCNode->get_node_opt().has_value() )
        {
            return;
        }

        auto pINode = pCNode->get_node_opt().value();

        if( auto pIContext = db_cast< Interface::IContext >( pINode ) )
        {
            if( auto pInterupt = db_cast< Interface::Interupt >( pIContext ) )
            {
                database.construct< Concrete::Interupt >(
                    Concrete::Interupt::Args{ Concrete::Context::Args{ pCNode, pIContext }, pInterupt } );
            }
            else if( auto pEvent = db_cast< Interface::Event >( pIContext ) )
            {
                database.construct< Concrete::Event >(
                    Concrete::Event::Args{ Concrete::Context::Args{ pCNode, pIContext }, pEvent } );
            }
            else if( auto pAction = db_cast< Interface::Action >( pIContext ) )
            {
                database.construct< Concrete::Action >( Concrete::Action::Args{
                    Concrete::State::Args{ Concrete::Context::Args{ pCNode, pIContext }, pAction }, pAction } );
            }
            else if( auto pState = db_cast< Interface::State >( pIContext ) )
            {
                database.construct< Concrete::State >(
                    Concrete::State::Args{ Concrete::Context::Args{ pCNode, pIContext }, pState } );
            }
            else if( auto pDecider = db_cast< Interface::Decider >( pIContext ) )
            {
                auto pCDecider = database.construct< Concrete::Decider >(
                    Concrete::Decider::Args{ Concrete::Context::Args{ pCNode, pIContext }, pDecider } );

                VERIFY_RTE( pParentObject );
                pParentObject->push_back_deciders( pCDecider );
            }
            else if( auto pObject = db_cast< Interface::Object >( pIContext ) )
            {
                pParentObject = database.construct< Concrete::Object >(
                    Concrete::Object::Args{ Concrete::Context::Args{ pCNode, pIContext }, pObject, {} } );
                // object is parent of itself
                pCNode->set_parent_object( pParentObject );

                // construct compiler generated elements - NOTE: set_parent_object in recursion for added aggregates

                // Activation Bitset
                {
                    auto pActivationBitSet
                        = database.construct< Concrete::ActivationBitSet >( Concrete::ActivationBitSet::Args{
                            Concrete::Node::Args{ Concrete::NodeGroup::Args{ {} }, pParentObject, std::nullopt } } );
                    pParentObject->set_activation_bitset( pActivationBitSet );
                    pParentObject->push_back_children( pActivationBitSet );
                }

                // Ownership Link
                {
                    auto pOwnershipLink = database.construct< Concrete::OwnershipLink >(
                        Concrete::OwnershipLink::Args{ Concrete::Link::Args{
                            Concrete::Node::Args{ Concrete::NodeGroup::Args{ {} }, pParentObject, std::nullopt } } } );
                    pParentObject->set_ownership( pOwnershipLink );
                    pParentObject->push_back_children( pOwnershipLink );
                }
            }
            else
            {
                database.construct< Concrete::Context >( Concrete::Context::Args{ pCNode, pIContext } );
            }
        }
        else if( auto pIUserLink = db_cast< Interface::UserLink >( pINode ) )
        {
            auto pUserLink = database.construct< Concrete::UserLink >(
                Concrete::UserLink::Args{ Concrete::Link::Args{ pCNode }, pIUserLink } );
        }
        else
        {
            // leave as Concrete::Node
        }

        VERIFY_RTE( pParentObject );

        for( auto pChild : pCNode->get_children() )
        {
            refine( database, pChild, pParentObject );
        }
    }

    using Realisers = std::multimap< Interface::Node*, Concrete::Node* >;

    void findRealisers( Concrete::Node* pCNode, Realisers& realisers )
    {
        if( auto interfaceNodeOpt = pCNode->get_node_opt() )
        {
            realisers.insert( { interfaceNodeOpt.value(), pCNode } );
        }

        for( auto pChild : pCNode->get_children() )
        {
            findRealisers( pChild, realisers );
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

        Inheritors       inheritors;
        DirectInheritors directInheritors;
        {
            std::vector< Concrete::Node* > objects;
            for( auto pINode : pInterfaceRoot->get_children() )
            {
                inherit( database, pConcreteRoot, pINode, nullptr, objects, inheritors, directInheritors );
            }
            pConcreteRoot->set_children( objects );
        }

        // determine realisers
        Realisers realisers;
        for( auto pCNode : pConcreteRoot->get_children() )
        {
            findRealisers( pCNode, realisers );
        }

        // record all inheritors and realisers
        for( auto pINode : database.many< Interface::Node >( projectManifestPath ) )
        {
            std::vector< Concrete::Node* > inherits;
            {
                for( auto i = inheritors.lower_bound( pINode ), iNext = inheritors.upper_bound( pINode ); i != iNext;
                     ++i )
                {
                    inherits.push_back( i->second );
                }
            }

            std::vector< Concrete::Node* > realise;
            {
                for( auto i = realisers.lower_bound( pINode ), iNext = realisers.upper_bound( pINode ); i != iNext;
                     ++i )
                {
                    realise.push_back( i->second );
                }
            }

            database.construct< Interface::Node >( Interface::Node::Args{ pINode, inherits, realise } );
        }

        for( auto pCNode : pConcreteRoot->get_children() )
        {
            refine( database, pCNode, nullptr );
        }

        // detect flags
        {
            std::map< Interface::IContext*, IContextFlags > contextFlags;
            for( auto pInterface : database.many< Interface::Abstract >( projectManifestPath ) )
            {
                for( U64 i = 1; i != IContextFlags::TOTAL_FLAGS; ++i )
                {
                    const auto bit = static_cast< IContextFlags::Value >( i );
                    if( pInterface->get_symbol()->get_token() == IContextFlags::str( bit ) )
                    {
                        if( IContextFlags::isDirect( bit ) )
                        {
                            for( auto i    = directInheritors.lower_bound( pInterface ),
                                      iEnd = directInheritors.upper_bound( pInterface );
                                 i != iEnd;
                                 ++i )
                            {
                                auto pContext = db_cast< Interface::IContext >( i->second );
                                VERIFY_RTE( pContext );
                                contextFlags[ pContext ].set( bit );
                            }
                        }
                        else
                        {
                            for( auto i    = inheritors.lower_bound( pInterface ),
                                      iEnd = inheritors.upper_bound( pInterface );
                                 i != iEnd;
                                 ++i )
                            {
                                if( auto interfaceNodeOpt = i->second->get_node_opt() )
                                {
                                    if( auto pContext = db_cast< Interface::IContext >( interfaceNodeOpt.value() ) )
                                    {
                                        contextFlags[ pContext ].set( bit );
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for( auto pContext : database.many< Interface::IContext >( projectManifestPath ) )
            {
                database.construct< Interface::IContext >(
                    Interface::IContext::Args{ pContext, contextFlags[ pContext ] } );
            }
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
