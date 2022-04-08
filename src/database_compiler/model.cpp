
#include "model.hpp"
#include "grammar.hpp"

#include "common/string.hpp"

#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <set>

namespace db
{
    namespace model
    {
        namespace
        {

            class NameResolution
            {
            public:
                Namespace::Ptr         m_namespace;
                schema::IdentifierList id;
            };

            using TypeNameResMap = std::map< RefType::Ptr, NameResolution >;
            using InheritanceNameResMap = std::map< Object::Ptr, NameResolution >;

            // every object can have at most a single object part in each file
            using ObjectFilePair = std::pair< Object::Ptr, schema::IdentifierList >;
            class ObjectFilePairComparator
            {
            public:
                bool operator()( const ObjectFilePair& left, const ObjectFilePair& right ) const
                {
                    return left.first->getCounter() < right.first->getCounter();
                }
            };

            using ObjectFilePairToObjectPartMap
                = std::map< ObjectFilePair, ObjectPart::Ptr, ObjectFilePairComparator >;
            using FileMap = std::map< schema::IdentifierList, File::Ptr >;
            using StageMap = std::map< schema::Identifier, Stage::Ptr >;
            using NamespaceMap = std::map< schema::Identifier, Namespace::Ptr >;
            using ObjectVector = std::vector< Object::Ptr >;
            using ObjectRefVector = std::vector< RefType::Ptr >;
            using InheritanceMap = std::map< schema::IdentifierList, Object::Ptr >;

            struct Mapping
            {
                Counter counter = 0U;

                TypeNameResMap        typeNameResMap;
                InheritanceNameResMap inheritanceNameResMap;

                ObjectFilePairToObjectPartMap objectPartMap;
                FileMap                       fileMap;
                StageMap                      stageMap;
                NamespaceMap                  namespaceMap;
                ObjectVector                  objects;
                ObjectRefVector               objectRefs;
            };

            Type::Ptr getType( const schema::Type& type, Mapping& mapping,
                               Namespace::Ptr pNamespace )
            {
                if ( type.m_idList.size() == 1U )
                {
                    const std::string& strID = type.m_idList.front();

                    if ( strID == "value" )
                    {
                        // value< typename >
                        if ( type.m_children.size() == 1U )
                        {
                            const schema::Type& cppType = type.m_children.front();
                            if ( cppType.m_children.empty() && !cppType.m_idList.empty() )
                            {
                                std::ostringstream osCPPTypeName;
                                common::delimit( cppType.m_idList.begin(), cppType.m_idList.end(),
                                                 "::", osCPPTypeName );

                                ValueType::Ptr pValueType
                                    = std::make_shared< ValueType >( mapping.counter );
                                pValueType->m_cppType = osCPPTypeName.str();
                                return pValueType;
                            }
                        }
                    }
                    else if ( strID == "array" )
                    {
                        // array< value< cpptype > >
                        if ( type.m_children.size() == 1U )
                        {
                            const schema::Type& underlyingType = type.m_children.front();
                            if ( !underlyingType.m_children.empty()
                                 && !underlyingType.m_idList.empty() )
                            {
                                ArrayType::Ptr pArray
                                    = std::make_shared< ArrayType >( mapping.counter );
                                pArray->m_underlyingType
                                    = getType( underlyingType, mapping, pNamespace );
                                return pArray;
                            }
                        }
                    }
                    else if ( strID == "ref" )
                    {
                        // ref< ObjectType >
                        if ( type.m_children.size() == 1U )
                        {
                            const schema::Type& cppType = type.m_children.front();
                            if ( cppType.m_children.empty() && !cppType.m_idList.empty() )
                            {
                                RefType::Ptr pRefType
                                    = std::make_shared< RefType >( mapping.counter );
                                mapping.objectRefs.push_back( pRefType );
                                // record the type name for later resolution
                                mapping.typeNameResMap.insert( std::make_pair(
                                    pRefType, NameResolution{ pNamespace, cppType.m_idList } ) );
                                return pRefType;
                            }
                        }
                    }
                }
                THROW_RTE( "Failed to resolve type for: " << type );
            }

            struct StageElementVariantVisitor : boost::static_visitor< void >
            {
                Stage::Ptr pStage;
                Mapping&   mapping;
                StageElementVariantVisitor( Mapping& mapping, Stage::Ptr pStage )
                    : mapping( mapping )
                    , pStage( pStage )
                {
                }
                void operator()( const schema::PerProgramAccessor& accessor ) const
                {
                    Accessor::Ptr pAccessor = std::make_shared< Accessor >( mapping.counter );
                    pAccessor->m_stage = pStage;
                    pAccessor->m_bPerObject = false;
                    pAccessor->m_type = getType( accessor.m_type, mapping, Namespace::Ptr() );
                    pStage->m_accessors.push_back( pAccessor );
                }
                void operator()( const schema::PerObjectAccessor& accessor ) const
                {
                    Accessor::Ptr pAccessor = std::make_shared< Accessor >( mapping.counter );
                    pAccessor->m_stage = pStage;
                    pAccessor->m_bPerObject = true;
                    pAccessor->m_type = getType( accessor.m_type, mapping, Namespace::Ptr() );
                    pStage->m_accessors.push_back( pAccessor );
                }
                void operator()( const schema::Identifier& file ) const
                {
                    const schema::IdentifierList idlist = { pStage->m_strName, file };
                    File::Ptr pFile = std::make_shared< File >( mapping.counter );
                    pFile->m_strName = file;
                    pFile->m_stage = pStage;
                    FileMap::iterator iFind = mapping.fileMap.find( idlist );
                    VERIFY_RTE_MSG(
                        iFind == mapping.fileMap.end(), "Duplicate file name found: " << idlist );
                    mapping.fileMap.insert( std::make_pair( idlist, pFile ) );
                    pStage->m_files.push_back( pFile );
                }
            };

            struct NamespaceVariantVisitor : boost::static_visitor< void >
            {
                Namespace::Ptr pNamespace;
                Mapping&       mapping;
                NamespaceVariantVisitor( Mapping& mapping, Namespace::Ptr pNamespace )
                    : mapping( mapping )
                    , pNamespace( pNamespace )
                {
                }
                void operator()( const schema::Namespace& namespace_ ) const
                {
                    Namespace::Ptr pChildNamespace
                        = std::make_shared< Namespace >( mapping.counter );
                    pChildNamespace->m_strName = namespace_.m_name;
                    pChildNamespace->m_strFullName
                        = pNamespace->m_strName + "::" + namespace_.m_name;

                    NamespaceMap::iterator iFind
                        = mapping.namespaceMap.find( pChildNamespace->m_strFullName );
                    VERIFY_RTE_MSG(
                        iFind == mapping.namespaceMap.end(),
                        "Duplicate namespace name found: " << pChildNamespace->m_strFullName );
                    mapping.namespaceMap.insert(
                        std::make_pair( pChildNamespace->m_strFullName, pChildNamespace ) );

                    NamespaceVariantVisitor visitor( mapping, pChildNamespace );
                    for ( const schema::NamespaceVariant& element : namespace_.m_elements )
                    {
                        boost::apply_visitor( visitor, element );
                    }

                    pNamespace->m_namespaces.push_back( pChildNamespace );
                }

                ObjectPart::Ptr getObjectPart( const ObjectFilePair& objectFilePair,
                                               File::Ptr pFile, bool bIsPrimary ) const
                {
                    ObjectFilePairToObjectPartMap::iterator iFind
                        = mapping.objectPartMap.find( objectFilePair );

                    Object::Ptr pObject = objectFilePair.first;

                    if ( bIsPrimary )
                    {
                        VERIFY_RTE( iFind == mapping.objectPartMap.end() );
                        PrimaryObjectPart::Ptr pPrimaryObjectPart
                            = std::make_shared< PrimaryObjectPart >( mapping.counter );
                        pPrimaryObjectPart->m_object = pObject;
                        pPrimaryObjectPart->m_file = pFile;
                        pPrimaryObjectPart->m_typeID = mapping.objectPartMap.size();

                        pFile->m_parts.push_back( pPrimaryObjectPart );

                        pObject->m_primaryObjectPart = pPrimaryObjectPart;

                        return pPrimaryObjectPart;
                    }
                    else if ( iFind == mapping.objectPartMap.end() )
                    {
                        SecondaryObjectPart::Ptr pSecondaryObjectPart
                            = std::make_shared< SecondaryObjectPart >( mapping.counter );

                        pSecondaryObjectPart->m_object = pObject;
                        pSecondaryObjectPart->m_file = pFile;
                        pSecondaryObjectPart->m_typeID = mapping.objectPartMap.size();

                        pFile->m_parts.push_back( pSecondaryObjectPart );

                        pObject->m_secondaryParts.push_back( pSecondaryObjectPart );

                        mapping.objectPartMap.insert(
                            std::make_pair( objectFilePair, pSecondaryObjectPart ) );

                        return pSecondaryObjectPart;
                    }
                    else
                    {
                        return iFind->second;
                    }
                }

                void operator()( const schema::Object& object ) const
                {
                    Object::Ptr pObject = std::make_shared< Object >( mapping.counter );
                    mapping.objects.push_back( pObject );

                    pObject->m_strName = object.m_name;
                    pObject->m_namespace = pNamespace;
                    if ( object.m_optInheritance.has_value() )
                    {
                        mapping.inheritanceNameResMap.insert( std::make_pair(
                            pObject,
                            NameResolution{ pNamespace, object.m_optInheritance.value() } ) );
                    }

                    FileMap::iterator iFind = mapping.fileMap.find( object.m_file );
                    VERIFY_RTE_MSG( iFind != mapping.fileMap.end(),
                                    "Could not find file: " << object.m_file
                                                            << " for object: " << object.m_name );

                    File::Ptr pFile = iFind->second;
                    pObject->m_primaryFile = pFile;
                    pObject->m_stage = pFile->m_stage;
                    pFile->m_stage.lock()->m_readWriteObjects.push_back( pObject );

                    ObjectFilePair objectFilePair( pObject, object.m_file );

                    // create first object part if not one already
                    ObjectPart::Ptr pObjectPart = getObjectPart( objectFilePair, pFile, true );

                    for ( const schema::Property& property : object.m_properties )
                    {
                        Property::Ptr pProperty = std::make_shared< Property >( mapping.counter );
                        pProperty->m_strName = property.m_name;
                        if ( property.m_optFile.has_value() )
                        {
                            FileMap::iterator i
                                = mapping.fileMap.find( property.m_optFile.value() );
                            VERIFY_RTE_MSG(
                                i != mapping.fileMap.end(),
                                "Could not find file: " << property.m_optFile.value()
                                                        << " for object: " << object.m_name );
                            pObjectPart = getObjectPart(
                                ObjectFilePair( pObject, property.m_optFile.value() ), i->second,
                                false );
                        }
                        pProperty->m_objectPart = pObjectPart;
                        pObjectPart->m_properties.push_back( pProperty );

                        pProperty->m_type = getType( property.m_type, mapping, pNamespace );

                        VERIFY_RTE_MSG(
                            pProperty->m_type,
                            "Failed to resolve type for property: " << property.m_name );
                    }

                    pNamespace->m_objects.push_back( pObject );
                }
            };

            struct StageVariantVisitor : boost::static_visitor< void >
            {
                Mapping&    mapping;
                Schema::Ptr pSchema;
                StageVariantVisitor( Mapping& mapping, Schema::Ptr pSchema )
                    : mapping( mapping )
                    , pSchema( pSchema )
                {
                }

                void operator()( const schema::PerObjectStage& stage ) const
                {
                    Stage::Ptr pStage = std::make_shared< Stage >( mapping.counter );
                    pStage->m_strName = stage.m_name;
                    pStage->m_bPerObject = true;

                    StageMap::iterator iFind = mapping.stageMap.find( stage.m_name );
                    VERIFY_RTE_MSG( iFind == mapping.stageMap.end(),
                                    "Duplicate stage name found: " << stage.m_name );
                    mapping.stageMap.insert( std::make_pair( stage.m_name, pStage ) );

                    StageElementVariantVisitor visitor( mapping, pStage );
                    for ( const schema::StageElementVariant& file : stage.m_elements )
                    {
                        boost::apply_visitor( visitor, file );
                    }

                    pSchema->m_stages.push_back( pStage );
                }

                void operator()( const schema::PerProgramStage& stage ) const
                {
                    Stage::Ptr pStage = std::make_shared< Stage >( mapping.counter );
                    pStage->m_strName = stage.m_name;
                    pStage->m_bPerObject = false;

                    StageMap::iterator iFind = mapping.stageMap.find( stage.m_name );
                    VERIFY_RTE_MSG( iFind == mapping.stageMap.end(),
                                    "Duplicate stage name found: " << stage.m_name );
                    mapping.stageMap.insert( std::make_pair( stage.m_name, pStage ) );

                    StageElementVariantVisitor visitor( mapping, pStage );
                    for ( const schema::StageElementVariant& file : stage.m_elements )
                    {
                        boost::apply_visitor( visitor, file );
                    }

                    pSchema->m_stages.push_back( pStage );
                }
            };

            struct SchemaVariantVisitor : boost::static_visitor< void >
            {
                Mapping&    mapping;
                Schema::Ptr pSchema;
                SchemaVariantVisitor( Mapping& mapping, Schema::Ptr pSchema )
                    : mapping( mapping )
                    , pSchema( pSchema )
                {
                }

                void operator()( const schema::StageVariant& stage ) const
                {
                    StageVariantVisitor visitor( mapping, pSchema );
                    boost::apply_visitor( visitor, stage );
                }

                void operator()( const schema::Namespace& namespace_ ) const
                {
                    Namespace::Ptr pNamespace = std::make_shared< Namespace >( mapping.counter );
                    pNamespace->m_strName = namespace_.m_name;
                    pNamespace->m_strFullName = namespace_.m_name;

                    NamespaceMap::iterator iFind
                        = mapping.namespaceMap.find( pNamespace->m_strFullName );
                    VERIFY_RTE_MSG(
                        iFind == mapping.namespaceMap.end(),
                        "Duplicate namespace name found: " << pNamespace->m_strFullName );
                    mapping.namespaceMap.insert(
                        std::make_pair( pNamespace->m_strFullName, pNamespace ) );

                    NamespaceVariantVisitor visitor( mapping, pNamespace );
                    for ( const schema::NamespaceVariant& element : namespace_.m_elements )
                    {
                        boost::apply_visitor( visitor, element );
                    }

                    pSchema->m_namespaces.push_back( pNamespace );
                }
            };
        } // namespace

        void checkAmbiguity( Object::Ptr pResult, const NameResolution& nameRes )
        {
            if ( nameRes.m_namespace )
            {
                VERIFY_RTE_MSG( !pResult,
                                "Ambiguous name resolution for: "
                                    << nameRes.id
                                    << " from namespace: " << nameRes.m_namespace->m_strFullName );
            }
            else
            {
                VERIFY_RTE_MSG( !pResult, "Ambiguous name resolution for: " << nameRes.id );
            }
        }

        Object::Ptr findType( const NameResolution& nameRes, Namespace::Ptr pNamespace,
                              schema::IdentifierList::const_iterator i,
                              schema::IdentifierList::const_iterator iEnd )
        {
            Object::Ptr pResult;

            const std::size_t szDist = std::distance( i, iEnd );
            if ( szDist == 1U )
            {
                for ( Object::Ptr pObject : pNamespace->m_objects )
                {
                    if ( pObject->m_strName == *i )
                    {
                        checkAmbiguity( pResult, nameRes );
                        pResult = pObject;
                    }
                }
            }
            else if ( szDist > 1U )
            {
                for ( Namespace::Ptr pNestedNamespace : pNamespace->m_namespaces )
                {
                    if ( pNestedNamespace->m_strName == *i )
                    {
                        if ( Object::Ptr pObject
                             = findType( nameRes, pNestedNamespace, i + 1, iEnd ) )
                        {
                            checkAmbiguity( pResult, nameRes );
                            pResult = pObject;
                        }
                    }
                }
            }
            return pResult;
        }

        Object::Ptr findType( Schema::Ptr pSchema, const NameResolution& nameRes )
        {
            Object::Ptr pResult;

            // attempt to search from the namespace outwards
            {
                Namespace::Ptr pNamespace = nameRes.m_namespace;
                while ( pNamespace )
                {
                    if ( Object::Ptr pObject
                         = findType( nameRes, pNamespace, nameRes.id.begin(), nameRes.id.end() ) )
                    {
                        checkAmbiguity( pResult, nameRes );
                        pResult = pObject;
                    }
                    pNamespace = pNamespace->m_namespace.lock();
                }
            }

            // search all global namespaces
            if ( !pResult )
            {
                const std::size_t szDist = std::distance( nameRes.id.begin(), nameRes.id.end() );
                if ( szDist > 1U )
                {
                    for ( Namespace::Ptr pNamespace : pSchema->m_namespaces )
                    {
                        if ( pNamespace->m_strName == *nameRes.id.begin() )
                        {
                            if ( Object::Ptr pObject
                                 = findType( nameRes, pNamespace, nameRes.id.begin() + 1,
                                             nameRes.id.end() ) )
                            {
                                checkAmbiguity( pResult, nameRes );
                                pResult = pObject;
                            }
                        }
                    }
                }
            }

            if ( nameRes.m_namespace )
            {
                VERIFY_RTE_MSG( pResult,
                                "Failed to resolve name resolution for: "
                                    << nameRes.id
                                    << " from namespace: " << nameRes.m_namespace->m_strFullName );
            }
            else
            {
                VERIFY_RTE_MSG( pResult, "Failed to resolve name resolution for: " << nameRes.id );
            }

            return pResult;
        }

        Schema::Ptr from_ast( const ::db::schema::Schema& schema )
        {
            Mapping mapping;

            Schema::Ptr pSchema = std::make_shared< Schema >( mapping.counter );

            SchemaVariantVisitor visitor( mapping, pSchema );

            for ( const schema::SchemaVariant& element : schema.m_elements )
            {
                boost::apply_visitor( visitor, element );
            }

            // resolve RefType type names
            for ( TypeNameResMap::iterator i = mapping.typeNameResMap.begin(),
                                           iEnd = mapping.typeNameResMap.end();
                  i != iEnd;
                  ++i )
            {
                i->first->m_object = findType( pSchema, i->second );
            }

            // resolve object inheritance type names
            for ( InheritanceNameResMap::iterator i = mapping.inheritanceNameResMap.begin(),
                                                  iEnd = mapping.inheritanceNameResMap.end();
                  i != iEnd;
                  ++i )
            {
                i->first->m_base = findType( pSchema, i->second );
            }

            // calculate the accumulated read only objects for each stage
            {
                std::vector< std::weak_ptr< Object > > objectsSoFar;
                for ( Stage::Ptr pStage : pSchema->m_stages )
                {
                    pStage->m_readOnlyObjects = objectsSoFar;
                    // for each stage add the new set of objects introduced by the stage
                    std::copy( pStage->m_readWriteObjects.begin(), pStage->m_readWriteObjects.end(),
                               std::back_inserter( objectsSoFar ) );
                }
            }

            // calculate the stage interfaces
            for ( Stage::Ptr pStage : pSchema->m_stages )
            {
                using InterfaceMap = std::map< Object::Ptr, Interface::Ptr >;
                InterfaceMap interfaceMap;
                // for each stage add the new set of objects introduced by the stage
                for ( std::weak_ptr< Object > pObjectWeak : pStage->m_readOnlyObjects )
                {
                    Object::Ptr pObject = pObjectWeak.lock();

                    Interface::Ptr pInterface = std::make_shared< Interface >( mapping.counter );
                    pInterface->m_object = pObject;
                    interfaceMap.insert( std::make_pair( pObject, pInterface ) );

                    for ( Property::Ptr pProperty : pObject->m_primaryObjectPart->m_properties )
                    {
                        FunctionGetter::Ptr pGetter
                            = std::make_shared< FunctionGetter >( mapping.counter );
                        pGetter->m_property = pProperty;
                        pInterface->m_functions.push_back( pGetter );
                    }

                    for ( SecondaryObjectPart::Ptr pPart : pObject->m_secondaryParts )
                    {
                        for ( Property::Ptr pProperty : pPart->m_properties )
                        {
                            FunctionGetter::Ptr pGetter
                                = std::make_shared< FunctionGetter >( mapping.counter );
                            pGetter->m_property = pProperty;
                            pInterface->m_functions.push_back( pGetter );
                        }
                    }
                    pStage->m_interfaces.push_back( pInterface );
                }
                for ( std::weak_ptr< Object > pObjectWeak : pStage->m_readWriteObjects )
                {
                    Object::Ptr pObject = pObjectWeak.lock();

                    Interface::Ptr pInterface = std::make_shared< Interface >( mapping.counter );
                    pInterface->m_object = pObject;
                    interfaceMap.insert( std::make_pair( pObject, pInterface ) );

                    for ( Property::Ptr pProperty : pObject->m_primaryObjectPart->m_properties )
                    {
                        FunctionGetter::Ptr pGetter
                            = std::make_shared< FunctionGetter >( mapping.counter );
                        pGetter->m_property = pProperty;
                        pInterface->m_functions.push_back( pGetter );

                        FunctionSetter::Ptr pSetter
                            = std::make_shared< FunctionSetter >( mapping.counter );
                        pGetter->m_property = pProperty;
                        pInterface->m_functions.push_back( pSetter );
                    }

                    for ( SecondaryObjectPart::Ptr pPart : pObject->m_secondaryParts )
                    {
                        for ( Property::Ptr pProperty : pPart->m_properties )
                        {
                            FunctionGetter::Ptr pGetter
                                = std::make_shared< FunctionGetter >( mapping.counter );
                            pGetter->m_property = pProperty;
                            pInterface->m_functions.push_back( pGetter );

                            FunctionSetter::Ptr pSetter
                                = std::make_shared< FunctionSetter >( mapping.counter );
                            pGetter->m_property = pProperty;
                            pInterface->m_functions.push_back( pSetter );
                        }
                    }
                    pStage->m_interfaces.push_back( pInterface );
                }

                // resolve interface inheritance
                for ( Interface::Ptr pInterface : pStage->m_interfaces )
                {
                    Object::Ptr pObject = pInterface->m_object.lock();
                    if ( pObject->m_base )
                    {
                        InterfaceMap::iterator iFind = interfaceMap.find( pObject->m_base );
                        VERIFY_RTE( iFind != interfaceMap.end() );
                        pInterface->m_base = iFind->second;
                    }
                }
            }

            // calculate the stage super interfaces
            for ( Stage::Ptr pStage : pSchema->m_stages )
            {
                std::vector< Interface::Ptr > interfaces = pStage->m_interfaces;

                std::sort( interfaces.begin(), interfaces.end(),
                           CountedObjectComparator< Interface::Ptr >() );

                std::vector< std::vector< Interface::Ptr > > disjointInheritanceSets;
                {
                    std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > remaining(
                        interfaces.begin(), interfaces.end() );
                    while ( !remaining.empty() )
                    {
                        Interface::Ptr pInterface = *remaining.begin();

                        std::vector< Interface::Ptr > group;
                        {
                            while ( pInterface )
                            {
                                remaining.erase( pInterface );
                                group.push_back( pInterface );
                                pInterface = pInterface->m_base;
                            }
                            std::sort( group.begin(), group.end(),
                                       CountedObjectComparator< Interface::Ptr >() );
                        }

                        bool bFound = false;
                        for ( std::vector< Interface::Ptr >& existingGroup :
                              disjointInheritanceSets )
                        {
                            // is the object in the group?
                            std::vector< Interface::Ptr > intersection;
                            std::set_intersection( group.begin(), group.end(),
                                                   existingGroup.begin(), existingGroup.end(),
                                                   std::back_inserter( intersection ) );
                            if ( !intersection.empty() )
                            {
                                std::copy( group.begin(), group.end(),
                                           std::back_inserter( existingGroup ) );
                                std::sort( existingGroup.begin(), existingGroup.end(),
                                           CountedObjectComparator< Interface::Ptr >() );
                                bFound = true;
                                break;
                            }
                        }

                        if ( !bFound )
                        {
                            // add a new group
                            disjointInheritanceSets.push_back( group );
                        }
                    }
                }
                for ( const std::vector< Interface::Ptr >& group : disjointInheritanceSets )
                {
                    SuperInterface::Ptr pSuperInterface
                        = std::make_shared< SuperInterface >( mapping.counter );
                    pSuperInterface->m_stage = pStage;
                    pSuperInterface->m_interfaces = group;
                    pStage->m_superInterfaces.push_back( pSuperInterface );

                    for ( Interface::Ptr pInterface : pSuperInterface->m_interfaces )
                    {
                        pInterface->m_superInterface = pSuperInterface;
                    }
                }
            }

            return pSchema;
        }
    } // namespace model
} // namespace db
