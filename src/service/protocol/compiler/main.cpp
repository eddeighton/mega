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


#include "service/protocol/compiler/json_converter.hpp"
#include "service/protocol/compiler/generator.hpp"
#include "service/protocol/compiler/grammar.hpp"
#include "service/protocol/compiler/generator.hpp"

#include "common/file.hpp"
#include "common/hash.hpp"
#include "common/assert_verify.hpp"

#include "nlohmann/json.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>
#include <boost/stacktrace.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>

boost::filesystem::path inputStringToPath( const std::string strPath )
{
    VERIFY_RTE_MSG( !strPath.empty(), "No output folder specified" );
    const boost::filesystem::path actualPath
        = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( strPath ) );
    {
        VERIFY_RTE_MSG( boost::filesystem::exists( actualPath ),
                        "Could not locate output folder directory: " << actualPath.string() );
    }
    return actualPath;
}

int main( int argc, const char* argv[] )
{
    namespace po = boost::program_options;
    try
    {
        bool bHelp             = false;
        bool bGeneralWait      = false;
        bool bGenerateJSON     = false;
        bool bExecuteTemplates = false;

        std::vector< std::string > sourceFiles;
        std::string                outputAPIDir, outputSrcDir, dataDir, injaDir;

        po::options_description commandOptions( " Commands" );
        {
            commandOptions.add_options()
                // clang-format off
            ( "help",   po::bool_switch( &bHelp ), "Print command line help info." )
            ( "wait",   po::bool_switch( &bGeneralWait ), "Wait at startup for attaching a debugger" )

            ( "api", po::value< std::string >( &outputAPIDir ), "Output folder to generate API" )
            ( "src", po::value< std::string >( &outputSrcDir ), "Output folder to generate source" )

            ( "data_dir", po::value< std::string >( &dataDir ), "Directory for inja templates and data sub directories" )
            ( "inja_dir", po::value< std::string >( &injaDir ), "Directory for inja templates and data sub directories" )

            ( "json",   po::bool_switch( &bGenerateJSON ), "Generate JSON data from input schema." )
            ( "template",   po::bool_switch( &bExecuteTemplates ), "Execute output templates on JSON data." )

            ( "input",  po::value< std::vector< std::string > >( &sourceFiles ), "Input source file" )
            ;
            // clang-format on
        }

        po::positional_options_description p;
        p.add( "input", -1 );

        po::variables_map vm;
        po::store( po::command_line_parser( argc, argv ).options( commandOptions ).positional( p ).run(), vm );
        po::notify( vm );

        if ( bHelp )
        {
            std::cout << commandOptions << "\n";
        }
        else
        {
            using Path       = boost::filesystem::path;
            using PathVector = std::vector< Path >;

            const Path outputAPIFolderPath = inputStringToPath( outputAPIDir );
            const Path outputSrcFolderPath = inputStringToPath( outputSrcDir );

            const Path dataFolderPath = inputStringToPath( dataDir );
            const Path injaFolderPath = inputStringToPath( injaDir );

            if ( bGenerateJSON )
            {
                PathVector inputSourceFiles;
                for ( const std::string& strFile : sourceFiles )
                {
                    inputSourceFiles.push_back( inputStringToPath( strFile ) );
                }

                std::map< std::string, protocol::schema::Schema > allSchemas;
                for ( const Path& sourceFilePath : inputSourceFiles )
                {
                    std::string strFileContents;
                    boost::filesystem::loadAsciiFile( sourceFilePath, strFileContents, true );

                    std::ostringstream            osError;
                    protocol::schema::Schema      schema;
                    protocol::schema::ParseResult result = protocol::schema::parse( strFileContents, schema, osError );
                    if ( result.bSuccess )
                    {
                        const std::string strName = sourceFilePath.filename().replace_extension( "" ).string();
                        allSchemas.insert( std::make_pair( strName, schema ) );
                        std::string::const_iterator iterReached = result.iterReached.base();
                        if ( iterReached == strFileContents.end() )
                        {
                            // write schema to json data
                            protocol::jsonconv::toJSON( dataFolderPath, strName, schema );
                        }
                        else
                        {
                            const int         distance      = std::distance( strFileContents.cbegin(), iterReached );
                            const int         distanceToEnd = std::distance( iterReached, strFileContents.cend() );
                            const std::string strError( strFileContents.cbegin() + std::max( 0, distance - 30 ),
                                                        iterReached + std::min( distanceToEnd - distance, 30 ) );
                            THROW_RTE( "Failed to load schema file: " << sourceFilePath.string()
                                                                      << " Could not parse beyond line: "
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
                protocol::jsonconv::toMessagesJSON( dataFolderPath, allSchemas );
            }

            if ( bExecuteTemplates )
            {
                const protocol::gen::Environment env{
                    outputAPIFolderPath, outputSrcFolderPath, dataFolderPath, injaFolderPath };
                protocol::gen::generate( env );
            }
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
