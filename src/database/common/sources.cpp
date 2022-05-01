#include "database/common/sources.hpp"

namespace mega
{
    namespace io
    {
        boost::filesystem::path manifestFilePath::EXTENSION    = ".manifest";
        boost::filesystem::path megaFilePath::EXTENSION        = ".mega";
        boost::filesystem::path CompilationFilePath::EXTENSION = ".db";
        boost::filesystem::path GeneratedHPPSourceFilePath::EXTENSION = ".hxx";
        boost::filesystem::path GeneratedCPPSourceFilePath::EXTENSION = ".cxx";
    } // namespace io
} // namespace mega