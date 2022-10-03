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

#include "service/protocol/common/project.hpp"

namespace mega::network
{

Project::Project() {}

Project::Project( const boost::filesystem::path& projectInstallPath )
    : m_projectInstallPath( projectInstallPath )
{
}

const bool              Project::isEmpty() const { return m_projectInstallPath.empty(); }
boost::filesystem::path Project::getProjectBin() const { return m_projectInstallPath / "bin/"; }
boost::filesystem::path Project::getProjectDatabase() const { return getProjectBin() / "archive.adb"; }
boost::filesystem::path Project::getProjectTempDir() const { return m_projectInstallPath / "tmp/"; }

} // namespace mega::network
