
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
} // namespace network
} // namespace mega