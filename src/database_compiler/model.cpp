
#include "model.hpp"
#include "grammar.hpp"

#include "common/string.hpp"

#include <vector>
#include <utility>
#include <map>
#include <algorithm>

namespace db
{
    namespace model
    {
        namespace
        {
            using ObjectPartFile = std::pair< Object::Ptr, schema::IdentifierList >;
            using ObjectPartFileMap = std::map< ObjectPartFile, ObjectPart::Ptr >;
            using FileMap = std::map< schema::IdentifierList, File::Ptr >;
            using StageMap = std::map< schema::Identifier, Stage::Ptr >;
            using NamespaceMap = std::map< schema::Identifier, Namespace::Ptr >;
            using ObjectVector = std::vector< Object::Ptr >;
            using ObjectRefVector = std::vector< RefType::Ptr >;

            struct Mapping
            {
                ObjectPartFileMap objectPartMap;
                FileMap           fileMap;
                StageMap          stageMap;
                NamespaceMap      namespaceMap;
                ObjectVector      objects;
                ObjectRefVector   objectRefs;
            };

            Type::Ptr getType( const schema::Type& type, Mapping& mappings )
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

                                ValueType::Ptr pValueType = std::make_shared< ValueType >();
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
                                ArrayType::Ptr pArray = std::make_shared< ArrayType >();
                                pArray->m_underlyingType = getType( underlyingType, mappings );
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
                                std::ostringstream osCPPTypeName;
                                common::delimit( cppType.m_idList.begin(), cppType.m_idList.end(),
                                                 "::", osCPPTypeName );

                                RefType::Ptr pRefType = std::make_shared< RefType >();
                                mappings.objectRefs.push_back( pRefType );
                                pRefType->m_strObjectType = osCPPTypeName.str();
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
                    Accessor::Ptr pAccessor = std::make_shared< Accessor >();
                    pAccessor->m_stage = pStage;
                    pAccessor->m_bPerObject = false;
                    pAccessor->m_type = getType( accessor.m_type, mapping );
                    pStage->m_accessors.push_back( pAccessor );
                }
                void operator()( const schema::PerObjectAccessor& accessor ) const
                {
                    Accessor::Ptr pAccessor = std::make_shared< Accessor >();
                    pAccessor->m_stage = pStage;
                    pAccessor->m_bPerObject = true;
                    pAccessor->m_type = getType( accessor.m_type, mapping );
                    pStage->m_accessors.push_back( pAccessor );
                }
                void operator()( const schema::Identifier& file ) const
                {
                    const schema::IdentifierList idlist = { pStage->m_strName, file };
                    File::Ptr                    pFile = std::make_shared< File >();
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
                    Namespace::Ptr pChildNamespace = std::make_shared< Namespace >();
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

                ObjectPart::Ptr getObjectPart( const ObjectPartFile& objectPartFile,
                                               File::Ptr             pFile ) const
                {
                    ObjectPart::Ptr             pObjectPart;
                    ObjectPartFileMap::iterator oFind
                        = mapping.objectPartMap.find( objectPartFile );
                    if ( oFind == mapping.objectPartMap.end() )
                    {
                        pObjectPart = std::make_shared< ObjectPart >();
                        pObjectPart->m_object = objectPartFile.first;
                        pObjectPart->m_file = pFile;
                        pObjectPart->m_typeID = mapping.objectPartMap.size();
                        pFile->m_parts.push_back( pObjectPart );
                        objectPartFile.first->m_parts.push_back( pObjectPart );
                        mapping.objectPartMap.insert(
                            std::make_pair( objectPartFile, pObjectPart ) );
                    }
                    else
                    {
                        pObjectPart = oFind->second;
                    }
                    return pObjectPart;
                }

                void operator()( const schema::Object& object ) const
                {
                    Object::Ptr pObject = std::make_shared< Object >();
                    mapping.objects.push_back( pObject );

                    pObject->m_strName = object.m_name;
                    pObject->m_namespace = pNamespace;
                    if ( object.m_optInheritance.has_value() )
                    {
                        const schema::IdentifierList& idlist = object.m_optInheritance.value();
                        if( idlist.size() == 1U )
                        {
                            //use current namespace
                            std::ostringstream            os;
                            os << pNamespace->m_strFullName << "::" << idlist.front();
                            pObject->m_strInheritance = os.str();
                        }
                        else
                        {
                            VERIFY_RTE( idlist.size() > 1U );
                            std::ostringstream            os;
                            common::delimit( idlist.begin(), idlist.end(), "::", os );
                            pObject->m_strInheritance = os.str();
                        }
                    }

                    FileMap::iterator iFind = mapping.fileMap.find( object.m_file );
                    VERIFY_RTE_MSG( iFind != mapping.fileMap.end(),
                                    "Could not find file: " << object.m_file
                                                            << " for object: " << object.m_name );

                    File::Ptr pFile = iFind->second;
                    pObject->m_primaryFile = pFile;

                    ObjectPartFile objectPartFile( pObject, object.m_file );

                    // create first object part if not one already
                    ObjectPart::Ptr pObjectPart = getObjectPart( objectPartFile, pFile );

                    for ( const schema::Property& property : object.m_properties )
                    {
                        Property::Ptr pProperty = std::make_shared< Property >();
                        pProperty->m_strName = property.m_name;
                        if ( property.m_optFile.has_value() )
                        {
                            FileMap::iterator i = mapping.fileMap.find( property.m_optFile.value() );
                            VERIFY_RTE_MSG( i != mapping.fileMap.end(),
                                            "Could not find file: " << property.m_optFile.value()
                                                                    << " for object: " << object.m_name );
                            pObjectPart = getObjectPart(
                                ObjectPartFile( pObject, property.m_optFile.value() ), i->second );
                        }
                        pProperty->m_objectPart = pObjectPart;
                        pObjectPart->m_properties.push_back( pProperty );

                        pProperty->m_type = getType( property.m_type, mapping );

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
                    Stage::Ptr pStage = std::make_shared< Stage >();
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
                    Stage::Ptr pStage = std::make_shared< Stage >();
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
                    Namespace::Ptr pNamespace = std::make_shared< Namespace >();
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

        Schema::Ptr from_ast( const ::db::schema::Schema& schema )
        {
            Mapping mapping;

            Schema::Ptr pSchema = std::make_shared< Schema >();

            SchemaVariantVisitor visitor( mapping, pSchema );

            for ( const schema::SchemaVariant& element : schema.m_elements )
            {
                boost::apply_visitor( visitor, element );
            }

            std::map< std::string, Object::Ptr > objectNames;
            for ( Object::Ptr pObject : mapping.objects )
            {
                std::ostringstream os;
                os << pObject->m_namespace.lock()->m_strFullName << "::" << pObject->m_strName;
                objectNames.insert( std::make_pair( os.str(), pObject ) );
            }

            // hook up object references
            for ( RefType::Ptr pObjectRef : mapping.objectRefs )
            {
                std::map< std::string, Object::Ptr >::const_iterator iFind
                    = objectNames.find( pObjectRef->m_strObjectType );
                VERIFY_RTE_MSG(
                    iFind != objectNames.end(),
                    "Failed to locate object reference type: " << pObjectRef->m_strObjectType );
                pObjectRef->m_object = iFind->second;
            }

            // hook up object inheritance
            for ( Object::Ptr pObject : mapping.objects )
            {
                if ( !pObject->m_strInheritance.empty() )
                {
                    std::map< std::string, Object::Ptr >::const_iterator iFind
                        = objectNames.find( pObject->m_strInheritance );
                    VERIFY_RTE_MSG(
                        iFind != objectNames.end(),
                        "Failed to locate object inheritance type: " << pObject->m_strInheritance );
                    pObject->m_base = iFind->second;
                }
            }

            return pSchema;
        }
    } // namespace model
} // namespace db
