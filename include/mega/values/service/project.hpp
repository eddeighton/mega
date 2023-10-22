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

#ifndef PROJECT_20_JUNE_2022
#define PROJECT_20_JUNE_2022

#include "common/serialisation.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/serialization/nvp.hpp>

#include <ostream>

namespace mega
{

class Project
{
public:
    Project();
    Project( const boost::filesystem::path& projectInstallPath );

    const bool isEmpty() const;

    const boost::filesystem::path& getProjectInstallPath() const { return m_projectInstallPath; }

    boost::filesystem::path getProjectBin() const;
    boost::filesystem::path getProjectDatabase() const;
    boost::filesystem::path getProjectUnityDatabase() const;
    boost::filesystem::path getProjectTempDir() const;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "installationPath", m_projectInstallPath );
    }

    inline bool operator==( const Project& cmp ) const { return m_projectInstallPath == cmp.m_projectInstallPath; }

private:
    boost::filesystem::path m_projectInstallPath;
};

inline std::ostream& operator<<( std::ostream& os, const Project& project )
{
    return os << project.getProjectInstallPath().string();
}

} // namespace mega

#endif // PROJECT_20_JUNE_2022
