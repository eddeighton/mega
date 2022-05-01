
#ifndef FORMAT_1_MAY_2022
#define FORMAT_1_MAY_2022

#include "boost/filesystem/path.hpp"

#include <optional>
#include <string>

namespace mega
{
    namespace utilities
    {
        void clang_format( std::string& str, std::optional< boost::filesystem::path > formatSpecPath );
    }
}

#endif //FORMAT_1_MAY_2022
