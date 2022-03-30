
#include "database/io/component_info.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/file_status.hpp>
#include <fstream>
#include <optional>

namespace mega
{
namespace io
{
    ComponentInfo::ComponentInfo()
    {
    }

    ComponentInfo::ComponentInfo( const std::string&              strName,
                                  const boost::filesystem::path&  directory,
                                  const ComponentInfo::PathArray& sourceFiles,
                                  const PathArray&                includeDirectories )
        : m_strName( strName )
        , m_directory( directory )
        , m_sourceFiles( sourceFiles )
        , m_includeDirectories( includeDirectories )
    {
    }

} // namespace io
} // namespace mega

BOOST_CLASS_IMPLEMENTATION( mega::io::ComponentInfo, object_serializable )
