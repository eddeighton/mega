#include "database/types/sources.hpp"

namespace mega
{
namespace io
{
boost::filesystem::path manifestFilePath::EXTENSION           = ".manifest";
boost::filesystem::path megaFilePath::EXTENSION               = ".mega";
boost::filesystem::path CompilationFilePath::EXTENSION        = ".db";
boost::filesystem::path GeneratedHPPSourceFilePath::EXTENSION = ".hpp";
boost::filesystem::path GeneratedCPPSourceFilePath::EXTENSION = ".cpp";
boost::filesystem::path PrecompiledHeaderFile::EXTENSION      = ".pch";
boost::filesystem::path ObjectFilePath::EXTENSION             = ".obj";
} // namespace io
} // namespace mega
