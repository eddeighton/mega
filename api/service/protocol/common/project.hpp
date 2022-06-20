
#ifndef PROJECT_20_JUNE_2022
#define PROJECT_20_JUNE_2022

#include "utilities/serialization_helpers.hpp"

#include "boost/filesystem/path.hpp"
#include "boost/serialization/nvp.hpp"

namespace mega
{
namespace network
{
class Project
{
public:
    Project();
    Project( const boost::filesystem::path& projectInstallPath );

    const boost::filesystem::path& getProjectInstallPath() const { return m_projectInstallPath; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "installationPath", m_projectInstallPath );
    }

private:
    boost::filesystem::path m_projectInstallPath;
};
} // namespace network
} // namespace mega

#endif // PROJECT_20_JUNE_2022
