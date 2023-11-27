
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

#include "jit/symbol_utils.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include "nlohmann/json.hpp"

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

void generateTypes( inja::Environment& injaEnv, const Model& model, const boost::filesystem::path& outputFile )
{
    try
    {
        std::string strOutput;
        {
            inja::Template tmp = injaEnv.parse_template( "/types.hxx.jinja" );

            nlohmann::json data( { { "dataTypes", nlohmann::json::array() } } );

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
                data[ "dataTypes" ].push_back( addType( nativeType, "native" ) );
            }

            for( const auto& inlineType : model.inlineTypes )
            {
                data[ "dataTypes" ].push_back( addType( inlineType, "inline" ) );
            }

            for( const auto& externType : model.externTypes )
            {
                data[ "dataTypes" ].push_back( addType( externType, "extern" ) );
            }

            std::ostringstream osOutput;
            injaEnv.render_to( osOutput, tmp, data );

            strOutput = osOutput.str();
        }
        boost::filesystem::updateFileIfChanged( outputFile, strOutput );
    }
    catch( std::exception& ex )
    {
        THROW_RTE( "Error generating template: types.hxx.jinja to output: " << outputFile.string()
                                                                            << " Error: " << ex.what() );
    }
}

void generateFunctions( inja::Environment& injaEnv, const Model& model, const boost::filesystem::path& outputFile )
{
    try
    {
        std::string strOutput;
        {
            inja::Template tmp = injaEnv.parse_template( "/functions.hxx.jinja" );

            nlohmann::json data( { { "inlines", nlohmann::json::array() }, //
                                   { "externs", nlohmann::json::array() } } );

            auto addFunction = [ & ]( const Function& function ) -> nlohmann::json
            {
                nlohmann::json functionData( {

                    { "element_name", function.name },
                    { "function_name", function.name },
                    { "return_variable_type", getVariableType( function.returnType ) },
                    { "return_data_type", strEnumMangle( getDataType( function.returnType ).name ) },
                    { "parameters", nlohmann::json::array() }

                } );

                for( const auto& param : function.arguments )
                {
                    nlohmann::json paramData( { { "variable_type", getVariableType( param.type ) },
                                                { "data_type", strEnumMangle( getDataType( param.type ).name ) } } );
                    functionData[ "parameters" ].push_back( paramData );
                }

                return functionData;
            };

            for( const auto function : model.inlineFunctions )
            {
                data[ "inlines" ].push_back( addFunction( function ) );
            }

            for( const auto function : model.externFunctions )
            {
                data[ "externs" ].push_back( addFunction( function ) );
            }

            std::ostringstream osOutput;
            injaEnv.render_to( osOutput, tmp, data );

            strOutput = osOutput.str();
        }
        boost::filesystem::updateFileIfChanged( outputFile, strOutput );
    }
    catch( std::exception& ex )
    {
        THROW_RTE( "Error generating template: types.hxx.jinja to output: " << outputFile.string()
                                                                            << " Error: " << ex.what() );
    }
}

} // namespace il_gen