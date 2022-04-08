
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
        /*
        //view.hxx
        {
            inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
            injaEnv.set_trim_blocks(true);
            const boost::filesystem::path jsonFile = env.dataDir / "view.json";
            const auto data = loadJson( jsonFile );
            injaEnv.write( "/view.hxx.jinja", data, "/view.hxx" );
        }

        //view.cxx
        {
            inja::Environment injaEnv( env.injaDir.native(), env.srcDir.native() );
            injaEnv.set_trim_blocks(true);
            const boost::filesystem::path jsonFile = env.dataDir / "view.json";
            const auto data = loadJson( jsonFile );
            injaEnv.write( "/view.cxx.jinja", data, "/view.cxx" );
        }

        //data.hxx
        {
            inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
            injaEnv.set_trim_blocks(true);
            const boost::filesystem::path jsonFile = env.dataDir / "data.json";
            const auto data = loadJson( jsonFile );
            injaEnv.write( "/data.hxx.jinja", data, "/data.hxx" );
        }

        //data.cxx
        {
            inja::Environment injaEnv( env.injaDir.native(), env.srcDir.native() );
            injaEnv.set_trim_blocks(true);
            const boost::filesystem::path jsonFile = env.dataDir / "data.json";
            const auto data = loadJson( jsonFile );
            injaEnv.write( "/data.cxx.jinja", data, "/data.cxx" );
        }

        //file_types_global.hxx
        {
            inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
            injaEnv.set_trim_blocks(true);
            const boost::filesystem::path jsonFile = env.dataDir / "data.json";
            const auto data = loadJson( jsonFile );
            injaEnv.write( "/file_types_global.hxx.jinja", data, "/file_types_global.hxx" );
        }
        
        //file_types_object.hxx
        {
            inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
            injaEnv.set_trim_blocks(true);
            const boost::filesystem::path jsonFile = env.dataDir / "data.json";
            const auto data = loadJson( jsonFile );
            injaEnv.write( "/file_types_object.hxx.jinja", data, "/file_types_object.hxx" );
        }

        //stage_types.hxx
        {
            inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
            injaEnv.set_trim_blocks(true);
            const boost::filesystem::path jsonFile = env.dataDir / "view.json";
            const auto data = loadJson( jsonFile );
            injaEnv.write( "/stage_types.hxx.jinja", data, "/stage_types.hxx" );
        }*/
    }
} // namespace gen
} // namespace db
