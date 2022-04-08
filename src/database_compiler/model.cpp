
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

        std::string ObjectPart::getDataType( const std::string& strDelimiter ) const
        {
            std::ostringstream osObjectPartType;
            {
                osObjectPartType << m_file.lock()->m_strName << strDelimiter
                                 << m_object.lock()->m_strName;
            }
            return osObjectPartType.str();
        }

        std::string ObjectPart::getPointerName() const
        {
            std::ostringstream osObjectPartType;
            {
                osObjectPartType << "p_" << m_file.lock()->m_strName << "_"
                                 << m_object.lock()->m_strName;
            }
            return osObjectPartType.str();
        }

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
                    // pFile->m_stage.lock()->m_readWriteObjects.push_back( pObject );

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
                            File::Ptr pObjectPartFile = i->second;

                            VERIFY_RTE_MSG(
                                i != mapping.fileMap.end(),
                                "Could not find file: " << property.m_optFile.value()
                                                        << " for object: " << object.m_name );

                            Stage::Ptr pObjectStage = pFile->m_stage.lock();
                            Stage::Ptr pObjectPartStage = pObjectPartFile->m_stage.lock();

                            // ensure the object part is from current or later stage
                            VERIFY_RTE_MSG(
                                pObjectStage->getCounter() <= pObjectPartStage->getCounter(),
                                "Property: " << property.m_name
                                             << " specifies file which has earlier stage: "
                                             << pObjectPartStage->m_strName
                                             << " than containing object: " << object.m_name
                                             << " which is: " << pObjectStage->m_strName );

                            pObjectPart = getObjectPart(
                                ObjectFilePair( pObject, property.m_optFile.value() ),
                                pObjectPartFile, false );
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

        void getObjects( Namespace::Ptr pNamespace, std::vector< Object::Ptr >& objects )
        {
            for ( Object::Ptr pObject : pNamespace->m_objects )
            {
                objects.push_back( pObject );
            }
            for ( Namespace::Ptr pIter : pNamespace->m_namespaces )
            {
                getObjects( pIter, objects );
            }
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
                Object::Ptr pObject = findType( pSchema, i->second );
                i->first->m_base = pObject;
                pObject->m_deriving.push_back( i->first );
            }

            using ObjectPtrVector = std::vector< ObjectPart::Ptr >;
            using ObjectPartMap
                = std::map< Object::Ptr, ObjectPtrVector, CountedObjectComparator< Object::Ptr > >;

            // once an object exists it WILL ALWAYS exist in every stage from then on
            // so we only need to know the objects in the previous and current stage
            // each interface must correspond to an object.

            Stage::Ptr pPreviousStage;
            for ( Stage::Ptr pStage : pSchema->m_stages )
            {
                // collect the new accumulation parts
                ObjectPartMap objectParts;
                for ( File::Ptr pFile : pStage->m_files )
                {
                    for ( ObjectPart::Ptr pPart : pFile->m_parts )
                    {
                        Object::Ptr             pObject = pPart->m_object.lock();
                        ObjectPartMap::iterator iFind = objectParts.find( pObject );
                        if ( iFind != objectParts.end() )
                        {
                            iFind->second.push_back( pPart );
                        }
                        else
                        {
                            objectParts.insert(
                                std::make_pair( pObject, ObjectPtrVector{ pPart } ) );
                        }
                    }
                }

                using InterfaceMap = std::map< Object::Ptr, Interface::Ptr >;
                InterfaceMap interfaceMap;

                for ( ObjectPartMap::iterator i = objectParts.begin(), iEnd = objectParts.end();
                      i != iEnd; ++i )
                {
                    Interface::Ptr pInterface = std::make_shared< Interface >( mapping.counter );
                    std::copy( i->second.begin(), i->second.end(),
                               std::back_inserter( pInterface->m_readWriteObjectParts ) );
                    pStage->m_readWriteInterfaces.push_back( pInterface );
                    pInterface->m_object = i->first;
                    interfaceMap.insert( std::make_pair( i->first, pInterface ) );
                }

                if ( pPreviousStage )
                {
                    std::vector< Interface::Ptr > previousInterfaces;
                    std::copy( pPreviousStage->m_readWriteInterfaces.begin(),
                               pPreviousStage->m_readWriteInterfaces.end(),
                               std::back_inserter( previousInterfaces ) );
                    std::copy( pPreviousStage->m_readOnlyInterfaces.begin(),
                               pPreviousStage->m_readOnlyInterfaces.end(),
                               std::back_inserter( previousInterfaces ) );
                    for ( Interface::Ptr pPreviousInterface : previousInterfaces )
                    {
                        Object::Ptr            pObject = pPreviousInterface->m_object.lock();
                        InterfaceMap::iterator iFind = interfaceMap.find( pObject );
                        if ( iFind == interfaceMap.end() )
                        {
                            Interface::Ptr pInterface
                                = std::make_shared< Interface >( mapping.counter );
                            interfaceMap.insert( std::make_pair( pObject, pInterface ) );
                            pInterface->m_readOnlyObjectParts
                                = pPreviousInterface->m_readOnlyObjectParts;
                            std::copy( pPreviousInterface->m_readWriteObjectParts.begin(),
                                       pPreviousInterface->m_readWriteObjectParts.end(),
                                       std::back_inserter( pInterface->m_readOnlyObjectParts ) );
                            pInterface->m_object = pPreviousInterface->m_object;
                            pStage->m_readOnlyInterfaces.push_back( pInterface );
                        }
                        else
                        {
                            Interface::Ptr pNewInterface = iFind->second;
                            std::copy( pPreviousInterface->m_readOnlyObjectParts.begin(),
                                       pPreviousInterface->m_readOnlyObjectParts.end(),
                                       std::back_inserter( pNewInterface->m_readOnlyObjectParts ) );
                            std::copy( pPreviousInterface->m_readWriteObjectParts.begin(),
                                       pPreviousInterface->m_readWriteObjectParts.end(),
                                       std::back_inserter( pNewInterface->m_readOnlyObjectParts ) );
                        }
                    }
                }

                // create interface functions and set base interfaces
                for ( Interface::Ptr pInterface : pStage->m_readOnlyInterfaces )
                {
                    Object::Ptr pObject = pInterface->m_object.lock();
                    if ( pObject->m_base )
                    {
                        InterfaceMap::iterator iFind = interfaceMap.find( pObject->m_base );
                        VERIFY_RTE( iFind != interfaceMap.end() );
                        pInterface->m_base = iFind->second;
                    }
                    VERIFY_RTE( pInterface->m_readWriteObjectParts.empty() );
                    for ( ObjectPart::Ptr pObjectPart : pInterface->m_readOnlyObjectParts )
                    {
                        for ( Property::Ptr pProperty : pObjectPart->m_properties )
                        {
                            if ( pProperty->isGet() )
                            {
                                FunctionGetter::Ptr pGetter
                                    = std::make_shared< FunctionGetter >( mapping.counter );
                                pGetter->m_property = pProperty;
                                pInterface->m_functions.push_back( pGetter );
                            }
                        }
                    }
                }
                for ( Interface::Ptr pInterface : pStage->m_readWriteInterfaces )
                {
                    Object::Ptr pObject = pInterface->m_object.lock();
                    if ( pObject->m_base )
                    {
                        InterfaceMap::iterator iFind = interfaceMap.find( pObject->m_base );
                        VERIFY_RTE( iFind != interfaceMap.end() );
                        pInterface->m_base = iFind->second;
                    }
                    for ( ObjectPart::Ptr pObjectPart : pInterface->m_readWriteObjectParts )
                    {
                        for ( Property::Ptr pProperty : pObjectPart->m_properties )
                        {
                            if ( pProperty->isGet() )
                            {
                                FunctionGetter::Ptr pGetter
                                    = std::make_shared< FunctionGetter >( mapping.counter );
                                pGetter->m_property = pProperty;
                                pInterface->m_functions.push_back( pGetter );
                            }
                            if ( pProperty->isSet() )
                            {
                                FunctionSetter::Ptr pSetter
                                    = std::make_shared< FunctionSetter >( mapping.counter );
                                pSetter->m_property = pProperty;
                                pInterface->m_functions.push_back( pSetter );
                            }
                        }
                    }
                    for ( ObjectPart::Ptr pObjectPart : pInterface->m_readOnlyObjectParts )
                    {
                        for ( Property::Ptr pProperty : pObjectPart->m_properties )
                        {
                            if ( pProperty->isGet() )
                            {
                                FunctionGetter::Ptr pGetter
                                    = std::make_shared< FunctionGetter >( mapping.counter );
                                pGetter->m_property = pProperty;
                                pInterface->m_functions.push_back( pGetter );
                            }
                        }
                    }
                }

                // create stage functions
                for ( Interface::Ptr pInterface : pStage->m_readWriteInterfaces )
                {
                    std::vector< Interface::Ptr > interfacesCreatedThisStage{ pInterface };
                    std::vector< Interface::Ptr > interfacesCreatedPreviously;
                    Interface::Ptr                pBase = pInterface->m_base;
                    while ( pBase )
                    {
                        if ( !pBase->m_readWriteObjectParts.empty() )
                        {
                            interfacesCreatedThisStage.push_back( pBase );
                        }
                        else
                        {
                            interfacesCreatedPreviously.push_back( pBase );
                        }
                        pBase = pBase->m_base;
                    }

                    const bool bInterfaceHasPrimaryPart
                        = pInterface->m_readWriteObjectParts.end()
                          == std::find( pInterface->m_readWriteObjectParts.begin(),
                                        pInterface->m_readWriteObjectParts.end(),
                                        pInterface->m_object.lock()->m_primaryObjectPart );

                    if ( !interfacesCreatedPreviously.empty() )
                    {
                        Interface::Ptr pPrevious = interfacesCreatedPreviously.front();

                        // create refinement from previous stage interface
                        Refinement::Ptr pRefinement
                            = std::make_shared< Refinement >( mapping.counter );
                        pRefinement->m_fromInterface = pPrevious;
                        pRefinement->m_toInterface = pInterface;
                        pRefinement->m_stage = pStage;

                        for ( Interface::Ptr pInherited : interfacesCreatedThisStage )
                        {
                            for ( ObjectPart::Ptr pPart : pInherited->m_readWriteObjectParts )
                            {
                                for ( Property::Ptr pProperty : pPart->m_properties )
                                {
                                    if ( pProperty->isCtorParam() )
                                        pRefinement->m_parameters.push_back( pProperty );
                                }
                            }
                        }

                        pStage->m_refinements.push_back( pRefinement );
                    }
                    else
                    {
                        Constructor::Ptr pConstructor
                            = std::make_shared< Constructor >( mapping.counter );
                        pConstructor->m_interface = pInterface;
                        pConstructor->m_stage = pStage;
                        for ( Interface::Ptr pInherited : interfacesCreatedThisStage )
                        {
                            for ( ObjectPart::Ptr pPart : pInherited->m_readWriteObjectParts )
                            {
                                for ( Property::Ptr pProperty : pPart->m_properties )
                                {
                                    if ( pProperty->isCtorParam() )
                                        pConstructor->m_parameters.push_back( pProperty );
                                }
                            }
                        }
                        pStage->m_constructors.push_back( pConstructor );
                    }

                    for ( std::vector< Interface::Ptr >::iterator i
                          = interfacesCreatedThisStage.begin(),
                          iEnd = interfacesCreatedThisStage.end();
                          i != iEnd;
                          ++i )
                    {
                        Interface::Ptr pFrom = *i;
                        if ( pFrom != pInterface )
                        {
                            Refinement::Ptr pRefinement
                                = std::make_shared< Refinement >( mapping.counter );
                            pRefinement->m_fromInterface = pFrom;
                            pRefinement->m_toInterface = pInterface;
                            pRefinement->m_stage = pStage;

                            for ( std::vector< Interface::Ptr >::iterator j = i + 1; j != iEnd;
                                  ++j )
                            {
                                Interface::Ptr pInherited = *j;
                                for ( ObjectPart::Ptr pPart : pInherited->m_readWriteObjectParts )
                                {
                                    for ( Property::Ptr pProperty : pPart->m_properties )
                                    {
                                        if ( pProperty->isCtorParam() )
                                            pRefinement->m_parameters.push_back( pProperty );
                                    }
                                }
                            }
                            pStage->m_refinements.push_back( pRefinement );
                        }
                    }
                }

                pPreviousStage = pStage;
            }

            // calculate the stage super interfaces
            for ( Stage::Ptr pStage : pSchema->m_stages )
            {
                std::vector< Interface::Ptr > interfaces = pStage->m_readOnlyInterfaces;
                std::copy( pStage->m_readWriteInterfaces.begin(),
                           pStage->m_readWriteInterfaces.end(),
                           std::back_inserter( interfaces ) );

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
                                existingGroup.erase(
                                    std::unique( existingGroup.begin(), existingGroup.end() ),
                                    existingGroup.end() );
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

                    // calculate topological sort of the inheritance tree
                    std::vector< Interface::Ptr > topological;
                    {
                        std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > >
                            closed;
                        std::set< Interface::Ptr, CountedObjectComparator< Interface::Ptr > > open(
                            group.begin(), group.end() );

                        while ( !open.empty() )
                        {
                            bool bFound = false;
                            for ( Interface::Ptr pIter : open )
                            {
                                if ( pIter->m_base )
                                {
                                    if ( closed.count( pIter->m_base ) )
                                    {
                                        bFound = true;
                                        closed.insert( pIter );
                                        topological.push_back( pIter );
                                        open.erase( pIter );
                                        break;
                                    }
                                }
                                else
                                {
                                    bFound = true;
                                    closed.insert( pIter );
                                    topological.push_back( pIter );
                                    open.erase( pIter );
                                    break;
                                }
                            }
                            if ( !bFound )
                            {
                                THROW_RTE( "Failed to solve topological sort of group" );
                            }
                        }
                    }
                    std::copy( topological.begin(), topological.end(),
                               std::back_inserter( pStage->m_interfaceTopological ) );
                }
            }

            // calculate the object part conversions
            {
                std::vector< Object::Ptr > objects;
                {
                    for ( Namespace::Ptr pNamespace : pSchema->m_namespaces )
                    {
                        getObjects( pNamespace, objects );
                    }
                }

                for ( Object::Ptr pObject : objects )
                {
                    // to self case
                    pSchema->m_conversions.insert(
                        std::make_pair( Schema::ObjectPartPair{ pObject->m_primaryObjectPart,
                                                                pObject->m_primaryObjectPart },
                                        Schema::ObjectPartVector{} ) );

                    for ( ObjectPart::Ptr pSecondary : pObject->m_secondaryParts )
                    {
                        pSchema->m_conversions.insert( std::make_pair(
                            Schema::ObjectPartPair{ pObject->m_primaryObjectPart, pSecondary },
                            Schema::ObjectPartVector{ pSecondary } ) );
                    }

                    Object::Ptr              pBase = pObject->m_base;
                    Schema::ObjectPartVector baseList;
                    while ( pBase )
                    {
                        baseList.push_back( pBase->m_primaryObjectPart );

                        pSchema->m_conversions.insert(
                            std::make_pair( Schema::ObjectPartPair{ pObject->m_primaryObjectPart,
                                                                    pBase->m_primaryObjectPart },
                                            baseList ) );

                        for ( ObjectPart::Ptr pSecondary : pObject->m_secondaryParts )
                        {
                            Schema::ObjectPartVector baseListPlusSecondary = baseList;
                            baseListPlusSecondary.push_back( pSecondary );
                            pSchema->m_conversions.insert( std::make_pair(
                                Schema::ObjectPartPair{ pObject->m_primaryObjectPart, pSecondary },
                                baseListPlusSecondary ) );
                        }

                        pBase = pBase->m_base;
                    }
                }
            }

            return pSchema;
        }
    } // namespace model
} // namespace db
