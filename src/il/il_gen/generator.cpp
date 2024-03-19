
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

#include "generator.hpp"

#include "runtime/mangle.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <boost/algorithm/string.hpp>

#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>

namespace il_gen
{
namespace
{
std::string strEnumMangle( const std::string& strType )
{
    std::ostringstream osEnum;
    // generate enumerate
    osEnum << "e_";
    std::string strMangle = mega::runtime::megaMangle( strType );
    boost::replace_all( strMangle, "mega00", "" );
    osEnum << strMangle;
    return osEnum.str();
}
} // namespace

nlohmann::json generateJSON( const Model& model, const std::vector< boost::filesystem::path >& includes )
{
    nlohmann::json jsonModel( { { "includes", nlohmann::json::array() },
                                { "dataTypes", nlohmann::json::array() },
                                { "inlines", nlohmann::json::array() }, //
                                { "externs", nlohmann::json::array() },
                                { "materialisers", nlohmann::json::array() } } );

    for( const auto& p : includes )
    {
        jsonModel[ "includes" ].push_back( p.string() );
    }

    auto addVariable = [ & ]( const Variable& variable ) -> nlohmann::json
    {
        nlohmann::json varData( { { "type_var", getVariableType( variable.type ) },
                                  { "type_cpp", getCPPType( variable.type ) },
                                  { "type_erased", getCPPErasedType( variable.type ) },
                                  { "type_data", strEnumMangle( getDataType( variable.type ).name ) },
                                  { "type_cpp", strEnumMangle( getDataType( variable.type ).name ) },
                                  { "name", variable.name } } );
        return varData;
    };

    auto addFunction = [ & ]( const Function& function ) -> nlohmann::json
    {
        nlohmann::json functionData( {

            { "name", function.name },
            { "return_type_var", getVariableType( function.returnType ) },
            { "return_type_cpp", getCPPType( function.returnType ) },
            { "return_type_erased", getCPPErasedType( function.returnType ) },
            { "return_type_data", strEnumMangle( getDataType( function.returnType ).name ) },
            { "parameters", nlohmann::json::array() },
            { "symbol_prefix", "" },
            { "symbol_postfix", "" }

        } );

        if( !function.mangle.empty() )
        {
            auto iFind = std::search(
                function.mangle.begin(), function.mangle.end(), function.name.begin(), function.name.end() );
            VERIFY_RTE_MSG( iFind != function.mangle.end(),
                            "Failed to locate function name: " << function.name << " in mangle: " << function.mangle );
            const auto iNameStart = std::distance( function.mangle.begin(), iFind );

            // remove additional bytes for existing name length digits
            std::ostringstream os;
            os << std::dec << function.name.size();
            const auto iExistingNameLen = os.str().size();
            VERIFY_RTE( iNameStart > static_cast< int >( iExistingNameLen ) );

            functionData[ "symbol_prefix" ]  = function.mangle.substr( 0, iNameStart - iExistingNameLen );
            functionData[ "symbol_postfix" ] = function.mangle.substr( iNameStart + function.name.size() );
        }

        for( const auto& var : function.arguments )
        {
            functionData[ "parameters" ].push_back( addVariable( var ) );
        }

        return functionData;
    };

    for( const auto& materialiser : model.materialisers )
    {
        nlohmann::json materialiserData( {

            { "name", materialiser.name },
            { "functions", nlohmann::json::array() },
            { "parameters", nlohmann::json::array() }

        } );
        for( const auto& var : materialiser.arguments )
        {
            materialiserData[ "parameters" ].push_back( addVariable( var ) );
        }

        for( const auto& function : materialiser.functions )
        {
            materialiserData[ "functions" ].push_back( addFunction( function ) );
        }
        jsonModel[ "materialisers" ].push_back( materialiserData );
    }

    for( const auto& function : model.inlineFunctions )
    {
        jsonModel[ "inlines" ].push_back( addFunction( function ) );
    }

    for( const auto& function : model.externFunctions )
    {
        jsonModel[ "externs" ].push_back( addFunction( function ) );
    }

    auto addType = [ & ]( const Type& type, const std::string& strComment ) -> nlohmann::json
    {
        std::string strTypeName;
        {
            std::ostringstream osTypeName;
            osTypeName << type;
            strTypeName = osTypeName.str();
        }

        nlohmann::json dataType( {

            { "enum", strEnumMangle( strTypeName ) },
            { "name", strTypeName },  //
            { "comment", strComment } //

        } );
        return dataType;
    };

    for( const auto& nativeType : model.nativeTypes )
    {
        jsonModel[ "dataTypes" ].push_back( addType( nativeType, "native" ) );
    }

    for( const auto& inlineType : model.inlineTypes )
    {
        jsonModel[ "dataTypes" ].push_back( addType( inlineType, "inline" ) );
    }

    for( const auto& externType : model.externTypes )
    {
        jsonModel[ "dataTypes" ].push_back( addType( externType, "extern" ) );
    }

    return jsonModel;
}

void generate( inja::Environment& injaEnv, const nlohmann::json& model, const std::string& strTemplate,
               const boost::filesystem::path& outputFile )
{
    try
    {
        inja::Template     tmp = injaEnv.parse_template( strTemplate );
        std::ostringstream osOutput;
        injaEnv.render_to( osOutput, tmp, model );
        boost::filesystem::updateFileIfChanged( outputFile, osOutput.str() );
    }
    catch( std::exception& ex )
    {
        THROW_RTE( "Error generating template: " << strTemplate << " to output: " << outputFile.string()
                                                 << " Error: " << ex.what() );
    }
}

} // namespace il_gen