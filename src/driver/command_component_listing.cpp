

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environment_build.hpp"

#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"
#include "utilities/cmake.hpp"

#include "common/assert_verify.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace component_listing
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path srcDir, buildDir, componentSrcDir, componentBuildDir, componentFilePath;
    std::string strType, strComponentName, strCPPFlags, strCPPDefines, strIncludeDirectories, strDependencyFilePaths;
    std::vector< std::string > objectSourceFileNames;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Generate component listing file" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "type",           po::value< std::string >( &strType ),                               "Component type: Interface | Component" )
            ( "root_src_dir",   po::value< boost::filesystem::path >( &srcDir ),                    "Root source directory" )
            ( "root_build_dir", po::value< boost::filesystem::path >( &buildDir ),                  "Root build directory" )
            ( "src_dir",        po::value< boost::filesystem::path >( &componentSrcDir ),           "Source directory" )
            ( "build_dir",      po::value< boost::filesystem::path >( &componentBuildDir ),         "Build Directory" )
            ( "name",           po::value< std::string >( &strComponentName ),                      "Component name" )
            ( "file_path",      po::value< boost::filesystem::path >( &componentFilePath ),         "Component file path" )
            ( "cpp_flags",      po::value< std::string >( &strCPPFlags ),                           "C++ Compiler Flags" )
            ( "cpp_defines",    po::value< std::string >( &strCPPDefines ),                         "C++ Compiler Defines" )
            ( "include_dirs",   po::value< std::string >( &strIncludeDirectories ),                 "Include directories ( semicolon delimited )" )
            ( "dependencies",   po::value< std::string >( &strDependencyFilePaths ),                "Mega source file dependencies" )
            ( "src",            po::value< std::vector< std::string > >( &objectSourceFileNames ),  "Mega source file names" )
            ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "src", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if ( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        // tokenize semi colon delimited names into absolute mega source file paths
        const std::vector< boost::filesystem::path > inputSourceFiles
            = mega::utilities::pathListToFiles( componentSrcDir, objectSourceFileNames );

        const std::vector< boost::filesystem::path > dependencySourceFiles = mega::utilities::pathListToFiles(
            componentSrcDir, mega::utilities::parseCMakeStringList( strDependencyFilePaths, " " ) );

        const std::vector< std::string > cppFlags   = mega::utilities::parseCMakeStringList( strCPPFlags, " " );
        const std::vector< std::string > cppDefines = mega::utilities::parseCMakeStringList( strCPPDefines, " " );
        const std::vector< boost::filesystem::path > includeDirectories
            = mega::utilities::pathListToFolders( mega::utilities::parseCMakeStringList( strIncludeDirectories, ";" ) );

        mega::compiler::Directories directories{ srcDir, buildDir, "", "" };
        mega::io::BuildEnvironment  environment( directories );

        const mega::ComponentType componentType = mega::ComponentType::fromStr( strType.c_str() );

        switch ( componentType.get() )
        {
            case mega::ComponentType::eInterface:
            {
                for ( const boost::filesystem::path& srcFile : inputSourceFiles )
                {
                    VERIFY_RTE_MSG( srcFile.extension() == mega::io::megaFilePath::extension(),
                                    "Source file is NOT a mega source file: " << srcFile.string() );
                }
            }
            break;
            case mega::ComponentType::eLibrary:
            {
                for ( const boost::filesystem::path& srcFile : inputSourceFiles )
                {
                    VERIFY_RTE_MSG( srcFile.extension() == mega::io::cppFilePath::extension(),
                                    "Source file is NOT a C++ source file: " << srcFile.string() );
                }
            }
            break;
            case mega::ComponentType::TOTAL_COMPONENT_TYPES:
            default:
                THROW_RTE( "Unknown component type" );
        }

        const mega::io::ComponentInfo componentInfo( componentType, strComponentName, componentFilePath, cppFlags, cppDefines,
                                                     componentSrcDir, componentBuildDir, inputSourceFiles,
                                                     dependencySourceFiles, includeDirectories );

        const mega::io::ComponentListingFilePath componentListingFilePath
            = environment.ComponentListingFilePath_fromPath( componentBuildDir );
        {
            boost::filesystem::path         tempFile;
            std::unique_ptr< std::ostream > pOfstream = environment.write_temp( componentListingFilePath, tempFile );
            mega::OutputArchiveType         oa( *pOfstream );
            oa << boost::serialization::make_nvp( "componentInfo", componentInfo );
        }
        environment.temp_to_real( componentListingFilePath );
    }
}
} // namespace list
} // namespace driver
