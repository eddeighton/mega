#include "grammar.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>
#include <boost/stacktrace.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>

int main( int argc, const char* argv[] )
{
    namespace po = boost::program_options;
    po::variables_map vm;
    try
    {
        bool bHelp = false;
        bool bGeneralWait = false;

        std::vector< std::string > sourceFiles;
        std::string                outputAPIDir, outputSrcDir;

        po::options_description commandOptions( " Commands" );
        {
            commandOptions.add_options()
                // clang-format off
            ( "help",   po::bool_switch( &bHelp ), "Print command line help info." )
            ( "wait",   po::bool_switch( &bGeneralWait ), "Wait at startup for attaching a debugger" )
            ( "input",  po::value< std::vector< std::string > >( &sourceFiles ), "Input source file" )
            ( "api", po::value< std::string >( &outputAPIDir ), "Output folder to generate API" )
            ( "src", po::value< std::string >( &outputSrcDir ), "Output folder to generate source" )
            ;
            // clang-format on
        }

        po::positional_options_description p;
        p.add( "input", -1 );

        po::variables_map vm;
        po::store(
            po::command_line_parser( argc, argv ).options( commandOptions ).positional( p ).run(),
            vm );
        po::notify( vm );

        if ( bHelp )
        {
            std::cout << commandOptions << "\n";
        }
        else
        {
            using Path = boost::filesystem::path;
            using PathVector = std::vector< Path >;

            Path outputAPIFolderPath;
            {
                VERIFY_RTE_MSG( !outputAPIDir.empty(), "No output folder specified" );
                outputAPIFolderPath = boost::filesystem::edsCannonicalise(
                    boost::filesystem::absolute( outputAPIDir ) );
                {
                    VERIFY_RTE_MSG( boost::filesystem::exists( outputAPIFolderPath ),
                                    "Could not locate output folder directory: "
                                        << outputAPIFolderPath.string() );
                    std::cout << "Generating to output API folder: " << outputAPIFolderPath.string()
                              << std::endl;
                }
            }

            Path outputSrcFolderPath;
            {
                VERIFY_RTE_MSG( !outputSrcDir.empty(), "No output folder specified" );
                outputSrcFolderPath = boost::filesystem::edsCannonicalise(
                    boost::filesystem::absolute( outputSrcDir ) );
                {
                    VERIFY_RTE_MSG( boost::filesystem::exists( outputSrcFolderPath ),
                                    "Could not locate output folder directory: "
                                        << outputSrcFolderPath.string() );
                    std::cout << "Generating to output source folder: "
                              << outputSrcFolderPath.string() << std::endl;
                }
            }

            PathVector inputSourceFiles;
            {
                VERIFY_RTE_MSG( !sourceFiles.empty(), "No input source files specified" );
                for ( const std::string& strPath : sourceFiles )
                {
                    const boost::filesystem::path sourceFilePath
                        = boost::filesystem::edsCannonicalise(
                            boost::filesystem::absolute( strPath ) );
                    VERIFY_RTE_MSG(
                        boost::filesystem::exists( sourceFilePath ),
                        "Could not find specified input file: " << sourceFilePath.string() );
                    inputSourceFiles.push_back( sourceFilePath );
                }
            }

            dbcomp::Schema schema;
            for ( const Path& sourceFilePath : inputSourceFiles )
            {
                std::string strFileContents;
                boost::filesystem::loadAsciiFile( sourceFilePath, strFileContents, true );

                std::ostringstream  osError;
                dbcomp::Schema      fileSchema;
                dbcomp::ParseResult result = dbcomp::parse( strFileContents, fileSchema, osError );
                if ( result.bSuccess )
                {
                    std::string::const_iterator iterReached = result.iterReached.base();
                    if ( iterReached == strFileContents.end() )
                    {
                        std::copy( fileSchema.m_elements.begin(), fileSchema.m_elements.end(),
                                   std::back_inserter( schema.m_elements ) );
                    }
                    else
                    {
                        const int distance = std::distance( strFileContents.cbegin(), iterReached );
                        const int distanceToEnd
                            = std::distance( iterReached, strFileContents.cend() );
                        const std::string strError(
                            strFileContents.cbegin() + std::max( 0, distance - 30 ),
                            iterReached + std::min( distanceToEnd - distance, 30 ) );
                        THROW_RTE( "Failed to load schema file: "
                                   << sourceFilePath.string() << " Could not parse beyond line: "
                                   << result.iterReached.position() << "\n"
                                   << strError << "\n"
                                   << osError.str() );
                    }
                }
                else
                {
                    THROW_RTE( "Failed to load schema file: " << sourceFilePath.string()
                                                              << " Error: " << osError.str() );
                }
            }

            std::cout << "Parsed schema:\n" << schema << std::endl;
        }
    }
    catch ( boost::program_options::error& e )
    {
        std::cout << "Invalid input. " << e.what() << "\nType '--help' for options" << std::endl;
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        return 1;
    }
    catch ( std::exception& e )
    {
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        return 1;
    }
    catch ( ... )
    {
        std::cout << "Unknown error.\n" << std::endl;
        std::cout << boost::stacktrace::stacktrace() << std::endl;
        return 1;
    }

    return 0;
}
