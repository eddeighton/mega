
#include "generator.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <fstream>
#include <ios>
#include <iostream>

namespace db
{
    namespace gen
    {
        namespace
        {
            nlohmann::json loadJson( const boost::filesystem::path& filePath )
            {
                VERIFY_RTE_MSG( boost::filesystem::exists( filePath ),
                                "Could not locate file: " << filePath.string() );
                std::ifstream file( filePath.native(), std::ios_base::in );
                VERIFY_RTE_MSG( !file.fail(), "Failed to open json file: " << filePath.string() );
                return nlohmann::json::parse(
                    std::istreambuf_iterator< char >( file ), std::istreambuf_iterator< char >() );
            }
        } // namespace

        void generate( const Environment& env )
        {
            for ( boost::filesystem::directory_iterator iter( env.dataDir );
                  iter != boost::filesystem::directory_iterator();
                  ++iter )
            {
                const boost::filesystem::path& filePath = *iter;
                if ( !boost::filesystem::is_directory( filePath ) )
                {
                    if ( boost::filesystem::extension( *iter ) == ".json"
                         && filePath.filename() != "data.json" 
                         && filePath.filename() != "stages.json")
                    {
                        // view.hxx
                        try
                        {
                            inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
                            injaEnv.set_trim_blocks( true );
                            const boost::filesystem::path jsonFile
                                = env.dataDir / filePath.filename();
                            const auto         data = loadJson( jsonFile );
                            std::ostringstream osTargetFile;
                            osTargetFile
                                << "/" << filePath.filename().replace_extension( ".hxx" ).string();
                            injaEnv.write( "/view.hxx.jinja", data, osTargetFile.str() );
                        }
                        catch ( std::exception& ex )
                        {
                            THROW_RTE( "Error processing template: "
                                       << "view.hxx.jinja"
                                       << " with data: " << filePath.string()
                                       << " Error: " << ex.what() );
                        }

                        // view.cxx
                        try
                        {
                            inja::Environment injaEnv( env.injaDir.native(), env.srcDir.native() );
                            injaEnv.set_trim_blocks( true );
                            const boost::filesystem::path jsonFile
                                = env.dataDir / filePath.filename();
                            const auto         data = loadJson( jsonFile );
                            std::ostringstream osTargetFile;
                            osTargetFile
                                << "/" << filePath.filename().replace_extension( ".cxx" ).string();
                            injaEnv.write( "/view.cxx.jinja", data, osTargetFile.str() );
                        }
                        catch ( std::exception& ex )
                        {
                            THROW_RTE( "Error processing template: "
                                       << "view.cxx.jinja"
                                       << " with data: " << filePath.string()
                                       << " Error: " << ex.what() );
                        }
                    }
                }
            }

            // data.hxx
            {
                const boost::filesystem::path jsonFile = env.dataDir / "data.json";
                try
                {
                    inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
                    injaEnv.set_trim_blocks( true );
                    const auto data = loadJson( jsonFile );
                    injaEnv.write( "/data.hxx.jinja", data, "/data.hxx" );
                }
                catch ( std::exception& ex )
                {
                    THROW_RTE( "Error processing template: "
                               << "data.hxx.jinja"
                               << " with data: " << jsonFile.string() << " Error: " << ex.what() );
                }
            }

            // data.cxx
            {
                const boost::filesystem::path jsonFile = env.dataDir / "data.json";
                try
                {
                    inja::Environment injaEnv( env.injaDir.native(), env.srcDir.native() );
                    injaEnv.set_trim_blocks( true );
                    const auto data = loadJson( jsonFile );
                    injaEnv.write( "/data.cxx.jinja", data, "/data.cxx" );
                }
                catch ( std::exception& ex )
                {
                    THROW_RTE( "Error processing template: "
                               << "data.cxx.jinja"
                               << " with data: " << jsonFile.string() << " Error: " << ex.what() );
                }
            }

            // file_types_global.hxx
            {
                const boost::filesystem::path jsonFile = env.dataDir / "data.json";
                try
                {
                    inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
                    injaEnv.set_trim_blocks( true );
                    const auto data = loadJson( jsonFile );
                    injaEnv.write( "/file_types_global.hxx.jinja", data, "/file_types_global.hxx" );
                }
                catch ( std::exception& ex )
                {
                    THROW_RTE( "Error processing template: "
                               << "file_types_global.hxx.jinja"
                               << " with data: " << jsonFile.string() << " Error: " << ex.what() );
                }
            }

            // file_types_object.hxx
            {
                const boost::filesystem::path jsonFile = env.dataDir / "data.json";
                try
                {
                    inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
                    injaEnv.set_trim_blocks( true );
                    const auto data = loadJson( jsonFile );
                    injaEnv.write( "/file_types_object.hxx.jinja", data, "/file_types_object.hxx" );
                }
                catch ( std::exception& ex )
                {
                    THROW_RTE( "Error processing template: "
                               << "file_types_object.hxx.jinja"
                               << " with data: " << jsonFile.string() << " Error: " << ex.what() );
                }
            }

            // stage_types.hxx
            {
                const boost::filesystem::path jsonFile = env.dataDir / "stages.json";
                try
                {
                    inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
                    injaEnv.set_trim_blocks( true );
                    const auto data = loadJson( jsonFile );
                    injaEnv.write( "/stage_types.hxx.jinja", data, "/stage_types.hxx" );
                }
                catch ( std::exception& ex )
                {
                    THROW_RTE( "Error processing template: "
                               << "stage_types.hxx.jinja"
                               << " with data: " << jsonFile.string() << " Error: " << ex.what() );
                }
            }
        }
    } // namespace gen
} // namespace db
