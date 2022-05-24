
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
    std::ifstream file( filePath.native(), std::ios_base::in );
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
        if ( !boost::filesystem::is_directory( filePath ) )
        {
            if ( boost::filesystem::extension( *iter ) == ".json" )
            {
                // view.hxx
                try
                {
                    std::string strOutput;
                    {
                        std::ostringstream osOutput;
                        inja::Environment  injaEnv( env.injaDir.native(), env.apiDir.native() );
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
                        env.apiDir.native() / boost::filesystem::path( osTargetFile.str() ), strOutput );
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
                        inja::Environment  injaEnv( env.injaDir.native(), env.srcDir.native() );
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
                        env.srcDir.native() / boost::filesystem::path( osTargetFile.str() ), strOutput );
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
}
} // namespace gen
} // namespace protocol
