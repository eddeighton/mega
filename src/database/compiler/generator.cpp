
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
                VERIFY_RTE_MSG( boost::filesystem::exists( filePath ), "Could not locate file: " << filePath.string() );
                std::ifstream file( filePath.native(), std::ios_base::in );
                VERIFY_RTE_MSG( !file.fail(), "Failed to open json file: " << filePath.string() );
                return nlohmann::json::parse( std::istreambuf_iterator< char >( file ), std::istreambuf_iterator< char >() );
            }
        } // namespace

        void generate( const Environment& env )
        {
            for ( boost::filesystem::directory_iterator iter( env.dataDir ); iter != boost::filesystem::directory_iterator(); ++iter )
            {
                const boost::filesystem::path& filePath = *iter;
                if ( !boost::filesystem::is_directory( filePath ) )
                {
                    if ( boost::filesystem::extension( *iter ) == ".json" && filePath.filename() != "data.json"
                         && filePath.filename() != "stages.json" )
                    {
                        // view.hxx
                        try
                        {
                            inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
                            injaEnv.set_trim_blocks( true );
                            const boost::filesystem::path jsonFile = env.dataDir / filePath.filename();
                            const auto                    data = loadJson( jsonFile );
                            std::ostringstream            osTargetFile;
                            osTargetFile << "/" << filePath.filename().replace_extension( ".hxx" ).string();
                            injaEnv.write( "/view.hxx.jinja", data, osTargetFile.str() );
                        }
                        catch ( std::exception& ex )
                        {
                            THROW_RTE( "Error processing template: "
                                       << "view.hxx.jinja"
                                       << " with data: " << filePath.string() << " Error: " << ex.what() );
                        }

                        // view.cxx
                        try
                        {
                            inja::Environment injaEnv( env.injaDir.native(), env.srcDir.native() );
                            injaEnv.set_trim_blocks( true );
                            const boost::filesystem::path jsonFile = env.dataDir / filePath.filename();
                            const auto                    data = loadJson( jsonFile );
                            std::ostringstream            osTargetFile;
                            osTargetFile << "/" << filePath.filename().replace_extension( ".cxx" ).string();
                            injaEnv.write( "/view.cxx.jinja", data, osTargetFile.str() );
                        }
                        catch ( std::exception& ex )
                        {
                            THROW_RTE( "Error processing template: "
                                       << "view.cxx.jinja"
                                       << " with data: " << filePath.string() << " Error: " << ex.what() );
                        }
                    }
                }
            }

            // clang-format off
            std::vector< std::pair< std::string, std::string > > filenames = 
            {
                { "data"        , "data.json" },
                { "environment" , "stages.json" },    
                { "manifest"    , "stages.json" },    
                { "file_info"   , "stages.json" }   
            };
            // clang-format on

            for ( const std::pair< std::string, std::string >& names : filenames )
            {
                const boost::filesystem::path jsonFile = env.dataDir / names.second;
                {
                    try
                    {
                        inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );
                        injaEnv.set_trim_blocks( true );
                        const auto data = loadJson( jsonFile );
                        injaEnv.write( "/" + names.first + ".hxx.jinja", data, "/" + names.first + ".hxx" );
                    }
                    catch ( std::exception& ex )
                    {
                        THROW_RTE( "Error processing template: " << names.first << ".hxx.jinja"
                                                                 << " with data: " << jsonFile.string() << " Error: " << ex.what() );
                    }
                }
                {
                    try
                    {
                        inja::Environment injaEnv( env.injaDir.native(), env.srcDir.native() );
                        injaEnv.set_trim_blocks( true );
                        const auto data = loadJson( jsonFile );
                        injaEnv.write( "/" + names.first + ".cxx.jinja", data, "/" + names.first + ".cxx" );
                    }
                    catch ( std::exception& ex )
                    {
                        THROW_RTE( "Error processing template: " << names.first << ".cxx.jinja"
                                                                 << " with data: " << jsonFile.string() << " Error: " << ex.what() );
                    }
                }
            }
        }
    } // namespace gen
} // namespace db
