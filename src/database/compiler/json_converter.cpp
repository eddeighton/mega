#include "json_converter.hpp"

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
                nlohmann::json data( { { "sources", nlohmann::json::array() }, { "stages", nlohmann::json::array() } } );

                // get the unique source types
                for ( model::Source::Ptr pSource : pSchema->m_sources )
                {
                    nlohmann::json source
                        = nlohmann::json::object( { { "type", pSource->m_strName }, { "files", nlohmann::json::array() } } );

                    for ( std::weak_ptr< model::Stage > pStageWeak : pSource->m_stages )
                    {
                        model::Stage::Ptr pStage = pStageWeak.lock();
                        for ( model::File::Ptr pFile : pStage->m_files )
                        {
                            nlohmann::json file
                                = nlohmann::json::object( { { "name", pFile->m_strName }, { "stage", pStage->m_strName } } );
                            source[ "files" ].push_back( file );
                        }
                    }
                    data[ "sources" ].push_back( source );
                }
                for ( model::Stage::Ptr pStage : pSchema->m_stages )
                {
                    VERIFY_RTE( pStage->m_source );
                    nlohmann::json stage = nlohmann::json::object( { { "name", pStage->m_strName },
                                                                     { "source", pStage->m_source->m_strName },
                                                                     { "files", nlohmann::json::array() } } );

                    for ( model::File::Ptr pFile : pStage->m_files )
                    {
                        nlohmann::json file
                            = nlohmann::json::object( { { "name", pFile->m_strName }, { "dependencies", nlohmann::json::array() } } );

                        for ( std::weak_ptr< model::File > pDependencyWeak : pFile->m_dependencies )
                        {
                            model::File::Ptr pDependency    = pDependencyWeak.lock();
                            nlohmann::json   dependencyFile = nlohmann::json::object(
                                  { { "name", pDependency->m_strName }, { "stage", pDependency->m_stage.lock()->m_strName } } );
                            file[ "dependencies" ].push_back( dependencyFile );
                        }

                        stage[ "files" ].push_back( file );
                    }

                    data[ "stages" ].push_back( stage );
                }
                writeJSON( dataDir / "stages.json", data );
            }

            nlohmann::json writeFunctionSignature( model::Stage::Ptr pStage, model::Function::Ptr pFunction )
            {
                nlohmann::json function;
                function[ "name" ]       = pFunction->getName();
                function[ "returntype" ] = pFunction->getReturnType( pStage->m_strName );
                function[ "params" ]     = pFunction->getParams( pStage->m_strName );
                return function;
            }

            nlohmann::json writeInterface( model::Stage::Ptr pStage, model::Interface::Ptr pInterface )
            {
                model::Object::Ptr pObject   = pInterface->m_object.lock();
                nlohmann::json     interface = nlohmann::json::object( {
                    { "name", pObject->m_strName },
                    { "fullname", pInterface->delimitTypeName( pStage->m_strName, "_" ) },
                    { "readwrite", pInterface->m_isReadWrite },
                    { "functions", nlohmann::json::array() },
                    { "args_ctors", nlohmann::json::array() },
                    { "args_values", nlohmann::json::array() },
                    { "tests", nlohmann::json::array() },
                } );

                // base interface
                if ( pInterface->m_base )
                {
                    interface[ "has_base" ] = true;
                    interface[ "base" ]     = pInterface->m_base->delimitTypeName( pStage->m_strName, "::" );
                }
                else
                {
                    interface[ "has_base" ] = false;
                }

                for ( model::Interface::Ptr pOtherInterface : pInterface->m_superInterface.lock()->m_interfaces )
                {
                    interface[ "tests" ].push_back( pOtherInterface->delimitTypeName( pStage->m_strName, "_" ) );
                }

                // namespaces
                {
                    std::vector< model::Namespace::Ptr > namespaces;
                    {
                        model::Namespace::Ptr pNamespace = pObject->m_namespace.lock();
                        VERIFY_RTE( pNamespace );
                        while ( pNamespace )
                        {
                            namespaces.push_back( pNamespace );
                            pNamespace = pNamespace->m_namespace.lock();
                        }
                    }
                    std::reverse( namespaces.begin(), namespaces.end() );
                    for ( model::Namespace::Ptr pNamespace : namespaces )
                    {
                        interface[ "namespaces" ].push_back( pNamespace->m_strName );
                    }
                }

                // functions
                for ( model::Function::Ptr pFunction : pInterface->m_functions )
                {
                    interface[ "functions" ].push_back( writeFunctionSignature( pStage, pFunction ) );
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
                                osBaseArgs << pInterface->m_base->delimitTypeName( pStage->m_strName, "::" ) << "::Args";
                            }
                            {
                                nlohmann::json ctorBaseArgs = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                                nlohmann::json baseArg
                                    = nlohmann::json::object( { { "name", "base" }, { "type", osBaseArgs.str() }, { "value", "base" } } );
                                ctorBaseArgs[ "params" ].push_back( baseArg );

                                for ( model::Property::Ptr pProperty : pInterface->m_args )
                                {
                                    nlohmann::json param
                                        = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                    { "type", pProperty->m_type->getViewType( pStage->m_strName, true ) },
                                                                    { "value", pProperty->m_strName } } );
                                    ctorBaseArgs[ "params" ].push_back( param );
                                }

                                interface[ "args_ctors" ].push_back( ctorBaseArgs );
                            }
                            std::ostringstream osBasePointer;
                            {
                                osBasePointer << pInterface->m_base->delimitTypeName( pStage->m_strName, "::" ) << "*";
                            }
                            {
                                nlohmann::json ctorBasePointer = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                                nlohmann::json baseArg = nlohmann::json::object(
                                    { { "name", "base" }, { "type", osBasePointer.str() }, { "value", "base" } } );
                                ctorBasePointer[ "params" ].push_back( baseArg );

                                for ( model::Property::Ptr pProperty : pInterface->m_args )
                                {
                                    nlohmann::json param
                                        = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                    { "type", pProperty->m_type->getViewType( pStage->m_strName, true ) },
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
                                osBasePointer << pInterface->m_base->delimitTypeName( pStage->m_strName, "::" ) << "*";
                            }
                            {
                                nlohmann::json ctorBasePointer = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                                nlohmann::json baseArg = nlohmann::json::object(
                                    { { "name", "base" }, { "type", osBasePointer.str() }, { "value", "base" } } );
                                ctorBasePointer[ "params" ].push_back( baseArg );

                                for ( model::Property::Ptr pProperty : pInterface->m_args )
                                {
                                    nlohmann::json param
                                        = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                    { "type", pProperty->m_type->getViewType( pStage->m_strName, true ) },
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
                            nlohmann::json param
                                = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                            { "type", pProperty->m_type->getViewType( pStage->m_strName, true ) },
                                                            { "value", pProperty->m_strName } } );
                            ctor[ "params" ].push_back( param );
                        }

                        interface[ "args_ctors" ].push_back( ctor );
                    }

                    for ( model::Property::Ptr pProperty : pInterface->m_args )
                    {
                        std::ostringstream osType;
                        osType << "std::optional< " << pProperty->m_type->getViewType( pStage->m_strName, false ) << " >";
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
                            model::SuperInterface::Ptr pSuper     = pStage->getInterface( pPart->m_object.lock() )->m_superInterface.lock();
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
                    model::PrimaryObjectPart::Ptr pPart  = pInterface->getPrimaryObjectPart();
                    model::SuperInterface::Ptr    pSuper = pInterface->m_superInterface.lock();
                    nlohmann::json                conversion
                        = nlohmann::json::object( { { "type", pInterface->delimitTypeName( pStage->m_strName, "::" ) },
                                                    { "file", pPart->m_file.lock()->m_strName },
                                                    { "supertype", pSuper->getTypeName() },
                                                    { "object", pPart->m_object.lock()->m_strName },
                                                    { "index", pPart->m_typeID } } );
                    stage[ "interface_conversions" ].push_back( conversion );
                }
            }

            void writeAccessors( nlohmann::json& stage, model::Stage::Ptr pStage )
            {
                std::set< model::Interface::Ptr, model::CountedObjectComparator< model::Interface::Ptr > > manyAccessors;
                for ( model::Accessor::Ptr pAccessor : pStage->m_accessors )
                {
                    if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pAccessor->m_type ) )
                    {
                        model::Object::Ptr            pObject      = pRef->m_object;
                        model::Interface::Ptr         pInterface   = pStage->getInterface( pObject );
                        model::PrimaryObjectPart::Ptr pPrimaryPart = pInterface->getPrimaryObjectPart();
                        model::File::Ptr              pFile        = pPrimaryPart->m_file.lock();
                        const bool bReadWrite = std::find( pStage->m_files.begin(), pStage->m_files.end(), pFile ) != pStage->m_files.end();

                        nlohmann::json accessor
                            = nlohmann::json::object( { { "type", pInterface->delimitTypeName( pStage->m_strName, "::" ) },
                                                        { "longname", pInterface->delimitTypeName( pStage->m_strName, "_" ) },
                                                        { "read_write", bReadWrite },
                                                        { "object", pPrimaryPart->m_object.lock()->m_strName },
                                                        { "file", pPrimaryPart->m_file.lock()->m_strName },
                                                        { "stage", pPrimaryPart->m_file.lock()->m_stage.lock()->m_strName },
                                                        { "supertype", pInterface->m_superInterface.lock()->getTypeName() } } );

                        stage[ "one_accessors" ].push_back( accessor );
                        if ( manyAccessors.count( pInterface ) == 0 )
                        {
                            manyAccessors.insert( pInterface );
                            stage[ "many_accessors" ].push_back( accessor );
                        }
                    }
                    else if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pAccessor->m_type ) )
                    {
                        if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pArray->m_underlyingType ) )
                        {
                            model::Object::Ptr            pObject      = pRef->m_object;
                            model::Interface::Ptr         pInterface   = pStage->getInterface( pObject );
                            model::PrimaryObjectPart::Ptr pPrimaryPart = pInterface->getPrimaryObjectPart();
                            model::File::Ptr              pFile        = pPrimaryPart->m_file.lock();
                            const bool                    bReadWrite
                                = std::find( pStage->m_files.begin(), pStage->m_files.end(), pFile ) != pStage->m_files.end();

                            if ( manyAccessors.count( pInterface ) == 0 )
                            {
                                manyAccessors.insert( pInterface );
                                nlohmann::json accessor
                                    = nlohmann::json::object( { { "type", pInterface->delimitTypeName( pStage->m_strName, "::" ) },
                                                                { "longname", pInterface->delimitTypeName( pStage->m_strName, "_" ) },
                                                                { "read_write", bReadWrite },
                                                                { "object", pPrimaryPart->m_object.lock()->m_strName },
                                                                { "file", pPrimaryPart->m_file.lock()->m_strName },
                                                                { "stage", pPrimaryPart->m_file.lock()->m_stage.lock()->m_strName },
                                                                { "supertype", pInterface->m_superInterface.lock()->getTypeName() } } );

                                stage[ "many_accessors" ].push_back( accessor );
                            }
                        }
                        else
                        {
                            THROW_RTE( "Unsupported Accessor type: " );
                        }
                    }
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

                        if ( !pType->m_bLate )
                        {
                            std::ostringstream osExpression, osValidation, osErrorMsg;
                            osErrorMsg << '"';
                            if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
                            {
                                model::Object::Ptr pObject = pRef->m_object;
                                osExpression << "::toData( database, arguments." << pProperty->m_strName << ".value() )";
                                osValidation << "arguments." << pProperty->m_strName << ".has_value() && arguments." << pProperty->m_strName
                                             << ".value()";
                                osErrorMsg << pProperty->m_strName << " is not initialised";
                            }
                            else if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pType ) )
                            {
                                osExpression << "arguments." << pProperty->m_strName << ".value()";
                                osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                osErrorMsg << pProperty->m_strName << " is not initialised";
                            }
                            else if ( model::OptType::Ptr pOptional = std::dynamic_pointer_cast< model::OptType >( pType ) )
                            {
                                model::Type::Ptr pUnderlyingType = pOptional->m_underlyingType;
                                if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                                {
                                    osExpression << "arguments." << pProperty->m_strName << ".value()";
                                    osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                    osErrorMsg << pProperty->m_strName << " is not initialised";
                                }
                                else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                                {
                                    model::Object::Ptr pObject = pRef->m_object;
                                    osExpression << "::toData( database, arguments." << pProperty->m_strName << ".value() )";
                                    osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                    osErrorMsg << pProperty->m_strName << " is not initialised";
                                }
                                else
                                {
                                    THROW_RTE( "Unsupported type for ctor" );
                                }
                            }
                            else if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                            {
                                model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
                                if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                                {
                                    osExpression << "arguments." << pProperty->m_strName << ".value()";
                                    osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                    osErrorMsg << pProperty->m_strName << " is not initialised";
                                }
                                else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                                {
                                    model::Object::Ptr pObject = pRef->m_object;
                                    osExpression << "::toData( database, arguments." << pProperty->m_strName << ".value() )";
                                    osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                    osErrorMsg << pProperty->m_strName << " is not initialised";
                                }
                                else
                                {
                                    THROW_RTE( "Unsupported type for ctor" );
                                }
                            }
                            else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                            {
                                model::Type::Ptr pFrom = pMap->m_fromType;
                                model::Type::Ptr pTo   = pMap->m_toType;

                                if ( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                                {
                                    if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                                    {
                                        osExpression << "arguments." << pProperty->m_strName << ".value()";
                                        osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                        osErrorMsg << pProperty->m_strName << " is not initialised";
                                    }
                                    else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                    {
                                        model::Object::Ptr pObject = pToRef->m_object;
                                        osExpression << "::toData( database, arguments." << pProperty->m_strName << ".value() )";
                                        osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                        osErrorMsg << pProperty->m_strName << " is not initialised";
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
                                        osExpression << "::toData( database, arguments." << pProperty->m_strName << ".value() )";
                                        osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                        osErrorMsg << pProperty->m_strName << " is not initialised";
                                    }
                                    else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                                    {
                                        osExpression << "::toData( database, arguments." << pProperty->m_strName << ".value() )";
                                        osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                                        osErrorMsg << pProperty->m_strName << " is not initialised";
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unsupported type for map from type" );
                                    }
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

                            osErrorMsg << '"';
                            nlohmann::json arg = nlohmann::json::object( { { "expression", osExpression.str() },
                                                                           { "validation", osValidation.str() },
                                                                           { "errormsg", osErrorMsg.str() } } );
                            part[ "args" ].push_back( arg );
                        }
                    }
                }
                return part;
            }

            void writeConstructors( nlohmann::json& stage, model::Stage::Ptr pStage )
            {
                for ( model::Constructor::Ptr pConstructor : pStage->m_constructors )
                {
                    std::ostringstream os;
                    os << "construct_" << pConstructor->m_interface->delimitTypeName( pStage->m_strName, "_" );

                    model::Interface::Ptr      pInterface      = pConstructor->m_interface;
                    model::SuperInterface::Ptr pSuperInterface = pInterface->m_superInterface.lock();

                    nlohmann::json ctor = nlohmann::json::object(
                        { { "return_type", pConstructor->m_interface->delimitTypeName( pStage->m_strName, "::" ) },
                          { "function_name", os.str() },
                          { "super_type_name", pSuperInterface->getTypeName() },
                          { "bases", nlohmann::json::array() },
                          { "bases_reverse", nlohmann::json::array() } } );

                    // add self to list
                    model::Interface::Ptr                pBase = pInterface;
                    std::vector< model::Interface::Ptr > bases_reversed;
                    while ( pBase )
                    {
                        nlohmann::json base = nlohmann::json::object( { { "typename", pBase->delimitTypeName( pStage->m_strName, "::" ) },
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

                        if ( !pBase->ownsPrimaryObjectPart() )
                            break;
                        pBase = pBase->m_base;
                    }
                    std::reverse( bases_reversed.begin(), bases_reversed.end() );
                    for ( model::Interface::Ptr pBase : bases_reversed )
                    {
                        nlohmann::json base = nlohmann::json::object( { { "typename", pBase->delimitTypeName( pStage->m_strName, "::" ) },
                                                                        { "owns_primary_part", pBase->ownsPrimaryObjectPart() } } );

                        model::PrimaryObjectPart::Ptr pPrimaryPart = pBase->getPrimaryObjectPart();
                        base[ "primary_part" ] = nlohmann::json::object( { { "object", pPrimaryPart->m_object.lock()->m_strName },
                                                                           { "file", pPrimaryPart->m_file.lock()->m_strName } } );

                        ctor[ "bases_reverse" ].push_back( base );
                    }

                    stage[ "constructors" ].push_back( ctor );
                }
            }

            nlohmann::json writeFunctionBody( model::Stage::Ptr pStage, model::Function::Ptr pFunction )
            {
                nlohmann::json function;

                model::Type::Ptr pType = pFunction->m_property->m_type;

                std::ostringstream osFunctionBody;

                if ( std::dynamic_pointer_cast< model::FunctionGetter >( pFunction ) )
                {
                    const std::string strData = pType->m_bLate ? "data.value()" : "data";
                    // only for getters want to test late variables have a value set
                    if ( pType->m_bLate )
                    {
                        function[ "lines" ].push_back( "VERIFY_RTE( data.has_value() );" );
                    }
                    if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pType ) )
                    {
                        osFunctionBody << "return " << strData << ";";
                    }
                    else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
                    {
                        model::Object::Ptr    pObject    = pRef->m_object;
                        model::Interface::Ptr pInterface = pStage->getInterface( pObject );
                        osFunctionBody << "return toView( m_factory, " << strData << " );";
                    }
                    else if ( model::OptType::Ptr pOptional = std::dynamic_pointer_cast< model::OptType >( pType ) )
                    {
                        model::Type::Ptr pUnderlyingType = pOptional->m_underlyingType;
                        if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                        {
                            osFunctionBody << "return " << strData << ";";
                        }
                        else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                        {
                            osFunctionBody << "return " << strData << ".has_value() ? toView( m_factory, " << strData
                                           << ".value() ) : nullptr;";
                        }
                        else
                        {
                            THROW_RTE( "Unsupported type for getter" );
                        }
                    }
                    else if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                    {
                        model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
                        if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                        {
                            osFunctionBody << "return " << strData << ";";
                        }
                        else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                        {
                            osFunctionBody << "return toView( m_factory, " << strData << " );";
                        }
                        else
                        {
                            THROW_RTE( "Unsupported type for getter" );
                        }
                    }
                    else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                    {
                        model::Type::Ptr pFrom = pMap->m_fromType;
                        model::Type::Ptr pTo   = pMap->m_toType;
                        if ( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                        {
                            if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                            {
                                osFunctionBody << "return " << strData << ";";
                            }
                            else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                            {
                                osFunctionBody << "return toView( m_factory, " << strData << " );";
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
                                osFunctionBody << "return toView( m_factory, " << strData << " );";
                            }
                            else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                            {
                                osFunctionBody << "return toView( m_factory, " << strData << " );";
                            }
                            else
                            {
                                THROW_RTE( "Unsupported type for map from type" );
                            }
                        }
                        else
                        {
                            THROW_RTE( "Unsupported type for map from type" );
                        }
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
                        osFunctionBody << "data = value;";
                    }
                    else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
                    {
                        model::Object::Ptr pObject = pRef->m_object;
                        osFunctionBody << "data = toData( m_factory, value );\n";
                    }
                    else if ( model::OptType::Ptr pOptional = std::dynamic_pointer_cast< model::OptType >( pType ) )
                    {
                        model::Type::Ptr pUnderlyingType = pOptional->m_underlyingType;
                        if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                        {
                            osFunctionBody << "data = value;";
                        }
                        else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                        {
                            model::Object::Ptr pObject = pRef->m_object;
                            osFunctionBody << "data = toData( m_factory, value );\n";
                        }
                        else
                        {
                            THROW_RTE( "Unsupported type for setter" );
                        }
                    }
                    else if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                    {
                        model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
                        if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                        {
                            osFunctionBody << "data = value;";
                        }
                        else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                        {
                            model::Object::Ptr pObject = pRef->m_object;
                            osFunctionBody << "data = toData( m_factory, value );\n";
                        }
                        else
                        {
                            THROW_RTE( "Unsupported type for setter" );
                        }
                    }
                    else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                    {
                        model::Type::Ptr pFrom = pMap->m_fromType;
                        model::Type::Ptr pTo   = pMap->m_toType;
                        if ( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                        {
                            if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                            {
                                osFunctionBody << "data = value;";
                            }
                            else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                            {
                                model::Object::Ptr pObject = pToRef->m_object;
                                osFunctionBody << "data = toData( m_factory, value );\n";
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
                                osFunctionBody << "data = toData( m_factory, value );";
                            }
                            else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                            {
                                osFunctionBody << "data = toData( m_factory, value );";
                            }
                            else
                            {
                                THROW_RTE( "Unsupported type for map from type" );
                            }
                        }
                        else
                        {
                            THROW_RTE( "Unsupported type for map from type" );
                        }
                    }
                    else
                    {
                        THROW_RTE( "Unsupported type for setter" );
                    }
                }
                else if ( std::dynamic_pointer_cast< model::FunctionInserter >( pFunction ) )
                {
                    const std::string strData = pType->m_bLate ? "data.value()" : "data";
                    if ( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                    {
                        if ( pType->m_bLate )
                            osFunctionBody << "if( !data.has_value() ) data = " << pArray->getDatabaseType( model::Type::eNormal_NoLate )
                                           << "()\n;";
                        model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
                        if ( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                        {
                            osFunctionBody << strData << ".push_back( value );";
                        }
                        else if ( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
                        {
                            model::Object::Ptr pObject = pRef->m_object;
                            osFunctionBody << strData << ".push_back( toData( m_factory, value ) );";
                        }
                        else
                        {
                            THROW_RTE( "Unsupported type for inserter" );
                        }
                    }
                    else if ( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                    {
                        if ( pType->m_bLate )
                            osFunctionBody << "if( !data.has_value() ) data = " << pMap->getDatabaseType( model::Type::eNormal_NoLate )
                                           << "()\n;";
                        model::Type::Ptr pFrom = pMap->m_fromType;
                        model::Type::Ptr pTo   = pMap->m_toType;
                        if ( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                        {
                            if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                            {
                                osFunctionBody << strData << ".insert( std::make_pair( key, value ) );";
                            }
                            else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                            {
                                model::Object::Ptr pObject = pToRef->m_object;
                                osFunctionBody << strData << ".insert( std::make_pair( key, toData( m_factory, value ) ) );";
                            }
                            else
                            {
                                THROW_RTE( "Unsupported type for map from type" );
                            }
                        }
                        else if ( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
                        {
                            model::Object::Ptr pObject = pFromRef->m_object;
                            if ( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                            {
                                osFunctionBody << strData << ".insert( std::make_pair( toData( m_factory, key ), value ) );";
                            }
                            else if ( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                            {
                                osFunctionBody << strData << ".insert( std::make_pair( toData( m_factory, key ), toData( m_factory, value ) ) );";
                            }
                            else
                            {
                                THROW_RTE( "Unsupported type for map from type" );
                            }
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

                function[ "lines" ].push_back( osFunctionBody.str() );

                return function;
            }

            void writeSuperTypes( nlohmann::json& stage, model::Stage::Ptr pStage )
            {
                for ( model::SuperInterface::Ptr pSuperType : pStage->m_superInterfaces )
                {
                    for ( model::Interface::Ptr pInterface : pSuperType->m_interfaces )
                    {
                        nlohmann::json cast
                            = nlohmann::json::object( { { "type", pInterface->delimitTypeName( pStage->m_strName, "::" ) },
                                                        { "fullname", pInterface->delimitTypeName( pStage->m_strName, "_" ) } } );
                        stage[ "casts" ].push_back( cast );
                    }
                }

                for ( model::SuperInterface::Ptr pSuperType : pStage->m_superInterfaces )
                {
                    const std::string strSuperTypeName = pSuperType->getTypeName();

                    nlohmann::json stype = nlohmann::json::object( { { "name", strSuperTypeName },
                                                                     { "interfaces", nlohmann::json::array() },
                                                                     { "functions", nlohmann::json::array() } } );

                    for ( model::Interface::Ptr pInterface : pSuperType->m_interfaces )
                    {
                        model::ObjectPart::Ptr pPrimaryObjectPart = pInterface->m_object.lock()->m_primaryObjectPart;

                        nlohmann::json interface = nlohmann::json::object(
                            { { "name", pInterface->delimitTypeName( pStage->m_strName, "::" ) },
                              { "fullname", pInterface->delimitTypeName( pStage->m_strName, "_" ) },
                              { "part", pPrimaryObjectPart->getDataType( "::" ) },
                              { "casts", nlohmann::json::array() } } );

                        for ( model::Interface::Ptr pOtherInterfaces : pSuperType->m_interfaces )
                        {
                            model::ObjectPart::Ptr pOtherPrimaryObjectPart = pOtherInterfaces->m_object.lock()->m_primaryObjectPart;

                            bool bCastOK = false;

                            model::Interface::Ptr pIter = pOtherInterfaces;
                            while ( pIter )
                            {
                                if ( pIter == pInterface )
                                {
                                    bCastOK = true;
                                    break;
                                }
                                pIter = pIter->m_base;
                            }

                            nlohmann::json cast = nlohmann::json::object(
                                { { "type", pOtherPrimaryObjectPart->getDataType( "::" ) }, { "truth", bCastOK } } );
                            interface[ "casts" ].push_back( cast );
                        }
                        stype[ "interfaces" ].push_back( interface );
                    }

                    // the functions on the super type are groups by their mangled function name
                    // TODO: ignore return type and insead check returns types are the same
                    for ( model::SuperInterface::FunctionMultiMap::iterator iLower = pSuperType->m_functions.begin(),
                                                                            iEnd   = pSuperType->m_functions.end();
                          iLower != iEnd; )
                    {
                        // nlohmann::json function = writeFunction( pFunction );
                        model::Function::Ptr pFunction = iLower->second;

                        nlohmann::json function = nlohmann::json::object(
                            { { "name", pFunction->getName() },
                              { "returntype", pFunction->getReturnType( pStage->m_strName ) },
                              { "propertytype", pFunction->m_property->m_type->getDatabaseType( model::Type::eNormal ) },
                              { "property", pFunction->m_property->m_strName },
                              { "params", pFunction->getParams( pStage->m_strName ) },
                              { "body", writeFunctionBody( pStage, pFunction ) } } );

                        std::set< model::Interface::Ptr, model::CountedObjectComparator< model::Interface::Ptr > > remaining;
                        using InterfaceFunctionMap = std::map< model::Interface::Ptr, model::Function::Ptr,
                                                               model::CountedObjectComparator< model::Interface::Ptr > >;
                        InterfaceFunctionMap implemented;
                        {
                            for ( model::Interface::Ptr pInterface : pSuperType->m_interfaces )
                            {
                                remaining.insert( pInterface );
                            }
                        }

                        // functions in the group are where an inherited interface explicitly has the function
                        // NOTE: this DOES NOT include how interfaces inherit others
                        for ( model::SuperInterface::FunctionMultiMap::iterator iUpper
                              = pSuperType->m_functions.upper_bound( iLower->first );
                              iLower != iUpper;
                              ++iLower )
                        {
                            model::Function::Ptr  pFunctionVariant = iLower->second;
                            model::Interface::Ptr pInterface       = pFunctionVariant->m_interface.lock();
                            VERIFY_RTE( pInterface );
                            remaining.erase( pInterface );
                            implemented.insert( std::make_pair( pInterface, pFunctionVariant ) );

                            nlohmann::json variant = nlohmann::json::object(
                                { { "matched", true },
                                  { "primaryobjectpart", pInterface->getPrimaryObjectPart()->getDataType( "::" ) },
                                  { "dataobjectpart", pFunctionVariant->m_property->m_objectPart.lock()->getDataType( "::" ) } } );

                            function[ "variants" ].push_back( variant );
                        }
                        // so from the remaining interfaces in the super type either the
                        // interface inherits the function or it doesnt
                        for ( model::Interface::Ptr pInterface : remaining )
                        {
                            // see if the interface inherited from one that implemented the function
                            model::Function::Ptr pFunctionVariant;

                            if ( pInterface->m_base )
                            {
                                model::Interface::Ptr pBase = pInterface->m_base;
                                while ( pBase )
                                {
                                    InterfaceFunctionMap::iterator iFind = implemented.find( pBase );
                                    if ( iFind != implemented.end() )
                                    {
                                        pFunctionVariant = iFind->second;
                                        break;
                                    }
                                    pBase = pBase->m_base;
                                }
                            }

                            if ( pFunctionVariant )
                            {
                                nlohmann::json variant = nlohmann::json::object(
                                    { { "matched", true },
                                      { "primaryobjectpart", pInterface->getPrimaryObjectPart()->getDataType( "::" ) },
                                      { "dataobjectpart", pFunctionVariant->m_property->m_objectPart.lock()->getDataType( "::" ) } } );

                                function[ "variants" ].push_back( variant );
                            }
                            else
                            {
                                nlohmann::json variant = nlohmann::json::object(
                                    { { "matched", false },
                                      { "primaryobjectpart", pInterface->getPrimaryObjectPart()->getDataType( "::" ) },
                                      { "dataobjectpart", "" } } );
                                function[ "variants" ].push_back( variant );
                            }
                        }

                        stype[ "functions" ].push_back( function );
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
                                                                     { "perobject", true },
                                                                     { "super_conversions", nlohmann::json::array() },
                                                                     { "interface_conversions", nlohmann::json::array() },
                                                                     { "readwrite_files", nlohmann::json::array() },
                                                                     { "many_accessors", nlohmann::json::array() },
                                                                     { "one_accessors", nlohmann::json::array() },
                                                                     { "one_opt_accessors", nlohmann::json::array() },
                                                                     { "constructors", nlohmann::json::array() },
                                                                     { "interfaces", nlohmann::json::array() },
                                                                     { "casts", nlohmann::json::array() } } );

                    writeConversions( stage, pSchema, pStage );

                    for ( model::File::Ptr pFile : pStage->m_files )
                    {
                        nlohmann::json file = nlohmann::json::object( { { "name", pFile->m_strName } } );
                        stage[ "readwrite_files" ].push_back( file );
                    }

                    for ( model::Interface::Ptr pInterface : pStage->m_interfaceTopological )
                    {
                        stage[ "interfaces" ].push_back( writeInterface( pStage, pInterface ) );
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
                nlohmann::json data( { { "files", nlohmann::json::array() },
                                       { "conversions", nlohmann::json::array() },
                                       { "up_casts", nlohmann::json::array() } } );

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
                                                                    { "perobject", true }, // pFile->m_stage.lock()->m_bPerObject
                                                                    { "parts", nlohmann::json::array() } } );

                    for ( model::ObjectPart::Ptr pPart : pFile->m_parts )
                    {
                        model::Object::Ptr pObject = pPart->m_object.lock();

                        VERIFY_RTE( pObject->m_primaryObjectPart );
                        model::ObjectPart::Ptr pPrimaryPart   = pObject->m_primaryObjectPart;
                        const bool             bIsPrimaryPart = pPrimaryPart == pPart;

                        nlohmann::json part = nlohmann::json::object( { { "name", pObject->m_strName },
                                                                        { "typeID", pPart->m_typeID },
                                                                        { "has_base", false },
                                                                        { "pointers", nlohmann::json::array() },
                                                                        { "properties", nlohmann::json::array() },
                                                                        { "initialisations", nlohmann::json::array() } } );

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

                                part[ "has_base" ] = true;
                                part[ "base" ]     = pObject->m_base->m_primaryObjectPart->getPointerName();
                            }
                        }

                        for ( model::Property::Ptr pProperty : pPart->m_properties )
                        {
                            model::Type::Ptr pType = pProperty->m_type;
                            if ( !pType->m_bLate )
                            {
                                bool bIsPointer = false;
                                if ( std::dynamic_pointer_cast< model::RefType >( pType ) )
                                    bIsPointer = true;

                                nlohmann::json init
                                    = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                                { "type", pType->getDatabaseType( model::Type::eNormal ) },
                                                                { "argtype", pType->getDatabaseType( model::Type::eAsArgument ) },
                                                                { "is_pointer", bIsPointer } } );

                                part[ "initialisations" ].push_back( init );
                            }
                        }

                        for ( model::Property::Ptr pProperty : pPart->m_properties )
                        {
                            model::Type::Ptr pType = pProperty->m_type;

                            bool bIsPointer = false;
                            if ( std::dynamic_pointer_cast< model::RefType >( pType ) )
                                bIsPointer = true;

                            nlohmann::json property
                                = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                            { "type", pType->getDatabaseType( model::Type::eNormal ) },
                                                            { "argtype", pType->getDatabaseType( model::Type::eAsArgument ) },
                                                            { "is_pointer", bIsPointer },
                                                            { "has_validation", false } } );

                            if ( pType->m_bLate )
                            {
                                std::ostringstream os;
                                os << "VERIFY_RTE_MSG( " << pProperty->m_strName << ".has_value(), \""
                                   << pObject->m_primaryObjectPart->getDataType( "::" ) << "." << pProperty->m_strName
                                   << " has NOT been set\" );";
                                property[ "validation" ]     = os.str();
                                property[ "has_validation" ] = true;
                            }

                            part[ "properties" ].push_back( property );
                        }

                        file[ "parts" ].push_back( part );
                    }

                    data[ "files" ].push_back( file );
                }

                // conversions
                {
                    for ( model::Schema::ConversionMap::const_iterator i    = pSchema->m_conversions.begin(),
                                                                       iEnd = pSchema->m_conversions.end();
                          i != iEnd;
                          ++i )
                    {
                        const model::Schema::ObjectPartPair&   parts    = i->first;
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

                // up casts
                {
                    for ( model::Schema::ConversionMap::const_iterator i = pSchema->m_upcasts.begin(), iEnd = pSchema->m_upcasts.end();
                          i != iEnd; ++i )
                    {
                        model::ObjectPart::Ptr                 pFrom    = i->first.first;
                        model::ObjectPart::Ptr                 pTo      = i->first.second;
                        const model::Schema::ObjectPartVector& sequence = i->second;

                        nlohmann::json upcast = nlohmann::json::object( { { "from", pFrom->getDataType( "::" ) },
                                                                          { "to", pTo->getDataType( "::" ) },
                                                                          { "final_var", "from" },
                                                                          { "casts", nlohmann::json::array() },
                                                                          { "casts_reversed", nlohmann::json::array() } } );

                        std::size_t szCounter = 0;
                        std::string strFrom   = "from";
                        for ( model::ObjectPart::Ptr pPart : sequence )
                        {
                            std::ostringstream os;
                            os << "p" << ++szCounter;

                            nlohmann::json cast = nlohmann::json::object(
                                { { "type", pPart->getDataType( "::" ) }, { "var", os.str() }, { "from_var", strFrom } } );
                            strFrom = os.str();

                            upcast[ "casts" ].push_back( cast );
                            upcast[ "final_var" ] = os.str();
                        }

                        for ( model::ObjectPart::Ptr pPart : sequence )
                        {
                            std::ostringstream os;
                            os << "p" << --szCounter;
                            if ( os.str() == "p0" )
                                break;

                            nlohmann::json cast = nlohmann::json::object( { { "type", pPart->getDataType( "::" ) }, { "var", os.str() } } );
                            upcast[ "casts_reversed" ].push_back( cast );
                        }

                        nlohmann::json cast = nlohmann::json::object( { { "type", pFrom->getDataType( "::" ) }, { "var", "from" } } );
                        upcast[ "casts_reversed" ].push_back( cast );

                        data[ "up_casts" ].push_back( upcast );
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
