#include "json_converter.hpp"

#include "database_compiler/model.hpp"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <fstream>

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
                      { "return_type", pProperty->m_type->getReturnType() } } );
                return property;
            }

            using ObjectPartVector = std::vector< model::ObjectPart::Ptr >;
            using ObjectPartMap = std::map< model::Object::Ptr, ObjectPartVector >;

            nlohmann::json writeObject( model::Object::Ptr     pObject,
                                        ObjectPartMap&         objectPartsVector,
                                        model::ObjectPart::Ptr pNewObjectPart )
            {
                nlohmann::json obj = nlohmann::json::object(
                    { { "name", pObject->m_strName },
                      { "readonly_properties", nlohmann::json::array() },
                      { "readwrite_properties", nlohmann::json::array() } } );

                // work out the namespaces for this object...
                {
                    model::Namespace::Ptr pNamespace = pObject->m_namespace.lock();
                    VERIFY_RTE( pNamespace );
                    while ( pNamespace )
                    {
                        obj[ "namespaces" ].push_back( pNamespace->m_strName );
                        pNamespace = pNamespace->m_namespace.lock();
                    }
                }

                ObjectPartMap::iterator iFind = objectPartsVector.find( pObject );
                if ( iFind != objectPartsVector.end() )
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
                    if ( pNewObjectPart )
                        iFind->second.push_back( pNewObjectPart );
                }
                else if ( pNewObjectPart )
                {
                    objectPartsVector.insert(
                        std::make_pair( pObject, ObjectPartVector{ pNewObjectPart } ) );
                }
                if ( pNewObjectPart )
                {
                    for ( model::Property::Ptr pProperty : pNewObjectPart->m_properties )
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

                ObjectPartMap objectPartsVector;
                // using ObjectSet = std::set< model::Object::Ptr >;
                using ObjectVector = std::vector< model::Object::Ptr >;
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
                          { "readonly_objects", nlohmann::json::array() },
                          { "readwrite_objects", nlohmann::json::array() } } );

                    // first generate ALL predefined objects
                    for ( model::Object::Ptr pPredefinedObject : predefinedObjects )
                    {
                        stage[ "readonly_objects" ].push_back( writeObject(
                            pPredefinedObject, objectPartsVector, model::ObjectPart::Ptr() ) );
                    }

                    // now generate ALL new definitions
                    for ( model::File::Ptr pFile : pStage->m_files )
                    {
                        // each object part for files of this stage is read write
                        for ( model::ObjectPart::Ptr pObjectPart : pFile->m_parts )
                        {
                            model::Object::Ptr pObject = pObjectPart->m_object.lock();
                            stage[ "readwrite_objects" ].push_back(
                                writeObject( pObject, objectPartsVector, pObjectPart ) );
                        }
                    }

                    data[ "stages" ].push_back( stage );
                }

                writeJSON( dataDir / "view.json", data );
            }

            void writeDataData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
            {
                nlohmann::json data;

                data[ "guard" ] = "DATABASE_VIEW_GUARD_4_APRIL_2022";

                for ( model::Namespace::Ptr pNamespace : pSchema->m_namespaces )
                {
                    nlohmann::json ns
                        = nlohmann::json::object( { { "name", pNamespace->m_strName } } );

                    for ( model::Object::Ptr pObject : pNamespace->m_objects )
                    {
                        nlohmann::json obj = nlohmann::json::object(
                            { { "name", pObject->m_strName },
                              { "properties", nlohmann::json::array() } } );

                        for ( model::ObjectPart::Ptr pObjectPart : pObject->m_parts )
                        {
                            for ( model::Property::Ptr pProperty : pObjectPart->m_properties )
                            {
                                nlohmann::json property = nlohmann::json::object(
                                    { { "name", pProperty->m_strName },
                                      { "return_type", pProperty->m_type->getReturnType() } } );

                                obj[ "properties" ].push_back( property );
                            }
                        }
                        ns[ "objects" ].push_back( obj );
                    }
                    data[ "namespaces" ].push_back( ns );
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
