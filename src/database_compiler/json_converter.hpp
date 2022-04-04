#ifndef JSON_CONVERTER_4_APRIL_2022
#define JSON_CONVERTER_4_APRIL_2022

#include "model.hpp"

#include "boost/filesystem/path.hpp"

namespace dbcomp
{
    namespace jsonconv
    {
        void toJSON( const boost::filesystem::path& dataDir, model::Schema::Ptr pSchema );

    }
}


#endif //JSON_CONVERTER_4_APRIL_2022