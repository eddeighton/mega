//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
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

#include "json_converter.hpp"

#include "database/compiler/model.hpp"
#include "nlohmann/json.hpp"

#include "common/file.hpp"

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
    std::ostringstream os;
    os << data;

    if( boost::filesystem::updateFileIfChanged( filePath, os.str() ) )
    {
        std::cout << "Regenerated: " << filePath.string() << std::endl;
    }
}

void insertFunctionVariantIfUnique( nlohmann::json& function, const nlohmann::json& variant )
{
    bool bFound = false;

    for( const auto& existingVariant : function[ "variants" ] )
    {
        // see data.cxx.jinja:175 - visitor dispatch over unique primaryobjectpart
        // NOTE: not comparing existingVariant[ "dataobjectpart" ]     == variant[ "dataobjectpart" ] &&
        if( existingVariant[ "matched" ] == variant[ "matched" ]
            && existingVariant[ "primaryobjectpart" ] == variant[ "primaryobjectpart" ] )
        {
            bFound = true;
            break;
        }
    }

    if( !bFound )
    {
        function[ "variants" ].push_back( variant );
    }
}

void writeStageData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
{
    nlohmann::json data( { { "sources", nlohmann::json::array() },
                           { "stages", nlohmann::json::array() },
                           { "schema_version", pSchema->m_schemaHash.toHexString() } } );

    // get the unique source types
    for( model::Source::Ptr pSource : pSchema->m_sources )
    {
        nlohmann::json source
            = nlohmann::json::object( { { "type", pSource->m_strName }, { "files", nlohmann::json::array() } } );

        for( std::weak_ptr< model::Stage > pStageWeak : pSource->m_stages )
        {
            model::Stage::Ptr pStage = pStageWeak.lock();
            for( model::File::Ptr pFile : pStage->m_files )
            {
                nlohmann::json file
                    = nlohmann::json::object( { { "name", pFile->m_strName }, { "stage", pStage->m_strName } } );
                source[ "files" ].push_back( file );
            }
        }
        data[ "sources" ].push_back( source );
    }
    for( model::Stage::Ptr pStage : pSchema->m_stages )
    {
        VERIFY_RTE( !pStage->m_sources.empty() );
        nlohmann::json stage = nlohmann::json::object( { { "name", pStage->m_strName },
                                                         { "sources", nlohmann::json::array() },
                                                         { "files", nlohmann::json::array() } } );

        for( model::Source::Ptr pSource : pStage->m_sources )
        {
            stage[ "sources" ].push_back( pSource->m_strName );
        }

        for( model::File::Ptr pFile : pStage->m_files )
        {
            nlohmann::json file = nlohmann::json::object(
                { { "name", pFile->m_strName }, { "dependencies", nlohmann::json::array() } } );

            for( std::weak_ptr< model::File > pDependencyWeak : pFile->m_dependencies )
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
    function[ "short_name" ] = pFunction->getShortName();
    function[ "long_name" ]  = pFunction->getLongName();
    function[ "returntype" ] = pFunction->getReturnType( pStage->m_strName );
    function[ "params" ]     = nlohmann::json::array();
    for( const auto& param : pFunction->getParams( pStage->m_strName ) )
    {
        const nlohmann::json paramData = nlohmann::json::object( { { "name", param.name }, { "type", param.type } } );
        function[ "params" ].push_back( paramData );
    }
    return function;
}

nlohmann::json writeInterface( model::Stage::Ptr pStage, model::Interface::Ptr pInterface )
{
    model::Object::Ptr pObject   = pInterface->m_object.lock();
    nlohmann::json     interface = nlohmann::json::object( {
        { "name", pObject->getIdentifier() },
        { "fullname", pInterface->delimitTypeName( pStage->m_strName, "_" ) },
        { "readwrite", pInterface->m_isReadWrite },
        { "functions", nlohmann::json::array() },
        { "args_ctors", nlohmann::json::array() },
        { "args_values", nlohmann::json::array() },
        { "tests", nlohmann::json::array() },
    } );

    // base interface
    if( pInterface->m_base )
    {
        interface[ "has_base" ] = true;
        interface[ "base" ]     = pInterface->m_base->delimitTypeName( pStage->m_strName, "::" );
    }
    else
    {
        interface[ "has_base" ] = false;
    }

    for( model::Interface::Ptr pOtherInterface : pInterface->m_superInterface.lock()->m_interfaces )
    {
        interface[ "tests" ].push_back( pOtherInterface->delimitTypeName( pStage->m_strName, "_" ) );
    }

    // namespaces
    {
        std::vector< model::Namespace::Ptr > namespaces;
        {
            model::Namespace::Ptr pNamespace = pObject->m_namespace.lock();
            VERIFY_RTE( pNamespace );
            while( pNamespace )
            {
                namespaces.push_back( pNamespace );
                pNamespace = pNamespace->m_namespace.lock();
            }
        }
        std::reverse( namespaces.begin(), namespaces.end() );
        for( model::Namespace::Ptr pNamespace : namespaces )
        {
            interface[ "namespaces" ].push_back( pNamespace->m_strName );
        }
    }

    // functions
    {
        std::set< std::string > uniqueFunctions;
        for( model::Function::Ptr pFunction : pInterface->m_functions )
        {
            const std::string strMangled = pFunction->getMangledName( "" );
            if( uniqueFunctions.count( strMangled ) == 0U )
            {
                uniqueFunctions.insert( strMangled );
                interface[ "functions" ].push_back( writeFunctionSignature( pStage, pFunction ) );
            }
        }
    }

    // args
    if( pInterface->m_isReadWrite )
    {
        // calculate ctor overloads...
        if( !pInterface->m_args.empty() )
        {
            // add default
            nlohmann::json ctor = nlohmann::json::object( { { "params", nlohmann::json::array() } } );
            interface[ "args_ctors" ].push_back( ctor );
        }

        if( pInterface->m_base )
        {
            if( pInterface->m_base->ownsPrimaryObjectPart( pStage ) )
            {
                std::ostringstream osBaseArgs;
                {
                    osBaseArgs << pInterface->m_base->delimitTypeName( pStage->m_strName, "::" ) << "::Args";
                }
                {
                    nlohmann::json ctorBaseArgs = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                    nlohmann::json baseArg = nlohmann::json::object(
                        { { "name", "base" }, { "type", osBaseArgs.str() }, { "value", "base" } } );
                    ctorBaseArgs[ "params" ].push_back( baseArg );

                    for( model::Property::Ptr pProperty : pInterface->m_args )
                    {
                        nlohmann::json param = nlohmann::json::object(
                            { { "name", pProperty->m_strName },
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
                    nlohmann::json ctorBasePointer
                        = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                    nlohmann::json baseArg = nlohmann::json::object(
                        { { "name", "base" }, { "type", osBasePointer.str() }, { "value", "base" } } );
                    ctorBasePointer[ "params" ].push_back( baseArg );

                    for( model::Property::Ptr pProperty : pInterface->m_args )
                    {
                        nlohmann::json param = nlohmann::json::object(
                            { { "name", pProperty->m_strName },
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

                if( !pInterface->ownsPrimaryObjectPart( pStage ) )
                {
                    osBasePointer << pInterface->delimitTypeName( pStage->m_strName, "::" ) << "*";
                }
                else
                {
                    osBasePointer << pInterface->m_base->delimitTypeName( pStage->m_strName, "::" ) << "*";
                }
                {
                    nlohmann::json ctorBasePointer
                        = nlohmann::json::object( { { "params", nlohmann::json::array() } } );

                    nlohmann::json baseArg = nlohmann::json::object(
                        { { "name", "base" }, { "type", osBasePointer.str() }, { "value", "base" } } );
                    ctorBasePointer[ "params" ].push_back( baseArg );

                    for( model::Property::Ptr pProperty : pInterface->m_args )
                    {
                        nlohmann::json param = nlohmann::json::object(
                            { { "name", pProperty->m_strName },
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

            if( !pInterface->ownsPrimaryObjectPart( pStage ) )
            {
                std::ostringstream osBasePointer;
                {
                    osBasePointer << pInterface->delimitTypeName( pStage->m_strName, "::" ) << "*";
                }
                {
                    nlohmann::json baseArg = nlohmann::json::object(
                        { { "name", "base" }, { "type", osBasePointer.str() }, { "value", "base" } } );
                    ctor[ "params" ].push_back( baseArg );
                }
                std::ostringstream osType;
                osType << "std::optional< std::variant< " << osBasePointer.str() << " > >";
                nlohmann::json value = nlohmann::json::object( { { "name", "base" }, { "type", osType.str() } } );
                interface[ "args_values" ].push_back( value );
            }

            for( model::Property::Ptr pProperty : pInterface->m_args )
            {
                nlohmann::json param
                    = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                { "type", pProperty->m_type->getViewType( pStage->m_strName, true ) },
                                                { "value", pProperty->m_strName } } );
                ctor[ "params" ].push_back( param );
            }

            interface[ "args_ctors" ].push_back( ctor );
        }

        for( model::Property::Ptr pProperty : pInterface->m_args )
        {
            std::ostringstream osType;
            osType << "std::optional< " << pProperty->m_type->getViewType( pStage->m_strName, false ) << " >";
            nlohmann::json value
                = nlohmann::json::object( { { "name", pProperty->m_strName }, { "type", osType.str() } } );
            interface[ "args_values" ].push_back( value );
        }
    }

    return interface;
}

void writeConversions( nlohmann::json& stage, model::Schema::Ptr pSchema, model::Stage::Ptr pStage )
{
    std::vector< model::Stage::Ptr > dependencies;
    pStage->getDependencies( dependencies );
    for( model::Stage::Ptr pStageIter : dependencies )
    {
        for( model::File::Ptr pFile : pStageIter->m_files )
        {
            for( model::ObjectPart::Ptr pPart : pFile->m_parts )
            {
                VERIFY_RTE_MSG( pStage->isInterface( pPart->m_object.lock() ), "Stage missing interface for object" );
                {
                    model::SuperType::Ptr pSuper
                        = pStage->getInterface( pPart->m_object.lock() )->m_superInterface.lock();
                    nlohmann::json conversion
                        = nlohmann::json::object( { { "type_id", pPart->m_typeID },
                                                    { "supertype", pSuper->getTypeName() },
                                                    { "file", pFile->m_strName },
                                                    { "object", pPart->m_object.lock()->getDataTypeName() } } );
                    stage[ "super_conversions" ].push_back( conversion );
                }
            }
        }
    }
    // std::set< std::string > conversionsUnique;
    for( model::Interface::Ptr pInterface : pStage->m_interfaceTopological )
    {
        auto pPart = pInterface->getPrimaryObjectPart( pStage );
        // for ( auto pPart : pInterface->getPrimaryObjectParts() )
        {
            // auto partStage = pPart->m_file.lock()->m_stage.lock();
            //  if ( pStage->isDependency( partStage ) )
            //  if ( pStage == partStage )
            {
                const std::string strType = pInterface->delimitTypeName( pStage->m_strName, "::" );

                model::SuperType::Ptr pSuper = pInterface->m_superInterface.lock();
                nlohmann::json        conversion
                    = nlohmann::json::object( { { "type", strType },
                                                { "file", pPart->m_file.lock()->m_strName },
                                                { "supertype", pSuper->getTypeName() },
                                                { "object", pPart->m_object.lock()->getDataTypeName() },
                                                { "index", pPart->m_typeID } } );
                stage[ "conversions_view_to_data" ].push_back( conversion );
            }
        }
    }

    for( model::Interface::Ptr pInterface : pStage->m_interfaceTopological )
    {
        for( auto pPart : pInterface->getPrimaryObjectParts() )
        {
            const std::string     strType = pInterface->delimitTypeName( pStage->m_strName, "::" );
            model::SuperType::Ptr pSuper  = pInterface->m_superInterface.lock();
            nlohmann::json        conversion
                = nlohmann::json::object( { { "type", strType },
                                            { "file", pPart->m_file.lock()->m_strName },
                                            { "supertype", pSuper->getTypeName() },
                                            { "object", pPart->m_object.lock()->getDataTypeName() },
                                            { "index", pPart->m_typeID } } );
            stage[ "conversions_data_to_view" ].push_back( conversion );
        }
    }
}

nlohmann::json writeAccessor( model::Stage::Ptr pStage, model::Interface::Ptr pInterface )
{
    nlohmann::json accessor
        = nlohmann::json::object( { { "type", pInterface->delimitTypeName( pStage->m_strName, "::" ) },
                                    { "longname", pInterface->delimitTypeName( pStage->m_strName, "_" ) },
                                    { "files", nlohmann::json::array() } } );

    for( auto pPrimaryPart : pInterface->getPrimaryObjectParts() )
    {
        model::File::Ptr pFile = pPrimaryPart->m_file.lock();
        const bool       bReadWrite
            = std::find( pStage->m_files.begin(), pStage->m_files.end(), pFile ) != pStage->m_files.end();

        nlohmann::json file_accessor
            = nlohmann::json::object( { { "read_write", bReadWrite },
                                        { "object", pPrimaryPart->m_object.lock()->getDataTypeName() },
                                        { "file", pPrimaryPart->m_file.lock()->m_strName },
                                        { "stage", pPrimaryPart->m_file.lock()->m_stage.lock()->m_strName } } );

        accessor[ "files" ].push_back( file_accessor );
    }
    return accessor;
}

void writeAccessors( nlohmann::json& stage, model::Stage::Ptr pStage )
{
    std::set< model::Interface::Ptr, model::CountedObjectComparator< model::Interface::Ptr > > manyAccessors;
    for( model::Accessor::Ptr pAccessor : pStage->m_accessors )
    {
        if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pAccessor->m_type ) )
        {
            model::Object::Ptr pObject = pRef->m_object;
            VERIFY_RTE_MSG( pStage->isInterface( pObject ), "Stage missing interface for accessor" );
            model::Interface::Ptr pInterface = pStage->getInterface( pObject );

            nlohmann::json accessor = writeAccessor( pStage, pInterface );

            stage[ "one_accessors" ].push_back( accessor );
            if( manyAccessors.count( pInterface ) == 0 )
            {
                manyAccessors.insert( pInterface );
                stage[ "many_accessors" ].push_back( accessor );
            }
        }
        else if( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pAccessor->m_type ) )
        {
            if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pArray->m_underlyingType ) )
            {
                model::Object::Ptr pObject = pRef->m_object;
                VERIFY_RTE_MSG( pStage->isInterface( pObject ), "Stage missing interface for accessor" );
                model::Interface::Ptr pInterface = pStage->getInterface( pObject );

                if( manyAccessors.count( pInterface ) == 0 )
                {
                    manyAccessors.insert( pInterface );
                    nlohmann::json accessor = writeAccessor( pStage, pInterface );
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

nlohmann::json writeCtorPart( model::Stage::Ptr pStage, model::ObjectPart::Ptr pPart, bool bAddBaseArg )
{
    nlohmann::json part = nlohmann::json::object( { { "object", pPart->m_object.lock()->getDataTypeName() },
                                                    { "file", pPart->m_file.lock()->m_strName },
                                                    { "args", nlohmann::json::array() } } );
    {
        if( bAddBaseArg )
        {
            nlohmann::json arg = nlohmann::json::object(
                { { "expression", "pPrimaryObjectPart" }, { "validation", false }, { "errormsg", "" } } );
            part[ "args" ].push_back( arg );
        }

        for( model::Property::Ptr pProperty : pPart->m_properties )
        {
            model::Type::Ptr pType = pProperty->m_type;

            if( !pType->m_bLate )
            {
                std::ostringstream osExpression, osValidation, osErrorMsg;
                osErrorMsg << '"';
                if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
                {
                    model::Object::Ptr pObject = pRef->m_object;
                    osExpression << "::toData( database, arguments." << pProperty->m_strName << ".value() )";
                    osValidation << "arguments." << pProperty->m_strName << ".has_value() && arguments."
                                 << pProperty->m_strName << ".value()";
                    osErrorMsg << pProperty->m_strName << " is not initialised";
                }
                else if( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pType ) )
                {
                    osExpression << "arguments." << pProperty->m_strName << ".value()";
                    osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                    osErrorMsg << pProperty->m_strName << " is not initialised";
                }
                else if( model::OptType::Ptr pOptional = std::dynamic_pointer_cast< model::OptType >( pType ) )
                {
                    model::Type::Ptr pUnderlyingType = pOptional->m_underlyingType;
                    if( model::ValueType::Ptr pValue
                        = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                    {
                        osExpression << "arguments." << pProperty->m_strName << ".value()";
                        osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                        osErrorMsg << pProperty->m_strName << " is not initialised";
                    }
                    else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
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
                else if( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
                {
                    model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
                    if( model::ValueType::Ptr pValue
                        = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
                    {
                        osExpression << "arguments." << pProperty->m_strName << ".value()";
                        osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                        osErrorMsg << pProperty->m_strName << " is not initialised";
                    }
                    else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
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
                else if( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
                {
                    model::Type::Ptr pFrom = pMap->m_fromType;
                    model::Type::Ptr pTo   = pMap->m_toType;

                    if( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
                    {
                        if( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                        {
                            osExpression << "arguments." << pProperty->m_strName << ".value()";
                            osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                            osErrorMsg << pProperty->m_strName << " is not initialised";
                        }
                        else if( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
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
                    else if( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
                    {
                        if( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                        {
                            osExpression << "::toData( database, arguments." << pProperty->m_strName << ".value() )";
                            osValidation << "arguments." << pProperty->m_strName << ".has_value()";
                            osErrorMsg << pProperty->m_strName << " is not initialised";
                        }
                        else if( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
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
    for( model::Constructor::Ptr pConstructor : pStage->m_constructors )
    {
        std::ostringstream os;
        os << "construct_" << pConstructor->m_interface->delimitTypeName( pStage->m_strName, "_" );

        model::Interface::Ptr pInterface      = pConstructor->m_interface;
        model::SuperType::Ptr pSuperInterface = pInterface->m_superInterface.lock();

        // for ( auto pPart : pInterface->getPrimaryObjectParts() )
        {
            auto       pPart            = pInterface->getPrimaryObjectPart( pStage );
            const bool bIsReconstructor = !pInterface->ownsPrimaryObjectPart( pPart );

            nlohmann::json ctor = nlohmann::json::object(
                { { "return_type", pConstructor->m_interface->delimitTypeName( pStage->m_strName, "::" ) },
                  { "function_name", os.str() },
                  { "super_type_name", pSuperInterface->getTypeName() },
                  { "bases", nlohmann::json::array() },
                  { "bases_reverse", nlohmann::json::array() },
                  { "reconstructor", bIsReconstructor } } );

            // add self to list

            std::vector< nlohmann::json > bases;
            {
                model::Interface::Ptr pBase = pInterface;
                while( pBase )
                {
                    nlohmann::json base
                        = nlohmann::json::object( { { "typename", pBase->delimitTypeName( pStage->m_strName, "::" ) },
                                                    { "owns_primary_part", pBase->ownsPrimaryObjectPart( pStage ) },
                                                    { "parts", nlohmann::json::array() },
                                                    { "is_primary_part_base", false } } );

                    base[ "primary_part" ] = writeCtorPart( pStage, pBase->getPrimaryObjectPart( pStage ), false );
                    if( pBase->m_base )
                    {
                        base[ "is_primary_part_base" ] = true;
                        base[ "primary_part_base" ]
                            = writeCtorPart( pStage, pBase->m_base->getPrimaryObjectPart( pStage ), false );
                    }
                    // VERIFY_RTE( pBase->m_readOnlyObjectParts.empty() );
                    for( model::ObjectPart::Ptr pPart : pBase->m_readWriteObjectParts )
                    {
                        if( model::InheritedObjectPart::Ptr pSecondaryPart
                            = std::dynamic_pointer_cast< model::InheritedObjectPart >( pPart ) )
                        {
                            base[ "parts" ].push_back( writeCtorPart( pStage, pPart, true ) );
                        }
                        else if( model::AggregatedObjectPart::Ptr pSecondaryPart
                                 = std::dynamic_pointer_cast< model::AggregatedObjectPart >( pPart ) )
                        {
                            base[ "parts" ].push_back( writeCtorPart( pStage, pPart, false ) );
                        }
                    }
                    bases.push_back( base );
                    if( !pBase->ownsPrimaryObjectPart( pStage ) )
                        break;
                    pBase = pBase->m_base;
                }
            }
            for( const nlohmann::json& base : bases )
                ctor[ "bases" ].push_back( base );
            std::reverse( bases.begin(), bases.end() );
            for( const nlohmann::json& base : bases )
                ctor[ "bases_reverse" ].push_back( base );

            stage[ "constructors" ].push_back( ctor );
        }
    }
}

nlohmann::json writeFunctionBody( model::Stage::Ptr pStage, model::Function::Ptr pFunction )
{
    nlohmann::json function;

    model::Type::Ptr pType = pFunction->m_property->m_type;

    std::ostringstream osFunctionBody;

    if( std::dynamic_pointer_cast< model::FunctionGetter >( pFunction ) )
    {
        const std::string strData = pType->m_bLate ? "data.value()" : "data";
        // only for getters want to test late variables have a value set
        if( pType->m_bLate )
        {
            function[ "lines" ].push_back( "VERIFY_RTE( data.has_value() );" );
        }
        if( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pType ) )
        {
            osFunctionBody << "return " << strData << ";";
        }
        else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
        {
            model::Object::Ptr pObject = pRef->m_object;
            VERIFY_RTE_MSG( pStage->isInterface( pObject ),
                            "Stage: " << pStage->m_strName
                                      << " missing interface for object: " << pObject->delimitTypeName( "." ) );
            model::Interface::Ptr pInterface = pStage->getInterface( pObject );
            osFunctionBody << "return toView( m_factory, " << strData << " );";
        }
        else if( model::OptType::Ptr pOptional = std::dynamic_pointer_cast< model::OptType >( pType ) )
        {
            model::Type::Ptr pUnderlyingType = pOptional->m_underlyingType;
            if( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
            {
                osFunctionBody << "return " << strData << ";";
            }
            else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
            {
                osFunctionBody << "return " << strData << ".has_value() ? toView( m_factory, " << strData
                               << ".value() ) : " << pOptional->getViewType( pStage->m_strName, false ) << "();";
            }
            else
            {
                THROW_RTE( "Unsupported type for getter" );
            }
        }
        else if( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
        {
            model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
            if( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
            {
                osFunctionBody << "return " << strData << ";";
            }
            else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
            {
                osFunctionBody << "return toView( m_factory, " << strData << " );";
            }
            else
            {
                THROW_RTE( "Unsupported type for getter" );
            }
        }
        else if( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
        {
            model::Type::Ptr pFrom = pMap->m_fromType;
            model::Type::Ptr pTo   = pMap->m_toType;
            if( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
            {
                if( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                {
                    osFunctionBody << "return " << strData << ";";
                }
                else if( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                {
                    osFunctionBody << "return toView( m_factory, " << strData << " );";
                }
                else
                {
                    THROW_RTE( "Unsupported type for map from type" );
                }
            }
            else if( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
            {
                if( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                {
                    osFunctionBody << "return toView( m_factory, " << strData << " );";
                }
                else if( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
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
    else if( std::dynamic_pointer_cast< model::FunctionSetter >( pFunction ) )
    {
        if( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pType ) )
        {
            osFunctionBody << "data = value;";
        }
        else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pType ) )
        {
            model::Object::Ptr pObject = pRef->m_object;
            osFunctionBody << "data = toData( m_factory, value );\n";
        }
        else if( model::OptType::Ptr pOptional = std::dynamic_pointer_cast< model::OptType >( pType ) )
        {
            model::Type::Ptr pUnderlyingType = pOptional->m_underlyingType;
            if( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
            {
                osFunctionBody << "data = value;";
            }
            else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
            {
                model::Object::Ptr pObject = pRef->m_object;
                osFunctionBody << "data = toData( m_factory, value );\n";
            }
            else
            {
                THROW_RTE( "Unsupported type for setter" );
            }
        }
        else if( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
        {
            model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
            if( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
            {
                osFunctionBody << "data = value;";
            }
            else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
            {
                model::Object::Ptr pObject = pRef->m_object;
                osFunctionBody << "data = toData( m_factory, value );\n";
            }
            else
            {
                THROW_RTE( "Unsupported type for setter" );
            }
        }
        else if( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
        {
            model::Type::Ptr pFrom = pMap->m_fromType;
            model::Type::Ptr pTo   = pMap->m_toType;
            if( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
            {
                if( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                {
                    osFunctionBody << "data = value;";
                }
                else if( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                {
                    model::Object::Ptr pObject = pToRef->m_object;
                    osFunctionBody << "data = toData( m_factory, value );\n";
                }
                else
                {
                    THROW_RTE( "Unsupported type for map from type" );
                }
            }
            else if( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
            {
                if( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                {
                    osFunctionBody << "data = toData( m_factory, value );";
                }
                else if( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
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
    else if( std::dynamic_pointer_cast< model::FunctionInserter >( pFunction ) )
    {
        const std::string strData = pType->m_bLate ? "data.value()" : "data";
        if( model::ArrayType::Ptr pArray = std::dynamic_pointer_cast< model::ArrayType >( pType ) )
        {
            if( pType->m_bLate )
                osFunctionBody << "if( !data.has_value() ) data = "
                               << pArray->getDatabaseType( model::Type::eNormal_NoLate ) << "()\n;";
            model::Type::Ptr pUnderlyingType = pArray->m_underlyingType;
            if( model::ValueType::Ptr pValue = std::dynamic_pointer_cast< model::ValueType >( pUnderlyingType ) )
            {
                osFunctionBody << strData << ".push_back( value );";
            }
            else if( model::RefType::Ptr pRef = std::dynamic_pointer_cast< model::RefType >( pUnderlyingType ) )
            {
                model::Object::Ptr pObject = pRef->m_object;
                osFunctionBody << strData << ".push_back( toData( m_factory, value ) );";
            }
            else
            {
                THROW_RTE( "Unsupported type for inserter" );
            }
        }
        else if( model::MapType::Ptr pMap = std::dynamic_pointer_cast< model::MapType >( pType ) )
        {
            if( pType->m_bLate )
                osFunctionBody << "if( !data.has_value() ) data = "
                               << pMap->getDatabaseType( model::Type::eNormal_NoLate ) << "()\n;";
            model::Type::Ptr pFrom = pMap->m_fromType;
            model::Type::Ptr pTo   = pMap->m_toType;
            if( model::ValueType::Ptr pFromValue = std::dynamic_pointer_cast< model::ValueType >( pFrom ) )
            {
                if( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                {
                    osFunctionBody << strData << ".insert( std::make_pair( key, value ) );";
                }
                else if( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                {
                    model::Object::Ptr pObject = pToRef->m_object;
                    osFunctionBody << strData << ".insert( std::make_pair( key, toData( m_factory, value ) ) );";
                }
                else
                {
                    THROW_RTE( "Unsupported type for map from type" );
                }
            }
            else if( model::RefType::Ptr pFromRef = std::dynamic_pointer_cast< model::RefType >( pFrom ) )
            {
                model::Object::Ptr pObject = pFromRef->m_object;
                if( model::ValueType::Ptr pToValue = std::dynamic_pointer_cast< model::ValueType >( pTo ) )
                {
                    osFunctionBody << strData << ".insert( std::make_pair( toData( m_factory, key ), value ) );";
                }
                else if( model::RefType::Ptr pToRef = std::dynamic_pointer_cast< model::RefType >( pTo ) )
                {
                    osFunctionBody << strData
                                   << ".insert( std::make_pair( toData( m_factory, key ), toData( m_factory, value ) ) "
                                      ");";
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

void writeSuperTypes( nlohmann::json& stage, model::Stage::Ptr pStage, std::vector< nlohmann::json >& functions )
{
    {
        std::set< model::Interface::Ptr, model::CountedObjectComparator< model::Interface::Ptr > > interfaces;
        for( model::SuperType::Ptr pSuperType : pStage->m_superTypes )
        {
            for( model::Interface::Ptr pInterface : pSuperType->m_interfaces )
            {
                VERIFY_RTE( interfaces.count( pInterface ) == 0 );

                interfaces.insert( pInterface );
                nlohmann::json cast = nlohmann::json::object(
                    { { "type", pInterface->delimitTypeName( pStage->m_strName, "::" ) },
                      { "fullname", pInterface->delimitTypeName( pStage->m_strName, "_" ) } } );
                stage[ "casts" ].push_back( cast );
            }
        }
    }

    for( model::SuperType::Ptr pSuperType : pStage->m_superTypes )
    {
        const std::string strSuperTypeName = pSuperType->getTypeName();

        nlohmann::json stype = nlohmann::json::object(
            { { "name", strSuperTypeName },
              { "interfaces", nlohmann::json::array() },
              // the super type variant needs to have ALL possible types even those not defined for stage
              // due to how the inheritance works and the data_pointer::to_upper function expects the variant
              // for inheritance to be the same type as the super type at the moment. //model::Stage::Ptr{}
              { "variant_type", pSuperType->m_base_object->inheritanceGroupVariant( pStage ) },
              { "functions", nlohmann::json::array() } } );

        {
            std::set< model::Object::Ptr, model::CountedObjectComparator< model::Object::Ptr > > objects;
            for( model::Object::WeakPtr pObject : *pSuperType->m_base_object->m_pInheritanceGroup )
            {
                objects.insert( pObject.lock() );
            }
            std::set< std::string > uniqueNames;
            for( model::Object::Ptr pObject : objects )
            {
                if( model::Interface::Ptr pInterface = pStage->isInterface( pObject ) )
                {
                    // model::ObjectPart::Ptr pPrimaryObjectPart = pObject->getPrimaryObjectPart( pStage );
                    for( auto pPart : pInterface->getPrimaryObjectParts() )
                    {
                        const std::string strName = pObject->delimitTypeName( "::" );
                        const bool        bUnique = uniqueNames.count( strName ) == 0;
                        uniqueNames.insert( strName );
                        nlohmann::json interface = nlohmann::json::object(
                            { { "name", strName },
                              { "unique", bUnique },
                              { "fullname", pObject->delimitTypeName( "_" ) },
                              { "part", pPart->getDataType( "::" ) },
                              { "casts", nlohmann::json::array() } } );

                        for( model::Object::Ptr pOtherObjects : objects )
                        {
                            bool               bCastOK = false;
                            model::Object::Ptr pIter   = pOtherObjects;
                            while( pIter )
                            {
                                if( pIter == pObject )
                                {
                                    bCastOK = true;
                                    break;
                                }
                                pIter = pIter->m_base;
                            }
                            if( bCastOK )
                            {
                                nlohmann::json cast = nlohmann::json::object(
                                    { { "type", pOtherObjects->getPrimaryObjectPart( pStage )->getDataType( ":"
                                                                                                            ":" ) } } );
                                interface[ "casts" ].push_back( cast );
                            }
                        }
                        stype[ "interfaces" ].push_back( interface );
                    }
                }
            }
        }

        // the functions on the super type are groups by their mangled function name
        // TODO: ignore return type and insead check returns types are the same
        for( model::SuperType::FunctionMultiMap::iterator iLower = pSuperType->m_functions.begin(),
                                                          iEnd   = pSuperType->m_functions.end();
             iLower != iEnd; )
        {
            // nlohmann::json function = writeFunction( pFunction );
            model::Function::Ptr pFunction = iLower->second;

            nlohmann::json function = nlohmann::json::object(
                { { "short_name", pFunction->getShortName() },
                  { "long_name", pFunction->getLongName() },
                  { "returntype", pFunction->getReturnType( pStage->m_strName ) },
                  { "variant_type", pSuperType->m_base_object->inheritanceGroupVariant( pStage ) },
                  { "propertytype", pFunction->m_property->m_type->getDatabaseType( model::Type::eNormal ) },
                  { "property", pFunction->m_property->m_strName },
                  { "params", nlohmann::json::array() },
                  { "is_void", ( pFunction->getReturnType( pStage->m_strName ) == "void" ) ? true : false },
                  { "body", writeFunctionBody( pStage, pFunction ) } } );

            for( const auto& param : pFunction->getParams( pStage->m_strName ) )
            {
                const nlohmann::json paramData
                    = nlohmann::json::object( { { "name", param.name }, { "type", param.type } } );
                function[ "params" ].push_back( paramData );
            }

            std::set< model::Object::Ptr, model::CountedObjectComparator< model::Object::Ptr > > remaining;

            using InterfaceFunctionMap = std::map< model::Object::Ptr, model::Function::Ptr,
                                                   model::CountedObjectComparator< model::Object::Ptr > >;
            InterfaceFunctionMap implemented;
            {
                for( model::Object::WeakPtr pObject : *pSuperType->m_base_object->m_pInheritanceGroup )
                {
                    remaining.insert( pObject.lock() );
                }
            }

            // functions in the group are where an inherited interface explicitly has the function
            // NOTE: this DOES NOT include how interfaces inherit others
            for( model::SuperType::FunctionMultiMap::iterator iUpper
                 = pSuperType->m_functions.upper_bound( iLower->first );
                 iLower != iUpper;
                 ++iLower )
            {
                model::Function::Ptr  pFunctionVariant = iLower->second;
                model::Interface::Ptr pInterface       = pFunctionVariant->m_interface.lock();
                model::Object::Ptr    pObject          = pInterface->m_object.lock();
                VERIFY_RTE( pInterface );
                remaining.erase( pObject );
                implemented.insert( std::make_pair( pObject, pFunctionVariant ) );

                // for( auto pPart : pInterface->getPrimaryObjectParts() )
                auto pPart = pInterface->getPrimaryObjectPart( pStage );
                {
                    nlohmann::json variant = nlohmann::json::object(
                        { { "matched", true },
                          { "primaryobjectpart", pPart->getDataType( "::" ) },
                          { "dataobjectpart",
                            pFunctionVariant->m_property->m_objectPart.lock()->getDataType( "::" ) } } );
                    insertFunctionVariantIfUnique( function, variant );
                }
            }
            // so from the remaining interfaces in the super type either the
            // interface inherits the function or it doesnt
            for( model::Object::Ptr pObject : remaining )
            {
                // model::Interface::Ptr pInterface = pStage->getInterface( pObject );
                //  see if the interface inherited from one that implemented the function
                model::Function::Ptr pFunctionVariant;

                if( pObject->m_base )
                {
                    model::Object::Ptr pBase = pObject->m_base;
                    while( pBase )
                    {
                        InterfaceFunctionMap::iterator iFind = implemented.find( pBase );
                        if( iFind != implemented.end() )
                        {
                            pFunctionVariant = iFind->second;
                            break;
                        }
                        pBase = pBase->m_base;
                    }
                }

                if( pFunctionVariant && pStage->isInterface( pObject ) )
                {
                    nlohmann::json variant = nlohmann::json::object(
                        { { "matched", true },
                          { "primaryobjectpart", pObject->getPrimaryObjectPart( pStage )->getDataType( "::" ) },
                          { "dataobjectpart",
                            pFunctionVariant->m_property->m_objectPart.lock()->getDataType( "::" ) } } );
                    insertFunctionVariantIfUnique( function, variant );
                }
                else
                {
                    nlohmann::json variant = nlohmann::json::object(
                        { { "matched", false },
                          { "primaryobjectpart", pObject->getPrimaryObjectPart( pStage )->getDataType( "::" ) },
                          { "dataobjectpart", "" } } );
                    insertFunctionVariantIfUnique( function, variant );
                }
            }

            stype[ "functions" ].push_back( function );

            // record the function
            functions.push_back( function );
        }

        stage[ "supertypes" ].push_back( stype );
    }
}

void writeViewData( const boost::filesystem::path& dataDir,
                    model::Schema::Ptr             pSchema,
                    std::vector< nlohmann::json >& functions )
{
    for( model::Stage::Ptr pStage : pSchema->m_stages )
    {
        nlohmann::json data;
        {
            std::ostringstream os;
            os << "DATABASE_VIEW_" << pStage->m_strName << "_GUARD";
            data[ "guard" ] = os.str();
        }

        nlohmann::json stage = nlohmann::json::object( { { "name", pStage->m_strName },
                                                         { "perobject", true },
                                                         { "datafiles", nlohmann::json::array() },
                                                         { "super_conversions", nlohmann::json::array() },
                                                         { "supertypes", nlohmann::json::array() },
                                                         { "interface_conversions", nlohmann::json::array() },
                                                         { "readwrite_files", nlohmann::json::array() },
                                                         { "many_accessors", nlohmann::json::array() },
                                                         { "one_accessors", nlohmann::json::array() },
                                                         { "one_opt_accessors", nlohmann::json::array() },
                                                         { "constructors", nlohmann::json::array() },
                                                         { "interfaces", nlohmann::json::array() },
                                                         { "casts", nlohmann::json::array() } } );

        writeConversions( stage, pSchema, pStage );

        for( model::Stage::Ptr pStageIter : pSchema->m_stages )
        {
            for( model::File::Ptr pFile : pStageIter->m_files )
            {
                stage[ "datafiles" ].push_back( pFile->m_strName );
            }
            if( pStageIter == pStage )
                break;
        }

        for( model::File::Ptr pFile : pStage->m_files )
        {
            nlohmann::json file = nlohmann::json::object( { { "name", pFile->m_strName } } );
            stage[ "readwrite_files" ].push_back( file );
        }

        for( model::Interface::Ptr pInterface : pStage->m_interfaceTopological )
        {
            stage[ "interfaces" ].push_back( writeInterface( pStage, pInterface ) );
        }

        writeAccessors( stage, pStage );
        writeConstructors( stage, pStage );
        writeSuperTypes( stage, pStage, functions );

        data[ "stage" ] = stage;

        std::ostringstream os;
        os << pStage->m_strName << ".json";
        writeJSON( dataDir / os.str(), data );
    }
}

void writeDataData( const boost::filesystem::path& dataDir,
                    model::Schema::Ptr             pSchema,
                    std::vector< nlohmann::json >& functions )
{
    nlohmann::json data( { { "files", nlohmann::json::array() },
                           { "conversions", nlohmann::json::array() },
                           { "base_conversions", nlohmann::json::array() },
                           { "up_casts", nlohmann::json::array() },
                           { "functions", nlohmann::json::array() } } );

    data[ "guard" ] = "DATABASE_DATA_GUARD_4_APRIL_2022";

    std::vector< model::File::Ptr > files;
    model::Stage::Ptr               pFinalStage;
    {
        for( model::Stage::Ptr pStage : pSchema->m_stages )
        {
            std::copy( pStage->m_files.begin(), pStage->m_files.end(), std::back_inserter( files ) );
            pFinalStage = pStage;
        }
    }
    for( model::File::Ptr pFile : files )
    {
        model::Stage::Ptr pStage = pFile->m_stage.lock();
        nlohmann::json    file   = nlohmann::json::object(
            { { "name", pFile->m_strName }, { "stage", pStage->m_strName }, { "parts", nlohmann::json::array() } } );

        for( model::ObjectPart::Ptr pPart : pFile->m_parts )
        {
            model::Object::Ptr pObject = pPart->m_object.lock();

            nlohmann::json part = nlohmann::json::object(
                { { "name", pObject->getDataTypeName() },
                  { "typeID", pPart->m_typeID },
                  { "has_base", false },
                  { "has_inheritance", false },
                  //{ "inheritance_variant", pObject->inheritanceGroupVariant( pFinalStage ) },
                  { "inheritance_variant", pObject->inheritanceGroupVariant( pFinalStage ) },
                  { "data_pointers", nlohmann::json::array() },
                  { "raw_pointers", nlohmann::json::array() },
                  { "properties", nlohmann::json::array() },
                  { "initialisations", nlohmann::json::array() } } );

            if( model::PrimaryObjectPart::Ptr pPrimaryObjectPart
                = std::dynamic_pointer_cast< model::PrimaryObjectPart >( pPart ) )
            {
                VERIFY_RTE( pObject->getPrimaryObjectPart( pStage ) == pPrimaryObjectPart );

                part[ "has_inheritance" ] = true;

                for( model::ObjectPart::Ptr pSecondaryPart : pObject->m_secondaryParts )
                {
                    if( model::InheritedObjectPart::Ptr pSecondaryObjectPart
                        = std::dynamic_pointer_cast< model::InheritedObjectPart >( pSecondaryPart ) )
                    {
                        nlohmann::json pointer
                            = nlohmann::json::object( { { "longname", pSecondaryObjectPart->getPointerName() },
                                                        { "typename", pSecondaryObjectPart->getDataType( "::" ) } } );
                        part[ "raw_pointers" ].push_back( pointer );
                    }
                    else if( model::AggregatedObjectPart::Ptr pSecondaryObjectPart
                             = std::dynamic_pointer_cast< model::AggregatedObjectPart >( pSecondaryPart ) )
                    {
                        nlohmann::json pointer
                            = nlohmann::json::object( { { "longname", pSecondaryObjectPart->getPointerName() },
                                                        { "typename", pSecondaryObjectPart->getDataType( "::" ) },
                                                        { "no_ctor_arg", true } } );
                        part[ "data_pointers" ].push_back( pointer );
                    }
                    else
                    {
                        THROW_RTE( "Unknown secondary object part type" );
                    }
                }

                if( pObject->m_base )
                {
                    model::PrimaryObjectPart::Ptr pBasePrimaryObjectPart
                        = pObject->m_base->getPrimaryObjectPart( pStage );
                    nlohmann::json pointer
                        = nlohmann::json::object( { { "longname", pBasePrimaryObjectPart->getPointerName() },
                                                    { "typename", pBasePrimaryObjectPart->getDataType( "::" ) },
                                                    { "no_ctor_arg", true } } );
                    part[ "data_pointers" ].push_back( pointer );
                    part[ "has_base" ]        = true;
                    part[ "base" ]            = pBasePrimaryObjectPart->getPointerName();
                    part[ "inheritance_ptr" ] = "m_inheritance";
                    {
                        std::ostringstream os;
                        os << "data::Ptr< data::" << pPrimaryObjectPart->getDataType( "::" ) << " >( "
                           << pBasePrimaryObjectPart->getPointerName() << ", this )";
                        part[ "inheritance_ptr_init" ] = os.str();
                    }
                }
            }
            else if( model::InheritedObjectPart::Ptr pSecondaryObjectPart
                     = std::dynamic_pointer_cast< model::InheritedObjectPart >( pPart ) )
            {
                model::PrimaryObjectPart::Ptr pBasePrimaryObjectPart = pObject->getPrimaryObjectPart( pStage );
                nlohmann::json                pointer
                    = nlohmann::json::object( { { "longname", pBasePrimaryObjectPart->getPointerName() },
                                                { "typename", pBasePrimaryObjectPart->getDataType( "::" ) },
                                                { "no_ctor_arg", false } } );
                part[ "data_pointers" ].push_back( pointer );
                part[ "has_base" ]        = true;
                part[ "base" ]            = pBasePrimaryObjectPart->getPointerName();
                part[ "inheritance_ptr" ] = pSecondaryObjectPart->getPointerName();

                {
                    std::ostringstream os;
                    os << "data::Ptr< data::" << pSecondaryObjectPart->getDataType( "::" ) << " >( "
                       << pBasePrimaryObjectPart->getPointerName() << ", this )";
                    part[ "inheritance_ptr_init" ] = os.str();
                }

                {
                    std::ostringstream osBase;
                    osBase << "Ptr< " << pBasePrimaryObjectPart->getDataType( "::" ) << " >";
                    nlohmann::json init = nlohmann::json::object(
                        { { "name", pBasePrimaryObjectPart->getPointerName() }, { "argtype", osBase.str() } } );
                    part[ "initialisations" ].push_back( init );
                }
            }
            else if( model::AggregatedObjectPart::Ptr pSecondaryObjectPart
                     = std::dynamic_pointer_cast< model::AggregatedObjectPart >( pPart ) )
            {
            }
            else
            {
                THROW_RTE( "Unknown object part type" );
            }

            for( model::Property::Ptr pProperty : pPart->m_properties )
            {
                model::Type::Ptr pType = pProperty->m_type;

                bool bIsPointer = false;
                if( std::dynamic_pointer_cast< model::RefType >( pType ) )
                    bIsPointer = true;

                nlohmann::json property
                    = nlohmann::json::object( { { "name", pProperty->m_strName },
                                                { "type", pType->getDatabaseType( model::Type::eNormal ) },
                                                { "argtype", pType->getDatabaseType( model::Type::eAsArgument ) },
                                                { "is_pointer", bIsPointer },
                                                { "has_validation", false } } );

                if( pType->m_bLate )
                {
                    std::ostringstream os;
                    os << "VERIFY_RTE_MSG( " << pProperty->m_strName << ".has_value(), \""
                       << pObject->getPrimaryObjectPart( pStage )->getDataType( "::" ) << "." << pProperty->m_strName
                       << " has NOT been set\" );";
                    property[ "validation" ]     = os.str();
                    property[ "has_validation" ] = true;
                }
                else
                {
                    //{ "type", pType->getDatabaseType( model::Type::eNormal ) },
                    nlohmann::json init = nlohmann::json::object(
                        { { "name", pProperty->m_strName },
                          { "argtype", pType->getDatabaseType( model::Type::eAsArgument ) } } );
                    part[ "initialisations" ].push_back( init );
                }

                part[ "properties" ].push_back( property );
            }

            file[ "parts" ].push_back( part );
        }

        data[ "files" ].push_back( file );
    }

    // conversions
    {
        for( model::Schema::ConversionMap::const_iterator i    = pSchema->m_conversions.begin(),
                                                          iEnd = pSchema->m_conversions.end();
             i != iEnd;
             ++i )
        {
            const model::Schema::ObjectPartPair&   parts    = i->first;
            const model::Schema::ObjectPartVector& sequence = i->second;

            std::string strOwningFile;
            {
                model::Schema::ObjectPartVector allParts = sequence;
                allParts.push_back( parts.first );

                std::optional< model::File::Ptr > pLatestFileOpt;
                int                               index = -1;
                for( model::ObjectPart::Ptr pPart : allParts )
                {
                    model::File::Ptr pFile = pPart->m_file.lock();
                    auto             iFind = std::find( files.begin(), files.end(), pFile );
                    const int        iDist = std::distance( files.begin(), iFind );
                    if( iDist > index )
                    {
                        index          = iDist;
                        pLatestFileOpt = pFile;
                    }
                }
                VERIFY_RTE( pLatestFileOpt.has_value() );
                strOwningFile = pLatestFileOpt.value()->m_strName;
            }

            nlohmann::json conversion = nlohmann::json::object( { { "from", parts.first->getDataType( "::" ) },
                                                                  { "to", parts.second->getDataType( "::" ) },
                                                                  { "pointers", nlohmann::json::array() },
                                                                  { "file", strOwningFile } } );

            for( model::ObjectPart::Ptr pPart : sequence )
            {
                conversion[ "pointers" ].push_back( pPart->getPointerName() );
            }

            data[ "conversions" ].push_back( conversion );
        }
    }

    // base_conversions
    {
        for( model::Schema::ConversionMap::const_iterator i    = pSchema->m_base_conversions.begin(),
                                                          iEnd = pSchema->m_base_conversions.end();
             i != iEnd;
             ++i )
        {
            const model::Schema::ObjectPartPair&   parts    = i->first;
            const model::Schema::ObjectPartVector& sequence = i->second;

            std::string strOwningFile;
            {
                model::Schema::ObjectPartVector allParts = sequence;
                allParts.push_back( parts.first );

                std::optional< model::File::Ptr > pLatestFileOpt;
                int                               index = -1;
                for( model::ObjectPart::Ptr pPart : allParts )
                {
                    model::File::Ptr pFile = pPart->m_file.lock();
                    auto             iFind = std::find( files.begin(), files.end(), pFile );
                    const int        iDist = std::distance( files.begin(), iFind );
                    if( iDist > index )
                    {
                        index          = iDist;
                        pLatestFileOpt = pFile;
                    }
                }
                VERIFY_RTE( pLatestFileOpt.has_value() );
                strOwningFile = pLatestFileOpt.value()->m_strName;
            }

            nlohmann::json conversion = nlohmann::json::object( { { "from", parts.first->getDataType( "::" ) },
                                                                  { "to", parts.second->getDataType( "::" ) },
                                                                  { "pointers", nlohmann::json::array() },
                                                                  { "file", strOwningFile } } );

            for( model::ObjectPart::Ptr pPart : sequence )
            {
                conversion[ "pointers" ].push_back( pPart->getPointerName() );
            }

            data[ "base_conversions" ].push_back( conversion );
        }
    }

    // functions
    {
        std::sort( functions.begin(), functions.end(),
                   []( const nlohmann::json& left, const nlohmann::json& right )
                   {
                       return ( left[ "long_name" ] != right[ "long_name" ] )
                                  ? ( left[ "long_name" ] < right[ "long_name" ] )
                              : ( left[ "variant_type" ] != right[ "variant_type" ] )
                                  ? ( left[ "variant_type" ] < right[ "variant_type" ] )
                                  : false;
                   } );

        std::vector< nlohmann::json > mergedFunctions;
        for( const auto& function : functions )
        {
            if( mergedFunctions.empty() )
            {
                mergedFunctions.push_back( function );
            }
            else
            {
                if( mergedFunctions.back()[ "long_name" ] == function[ "long_name" ]
                    && mergedFunctions.back()[ "variant_type" ] == function[ "variant_type" ] )
                {
                    auto& existingFunction = mergedFunctions.back();
                    for( const auto& variant : function[ "variants" ] )
                    {
                        insertFunctionVariantIfUnique( existingFunction, variant );
                    }
                }
                else
                {
                    mergedFunctions.push_back( function );
                }
            }
        }

        functions.erase( std::unique( functions.begin(), functions.end(),
                                      []( const nlohmann::json& left, const nlohmann::json& right ) {
                                          return left[ "long_name" ] == right[ "long_name" ]
                                                 && left[ "variant_type" ] == right[ "variant_type" ];
                                      } ),
                         functions.end() );

        for( nlohmann::json& function : mergedFunctions )
        {
            data[ "functions" ].push_back( function );
        }
    }

    writeJSON( dataDir / "data.json", data );
}

} // namespace

void toJSON( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
{
    writeStageData( dataDir, pSchema );
    std::vector< nlohmann::json > functions;
    writeViewData( dataDir, pSchema, functions );
    writeDataData( dataDir, pSchema, functions );
}

} // namespace jsonconv
} // namespace db
