#include "json_converter.hpp"

#include "database_compiler/model.hpp"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <fstream>
#include <set>

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
            
            void writeViewData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
            {
                nlohmann::json data( { { "stages", nlohmann::json::array() } } );

                data[ "guard" ] = "DATABASE_VIEW_GUARD_4_APRIL_2022";
                writeJSON( dataDir / "view.json", data );
            }

            void writeDataData( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
            {
                nlohmann::json data( { { "files", nlohmann::json::array() } } );

                data[ "guard" ] = "DATABASE_DATA_GUARD_4_APRIL_2022";

                writeJSON( dataDir / "data.json", data );
            }

        } // namespace

        void toJSON( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema )
        {
            writeViewData( dataDir, pSchema );
            writeDataData( dataDir, pSchema );
        }

    } // namespace jsonconv
} // namespace db
