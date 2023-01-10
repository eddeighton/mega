//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/environment.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <cstdlib>

namespace mega::network
{

MegastructureInstallation::MegastructureInstallation()
{
}

MegastructureInstallation::MegastructureInstallation( const boost::filesystem::path& megastructureInstallationPath )
    : m_installationPath( megastructureInstallationPath )
{
}
/*
MegastructureInstallation MegastructureInstallation::fromEnvironment()
{
    std::ostringstream os;
    os << std::getenv( mega::environment::BUILD_PATH ) << "/" << std::getenv( mega::environment::CFG_TUPLE )
       << "/mega/install";
    return MegastructureInstallation( os.str() );
}
*/
const bool MegastructureInstallation::isEmpty() const
{
    return m_installationPath.empty();
}

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

boost::filesystem::path MegastructureInstallation::getParserPath() const
{
    return getToolchainXML().parserDllPath;
}
boost::filesystem::path MegastructureInstallation::getClangPath() const
{
    return getToolchainXML().clangCompilerPath;
}

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
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateStart() const
{
    return m_installationPath / "templates/start.jinja";
}
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateStop() const
{
    return m_installationPath / "templates/stop.jinja";
}
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateSave() const
{
    return m_installationPath / "templates/save.jinja";
}
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateLoad() const
{
    return m_installationPath / "templates/load.jinja";
}
boost::filesystem::path MegastructureInstallation::getRuntimeTemplateProgram() const
{
    return m_installationPath / "templates/program.jinja";
}

} // namespace mega::network
