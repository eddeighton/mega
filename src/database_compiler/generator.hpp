#ifndef DATABASE_COMPILER_GENERATOR_4_APRIL_2022
#define DATABASE_COMPILER_GENERATOR_4_APRIL_2022

#include "boost/filesystem/path.hpp"

namespace dbcomp
{
namespace gen
{
    struct Environment
    {
        boost::filesystem::path apiDir, srcDir, dataDir, injaDir;
    };

    void generate_view( const Environment& env );
} // namespace gen
} // namespace dbcomp

#endif // DATABASE_COMPILER_GENERATOR_4_APRIL_2022
