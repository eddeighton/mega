
#include "service/protocol/common/project.hpp"

namespace mega
{
namespace network
{

Project::Project() {}

Project::Project( const boost::filesystem::path& projectInstallPath )
    : m_projectInstallPath( projectInstallPath )
{
}

const bool Project::isEmpty() const 
{
    return m_projectInstallPath.empty();
}

const boost::filesystem::path Project::getProjectDatabase() const
{
    return m_projectInstallPath / "bin/archive.adb";
}

} // namespace network
} // namespace mega