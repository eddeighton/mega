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

#include "model.hpp"
#include "generator.hpp"

#include "compiler/cmake.hpp"

#include "inja/environment.hpp"

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>
#include <iomanip>

namespace il_gen
{
extern void parseNative( const boost::filesystem::path& filePath, Model& model );
extern void parseInline( const boost::filesystem::path& filePath, Model& model );
extern void parseExtern( const boost::filesystem::path& filePath, Model& model );
} // namespace il_gen

int main( int argc, const char* argv[] )
{
    bool bWait = false;

    {
        namespace po = boost::program_options;
        po::variables_map vm;
        bool              bGeneralWait = false;

        po::options_description genericOptions( " General" );
        {
            // clang-format off
            genericOptions.add_options()
            ( "help,?",                                         "Produce general or command help message" )
            ( "wait",       po::bool_switch( &bGeneralWait ),   "Wait at startup for attaching a debugger" );
            // clang-format on
        }

        std::string             inlineFiles, nativeFiles, externFiles, templateFiles;
        boost::filesystem::path injaTemplatesDir, ilTypesFilePath, ilFunctionsFilePath;

        namespace po = boost::program_options;
        po::options_description commandOptions( " Execute inja template" );
        {
            // clang-format off
            commandOptions.add_options()
                ( "native",   po::value< std::string >( &nativeFiles ),     "CMake file list describing IL native types and functions" )
                ( "inline",   po::value< std::string >( &inlineFiles ),     "CMake file list describing IL inline types and functions" )
                ( "extern",   po::value< std::string >( &externFiles ),     "CMake file list describing extern runtime support functions" )
                ( "template", po::value< std::string >( &templateFiles ),   "CMake file list describing materialised functions" )
                ( "inja",     po::value< boost::filesystem::path >( &injaTemplatesDir ), "Inja templates directory path" )

                ( "il_types", po::value< boost::filesystem::path >( &ilTypesFilePath ), "IL types.hxx output file" )
                ( "il_functions", po::value< boost::filesystem::path >( &ilFunctionsFilePath ), "IL functions.hxx output file" )
                ;
            // clang-format on
        }

        po::options_description visibleOptions( "Allowed options" );
        visibleOptions.add( genericOptions ).add( commandOptions );

        po::parsed_options parsedOptions = po::command_line_parser( argc, argv ).options( visibleOptions ).run();
        po::store( parsedOptions, vm );
        po::notify( vm );

        if( vm.count( "help" ) )
        {
            std::cout << visibleOptions << "\n";
            return 0;
        }

        try
        {
            if( bGeneralWait )
            {
                std::cout << "Waiting for input..." << std::endl;
                char c;
                std::cin >> c;
            }

            VERIFY_RTE_MSG( boost::filesystem::exists( injaTemplatesDir ),
                            "Failed to locate inja template directory at: " << injaTemplatesDir.string() );

            // determine the model
            il_gen::Model model;
            {
                const std::vector< boost::filesystem::path > nativeFilePaths
                    = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( nativeFiles, " " ) );
                const std::vector< boost::filesystem::path > inlineFilePaths
                    = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( inlineFiles, " " ) );
                const std::vector< boost::filesystem::path > externFilePaths
                    = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( externFiles, " " ) );
                const std::vector< boost::filesystem::path > templateFilePaths
                    = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( templateFiles, " " ) );

                {
                    for( const auto& f : nativeFilePaths )
                    {
                        std::cout << "Native file: " << f.string() << std::endl;
                        parseNative( f, model );
                    }
                }
                {
                    for( const auto& f : inlineFilePaths )
                    {
                        std::cout << "Inline file: " << f.string() << std::endl;
                        parseInline( f, model );
                    }
                }

                {
                    for( const auto& f : externFilePaths )
                    {
                        std::cout << "Extern file: " << f.string() << std::endl;
                        parseExtern( f, model );
                    }
                }

                for( const auto& f : templateFilePaths )
                {
                    std::cout << "Template file: " << f.string() << std::endl;
                }
            }

            inja::Environment injaEnv( injaTemplatesDir.string() );
            injaEnv.set_trim_blocks( true );

            // generate the outputs

            // 1. Generate the IL mega/src/include/il/elements/types.hxx
            std::cout << "Generating: " << ilTypesFilePath.string() << std::endl;
            generateTypes( injaEnv, model, ilTypesFilePath );

            // 2. Generate the IL mega/src/include/il/elements/functions.hxx
            std::cout << "Generating: " << ilFunctionsFilePath.string() << std::endl;
            generateFunctions( injaEnv, model, ilFunctionsFilePath );

            // 3. Generate the mega mangle adaptors for extern type erasure

            // 4. Generate the C++ JIT Functors

            // 5. Generate the C JIT Functors

            // 6. Generate whatever needed for how JIT internally dispatches to IL code generators

            return 0;
        }
        catch( std::exception& e )
        {
            std::cout << "Exception: " << e.what() << std::endl;
            return 1;
        }
        catch( ... )
        {
            std::cout << "Unknown error" << std::endl;
            return 1;
        }
    }

    return 0;
}
