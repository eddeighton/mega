
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

const bool              Project::isEmpty() const { return m_projectInstallPath.empty(); }
boost::filesystem::path Project::getProjectBin() const { return m_projectInstallPath / "bin/"; }
boost::filesystem::path Project::getProjectDatabase() const { return getProjectBin() / "archive.adb"; }
boost::filesystem::path Project::getProjectTempDir() const { return m_projectInstallPath / "tmp/"; }

} // namespace network
} // namespace mega
