#include "json_converter.hpp"

#include "database_compiler/model.hpp"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <fstream>
#include <memory>
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

            void writeStageData( const boost::filesystem::path& dataDir,
                                 model::Schema::Ptr             pSchema )
            {
                nlohmann::json data( { { "stages", nlohmann::json::array() } } );
                for ( model::Stage::Ptr pStage : pSchema->m_stages )
                {
                    nlohmann::json stage
                        = nlohmann::json::object( { { "name", pStage->m_strName } } );

                    data[ "stages" ].push_back( stage );
                }
                writeJSON( dataDir / "stages.json", data );
            }

            nlohmann::json writeInterface( model::Interface::Ptr pInterface )
            {
                model::Object::Ptr pObject = pInterface->m_object.lock();
                nlohmann::json     interface = nlohmann::json::object( {
                    { "name", pObject->m_strName },
                    { "functions", nlohmann::json::array() },
                } );

                // namespaces
                {
                    // std::ostringstream    os;
                    model::Namespace::Ptr pNamespace = pObject->m_namespace.lock();
                    VERIFY_RTE( pNamespace );
                    while ( pNamespace )
                    {
                        // os << pNamespace->m_strName << "::";
                        interface[ "namespaces" ].push_back( pNamespace->m_strName );
                        pNamespace = pNamespace->m_namespace.lock();
                    }
                    // os << pObject->m_strName;
                    // interface[ "full_name" ] = os.str();
                }

                for ( model::Function::Ptr pFunction : pInterface->m_functions )
                {
                    nlohmann::json function;

                    std::ostringstream osName, osReturnType, osParams;
                    if ( std::dynamic_pointer_cast< model::FunctionGetter >( pFunction ) )
                    {
                        osName << "get_" << pFunction->m_property->m_strName;
                        osReturnType << "const " << pFunction->m_property->m_type->getViewType()
                                     << "&";
                    }
                    else if ( std::dynamic_pointer_cast< model::FunctionSetter >( pFunction ) )
                    {
                        osName << "get_" << pFunction->m_property->m_strName;
                        osReturnType << "void";
                        osParams << " const " << pFunction->m_property->m_type->getViewType()
                                 << "& value ";
                    }
                    else
                    {
                        THROW_RTE( "Unknown function type" );
                    }

                    function[ "name" ] = osName.str();
                    function[ "returntype" ] = osReturnType.str();
                    function[ "params" ] = osParams.str();

                    interface[ "functions" ].push_back( function );
                }

                return interface;
            }

            void writeViewData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
            {
                for ( model::Stage::Ptr pStage : pSchema->m_stages )
                {
                    nlohmann::json data;
                    {
                        std::ostringstream os;
                        os << "DATABASE_VIEW_" << pStage->m_strName << "_GUARD";
                        data[ "guard" ] = os.str();
                    }

                    nlohmann::json stage
                        = nlohmann::json::object( { { "name", pStage->m_strName },
                                                    { "perobject", pStage->m_bPerObject },
                                                    { "accessors", nlohmann::json::array() },
                                                    { "constructors", nlohmann::json::array() },
                                                    { "refinements", nlohmann::json::array() },
                                                    { "interfaces", nlohmann::json::array() } } );

                    for ( model::Interface::Ptr pInterface : pStage->m_readOnlyInterfaces )
                    {
                        stage[ "interfaces" ].push_back( writeInterface( pInterface ) );
                    }
                    for ( model::Interface::Ptr pInterface : pStage->m_readWriteInterfaces )
                    {
                        stage[ "interfaces" ].push_back( writeInterface( pInterface ) );
                    }
                    /*for( model::Accessor::Ptr pAccessor : pStage->m_accessors )
                    {

                    }*/
                    for ( model::Constructor::Ptr pConstructor : pStage->m_constructors )
                    {
                        std::ostringstream os;
                        os << "construct_" << pConstructor->m_interface->delimitTypeName( "_" );

                        nlohmann::json ctor = nlohmann::json::object(
                            { { "return_type", pConstructor->m_interface->delimitTypeName( "::" ) },
                              { "function_name", os.str() } } );

                        stage[ "constructors" ].push_back( ctor );
                    }

                    data[ "stage" ] = stage;

                    std::ostringstream os;
                    os << pStage->m_strName << ".json";
                    writeJSON( dataDir / os.str(), data );
                }
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

                        VERIFY_RTE( pObject->m_primaryObjectPart );
                        model::ObjectPart::Ptr pPrimaryPart = pObject->m_primaryObjectPart;
                        const bool             bIsPrimaryPart = pPrimaryPart == pPart;

                        nlohmann::json part = nlohmann::json::object(
                            { { "name", pObject->m_strName },
                              { "typeID", pPart->m_typeID },
                              { "pointers", nlohmann::json::array() },
                              { "properties", nlohmann::json::array() } } );

                        if ( bIsPrimaryPart )
                        {
                            for ( model::ObjectPart::Ptr pObjectPart : pObject->m_secondaryParts )
                            {
                                nlohmann::json pointer = nlohmann::json::object(
                                    { { "namespace", pObjectPart->m_file.lock()->m_strName },
                                      { "name", pObjectPart->m_object.lock()->m_strName } } );
                                part[ "pointers" ].push_back( pointer );
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
            writeStageData( dataDir, pSchema );
            writeViewData( dataDir, pSchema );
            writeDataData( dataDir, pSchema );
        }

    } // namespace jsonconv
} // namespace db
