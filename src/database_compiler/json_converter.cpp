#include "json_converter.hpp"

#include "database_compiler/model.hpp"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <fstream>
#include <set>

namespace db
{
    namespace jsonconv
    {
        namespace
        {
            void writeJSON( const boost::filesystem::path& filePath, const nlohmann::json& data )
            {
                std::ofstream os( filePath.native(), std::ios_base::trunc | std::ios_base::out );
                os << data;
            }

            nlohmann::json writeViewProperty( model::Property::Ptr pProperty )
            {
                nlohmann::json property = nlohmann::json::object(
                    { { "name", pProperty->m_strName },
                      { "file", pProperty->m_objectPart.lock()->m_file.lock()->m_strName },
                      { "return_type", pProperty->m_type->getViewType() } } );
                return property;
            }

            using ObjectPartVector = std::vector< model::ObjectPart::Ptr >;
            using ObjectPartMap = std::map< model::Object::Ptr, ObjectPartVector >;

            nlohmann::json writeObject( model::Object::Ptr pObject,
                                        ObjectPartVector&  newParts,
                                        ObjectPartMap&     oldParts )
            {
                model::File::Ptr pPrimaryFile = pObject->m_primaryFile.lock();

                nlohmann::json obj = nlohmann::json::object(
                    { { "name", pObject->m_strName },
                      { "primary_file", pPrimaryFile->m_strName },
                      { "secondary_files", nlohmann::json::array() },
                      { "readonly_properties", nlohmann::json::array() },
                      { "readwrite_properties", nlohmann::json::array() } } );

                // collect the secondary object parts within files of the stage besides the primary
                // file
                std::set< model::File::Ptr > secondaryFiles;
                for ( model::ObjectPart::Ptr pObjectPart : newParts )
                {
                    model::File::Ptr pFile = pObjectPart->m_file.lock();
                    if ( pFile != pPrimaryFile )
                        secondaryFiles.insert( pFile );
                }

                for ( model::File::Ptr pSecondaryFile : secondaryFiles )
                {
                    obj[ "secondary_files" ].push_back( pSecondaryFile->m_strName );
                }

                // work out the namespaces for this object...
                {
                    std::ostringstream    os;
                    model::Namespace::Ptr pNamespace = pObject->m_namespace.lock();
                    VERIFY_RTE( pNamespace );
                    while ( pNamespace )
                    {
                        os << pNamespace->m_strName << "::";
                        obj[ "namespaces" ].push_back( pNamespace->m_strName );
                        pNamespace = pNamespace->m_namespace.lock();
                    }
                    os << pObject->m_strName;
                    obj[ "full_name" ] = os.str();
                }

                ObjectPartMap::iterator iFind = oldParts.find( pObject );
                if ( iFind != oldParts.end() )
                {
                    const ObjectPartVector& readOnlyObjectParts = iFind->second;
                    for ( model::ObjectPart::Ptr pPriorPart : readOnlyObjectParts )
                    {
                        for ( model::Property::Ptr pProperty : pPriorPart->m_properties )
                        {
                            obj[ "readonly_properties" ].push_back(
                                writeViewProperty( pProperty ) );
                        }
                    }
                    std::copy(
                        newParts.begin(), newParts.end(), std::back_inserter( iFind->second ) );
                }
                else if ( !newParts.empty() )
                {
                    oldParts.insert( std::make_pair( pObject, newParts ) );
                }
                for ( model::ObjectPart::Ptr pObjectPart : newParts )
                {
                    for ( model::Property::Ptr pProperty : pObjectPart->m_properties )
                    {
                        obj[ "readwrite_properties" ].push_back( writeViewProperty( pProperty ) );
                    }
                }

                return obj;
            }

            void writeViewData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
            {
                nlohmann::json data;

                data[ "guard" ] = "DATABASE_VIEW_GUARD_4_APRIL_2022";

                ObjectPartMap objectPartsMap;
                using FileVector = std::vector< model::File::Ptr >;
                using ObjectVector = std::vector< model::Object::Ptr >;
                FileVector   filesSoFar;
                ObjectVector objectsSoFar;

                for ( model::Stage::Ptr pStage : pSchema->m_stages )
                {
                    // collect all objects with new elements in the stage
                    ObjectVector stageObjects;
                    {
                        for ( model::File::Ptr pFile : pStage->m_files )
                        {
                            // each object part for files of this stage is read write
                            for ( model::ObjectPart::Ptr pObjectPart : pFile->m_parts )
                            {
                                model::Object::Ptr pObject = pObjectPart->m_object.lock();
                                if ( std::find( stageObjects.begin(), stageObjects.end(), pObject )
                                     == stageObjects.end() )
                                    stageObjects.push_back( pObject );
                            }
                        }
                    }

                    // deterine the objects that need to be defined from previous stages
                    ObjectVector predefinedObjects;
                    {
                        std::sort( stageObjects.begin(), stageObjects.end() );
                        std::sort( objectsSoFar.begin(), objectsSoFar.end() );

                        std::set_difference( objectsSoFar.begin(), objectsSoFar.end(),
                                             stageObjects.begin(), stageObjects.end(),
                                             std::back_inserter( predefinedObjects ) );
                        std::copy( stageObjects.begin(), stageObjects.end(),
                                   std::back_inserter( objectsSoFar ) );
                    }

                    nlohmann::json stage = nlohmann::json::object(
                        { { "name", pStage->m_strName },
                          { "perobject", pStage->m_bPerObject },
                          { "readonly_objects", nlohmann::json::array() },
                          { "readwrite_objects", nlohmann::json::array() },
                          { "readonly_files", nlohmann::json::array() },
                          { "readwrite_files", nlohmann::json::array() } } );

                    // first generate ALL predefined objects
                    for ( model::Object::Ptr pPredefinedObject : predefinedObjects )
                    {
                        ObjectPartVector emptyObjectParts;
                        stage[ "readonly_objects" ].push_back(
                            writeObject( pPredefinedObject, emptyObjectParts, objectPartsMap ) );
                    }

                    for ( model::File::Ptr pFile : filesSoFar )
                    {
                        nlohmann::json readwrite_file
                            = nlohmann::json::object( { { "name", pFile->m_strName } } );
                        stage[ "readonly_files" ].push_back( readwrite_file );
                    }

                    // now generate ALL new definitions
                    ObjectPartMap newObjectParts;
                    for ( model::File::Ptr pFile : pStage->m_files )
                    {
                        nlohmann::json readwrite_file
                            = nlohmann::json::object( { { "name", pFile->m_strName } } );
                        stage[ "readwrite_files" ].push_back( readwrite_file );
                        filesSoFar.push_back( pFile );

                        for ( model::ObjectPart::Ptr pObjectPart : pFile->m_parts )
                        {
                            model::Object::Ptr      pObject = pObjectPart->m_object.lock();
                            ObjectPartMap::iterator iFind = newObjectParts.find( pObject );
                            if ( iFind != newObjectParts.end() )
                            {
                                iFind->second.push_back( pObjectPart );
                            }
                            else
                            {
                                newObjectParts.insert(
                                    std::make_pair( pObject, ObjectPartVector{ pObjectPart } ) );
                            }
                        }
                    }

                    // with all ObjectParts across all files for the stage collated by object
                    // can record the new readwrite objects
                    for ( ObjectPartMap::iterator i = newObjectParts.begin(),
                                                  iEnd = newObjectParts.end();
                          i != iEnd;
                          ++i )
                    {
                        stage[ "readwrite_objects" ].push_back(
                            writeObject( i->first, i->second, objectPartsMap ) );
                    }

                    data[ "stages" ].push_back( stage );
                }

                writeJSON( dataDir / "view.json", data );
            }

            void writeDataData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
            {
                nlohmann::json data( { { "files", nlohmann::json::array() } } );

                data[ "guard" ] = "DATABASE_DATA_GUARD_4_APRIL_2022";

                std::vector< model::File::Ptr > files;
                {
                    for ( model::Stage::Ptr pStage : pSchema->m_stages )
                    {
                        std::copy( pStage->m_files.begin(), pStage->m_files.end(),
                                   std::back_inserter( files ) );
                    }
                }
                for ( model::File::Ptr pFile : files )
                {
                    nlohmann::json file = nlohmann::json::object(
                        { { "name", pFile->m_strName },
                          { "stage", pFile->m_stage.lock()->m_strName },
                          { "perobject", pFile->m_stage.lock()->m_bPerObject },
                          { "parts", nlohmann::json::array() } } );

                    for ( model::ObjectPart::Ptr pPart : pFile->m_parts )
                    {
                        model::Object::Ptr pObject = pPart->m_object.lock();

                        VERIFY_RTE( !pObject->m_parts.empty() );
                        model::ObjectPart::Ptr pPrimaryPart = pObject->m_parts.front();
                        const bool             bIsPrimaryPart = pPrimaryPart == pPart;

                        nlohmann::json part = nlohmann::json::object(
                            { { "name", pObject->m_strName },
                              { "typeID", pPart->m_typeID },
                              { "pointers", nlohmann::json::array() },
                              { "properties", nlohmann::json::array() } } );

                        if ( bIsPrimaryPart )
                        {
                            for ( model::ObjectPart::Ptr pObjectPart : pObject->m_parts )
                            {
                                if ( pObjectPart != pPart )
                                {
                                    nlohmann::json pointer = nlohmann::json::object(
                                        { { "namespace", pObjectPart->m_file.lock()->m_strName },
                                          { "name", pObjectPart->m_object.lock()->m_strName } } );
                                    part[ "pointers" ].push_back( pointer );
                                }
                            }
                        }
                        else
                        {
                            nlohmann::json pointer = nlohmann::json::object(
                                { { "namespace", pPrimaryPart->m_file.lock()->m_strName },
                                  { "name", pPrimaryPart->m_object.lock()->m_strName } } );
                            part[ "pointers" ].push_back( pointer );
                        }

                        for ( model::Property::Ptr pProperty : pPart->m_properties )
                        {
                            model::Type::Ptr pType = pProperty->m_type;
                            nlohmann::json   property
                                = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                            { "type", pType->getDataType() } } );

                            part[ "properties" ].push_back( property );
                        }

                        file[ "parts" ].push_back( part );
                    }

                    data[ "files" ].push_back( file );
                }
                writeJSON( dataDir / "data.json", data );
            }

        } // namespace

        void toJSON( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
        {
            writeViewData( dataDir, pSchema );
            writeDataData( dataDir, pSchema );
        }

    } // namespace jsonconv
} // namespace db
