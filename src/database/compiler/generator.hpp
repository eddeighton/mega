#ifndef DATABASE_COMPILER_GENERATOR_4_APRIL_2022
#define DATABASE_COMPILER_GENERATOR_4_APRIL_2022

#include "model.hpp"

#include "boost/filesystem/path.hpp"

namespace db
{
namespace gen
{
    struct Environment
    {
        boost::filesystem::path apiDir, srcDir, dataDir, injaDir;
    };

    void generate( const Environment& env, db::model::Schema::Ptr pSchema );
} // namespace gen
} // namespace db

#endif // DATABASE_COMPILER_GENERATOR_4_APRIL_2022
