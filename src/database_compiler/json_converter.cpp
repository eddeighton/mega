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

            void writeStageData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
            {
                nlohmann::json data( { { "stages", nlohmann::json::array() } } );
                for ( model::Stage::Ptr pStage : pSchema->m_stages )
                {
                    nlohmann::json stage = nlohmann::json::object( { { "name", pStage->m_strName } } );

                    data[ "stages" ].push_back( stage );
                }
                writeJSON( dataDir / "stages.json", data );
            }

            nlohmann::json writeFunction( model::Function::Ptr pFunction )
            {
                nlohmann::json     function;
                std::ostringstream osName, osReturnType, osParams;
                if ( std::dynamic_pointer_cast< model::FunctionGetter >( pFunction ) )
                {
                    osName << "get_" << pFunction->m_property->m_strName;
                    osReturnType << pFunction->m_property->m_type->getViewType( true );
                }
                else if ( std::dynamic_pointer_cast< model::FunctionSetter >( pFunction ) )
                {
                    osName << "set_" << pFunction->m_property->m_strName;
                    osReturnType << "void";
                    osParams << pFunction->m_property->m_type->getViewType( true ) << " value ";
                }
                else if ( std::dynamic_pointer_cast< model::FunctionInserter >( pFunction ) )
                {
                    model::Type::Ptr pType = pFunction->m_property->m_type;

                    if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                    {
                        osName << "push_back_" << pFunction->m_property->m_strName;
                        osReturnType << "void";
                        osParams << pArray->m_underlyingType->getViewType( true ) << " value ";
                    }
                    else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                    {
                        model::Type::Ptr pFrom = pMap->m_fromType;
                        model::Type::Ptr pTo = pMap->m_toType;

                        osName << "insert_" << pFunction->m_property->m_strName;
                        osReturnType << "void";
                        osParams << pFrom->getViewType( true ) << " key , " << pTo->getViewType( true ) << " value ";
                    }
                    else
                    {
                        THROW_RTE( "Unsupported inserter type" );
                    }
                }
                else
                {
                    THROW_RTE( "Unknown function type" );
                }

                function[ "name" ] = osName.str();
                function[ "returntype" ] = osReturnType.str();
                function[ "params" ] = osParams.str();
                return function;
            }

            nlohmann::json writeInterface( model::Interface::Ptr pInterface )
            {
                model::Object::Ptr pObject = pInterface->m_object.lock();
                nlohmann::json     interface = nlohmann::json::object( {
                    { "name", pObject->m_strName },
                    { "fullname", pInterface->delimitTypeName( "::" ) },
                    { "readwrite", pInterface->m_isReadWrite },
                    { "functions", nlohmann::json::array() },
                    { "args_ctors", nlohmann::json::array() },
                    { "args_values", nlohmann::json::array() },
                } );

                // base interface
                if ( pInterface->m_base )
                {
                    interface[ "has_base" ] = true;
                    interface[ "base" ] = pInterface->m_base->delimitTypeName( "::" );
                }
                else
                {
                    interface[ "has_base" ] = false;
                }

                // namespaces
                {
                    model::Namespace::Ptr pNamespace = pObject->m_namespace.lock();
                    VERIFY_RTE( pNamespace );
                    while ( pNamespace )
                    {
                        interface[ "namespaces" ].push_back( pNamespace->m_strName );
                        pNamespace = pNamespace->m_namespace.lock();
                    }
                }

                // functions
                for ( model::Function::Ptr pFunction : pInterface->m_functions )
                {
                    interface[ "functions" ].push_back( writeFunction( pFunction ) );
                }

                // args
                if ( pInterface->m_isReadWrite )
                {
                    // calculate ctor overloads...
                    if ( !pInterface->m_args.empty() )
                    {
                        // add default
                        nlohmann::json ctor = nlohmann::json::object( { { "params", nlohmann::json::array() } } );
                        interface[ "args_ctors" ].push_back( ctor );
                    }

                    if ( pInterface->m_base )
                    {
                        if ( pInterface->m_base->ownsPrimaryObjectPart() )
                        {
                            std::ostringstream osBaseArgs;
                            {
                                osBaseArgs << pInterface->m_base->delimitTypeName( "::" ) << "::Args";
                            }
                            {
                                nlohmann::json ctorBaseArgs = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                                nlohmann::json baseArg
                                    = nlohmann::json::object( { { "name", "base" }, { "type", osBaseArgs.str() }, { "value", "base" } } );
                                ctorBaseArgs[ "params" ].push_back( baseArg );

                                for ( model::Property::Ptr pProperty : pInterface->m_args )
                                {
                                    nlohmann::json param = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                                     { "type", pProperty->m_type->getViewType( true ) },
                                                                                     { "value", pProperty->m_strName } } );
                                    ctorBaseArgs[ "params" ].push_back( param );
                                }

                                interface[ "args_ctors" ].push_back( ctorBaseArgs );
                            }
                            std::ostringstream osBasePointer;
                            {
                                osBasePointer << pInterface->m_base->delimitTypeName( "::" ) << "*";
                            }
                            {
                                nlohmann::json ctorBasePointer = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                                nlohmann::json baseArg = nlohmann::json::object(
                                    { { "name", "base" }, { "type", osBasePointer.str() }, { "value", "base" } } );
                                ctorBasePointer[ "params" ].push_back( baseArg );

                                for ( model::Property::Ptr pProperty : pInterface->m_args )
                                {
                                    nlohmann::json param = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                                     { "type", pProperty->m_type->getViewType( true ) },
                                                                                     { "value", pProperty->m_strName } } );
                                    ctorBasePointer[ "params" ].push_back( param );
                                }

                                interface[ "args_ctors" ].push_back( ctorBasePointer );
                            }

                            std::ostringstream osVariant;
                            {
                                osVariant << "std::variant< " << osBaseArgs.str() << ", " << osBasePointer.str() << " >";
                            }

                            std::ostringstream osType;
                            osType << "std::optional< " << osVariant.str() << " >";
                            nlohmann::json value = nlohmann::json::object( { { "name", "base" }, { "type", osType.str() } } );
                            interface[ "args_values" ].push_back( value );
                        }
                        else
                        {
                            std::ostringstream osBasePointer;
                            {
                                osBasePointer << pInterface->m_base->delimitTypeName( "::" ) << "*";
                            }
                            {
                                nlohmann::json ctorBasePointer = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                                nlohmann::json baseArg = nlohmann::json::object(
                                    { { "name", "base" }, { "type", osBasePointer.str() }, { "value", "base" } } );
                                ctorBasePointer[ "params" ].push_back( baseArg );

                                for ( model::Property::Ptr pProperty : pInterface->m_args )
                                {
                                    nlohmann::json param = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                                     { "type", pProperty->m_type->getViewType( true ) },
                                                                                     { "value", pProperty->m_strName } } );
                                    ctorBasePointer[ "params" ].push_back( param );
                                }

                                interface[ "args_ctors" ].push_back( ctorBasePointer );
                            }
                            std::ostringstream osType;
                            osType << "std::optional< std::variant< " << osBasePointer.str() << " > >";
                            nlohmann::json value = nlohmann::json::object( { { "name", "base" }, { "type", osType.str() } } );
                            interface[ "args_values" ].push_back( value );
                        }
                    }
                    else
                    {
                        nlohmann::json ctor = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                        for ( model::Property::Ptr pProperty : pInterface->m_args )
                        {
                            nlohmann::json param = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                             { "type", pProperty->m_type->getViewType( true ) },
                                                                             { "value", pProperty->m_strName } } );
                            ctor[ "params" ].push_back( param );
                        }

                        interface[ "args_ctors" ].push_back( ctor );
                    }

                    for ( model::Property::Ptr pProperty : pInterface->m_args )
                    {
                        std::ostringstream osType;
                        osType << "std::optional< " << pProperty->m_type->getViewType( false ) << " >";
                        nlohmann::json value = nlohmann::json::object( { { "name", pProperty->m_strName }, { "type", osType.str() } } );
                        interface[ "args_values" ].push_back( value );
                    }
                }

                return interface;
            }

            void writeConversions( nlohmann::json& stage, model::Schema::Ptr pSchema, model::Stage::Ptr pStage )
            {
                for ( model::Stage::Ptr pStageIter : pSchema->m_stages )
                {
                    for ( model::File::Ptr pFile : pStageIter->m_files )
                    {
                        for ( model::ObjectPart::Ptr pPart : pFile->m_parts )
                        {
                            model::SuperInterface::Ptr pSuper = pStage->getInterface( pPart->m_object.lock() )->m_superInterface.lock();
                            nlohmann::json             conversion = nlohmann::json::object( { { "type_id", pPart->m_typeID },
                                                                                  { "supertype", pSuper->getTypeName() },
                                                                                  { "file", pFile->m_strName },
                                                                                  { "object", pPart->m_object.lock()->m_strName } } );
                            stage[ "super_conversions" ].push_back( conversion );
                        }
                    }
                    if ( pStageIter == pStage )
                        break;
                }
                for ( model::Interface::Ptr pInterface : pStage->m_interfaceTopological )
                {
                    model::PrimaryObjectPart::Ptr pPart = pInterface->getPrimaryObjectPart();
                    model::SuperInterface::Ptr    pSuper = pInterface->m_superInterface.lock();
                    nlohmann::json                conversion = nlohmann::json::object( { { "type", pInterface->delimitTypeName( "::" ) },
                                                                          { "file", pPart->m_file.lock()->m_strName },
                                                                          { "supertype", pSuper->getTypeName() },
                                                                          { "object", pPart->m_object.lock()->m_strName } } );
                    stage[ "interface_conversions" ].push_back( conversion );
                }
            }

            void writeAccessors( nlohmann::json& stage, model::Stage::Ptr pStage )
            {
                std::set< model::Interface::Ptr > manyAccessors;
                for ( model::Accessor::Ptr pAccessor : pStage->m_accessors )
                {
                    if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pAccessor->m_type ) )
                    {
                        model::Object::Ptr            pObject = pRef->m_object;
                        model::Interface::Ptr         pInterface = pStage->getInterface( pObject );
                        model::PrimaryObjectPart::Ptr pPrimaryPart = pInterface->getPrimaryObjectPart();
                        nlohmann::json                acessor
                            = nlohmann::json::object( { { "type", pInterface->delimitTypeName( "::" ) },
                                                        { "longname", pInterface->delimitTypeName( "_" ) },
                                                        { "object", pPrimaryPart->m_object.lock()->m_strName },
                                                        { "file", pPrimaryPart->m_file.lock()->m_strName },
                                                        { "stage", pPrimaryPart->m_file.lock()->m_stage.lock()->m_strName },
                                                        { "supertype", pInterface->m_superInterface.lock()->getTypeName() } } );

                        stage[ "one_accessors" ].push_back( acessor );
                        if ( manyAccessors.count( pInterface ) == 0 )
                        {
                            manyAccessors.insert( pInterface );
                            stage[ "many_accessors" ].push_back( acessor );
                        }
                    }
                    else if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pAccessor->m_type ) )
                    {
                        if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pArray->m_underlyingType ) )
                        {
                            model::Object::Ptr            pObject = pRef->m_object;
                            model::Interface::Ptr         pInterface = pStage->getInterface( pObject );
                            model::PrimaryObjectPart::Ptr pPrimaryPart = pInterface->getPrimaryObjectPart();

                            if ( manyAccessors.count( pInterface ) == 0 )
                            {
                                manyAccessors.insert( pInterface );
                                nlohmann::json acessor
                                    = nlohmann::json::object( { { "type", pInterface->delimitTypeName( "::" ) },
                                                                { "longname", pInterface->delimitTypeName( "_" ) },
                                                                { "object", pPrimaryPart->m_object.lock()->m_strName },
                                                                { "file", pPrimaryPart->m_file.lock()->m_strName },
                                                                { "stage", pPrimaryPart->m_file.lock()->m_stage.lock()->m_strName },
                                                                { "supertype", pInterface->m_superInterface.lock()->getTypeName() } } );

                                stage[ "many_accessors" ].push_back( acessor );
                            }
                        }
                        else
                        {
                            THROW_RTE( "Unsupported Accessor type: " );
                        }
                    }
                    /*else if( model::OptionalType::Ptr pArray =
                        std::dynamic_pointer_cast< model::ArrayType >( pAccessor->m_type ) )
                    {

                    }*/
                    else
                    {
                        THROW_RTE( "Unsupported Accessor type: " );
                    }
                }
            }

            nlohmann::json writeCtorPart( model::Stage::Ptr pStage, model::ObjectPart::Ptr pPart )
            {
                nlohmann::json part = nlohmann::json::object( { { "object", pPart->m_object.lock()->m_strName },
                                                                { "file", pPart->m_file.lock()->m_strName },
                                                                { "args", nlohmann::json::array() } } );
                {
                    for ( model::Property::Ptr pProperty : pPart->m_properties )
                    {
                        model::Type::Ptr pType = pProperty->m_type;

                        std::ostringstream osExpression;
                        if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
                        {
                            osExpression << "toData( arguments." << pProperty->m_strName << ".value() )";
                        }
                        else if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pType ) )
                        {
                            osExpression << "arguments." << pProperty->m_strName << ".value()";
                        }
                        else if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                        {
                            model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
                            if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                            {
                                osExpression << "arguments." << pProperty->m_strName << ".value()";
                            }
                            else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                            {
                                model::Object::Ptr pObject = pRef->m_object;
                                osExpression << pStage->m_strName << "::toData< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                             << " >( database, arguments." << pProperty->m_strName << ".value() )";
                            }
                            else
                            {
                                THROW_RTE( "Unsupported type for ctor" );
                            }
                        }
                        else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                        {
                            model::Type::Ptr pFrom = pMap->m_fromType;
                            model::Type::Ptr pTo = pMap->m_toType;

                            if ( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                            {
                                if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                {
                                    osExpression << "arguments." << pProperty->m_strName << ".value()";
                                }
                                else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                {
                                    model::Object::Ptr pObject = pToRef->m_object;
                                    osExpression << pStage->m_strName
                                                 << "::toData< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                                 << " >( database, arguments." << pProperty->m_strName << ".value() )";
                                }
                                else
                                {
                                    THROW_RTE( "Unsupported type for map from type" );
                                }
                            }
                            else if ( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
                            {
                                if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                {
                                    // osExpression << "arguments." << pProperty->m_strName << ".value()";
                                    model::Object::Ptr pObject = pFromRef->m_object;
                                    osExpression << pStage->m_strName
                                                 << "::toData< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                                 << " >( database, arguments." << pProperty->m_strName << ".value() )";
                                }
                                else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                {
                                    THROW_RTE( "Unsupported type for map from type" );
                                }
                                else
                                {
                                    THROW_RTE( "Unsupported type for map from type" );
                                }

                                /*if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                {
                                    osExpression << "arguments." << pProperty->m_strName << ".value()";
                                }
                                else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                {
                                    model::Object::Ptr pObject = pToRef->m_object;
                                    osExpression << pStage->m_strName << "::toData< data::" << pObject->m_primaryObjectPart->getDataType(
                                "::" ) << " >( database, arguments." << pProperty->m_strName << ".value() )";
                                }
                                else
                                {
                                    THROW_RTE( "Unsupported type for map from type" );
                                }*/

                                // model::Object::Ptr pObject = pRef->m_object;
                                // osExpression << pStage->m_strName << "::toData< data::" << pObject->m_primaryObjectPart->getDataType(
                                // "::" ) <<
                                //     " >( database, arguments." << pProperty->m_strName << ".value() )";
                            }
                            else
                            {
                                THROW_RTE( "Unsupported type for map from type" );
                            }
                        }
                        else
                        {
                            THROW_RTE( "Unsupported type for ctor" );
                        }

                        nlohmann::json arg = nlohmann::json::object( { { "expression", osExpression.str() } } );
                        part[ "args" ].push_back( arg );
                    }
                }
                return part;
            }

            void writeConstructors( nlohmann::json& stage, model::Stage::Ptr pStage )
            {
                for ( model::Constructor::Ptr pConstructor : pStage->m_constructors )
                {
                    std::ostringstream os;
                    os << "construct_" << pConstructor->m_interface->delimitTypeName( "_" );

                    model::Interface::Ptr      pInterface = pConstructor->m_interface;
                    model::SuperInterface::Ptr pSuperInterface = pInterface->m_superInterface.lock();

                    nlohmann::json ctor = nlohmann::json::object( { { "return_type", pConstructor->m_interface->delimitTypeName( "::" ) },
                                                                    { "function_name", os.str() },
                                                                    { "super_type_name", pSuperInterface->getTypeName() },
                                                                    { "bases", nlohmann::json::array() },
                                                                    { "bases_reverse", nlohmann::json::array() } } );

                    // add self to list
                    model::Interface::Ptr                pBase = pInterface;
                    std::vector< model::Interface::Ptr > bases_reversed;
                    while ( pBase )
                    {
                        nlohmann::json base = nlohmann::json::object( { { "typename", pBase->delimitTypeName( "::" ) },
                                                                        { "owns_primary_part", pBase->ownsPrimaryObjectPart() },
                                                                        { "parts", nlohmann::json::array() } } );

                        model::PrimaryObjectPart::Ptr pPrimaryPart = pBase->getPrimaryObjectPart();

                        base[ "primary_part" ] = writeCtorPart( pStage, pPrimaryPart );
                        // VERIFY_RTE( pBase->m_readOnlyObjectParts.empty() );
                        for ( model::ObjectPart::Ptr pPart : pBase->m_readWriteObjectParts )
                        {
                            if ( pPrimaryPart != pPart )
                            {
                                base[ "parts" ].push_back( writeCtorPart( pStage, pPart ) );
                            }
                        }

                        ctor[ "bases" ].push_back( base );
                        bases_reversed.push_back( pBase );
                        pBase = pBase->m_base;
                    }
                    std::reverse( bases_reversed.begin(), bases_reversed.end() );
                    for ( model::Interface::Ptr pBase : bases_reversed )
                    {
                        nlohmann::json base = nlohmann::json::object(
                            { { "typename", pBase->delimitTypeName( "::" ) }, { "owns_primary_part", pBase->ownsPrimaryObjectPart() } } );

                        model::PrimaryObjectPart::Ptr pPrimaryPart = pBase->getPrimaryObjectPart();
                        base[ "primary_part" ] = nlohmann::json::object( { { "object", pPrimaryPart->m_object.lock()->m_strName },
                                                                           { "file", pPrimaryPart->m_file.lock()->m_strName } } );

                        ctor[ "bases_reverse" ].push_back( base );
                    }

                    stage[ "constructors" ].push_back( ctor );
                }
            }

            void writeSuperTypes( nlohmann::json& stage, model::Stage::Ptr pStage )
            {
                for ( model::SuperInterface::Ptr pSuperType : pStage->m_superInterfaces )
                {
                    const std::string strSuperTypeName = pSuperType->getTypeName();

                    nlohmann::json stype
                        = nlohmann::json::object( { { "name", strSuperTypeName }, { "interfaces", nlohmann::json::array() } } );

                    for ( model::Interface::Ptr pInterface : pSuperType->m_interfaces )
                    {
                        model::ObjectPart::Ptr pPrimaryObjectPart = pInterface->m_object.lock()->m_primaryObjectPart;

                        nlohmann::json interface = nlohmann::json::object( { { "name", pInterface->delimitTypeName( "::" ) },
                                                                             { "part", pPrimaryObjectPart->getDataType( "::" ) },
                                                                             { "functions", nlohmann::json::array() } } );

                        for ( model::Function::Ptr pFunction : pInterface->m_functions )
                        {
                            nlohmann::json function = writeFunction( pFunction );

                            function[ "objectpart" ] = pFunction->m_property->m_objectPart.lock()->getDataType( "::" );

                            model::Type::Ptr pType = pFunction->m_property->m_type;

                            if ( std::dynamic_pointer_cast< model::FunctionGetter >( pFunction ) )
                            {
                                if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pType ) )
                                {
                                    std::ostringstream osFunctionBody;
                                    osFunctionBody << "return part->" << pFunction->m_property->m_strName << ";";
                                    function[ "body" ].push_back( osFunctionBody.str() );
                                }
                                else if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                                {
                                    std::ostringstream osFunctionBody;
                                    model::Type::Ptr   pUnderlyingType = pArray->m_underlyingType;
                                    if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                                    {
                                        osFunctionBody << "return part->" << pFunction->m_property->m_strName << ";";
                                    }
                                    else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                                    {
                                        model::Object::Ptr    pObject = pRef->m_object;
                                        model::Interface::Ptr pInterface = pStage->getInterface( pObject );
                                        osFunctionBody << "return toInterface< " << pInterface->delimitTypeName( "::" )
                                                       << " >( m_converter, part->" << pFunction->m_property->m_strName << " );";
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unsupported type for getter" );
                                    }
                                    function[ "body" ].push_back( osFunctionBody.str() );
                                }
                                else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                                {
                                    model::Type::Ptr pFrom = pMap->m_fromType;
                                    model::Type::Ptr pTo = pMap->m_toType;
                                    if ( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                                    {
                                        std::ostringstream osFunctionBody;
                                        if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                        {
                                            osFunctionBody << "return part->" << pFunction->m_property->m_strName << ";";
                                        }
                                        else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                        {
                                            model::Object::Ptr    pObject = pToRef->m_object;
                                            model::Interface::Ptr pInterface = pStage->getInterface( pObject );
                                            osFunctionBody << "return toInterface< " << pInterface->delimitTypeName( "::" )
                                                           << ", data::Ptr< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                                           << " >"
                                                           << " >( m_converter, part->" << pFunction->m_property->m_strName << " );";
                                        }
                                        else
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                        }
                                        function[ "body" ].push_back( osFunctionBody.str() );
                                    }
                                    else if ( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
                                    {
                                        std::ostringstream osFunctionBody;
                                        if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                        {
                                            model::Object::Ptr    pObject = pFromRef->m_object;
                                            model::Interface::Ptr pInterface = pStage->getInterface( pObject );
                                            osFunctionBody << "return toInterface< " << pInterface->delimitTypeName( "::" )
                                                           << ", data::Ptr< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                                           << " >"
                                                           << " >( m_converter, part->" << pFunction->m_property->m_strName << " );";
                                        }
                                        else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                            // model::Object::Ptr pObject = pToRef->m_object;
                                            // model::Interface::Ptr pInterface = pStage->getInterface( pObject );
                                            // osFunctionBody << "return toInterface< " << pInterface->delimitTypeName( "::" ) << ",
                                            // data::Ptr< data::" << pObject->m_primaryObjectPart->getDataType( "::" ) << " >"
                                            //                << " >( m_converter, part->" << pFunction->m_property->m_strName << " );";
                                        }
                                        else
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                        }
                                        function[ "body" ].push_back( osFunctionBody.str() );
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unsupported type for map from type" );
                                    }
                                }
                                else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
                                {
                                    std::ostringstream osFunctionBody;
                                    osFunctionBody << "return m_converter.toInterface( part->" << pFunction->m_property->m_strName << " );";
                                    function[ "body" ].push_back( osFunctionBody.str() );
                                }
                                else
                                {
                                    THROW_RTE( "Unsupported type for getter" );
                                }
                            }
                            else if ( std::dynamic_pointer_cast< model::FunctionSetter >( pFunction ) )
                            {
                                if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pType ) )
                                {
                                    std::ostringstream osFunctionBody;
                                    osFunctionBody << "part->" << pFunction->m_property->m_strName << " = value;";
                                    function[ "body" ].push_back( osFunctionBody.str() );
                                }
                                else if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                                {
                                    model::Type::Ptr   pUnderlyingType = pArray->m_underlyingType;
                                    std::ostringstream osFunctionBody;
                                    if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                                    {
                                        osFunctionBody << "part->" << pFunction->m_property->m_strName << " = value;";
                                    }
                                    else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                                    {
                                        model::Object::Ptr pObject = pRef->m_object;
                                        osFunctionBody << "part->" << pFunction->m_property->m_strName
                                                       << " = toData< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                                       << ">( m_converter, value );";
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unsupported type for setter" );
                                    }
                                    function[ "body" ].push_back( osFunctionBody.str() );
                                }
                                else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                                {
                                    model::Type::Ptr pFrom = pMap->m_fromType;
                                    model::Type::Ptr pTo = pMap->m_toType;
                                    if ( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                                    {
                                        std::ostringstream osFunctionBody;
                                        if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                        {
                                            osFunctionBody << "part->" << pFunction->m_property->m_strName << " = value;";
                                        }
                                        else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                        {
                                            model::Object::Ptr pObject = pToRef->m_object;
                                            osFunctionBody << "part->" << pFunction->m_property->m_strName << " = "
                                                           << "toData< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                                           << " >( m_converter, value );";
                                        }
                                        else
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                        }
                                        function[ "body" ].push_back( osFunctionBody.str() );
                                    }
                                    else if ( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
                                    {
                                        std::ostringstream osFunctionBody;
                                        if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                        {
                                            model::Object::Ptr    pObject = pFromRef->m_object;
                                            model::Interface::Ptr pInterface = pStage->getInterface( pObject );

                                            osFunctionBody << "part->" << pFunction->m_property->m_strName << " = "
                                                           << "toData< data::" << pObject->m_primaryObjectPart->getDataType( "::" ) << ", "
                                                           << pInterface->delimitTypeName( "::" ) << " >( m_converter, value );";
                                        }
                                        else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                            // model::Object::Ptr pObject = pToRef->m_object;
                                            // osFunctionBody << "part->" << pFunction->m_property->m_strName << " = "
                                            //                << "toData< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                            //                << " >( m_converter, value );";
                                        }
                                        else
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                        }
                                        function[ "body" ].push_back( osFunctionBody.str() );
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unsupported type for map from type" );
                                    }
                                }
                                else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
                                {
                                    std::ostringstream osFunctionBody;
                                    osFunctionBody << "part->" << pFunction->m_property->m_strName << " = m_converter.toData( value );";
                                    function[ "body" ].push_back( osFunctionBody.str() );
                                }
                                else
                                {
                                    THROW_RTE( "Unsupported type for setter" );
                                }
                            }
                            else if ( std::dynamic_pointer_cast< model::FunctionInserter >( pFunction ) )
                            {
                                if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                                {
                                    model::Type::Ptr   pUnderlyingType = pArray->m_underlyingType;
                                    std::ostringstream osFunctionBody;
                                    if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                                    {
                                        osFunctionBody << "part->" << pFunction->m_property->m_strName << ".push_back( value );";
                                    }
                                    else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                                    {
                                        model::Object::Ptr pObject = pRef->m_object;
                                        osFunctionBody << "part->" << pFunction->m_property->m_strName
                                                       << ".push_back( m_converter.toData( value ) );";
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unsupported type for inserter" );
                                    }
                                    function[ "body" ].push_back( osFunctionBody.str() );
                                }
                                else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                                {
                                    model::Type::Ptr pFrom = pMap->m_fromType;
                                    model::Type::Ptr pTo = pMap->m_toType;
                                    if ( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                                    {
                                        std::ostringstream osFunctionBody;
                                        if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                        {
                                            osFunctionBody << "part->" << pFunction->m_property->m_strName
                                                           << ".insert( std::make_pair( key, value ) );";
                                        }
                                        else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                        {
                                            osFunctionBody << "part->" << pFunction->m_property->m_strName
                                                           << ".insert( std::make_pair( key, m_converter.toData( value ) ) );";
                                        }
                                        else
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                        }
                                        function[ "body" ].push_back( osFunctionBody.str() );
                                    }
                                    else if ( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
                                    {
                                        std::ostringstream osFunctionBody;
                                        if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                        {
                                            osFunctionBody << "part->" << pFunction->m_property->m_strName
                                                           << ".insert( std::make_pair( m_converter.toData( key ), value ) );";
                                        }
                                        else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                            // model::Object::Ptr pObject = pToRef->m_object;
                                            // osFunctionBody << "part->" << pFunction->m_property->m_strName << " = "
                                            //                << "toData< data::" << pObject->m_primaryObjectPart->getDataType( "::" )
                                            //                << " >( m_converter, value );";
                                        }
                                        else
                                        {
                                            THROW_RTE( "Unsupported type for map from type" );
                                        }
                                        function[ "body" ].push_back( osFunctionBody.str() );
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unsupported type for map from type" );
                                    }
                                }
                            }
                            else
                            {
                                THROW_RTE( "Unknown function type" );
                            }

                            interface[ "functions" ].push_back( function );
                        }

                        stype[ "interfaces" ].push_back( interface );
                    }

                    stage[ "supertypes" ].push_back( stype );
                }
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

                    nlohmann::json stage = nlohmann::json::object( { { "name", pStage->m_strName },
                                                                     { "perobject", pStage->m_bPerObject },
                                                                     { "super_conversions", nlohmann::json::array() },
                                                                     { "interface_conversions", nlohmann::json::array() },
                                                                     { "readwrite_files", nlohmann::json::array() },
                                                                     { "many_accessors", nlohmann::json::array() },
                                                                     { "one_accessors", nlohmann::json::array() },
                                                                     { "one_opt_accessors", nlohmann::json::array() },
                                                                     { "constructors", nlohmann::json::array() },
                                                                     { "interfaces", nlohmann::json::array() } } );

                    writeConversions( stage, pSchema, pStage );

                    for ( model::File::Ptr pFile : pStage->m_files )
                    {
                        nlohmann::json file = nlohmann::json::object( { { "name", pFile->m_strName } } );
                        stage[ "readwrite_files" ].push_back( file );
                    }

                    for ( model::Interface::Ptr pInterface : pStage->m_interfaceTopological )
                    {
                        stage[ "interfaces" ].push_back( writeInterface( pInterface ) );
                    }

                    writeAccessors( stage, pStage );
                    writeConstructors( stage, pStage );
                    writeSuperTypes( stage, pStage );

                    data[ "stage" ] = stage;

                    std::ostringstream os;
                    os << pStage->m_strName << ".json";
                    writeJSON( dataDir / os.str(), data );
                }
            }

            void writeDataData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
            {
                nlohmann::json data( { { "files", nlohmann::json::array() }, { "conversions", nlohmann::json::array() } } );

                data[ "guard" ] = "DATABASE_DATA_GUARD_4_APRIL_2022";

                std::vector< model::File::Ptr > files;
                {
                    for ( model::Stage::Ptr pStage : pSchema->m_stages )
                    {
                        std::copy( pStage->m_files.begin(), pStage->m_files.end(), std::back_inserter( files ) );
                    }
                }
                for ( model::File::Ptr pFile : files )
                {
                    nlohmann::json file = nlohmann::json::object( { { "name", pFile->m_strName },
                                                                    { "stage", pFile->m_stage.lock()->m_strName },
                                                                    { "perobject", pFile->m_stage.lock()->m_bPerObject },
                                                                    { "parts", nlohmann::json::array() } } );

                    for ( model::ObjectPart::Ptr pPart : pFile->m_parts )
                    {
                        model::Object::Ptr pObject = pPart->m_object.lock();

                        VERIFY_RTE( pObject->m_primaryObjectPart );
                        model::ObjectPart::Ptr pPrimaryPart = pObject->m_primaryObjectPart;
                        const bool             bIsPrimaryPart = pPrimaryPart == pPart;

                        nlohmann::json part = nlohmann::json::object( { { "name", pObject->m_strName },
                                                                        { "typeID", pPart->m_typeID },
                                                                        { "pointers", nlohmann::json::array() },
                                                                        { "properties", nlohmann::json::array() } } );

                        if ( bIsPrimaryPart )
                        {
                            for ( model::ObjectPart::Ptr pObjectPart : pObject->m_secondaryParts )
                            {
                                nlohmann::json pointer = nlohmann::json::object(
                                    { { "longname", pObjectPart->getPointerName() }, { "typename", pObjectPart->getDataType( "::" ) } } );
                                part[ "pointers" ].push_back( pointer );
                            }
                            if ( pObject->m_base )
                            {
                                nlohmann::json pointer = nlohmann::json::object(
                                    { { "longname", pObject->m_base->m_primaryObjectPart->getPointerName() },
                                      { "typename", pObject->m_base->m_primaryObjectPart->getDataType( "::" ) } } );
                                part[ "pointers" ].push_back( pointer );
                            }
                        }

                        for ( model::Property::Ptr pProperty : pPart->m_properties )
                        {
                            model::Type::Ptr pType = pProperty->m_type;

                            bool bIsPointer = false;
                            if ( std::dynamic_pointer_cast< model::RefType >( pType ) )
                                bIsPointer = true;

                            nlohmann::json property = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                                { "type", pType->getDataType( false ) },
                                                                                { "argtype", pType->getDataType( true ) },
                                                                                { "is_pointer", bIsPointer } } );

                            part[ "properties" ].push_back( property );
                        }

                        file[ "parts" ].push_back( part );
                    }

                    data[ "files" ].push_back( file );
                }

                // conversions
                {
                    for ( model::Schema::ConversionMap::const_iterator i = pSchema->m_conversions.begin(),
                                                                       iEnd = pSchema->m_conversions.end();
                          i != iEnd;
                          ++i )
                    {
                        const model::Schema::ObjectPartPair&   parts = i->first;
                        const model::Schema::ObjectPartVector& sequence = i->second;

                        nlohmann::json conversion = nlohmann::json::object( { { "from", parts.first->getDataType( "::" ) },
                                                                              { "to", parts.second->getDataType( "::" ) },
                                                                              { "pointers", nlohmann::json::array() } } );

                        for ( model::ObjectPart::Ptr pPart : sequence )
                        {
                            conversion[ "pointers" ].push_back( pPart->getPointerName() );
                        }

                        data[ "conversions" ].push_back( conversion );
                    }
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
