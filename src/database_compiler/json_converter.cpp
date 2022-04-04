#include "json_converter.hpp"

#include "database_compiler/model.hpp"
#include "nlohmann/json.hpp"

#include <fstream>

namespace db
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

        for ( model::Namespace::Ptr pNamespace : pSchema->m_namespaces )
        {
            nlohmann::json ns = nlohmann::json::object( { { "name", pNamespace->m_strName } } );

            for ( model::Object::Ptr pObject : pNamespace->m_objects )
            {
                nlohmann::json obj = nlohmann::json::object( { { "name", pObject->m_strName } } );
                ns[ "objects" ].push_back( obj );
            }
            data[ "namespaces" ].push_back( ns );
        }

        writeJSON( dataDir / "view.json", data );
    }

} // namespace jsonconv
} // namespace db
