


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

namespace mega
{
namespace utilities
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

} // namespace utilities
} // namespace mega
#endif // DRIVER_COMMAND_UTILS_29_MAR_2022
