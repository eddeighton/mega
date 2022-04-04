#include "json_converter.hpp"

#include "nlohmann/json.hpp"

#include <fstream>

namespace dbcomp
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

    } // namespace

    void toJSON( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
    {
        nlohmann::json data;

        data[ "guard" ] = "DATABASE_VIEW_GUARD_4_APRIL_2022";

        {
            nlohmann::json ns = nlohmann::json::object( { { "name", "testnamespace" } } );

            {
                nlohmann::json obj = nlohmann::json::object( { { "name", "obj1" } } );
                ns[ "objects" ].push_back( obj );
            }
            {
                nlohmann::json obj = nlohmann::json::object( { { "name", "obj2" } } );
                ns[ "objects" ].push_back( obj );
            }

            data[ "namespaces" ].push_back( ns );
        }
        {
            nlohmann::json ns = nlohmann::json::object( { { "name", "testnamespace2" } } );

            {
                nlohmann::json obj = nlohmann::json::object( { { "name", "obj3" } } );
                ns[ "objects" ].push_back( obj );
            }
            {
                nlohmann::json obj = nlohmann::json::object( { { "name", "obj4" } } );
                ns[ "objects" ].push_back( obj );
            }

            data[ "namespaces" ].push_back( ns );
        }

        writeJSON( dataDir / "view.json", data );
    }

} // namespace jsonconv
} // namespace dbcomp
