
#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/environment.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <cstdlib>

namespace mega
{
namespace network
{

MegastructureInstallation::MegastructureInstallation() {}

MegastructureInstallation::MegastructureInstallation( const boost::filesystem::path& megastructureInstallationPath )
    : m_installationPath( megastructureInstallationPath )
{
}

MegastructureInstallation MegastructureInstallation::fromEnvironment()
{
    std::ostringstream os;
    os << std::getenv( mega::environment::BUILD_PATH ) << "/" << std::getenv( mega::environment::CFG_TUPLE )
       << "/mega/install";
    return MegastructureInstallation( os.str() );
}

const bool MegastructureInstallation::isEmpty() const { return m_installationPath.empty(); }

mega::utilities::ToolChain MegastructureInstallation::getToolchainXML() const
{
    VERIFY_RTE( !isEmpty() );

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

boost::filesystem::path MegastructureInstallation::getParserPath() const { return getToolchainXML().parserDllPath; }
boost::filesystem::path MegastructureInstallation::getClangPath() const { return getToolchainXML().clangCompilerPath; }

boost::filesystem::path MegastructureInstallation::getMegaIncludePath() const
{
    VERIFY_RTE( !isEmpty() );
    return m_installationPath / "include";
}

boost::filesystem::path MegastructureInstallation::getRuntimeTemplateAllocation() const
{
    return m_installationPath / "templates/allocation.jinja";
}
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateAllocate() const
{
    return m_installationPath / "templates/allocate.jinja";
}
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateRead() const
{
    return m_installationPath / "templates/read.jinja";
}
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateWrite() const
{
    return m_installationPath / "templates/write.jinja";
}
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateCall() const
{
    return m_installationPath / "templates/call.jinja";
}

} // namespace network
} // namespace mega