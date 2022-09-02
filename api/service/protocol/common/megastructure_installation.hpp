
#ifndef MEGASTRUCTURE_INSTALLATION_16_AUG_2022
#define MEGASTRUCTURE_INSTALLATION_16_AUG_2022

#include "utilities/serialization_helpers.hpp"
#include "utilities/tool_chain_hash.hpp"

#include "boost/filesystem/path.hpp"
#include "boost/serialization/nvp.hpp"

namespace mega
{
namespace network
{
class MegastructureInstallation
{
public:
    MegastructureInstallation();
    MegastructureInstallation( const boost::filesystem::path& megastructureInstallationPath );

    const bool isEmpty() const;

    const boost::filesystem::path& getInstallationPath() const { return m_installationPath; }
    mega::utilities::ToolChain     getToolchainXML() const;
    boost::filesystem::path        getParserPath() const;
    boost::filesystem::path        getClangPath() const;
    
    boost::filesystem::path        getRuntimeTemplateAllocation() const;
    boost::filesystem::path        getRuntimeTemplateAllocate() const;
    boost::filesystem::path        getRuntimeTemplateRead() const;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "installationPath", m_installationPath );
    }

private:
    boost::filesystem::path m_installationPath;
};
} // namespace network
} // namespace mega

#endif // MEGASTRUCTURE_INSTALLATION_16_AUG_2022
