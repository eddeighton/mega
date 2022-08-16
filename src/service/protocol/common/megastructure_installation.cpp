
#include "service/protocol/common/megastructure_installation.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

namespace mega
{
namespace network
{

MegastructureInstallation::MegastructureInstallation() {}

MegastructureInstallation::MegastructureInstallation( const boost::filesystem::path& projectInstallPath )
    : m_installationPath( projectInstallPath )
{
}

const bool MegastructureInstallation::isEmpty() const { return m_installationPath.empty(); }

const mega::utilities::ToolChain MegastructureInstallation::getToolchainXML() const
{
    const boost::filesystem::path toolChainXMLPath = m_installationPath / "etc/toolchain.xml";

    mega::utilities::ToolChain toolChain;
    {
        VERIFY_RTE_MSG( boost::filesystem::exists( toolChainXMLPath ),
                        "Failed to locate toolchain file: " << toolChainXMLPath.string() );
        auto                         pInStream = boost::filesystem::createBinaryInputFileStream( toolChainXMLPath );
        boost::archive::xml_iarchive ia( *pInStream );
        ia&                          boost::serialization::make_nvp( "toolchain", toolChain );
    }

    return toolChain;
}

const boost::filesystem::path MegastructureInstallation::getParserPath() const
{
    return getToolchainXML().parserDllPath;
}

} // namespace network
} // namespace mega