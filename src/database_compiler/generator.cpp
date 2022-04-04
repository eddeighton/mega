
#include "generator.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <fstream>
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

    void generate_view( const Environment& env )
    {
        inja::Environment injaEnv( env.injaDir.native(), env.apiDir.native() );

        const boost::filesystem::path jsonFile = env.dataDir / "view.json";
        const boost::filesystem::path viewHeaderFile = env.apiDir / "view.hxx";

        std::cout << "Writing view header file to: " << viewHeaderFile.string() << std::endl;
        injaEnv.write( "/view.jinja", loadJson( jsonFile ), "/view.hxx" );
    }
} // namespace gen
} // namespace db
