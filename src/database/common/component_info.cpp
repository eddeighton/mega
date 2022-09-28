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

#include "database/common/component_info.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/file_status.hpp>
#include <fstream>
#include <optional>

namespace mega
{
namespace io
{
ComponentInfo::ComponentInfo() {}

ComponentInfo::ComponentInfo( ComponentType componentType, const std::string& strName,
                              const boost::filesystem::path& filePath, const std::vector< std::string >& cppFlags,
                              const std::vector< std::string >& cppDefines, const boost::filesystem::path& srcDir,
                              const boost::filesystem::path& buildDir, const ComponentInfo::PathArray& sourceFiles,
                              const ComponentInfo::PathArray& dependencyFiles, const PathArray& includeDirectories )
    : m_componentType( componentType )
    , m_strName( strName )
    , m_filePath( filePath )
    , m_cppFlags( cppFlags )
    , m_cppDefines( cppDefines )
    , m_srcDir( srcDir )
    , m_buildDir( buildDir )
    , m_sourceFiles( sourceFiles )
    , m_dependencyFiles( dependencyFiles )
    , m_includeDirectories( includeDirectories )
{
}

} // namespace io
} // namespace mega

BOOST_CLASS_IMPLEMENTATION( mega::io::ComponentInfo, object_serializable )
