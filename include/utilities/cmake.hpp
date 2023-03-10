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

#ifndef DRIVER_COMMAND_UTILS_29_MAR_2022
#define DRIVER_COMMAND_UTILS_29_MAR_2022

#include "common/file.hpp"
#include "common/terminal.hpp"
#include "common/assert_verify.hpp"

#include <boost/tokenizer.hpp>
#include <boost/filesystem/path.hpp>

#include "common/string.hpp"

#include <string>
#include <vector>

namespace mega::utilities
{

inline std::vector< std::string > parseCMakeStringList( const std::string& strPathList, const char* pszDelimiters )
{
    return common::simpleTokenise( strPathList, pszDelimiters );
}

inline std::vector< boost::filesystem::path > pathListToFiles( const std::vector< std::string >& pathList )
{
    std::vector< boost::filesystem::path > result;
    for ( const std::string& strPath : pathList )
    {
        const boost::filesystem::path sourceFilePath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( strPath ) );
        VERIFY_RTE_MSG(
            boost::filesystem::exists( sourceFilePath ),
            common::COLOUR_RED_BEGIN << "ERROR: Failed to locate: " << sourceFilePath.string() << common::COLOUR_END );
        result.push_back( sourceFilePath );
    }
    return result;
}

inline std::vector< boost::filesystem::path > pathListToFiles( const boost::filesystem::path&    sourceDirectory,
                                                               const std::vector< std::string >& pathList )
{
    std::vector< boost::filesystem::path > result;
    for ( const std::string& strPath : pathList )
    {
        const boost::filesystem::path sourceFilePath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( sourceDirectory / strPath ) );
        VERIFY_RTE_MSG(
            boost::filesystem::exists( sourceFilePath ),
            common::COLOUR_RED_BEGIN << "ERROR: Failed to locate: " << sourceFilePath.string() << common::COLOUR_END );
        result.push_back( sourceFilePath );
    }
    return result;
}

inline std::vector< boost::filesystem::path > pathListToFolders( const std::vector< std::string >& pathList )
{
    std::vector< boost::filesystem::path > result;
    for ( const std::string& strPath : pathList )
    {
        const boost::filesystem::path folderPath
            = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( strPath ) );
        VERIFY_RTE_MSG(
            boost::filesystem::exists( folderPath ),
            common::COLOUR_RED_BEGIN << "ERROR: Failed to locate: " << folderPath.string() << common::COLOUR_END );
        result.push_back( folderPath );
    }
    return result;
}

} // namespace mega::utilities
#endif // DRIVER_COMMAND_UTILS_29_MAR_2022
