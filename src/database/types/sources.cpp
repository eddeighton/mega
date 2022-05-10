#include "database/types/sources.hpp"

namespace mega
{
    namespace io
    {
        boost::filesystem::path manifestFilePath::EXTENSION             = ".manifest";
        boost::filesystem::path megaFilePath::EXTENSION                 = ".mega";
        boost::filesystem::path CompilationFilePath::EXTENSION          = ".db";
        boost::filesystem::path GeneratedHPPSourceFilePath::EXTENSION   = ".hpp";
        boost::filesystem::path GeneratedCPPSourceFilePath::EXTENSION   = ".cpp";
        boost::filesystem::path PrecompiledHeaderFile::EXTENSION        = ".pch";
    } // namespace io
} // namespace mega
