
#include "model.hpp"
#include "grammar.hpp"

#include "common/string.hpp"

namespace db
{
namespace model
{
    using ObjectPartFile = std::pair< Object::Ptr, schema::Identifier >;
    using ObjectPartFileMap = std::map< ObjectPartFile, ObjectPart::Ptr >;
    using FileMap = std::map< schema::Identifier, File::Ptr >;
    using StageMap = std::map< schema::Identifier, Stage::Ptr >;
    using NamespaceMap = std::map< schema::Identifier, Namespace::Ptr >;

    struct Mapping
    {
        ObjectPartFileMap objectPartMap;
        FileMap           fileMap;
        StageMap          stageMap;
        NamespaceMap      namespaceMap;
    };

    Schema::Ptr from_ast( const ::db::schema::Schema& schema )
    {
        Mapping mapping;

        Schema::Ptr pSchema = std::make_shared< Schema >();

        struct FileVariantVisitor : boost::static_visitor< void >
        {
            Stage::Ptr pStage;
            Mapping&   mapping;
            FileVariantVisitor( Mapping& mapping, Stage::Ptr pStage )
                : mapping( mapping )
                , pStage( pStage )
            {
            }
            void operator()( const schema::PerObjectFile& file ) const
            {
                PerObjectFile::Ptr pFile = std::make_shared< PerObjectFile >();

                FileMap::iterator iFind = mapping.fileMap.find( file );
                VERIFY_RTE_MSG(
                    iFind == mapping.fileMap.end(), "Duplicate file name found: " << file );
                mapping.fileMap.insert( std::make_pair( file, pFile ) );

                pStage->m_files.push_back( pFile );
            }
            void operator()( const schema::PerProgramFile& file ) const
            {
                PerProgramFile::Ptr pFile = std::make_shared< PerProgramFile >();

                FileMap::iterator iFind = mapping.fileMap.find( file );
                VERIFY_RTE_MSG(
                    iFind == mapping.fileMap.end(), "Duplicate file name found: " << file );
                mapping.fileMap.insert( std::make_pair( file, pFile ) );

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
                pChildNamespace->m_strFullName = pNamespace->m_strName + "::" + namespace_.m_name;

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
                ObjectPartFileMap::iterator oFind = mapping.objectPartMap.find( objectPartFile );
                if ( oFind == mapping.objectPartMap.end() )
                {
                    pObjectPart = std::make_shared< ObjectPart >();
                    pObjectPart->m_object = objectPartFile.first;
                    pObjectPart->m_file = pFile;
                    pFile->m_parts.push_back( pObjectPart );
                    mapping.objectPartMap.insert( std::make_pair( objectPartFile, pObjectPart ) );
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

                pObject->m_strName = object.m_name;

                FileMap::iterator iFind = mapping.fileMap.find( object.m_file );
                VERIFY_RTE_MSG(
                    iFind != mapping.fileMap.end(),
                    "Could not find file: " << object.m_file << " for object: " << object.m_name );

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
                        pObjectPart = getObjectPart(
                            ObjectPartFile( pObject, property.m_optFile.value() ), pFile );
                    }
                    pProperty->m_objectPart = pObjectPart;

                    const schema::Type& type = property.m_type;

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
                                    common::delimit( cppType.m_idList.begin(),
                                                     cppType.m_idList.end(), "::", osCPPTypeName );

                                    ValueType::Ptr pValueType = std::make_shared< ValueType >();
                                    pValueType->m_cppType = osCPPTypeName.str();

                                    pProperty->m_type = pValueType;
                                }
                            }
                        }
                        else
                        {
                        }
                    }

                    VERIFY_RTE_MSG( pProperty->m_type,
                        "Failed to resolve type for property: " << property.m_name );
                }

                pNamespace->m_objects.push_back( pObject );
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

            void operator()( const schema::Stage& stage ) const
            {
                Stage::Ptr pStage = std::make_shared< Stage >();
                pStage->m_strName = stage.m_name;

                StageMap::iterator iFind = mapping.stageMap.find( stage.m_name );
                VERIFY_RTE_MSG( iFind == mapping.stageMap.end(),
                                "Duplicate stage name found: " << stage.m_name );
                mapping.stageMap.insert( std::make_pair( stage.m_name, pStage ) );

                FileVariantVisitor visitor( mapping, pStage );
                for ( const schema::FileVariant& file : stage.m_files )
                {
                    boost::apply_visitor( visitor, file );
                }

                pSchema->m_stages.push_back( pStage );
            }

            void operator()( const schema::Namespace& namespace_ ) const
            {
                Namespace::Ptr pNamespace = std::make_shared< Namespace >();
                pNamespace->m_strName = namespace_.m_name;
                pNamespace->m_strFullName = namespace_.m_name;

                NamespaceMap::iterator iFind
                    = mapping.namespaceMap.find( pNamespace->m_strFullName );
                VERIFY_RTE_MSG( iFind == mapping.namespaceMap.end(),
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
        SchemaVariantVisitor visitor( mapping, pSchema );

        for ( const schema::SchemaVariant& element : schema.m_elements )
        {
            boost::apply_visitor( visitor, element );
        }

        return pSchema;
    }
} // namespace model
} // namespace db
