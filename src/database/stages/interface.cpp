//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#include "database/stages/interface.hpp"

#include "database/model/interface.hpp"
#include "database/model/allocator.hpp"
#include "database/model/concrete.hpp"

#include <iostream>

namespace mega
{
namespace Stages
{
    Interface::Interface( const boost::filesystem::path& treePath )
        : Appending( treePath, io::Object::NO_FILE )
    {
        m_pDerivationAnalysis = io::oneOpt< DerivationAnalysis >( getMaster() );
        if ( !m_pDerivationAnalysis )
            m_pDerivationAnalysis = construct< DerivationAnalysis >();

        m_pTranslationUnitAnalysis = io::oneOpt< TranslationUnitAnalysis >( getMaster() );
        if ( !m_pTranslationUnitAnalysis )
            m_pTranslationUnitAnalysis = construct< TranslationUnitAnalysis >();

        m_pLinkAnalysis = io::oneOpt< LinkAnalysis >( getMaster() );
        if ( !m_pLinkAnalysis )
            m_pLinkAnalysis = construct< LinkAnalysis >();
    }

    void Interface::store( const boost::filesystem::path& filePath ) const
    {
        VERIFY_RTE( m_fileMap.size() == 1U );
        io::File* pFile = m_fileMap.find( io::Object::NO_FILE )->second;
        VERIFY_RTE( pFile );
        io::File::FileIDtoPathMap files;
        files.insert( std::make_pair( io::Object::NO_FILE, filePath ) );
        io::File::store( filePath, io::Object::NO_FILE, files, pFile->getObjects() );
    }

    void collateChildren( const interface::Context*                   pAction,
                          std::vector< const interface::Context* >&   actions,
                          std::vector< const interface::Dimension* >& dimensions )
    {
        for ( const interface::Element* pObject : pAction->getChildren() )
        {
            switch ( pObject->getType() )
            {
            case eAbstractOpaque:
            case eAbstractInclude:
            case eAbstractUsing:
            case eAbstractExport:
                break;
            case eAbstractDimension:
                dimensions.push_back( dynamic_cast< const interface::Dimension* >( pObject ) );
                break;
            case eAbstractObject:
                break;
            case eAbstractAbstract:
            case eAbstractEvent:
            case eAbstractFunction:
            case eAbstractAction:
            case eAbstractLink:
            case eAbstractRoot:
                if ( const interface::Context* pChildAction = dynamic_cast< const interface::Context* >( pObject ) )
                {
                    actions.push_back( dynamic_cast< const interface::Context* >( pObject ) );
                }
                break;
            default:
                THROW_RTE( "Unsupported type" );
                break;
            }
        }
    }

    concrete::Inheritance_Node* Interface::constructInheritanceNode( concrete::Action*           pConcreteAction,
                                                                     concrete::Inheritance_Node* pParent, const interface::Context* pAbstractAction )
    {
        concrete::Inheritance_Node* pInheritanceNode = construct< concrete::Inheritance_Node >();
        pInheritanceNode->m_pRootConcreteAction = pConcreteAction;
        pInheritanceNode->m_pParent = pParent;
        pInheritanceNode->m_pContext = pAbstractAction;

        if ( pParent )
            pParent->m_children.push_back( pInheritanceNode );

        m_pDerivationAnalysis->m_inheritanceMap.insert( std::make_pair( pAbstractAction, pInheritanceNode ) );

        return pInheritanceNode;
    }

    concrete::Inheritance_Node* Interface::constructInheritanceTree( concrete::Action*           pConcreteAction,
                                                                     concrete::Inheritance_Node* pParent, const interface::Context* pAbstractAction )
    {
        concrete::Inheritance_Node* pInheritanceNode
            = constructInheritanceNode( pConcreteAction, pParent, pAbstractAction );

        if ( !dynamic_cast< const interface::Link* >( pAbstractAction ) && !dynamic_cast< const interface::Function* >( pAbstractAction ) )
        {
            for ( const interface::Context* pBaseAbstractAction : pAbstractAction->m_baseContexts )
            {
                if ( dynamic_cast< const interface::Link* >( pBaseAbstractAction ) )
                {
                    THROW_RTE( "Invalid use of link type in inheritance list for: " << pAbstractAction->getIdentifier() );
                }
                constructInheritanceTree( pConcreteAction, pInheritanceNode, pBaseAbstractAction );
            }
        }

        return pInheritanceNode;
    }

    void Interface::constructInheritanceTree( concrete::Action* pConcreteAction )
    {
        // every concrete Action has an inheritance node tree describing how it inherits abstract interface types
        // each inheritance node has the same m_pRootConcreteAction which is pConcreteAction here.
        pConcreteAction->m_inheritance = constructInheritanceTree( pConcreteAction, nullptr, pConcreteAction->getContext() );
    }

    using ActionOverrideMap = std::map< interface::Context*, concrete::Action*, Interface::CompareNodeIdentity< interface::Context > >;
    using DimensionOverrideMap = std::map< interface::Dimension*, concrete::Dimension*, Interface::CompareNodeIdentity< interface::Dimension > >;

    void Interface::calculateInstanceActionName( concrete::Action* pAction )
    {
        if ( pAction->m_pParent )
        {
            concrete::Action* pParent = dynamic_cast< concrete::Action* >( pAction->m_pParent );
            if ( pParent->m_strName.empty() )
            {
                std::ostringstream os;
                os << pAction->getContext()->getIdentifier(); // << "_" << pAction->getIndex();
                pAction->m_strName = os.str();
            }
            else
            {
                std::ostringstream os;
                os << pParent->m_strName << '_' << pAction->getContext()->getIdentifier(); // << "_" << pAction->getIndex();
                pAction->m_strName = os.str();
            }
        }
    }

    void Interface::collateOverrides(
        concrete::Action*                              pInstance,
        concrete::Inheritance_Node*                    pInheritanceNode,
        ActionOverrideMap&                             actionInstances,
        DimensionOverrideMap&                          dimensionInstances,
        const std::vector< const interface::Object* >& objects,
        std::set< const interface::Object* >&          constructedObjects )
    {
        // get all the member actions and dimensions for the concrete Action we are processing
        std::vector< const interface::Context* >   actions;
        std::vector< const interface::Dimension* > dimensions;
        collateChildren( pInheritanceNode->m_pContext, actions, dimensions );

        // if this is the component root then add ALL objects
        // if( pInstance->getParent() && !pInstance->getParent()->getParent() )

        if ( const interface::Root* pRoot = dynamic_cast< const interface::Root* >( pInstance->getContext() ) )
        {
            if ( ( pRoot->getRootType() == eFileRoot ) || ( pRoot->getRootType() == eProjectName ) )
            {
                // std::copy( objects.begin(), objects.end(), std::back_inserter( actions ) );
                for ( const interface::Object* pObject : objects )
                {
                    bool bFoundParentRoot = false;
                    for ( const interface::Context* pIter = pObject; pIter;
                          pIter = dynamic_cast< const interface::Context* >( pIter->getParent() ) )
                    {
                        if ( pIter == pRoot )
                        {
                            bFoundParentRoot = true;
                            break;
                        }
                    }

                    if ( bFoundParentRoot )
                    {
                        VERIFY_RTE( !constructedObjects.count( pObject ) );
                        constructedObjects.insert( pObject );

                        // create the child instance node
                        concrete::Action* pChildInstance = construct< concrete::Action >();
                        pInstance->m_children.push_back( pChildInstance );
                        pChildInstance->m_pElement = pObject;
                        pChildInstance->m_pParent = pInstance;
                        calculateInstanceActionName( pChildInstance );

                        m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pObject, pChildInstance ) );

                        // record it in the inheritance node
                        pInheritanceNode->m_actions.push_back( pChildInstance );

                        actionInstances.insert( std::make_pair( pObject, pChildInstance ) );
                    }
                }
            }
        }

        for ( const interface::Context* pChildAction : actions )
        {
            if ( !pChildAction->isAbstract() )
            {
                std::map< const interface::Context*, concrete::Action* >::iterator iFind = actionInstances.find( pChildAction );
                if ( iFind == actionInstances.end() )
                {
                    // create the child instance node
                    concrete::Action* pChildInstance = construct< concrete::Action >();
                    pInstance->m_children.push_back( pChildInstance );
                    pChildInstance->m_pElement = pChildAction;
                    pChildInstance->m_pParent = pInstance;
                    calculateInstanceActionName( pChildInstance );

                    m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pChildAction, pChildInstance ) );

                    // record it in the inheritance node
                    pInheritanceNode->m_actions.push_back( pChildInstance );

                    actionInstances.insert( std::make_pair( pChildAction, pChildInstance ) );
                }
                else
                {
                    pInheritanceNode->m_actions.push_back( iFind->second );
                }
            }
        }

        for ( const interface::Dimension* pChildDimension : dimensions )
        {
            std::map< const interface::Dimension*, concrete::Dimension* >::iterator iFind = dimensionInstances.find( pChildDimension );
            if ( iFind == dimensionInstances.end() )
            {
                concrete::Dimension_User* pChildInstance = construct< concrete::Dimension_User >();
                pInstance->m_children.push_back( pChildInstance );
                pChildInstance->m_pElement = pChildDimension;
                pChildInstance->m_pParent = pInstance;

                m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pChildDimension, pChildInstance ) );

                pInheritanceNode->m_dimensions.push_back( pChildInstance );

                dimensionInstances.insert( std::make_pair( pChildDimension, pChildInstance ) );
            }
            else
            {
                pInheritanceNode->m_dimensions.push_back( iFind->second );
            }
        }

        // recurse over the inheritance node tree for the type to inherit and override their members into the concrete type
        for ( concrete::Inheritance_Node* pChildInheritance : pInheritanceNode->m_children )
        {
            collateOverrides( pInstance, pChildInheritance, actionInstances, dimensionInstances, objects, constructedObjects );
        }
    }

    void Interface::constructInstance( concrete::Action*                              pInstance,
                                       const std::vector< const interface::Object* >& objects,
                                       std::set< const interface::Object* >&          constructedObjects )
    {
        constructInheritanceTree( pInstance );

        ActionOverrideMap    actionInstances;
        DimensionOverrideMap dimensionInstances;

        collateOverrides( pInstance, pInstance->m_inheritance, actionInstances, dimensionInstances, objects, constructedObjects );

        for ( concrete::Element* pChild : pInstance->m_children )
        {
            if ( concrete::Action* pAction = dynamic_cast< concrete::Action* >( pChild ) )
            {
                constructInstance( pAction, objects, constructedObjects );
            }
        }
    }

    void Interface::constructAllocator( concrete::Action* pInstance, concrete::Action* pObject, std::vector< concrete::Allocator* >& allocators )
    {
        if ( dynamic_cast< const interface::Object* >( pInstance->getContext() ) )
            pObject = pInstance;

        pInstance->m_pObject = pObject;

        {
            std::vector< concrete::Element* > temp = pInstance->m_children;
            for ( concrete::Element* pChild : temp )
            {
                if ( concrete::Action* pChildAction = dynamic_cast< concrete::Action* >( pChild ) )
                {
                    if ( const interface::Abstract* pContext = dynamic_cast< const interface::Abstract* >( pChildAction->getContext() ) )
                    {
                        // do nothing
                    }
                    else if ( const interface::Event* pContext = dynamic_cast< const interface::Event* >( pChildAction->getContext() ) )
                    {
                        // generate allocator ONLY if event has user dimensions
                        if ( pChildAction->hasUserDimensions() )
                        {
                            concrete::NothingAllocator* pAllocator = construct< concrete::NothingAllocator >();
                            pAllocator->m_pContext_Allocating = pInstance;
                            pAllocator->m_pContext_Allocated = pChildAction;
                            pChildAction->m_pAllocator = pAllocator;
                            allocators.push_back( pChildAction->m_pAllocator );
                            pInstance->m_allocators.insert( std::make_pair( pChildAction, pChildAction->m_pAllocator ) );
                        }
                        else
                        {
                            pChildAction->m_pAllocator = chooseAllocator( *this, pInstance, pChildAction );
                            allocators.push_back( pChildAction->m_pAllocator );
                            pInstance->m_allocators.insert( std::make_pair( pChildAction, pChildAction->m_pAllocator ) );
                        }
                    }
                    else if ( const interface::Function* pContext = dynamic_cast< const interface::Function* >( pChildAction->getContext() ) )
                    {
                        concrete::NothingAllocator* pAllocator = construct< concrete::NothingAllocator >();
                        pAllocator->m_pContext_Allocating = pInstance;
                        pAllocator->m_pContext_Allocated = pChildAction;
                        pChildAction->m_pAllocator = pAllocator;
                        allocators.push_back( pChildAction->m_pAllocator );
                        pInstance->m_allocators.insert( std::make_pair( pChildAction, pChildAction->m_pAllocator ) );
                    }
                    else if ( const interface::Action* pContext = dynamic_cast< const interface::Action* >( pChildAction->getContext() ) )
                    {
                        pChildAction->m_pAllocator = chooseAllocator( *this, pInstance, pChildAction );
                        allocators.push_back( pChildAction->m_pAllocator );
                        pInstance->m_allocators.insert( std::make_pair( pChildAction, pChildAction->m_pAllocator ) );
                        constructAllocator( pChildAction, pObject, allocators );
                    }
                    else if ( const interface::Object* pContext = dynamic_cast< const interface::Object* >( pChildAction->getContext() ) )
                    {
                        pChildAction->m_pAllocator = chooseAllocator( *this, pInstance, pChildAction );
                        allocators.push_back( pChildAction->m_pAllocator );
                        pInstance->m_allocators.insert( std::make_pair( pChildAction, pChildAction->m_pAllocator ) );
                        constructAllocator( pChildAction, pObject, allocators );
                    }
                    else if ( const interface::Link* pContext = dynamic_cast< const interface::Link* >( pChildAction->getContext() ) )
                    {
                        pChildAction->m_pAllocator = chooseAllocator( *this, pInstance, pChildAction );
                        allocators.push_back( pChildAction->m_pAllocator );
                        pInstance->m_allocators.insert( std::make_pair( pChildAction, pChildAction->m_pAllocator ) );
                        constructAllocator( pChildAction, pObject, allocators );
                    }
                    else
                    {
                        THROW_RTE( "Unknown context type" );
                    }
                }
            }
        }
    }

    concrete::Action* Interface::instanceAnalysis()
    {
        // construct the root
        const interface::Context* pActionRoot = io::root< const interface::Object >( getAppendingObjects() );
        concrete::Action*         pRoot = construct< concrete::Action >();
        {
            pRoot->m_pElement = pActionRoot;
            pRoot->m_pParent = nullptr;
            calculateInstanceActionName( pRoot );
            m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pActionRoot, pRoot ) );
        }

        // collate the object list
        std::vector< const interface::Object* > objects;
        {
            std::vector< const interface::Object* > allObjects = io::many< const interface::Object >( getAppendingObjects() );
            for ( std::vector< const interface::Object* >::iterator
                      i
                  = allObjects.begin(),
                  iEnd = allObjects.end();
                  i != iEnd;
                  ++i )
            {
                const interface::Object* pObject = *i;
                if ( pObject != pActionRoot )
                {
                    if ( !dynamic_cast< const interface::Root* >( pObject ) )
                    {
                        objects.push_back( pObject );
                    }
                }
            }
        }

        std::set< const interface::Object* > constructedObjects;
        constructInstance( pRoot, objects, constructedObjects );

        VERIFY_RTE( objects.size() == constructedObjects.size() );

        // construct the allocators
        {
            std::vector< concrete::Allocator* > allocators;
            constructAllocator( pRoot, pRoot, allocators );
            for ( concrete::Allocator* pAllocator : allocators )
            {
                pAllocator->constructDimensions( *this );
            }
        }

        // calculate the compatibility between static and dynamic types
        {
            std::vector< const interface::Context* > interfaceActions = io::many_cst< const interface::Context >(
                getObjects( io::Object::NO_FILE ) );

            std::vector< const concrete::Inheritance_Node* > iNodes = io::many_cst< const concrete::Inheritance_Node >(
                getObjects( io::Object::NO_FILE ) );

            m_pDerivationAnalysis->analyseCompatibility( interfaceActions, iNodes );
        }

        return pRoot;
    }

    void Interface::linkAnalysis()
    {
        // calculate the link groups
        {
            std::vector< interface::Context* > contexts = io::many< interface::Context >( getMaster() );
            LinkAnalysis::ContextSetPtrSet     linkSets;
            linkSets = m_pLinkAnalysis->calculateSets( contexts );
            m_pLinkAnalysis->calculateGroups( linkSets, contexts, *m_pDerivationAnalysis, *this );
        }

        // generate link dimensions in target concrete types
        const LinkGroup::Vector& groups = m_pLinkAnalysis->getLinkGroups();
        for ( LinkGroup* pLinkGroup : groups )
        {
            for ( concrete::Action* pTarget : pLinkGroup->getTargets() )
            {
                concrete::Dimension_Generated* pLinkDimension = construct< concrete::Dimension_Generated >();
                pLinkDimension->m_type = concrete::Dimension_Generated::eLinkReference;
                pLinkDimension->m_pLinkGroup = pLinkGroup;
                pLinkDimension->m_pParent = pTarget;
                pTarget->m_links.insert( std::make_pair( pLinkGroup->getLinkName(), pLinkDimension ) );
                pTarget->m_children.push_back( pLinkDimension );
                pLinkGroup->m_dimensionMap.insert( std::make_pair( pTarget, pLinkDimension ) );
            }

            for ( interface::Link* pLink : pLinkGroup->getLinks() )
            {
                const interface::Dimension* pInterfaceDimension = pLink->getLinkBaseDimension();
                VERIFY_RTE( pInterfaceDimension );

                std::vector< const concrete::Element* > instances;
                m_pDerivationAnalysis->getInstances( pInterfaceDimension, instances, false );

                for ( const concrete::Element* pElement : instances )
                {
                    const concrete::Dimension_User* pUserDim = dynamic_cast< const concrete::Dimension_User* >( pElement );
                    VERIFY_RTE( pUserDim );
                    const_cast< concrete::Dimension_User* >( pUserDim )->m_pLinkGroup = pLinkGroup;
                }
            }
        }

        // final steps in derivation analysis
        {
            std::vector< const interface::Context* > interfaceActions = io::many_cst< const interface::Context >(
                getObjects( mega::io::Object::NO_FILE ) );
            m_pDerivationAnalysis->analyseLinkCompatibility( interfaceActions, groups );
        }
    }

    void Interface::translationUnitAnalysis_recurse( concrete::Action* pAction, TranslationUnitMap& translationUnitMap )
    {
        const interface::Context* pInterfaceAction = pAction->getContext();

        TranslationUnit::CoordinatorHostnameDefinitionFile coordinatorHostnameDefinitionFile;
        coordinatorHostnameDefinitionFile.definitionFile = pInterfaceAction->getDefinitionFile();

        if ( !pInterfaceAction->getCoordinatorHostname(
                 coordinatorHostnameDefinitionFile.pCoordinator,
                 coordinatorHostnameDefinitionFile.pHostName ) )
        {
            //?
        }

        TranslationUnitMap::iterator iFind = translationUnitMap.find( coordinatorHostnameDefinitionFile );
        if ( iFind != translationUnitMap.end() )
        {
            iFind->second.insert( pInterfaceAction );
        }
        else
        {
            translationUnitMap.insert( std::make_pair( coordinatorHostnameDefinitionFile,
                                                       TranslationUnit::ActionSet{ pInterfaceAction } ) );
        }

        for ( concrete::Element* pChild : pAction->m_children )
        {
            if ( concrete::Action* pChildAction = dynamic_cast< concrete::Action* >( pChild ) )
            {
                translationUnitAnalysis_recurse( pChildAction, translationUnitMap );
            }
        }
    }

    void Interface::translationUnitAnalysis( const boost::filesystem::path& rootFolder, TUFileIDIfExistsFPtr pTUFileIDIfExists )
    {
        concrete::Action* pRoot = io::root< concrete::Action >( getAppendingObjects() );

        TranslationUnitMap translationUnitMap;
        translationUnitAnalysis_recurse( pRoot, translationUnitMap );

        // first transform each eg source file to a fileName
        std::set< io::Object::FileID > usedTUFileIDs;
        std::set< TranslationUnit* >   unassignedTUs;

        for ( TranslationUnitMap::const_iterator
                  i
              = translationUnitMap.begin(),
              iEnd = translationUnitMap.end();
              i != iEnd;
              ++i )
        {
            TranslationUnit* pTranslationUnit = construct< TranslationUnit >();
            pTranslationUnit->m_chd = i->first;
            pTranslationUnit->m_actions = i->second;
            //pTranslationUnit->m_strName = TranslationUnit::TUNameFromEGSource( rootFolder, pTranslationUnit->m_chd );

            // Useful logging of translation units to check which actions are in which translation units and which have no definition file
            // pTranslationUnit->print( std::cout );

            // std::cout << "intermediate: " << rootFolder.string() <<
            //   " definition file: " << ( i->first.definitionFile ? i->first.definitionFile.value() : "none" ) <<
            //   " name: " << pTranslationUnit->m_strName << std::endl;

            //pTranslationUnit->m_databaseFileID = pTUFileIDIfExists( pTranslationUnit->m_strName );

            m_pTranslationUnitAnalysis->m_translationUnits.push_back( pTranslationUnit );

            for ( const interface::Context* pAction : pTranslationUnit->m_actions )
            {
                m_pTranslationUnitAnalysis->m_actionTUMap.insert(
                    std::make_pair( pAction, pTranslationUnit ) );
            }

            // now determine if the file actually exists and get the File ID if it does
            if ( pTranslationUnit->m_databaseFileID != io::Object::NO_FILE )
            {
                /*if( usedTUFileIDs.find( pTranslationUnit->m_databaseFileID ) != usedTUFileIDs.end() )
                {
                    THROW_RTE( "Unreachable" );
                }*/
                usedTUFileIDs.insert( pTranslationUnit->m_databaseFileID );
            }
            else
            {
                unassignedTUs.insert( pTranslationUnit );
            }
        }

        // assign FileIDs for unassigned
        {
            std::set< io::Object::FileID >::iterator i = usedTUFileIDs.begin();
            io::Object::FileID                       nextLogicalID = mega::io::Object::NO_FILE;
            for ( TranslationUnit* pTranslationUnit : unassignedTUs )
            {
                while ( i != usedTUFileIDs.end() )
                {
                    if ( nextLogicalID == *i )
                    {
                        ++nextLogicalID;
                        ++i;
                    }
                    else
                    {
                        break;
                    }
                }
                pTranslationUnit->m_databaseFileID = nextLogicalID;
                ++nextLogicalID;
            }
        }
    }

} // namespace Stages
} // namespace mega
