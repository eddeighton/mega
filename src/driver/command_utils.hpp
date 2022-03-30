#ifndef DRIVER_COMMAND_UTILS_29_MAR_2022
#define DRIVER_COMMAND_UTILS_29_MAR_2022

#include "common/file.hpp"
#include "common/terminal.hpp"
#include "common/assert_verify.hpp"

#include <boost/tokenizer.hpp>
#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace driver
{
inline std::vector< std::string > parsePathList( const std::string& strPathList )
{
    std::vector< std::string > paths;
    {
        using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
        boost::char_separator< char > sep( ";" );
        Tokeniser                     tokens( strPathList, sep );
        for ( Tokeniser::iterator i = tokens.begin(); i != tokens.end(); ++i )
            paths.push_back( *i );
    }
    return paths;
}

inline std::vector< boost::filesystem::path > pathListToFiles( const std::vector< std::string >& pathList )
{
    std::vector< boost::filesystem::path > result;
    for ( const std::string& strPath : pathList )
    {
        const boost::filesystem::path sourceFilePath = boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( strPath ) );
        VERIFY_RTE_MSG( boost::filesystem::exists( sourceFilePath ),
                        common::COLOUR_RED_BEGIN << "ERROR: Failed to locate: " << sourceFilePath.string() << common::COLOUR_END );
        result.push_back( sourceFilePath );
    }
    return result;
}

inline std::vector< boost::filesystem::path > pathListToFiles( const boost::filesystem::path& sourceDirectory, const std::vector< std::string >& pathList )
{
    std::vector< boost::filesystem::path > result;
    for ( const std::string& strPath : pathList )
    {
        const boost::filesystem::path sourceFilePath = boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( sourceDirectory / strPath ) );
        VERIFY_RTE_MSG( boost::filesystem::exists( sourceFilePath ),
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
        const boost::filesystem::path folderPath = boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( strPath ) );
        VERIFY_RTE_MSG( boost::filesystem::exists( folderPath ),
                        common::COLOUR_RED_BEGIN << "ERROR: Failed to locate: " << folderPath.string() << common::COLOUR_END );
        result.push_back( folderPath );
    }
    return result;
}

} // namespace driver

#endif // DRIVER_COMMAND_UTILS_29_MAR_2022
