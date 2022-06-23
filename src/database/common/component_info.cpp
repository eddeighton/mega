
#include "database/common/component_info.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/file_status.hpp>
#include <fstream>
#include <optional>

namespace mega
{
namespace io
{
ComponentInfo::ComponentInfo() {}

ComponentInfo::ComponentInfo( ComponentType componentType, const std::string& strName,
                              const std::vector< std::string >& cppFlags, const std::vector< std::string >& cppDefines,
                              const boost::filesystem::path& srcDir, const boost::filesystem::path& buildDir,
                              const ComponentInfo::PathArray& sourceFiles,
                              const ComponentInfo::PathArray& dependencyFiles, const PathArray& includeDirectories )
    : m_componentType( componentType )
    , m_strName( strName )
    , m_cppFlags( cppFlags )
    , m_cppDefines( cppDefines )
    , m_srcDir( srcDir )
    , m_buildDir( buildDir )
    , m_sourceFiles( sourceFiles )
    , m_dependencyFiles( dependencyFiles )
    , m_includeDirectories( includeDirectories )
{
}

} // namespace io
} // namespace mega

BOOST_CLASS_IMPLEMENTATION( mega::io::ComponentInfo, object_serializable )
