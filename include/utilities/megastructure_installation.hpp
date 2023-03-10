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

#include "serialization_helpers.hpp"
#include "tool_chain_hash.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/serialization/nvp.hpp>

namespace mega
{

class MegastructureInstallation
{
public:
    MegastructureInstallation();
    MegastructureInstallation( const boost::filesystem::path& megastructureInstallationPath );

    const bool isEmpty() const;

    const boost::filesystem::path& getInstallationPath() const { return m_installationPath; }
    mega::utilities::ToolChain     getToolchain() const;
    boost::filesystem::path        getParserPath() const;
    boost::filesystem::path        getClangPath() const;
    boost::filesystem::path        getMegaIncludePath() const;

    boost::filesystem::path getRuntimeTemplateAllocation() const;
    boost::filesystem::path getRuntimeTemplateAllocate() const;
    boost::filesystem::path getRuntimeTemplateRead() const;
    boost::filesystem::path getRuntimeTemplateWrite() const;
    boost::filesystem::path getRuntimeTemplateReadLink() const;
    boost::filesystem::path getRuntimeTemplateWriteLink() const;
    boost::filesystem::path getRuntimeTemplateCall() const;
    boost::filesystem::path getRuntimeTemplateStart() const;
    boost::filesystem::path getRuntimeTemplateStop() const;
    boost::filesystem::path getRuntimeTemplateSave() const;
    boost::filesystem::path getRuntimeTemplateLoad() const;
    boost::filesystem::path getRuntimeTemplateGet() const;
    boost::filesystem::path getRuntimeTemplateRelation() const;
    boost::filesystem::path getRuntimeTemplateProgram() const;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "installationPath", m_installationPath );
    }

private:
    boost::filesystem::path m_installationPath;
};

} // namespace mega::network

#endif // MEGASTRUCTURE_INSTALLATION_16_AUG_2022