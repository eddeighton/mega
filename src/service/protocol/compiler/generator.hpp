


#ifndef DATABASE_COMPILER_GENERATOR_4_APRIL_2022
#define DATABASE_COMPILER_GENERATOR_4_APRIL_2022

#include "boost/filesystem/path.hpp"

namespace protocol
{
namespace gen
{
    struct Environment
    {
        boost::filesystem::path apiDir, srcDir, dataDir, injaDir;
    };

    void generate( const Environment& env );
} // namespace gen
} // namespace protocol

#endif // DATABASE_COMPILER_GENERATOR_4_APRIL_2022
