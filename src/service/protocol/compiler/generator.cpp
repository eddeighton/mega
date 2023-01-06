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

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include "utilities/clang_format.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>

namespace protocol
{
namespace gen
{
namespace
{
nlohmann::json loadJson( const boost::filesystem::path& filePath )
{
    VERIFY_RTE_MSG( boost::filesystem::exists( filePath ), "Could not locate file: " << filePath.string() );
    std::ifstream file( filePath.string(), std::ios_base::in );
    VERIFY_RTE_MSG( !file.fail(), "Failed to open json file: " << filePath.string() );
    return nlohmann::json::parse( std::istreambuf_iterator< char >( file ), std::istreambuf_iterator< char >() );
}
} // namespace

void generate( const Environment& env )
{
    for ( boost::filesystem::directory_iterator iter( env.dataDir ); iter != boost::filesystem::directory_iterator();
          ++iter )
    {
        const boost::filesystem::path& filePath = *iter;
        if ( !boost::filesystem::is_directory( filePath ) && filePath.filename() != "messages.json" )
        {
            if ( filePath.extension() == ".json" )
            {
                // view.hxx
                try
                {
                    std::string strOutput;
                    {
                        std::ostringstream osOutput;
                        inja::Environment  injaEnv( env.injaDir.string(), env.apiDir.string() );
                        injaEnv.set_trim_blocks( true );
                        const boost::filesystem::path jsonFile = env.dataDir / filePath.filename();
                        const auto                    data     = loadJson( jsonFile );
                        inja::Template                tmp      = injaEnv.parse_template( "/protocol.hxx.jinja" );
                        injaEnv.render_to( osOutput, tmp, data );
                        strOutput = osOutput.str();

                        mega::utilities::clang_format( strOutput, std::optional< boost::filesystem::path >() );
                    }
                    std::ostringstream osTargetFile;
                    osTargetFile << "/" << filePath.filename().replace_extension( ".hxx" ).string();

                    boost::filesystem::updateFileIfChanged(
                        env.apiDir.string() / boost::filesystem::path( osTargetFile.str() ), strOutput );
                }
                catch ( std::exception& ex )
                {
                    THROW_RTE( "Error processing template: "
                               << "protocol.hxx.jinja"
                               << " with data: " << filePath.string() << " Error: " << ex.what() );
                }

                // view.cxx
                try
                {
                    std::string strOutput;
                    {
                        std::ostringstream osOutput;
                        inja::Environment  injaEnv( env.injaDir.string(), env.srcDir.string() );
                        injaEnv.set_trim_blocks( true );
                        const boost::filesystem::path jsonFile = env.dataDir / filePath.filename();
                        const auto                    data     = loadJson( jsonFile );
                        inja::Template                tmp      = injaEnv.parse_template( "/protocol.cxx.jinja" );
                        injaEnv.render_to( osOutput, tmp, data );
                        strOutput = osOutput.str();

                        mega::utilities::clang_format( strOutput, std::optional< boost::filesystem::path >() );
                    }
                    std::ostringstream osTargetFile;
                    osTargetFile << "/" << filePath.filename().replace_extension( ".cxx" ).string();
                    boost::filesystem::updateFileIfChanged(
                        env.srcDir.string() / boost::filesystem::path( osTargetFile.str() ), strOutput );
                }
                catch ( std::exception& ex )
                {
                    THROW_RTE( "Error processing template: "
                               << "protocol.cxx.jinja"
                               << " with data: " << filePath.string() << " Error: " << ex.what() );
                }
            }
        }
    }

    // clang-format off
    std::vector< std::pair< std::string, std::string > > filenames = 
    {
        { "messages", "messages.json" }   
    };
    // clang-format on

    for ( const std::pair< std::string, std::string >& names : filenames )
    {
        const boost::filesystem::path jsonFile = env.dataDir / names.second;
        {
            try
            {
                std::string strOutput;
                {
                    std::ostringstream osOutput;
                    inja::Environment  injaEnv( env.injaDir.string(), env.apiDir.string() );
                    injaEnv.set_trim_blocks( true );
                    const auto data = loadJson( jsonFile );

                    inja::Template headerTemplate = injaEnv.parse_template( "/" + names.first + ".hxx.jinja" );
                    injaEnv.render_to( osOutput, headerTemplate, data );
                    strOutput = osOutput.str();

                    mega::utilities::clang_format( strOutput, std::optional< boost::filesystem::path >() );
                }

                std::ostringstream osTargetFile;
                osTargetFile << "/" + names.first + ".hxx";
                boost::filesystem::updateFileIfChanged(
                    env.apiDir.string() / boost::filesystem::path( osTargetFile.str() ), strOutput );
            }
            catch ( std::exception& ex )
            {
                THROW_RTE( "Error processing template: " << names.first << ".hxx.jinja"
                                                         << " with data: " << jsonFile.string()
                                                         << " Error: " << ex.what() );
            }
        }
        {
            try
            {
                std::string strOutput;
                {
                    std::ostringstream osOutput;
                    inja::Environment  injaEnv( env.injaDir.string(), env.srcDir.string() );
                    injaEnv.set_trim_blocks( true );
                    const auto data = loadJson( jsonFile );

                    inja::Template sourceTemplate = injaEnv.parse_template( "/" + names.first + ".cxx.jinja" );
                    injaEnv.render_to( osOutput, sourceTemplate, data );
                    strOutput = osOutput.str();

                    mega::utilities::clang_format( strOutput, std::optional< boost::filesystem::path >() );
                }

                std::ostringstream osTargetFile;
                osTargetFile << "/" + names.first + ".cxx";
                boost::filesystem::updateFileIfChanged(
                    env.srcDir.string() / boost::filesystem::path( osTargetFile.str() ), strOutput );
            }
            catch ( std::exception& ex )
            {
                THROW_RTE( "Error processing template: " << names.first << ".cxx.jinja"
                                                         << " with data: " << jsonFile.string()
                                                         << " Error: " << ex.what() );
            }
        }
    }
}
} // namespace gen
} // namespace protocol
