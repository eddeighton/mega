


#ifndef ROOT_CONFIG_20_JUNE_2022
#define ROOT_CONFIG_20_JUNE_2022

#include "project.hpp"

#include "boost/serialization/nvp.hpp"

namespace mega
{
namespace network
{

class RootConfig
{
public:
    RootConfig();

    const Project& getProject() const { return m_project; }
    void setProject( const Project& project ) { m_project = project; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "project", m_project );
    }
private:
    Project m_project;
};

} // namespace network
} // namespace mega

#endif // ROOT_CONFIG_20_JUNE_2022
