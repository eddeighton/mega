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

#ifndef MEGASTRUCTURE_INSTALLATION_16_AUG_2022
#define MEGASTRUCTURE_INSTALLATION_16_AUG_2022

#include "mega/values/compilation/tool_chain_hash.hpp"

#include "common/serialisation.hpp"

#include <boost/filesystem/path.hpp>

#include <ostream>

namespace mega
{

class MegastructureInstallation
{
public:
    MegastructureInstallation();
    MegastructureInstallation( const boost::filesystem::path& megastructureInstallationPath );

    bool isEmpty() const;

    const boost::filesystem::path& getInstallationPath() const { return m_installationPath; }
    mega::utilities::ToolChain     getToolchain() const;
    boost::filesystem::path        getParserPath() const;
    boost::filesystem::path        getClangPath() const;
    boost::filesystem::path        getMegaIncludePath() const;
    boost::filesystem::path        getExecutorPath() const;
    boost::filesystem::path        getRuntimeTemplateDir() const;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "installationPath", m_installationPath );
        }
        else
        {
            archive& m_installationPath;
        }
    }

private:
    boost::filesystem::path m_installationPath;
};

inline std::ostream& operator<<( std::ostream& os, const MegastructureInstallation& megaInstall )
{
    return os << megaInstall.getInstallationPath().string();
}

} // namespace mega

#endif // MEGASTRUCTURE_INSTALLATION_16_AUG_2022
