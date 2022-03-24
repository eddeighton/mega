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

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <string>
#include <vector>
#include <iostream>

void command_build( bool bHelp, const std::string& strBuildCommand, const std::vector< std::string >& args )
{
    boost::filesystem::path strSourceDir, strBuildDir;
    bool bFullRebuild = false;
    std::vector< std::string > flags;
    std::string names;

    namespace po = boost::program_options;
    po::options_description commandOptions(" Build Project Command");
    {
        commandOptions.add_options()
            ("src_dir",     po::value< boost::filesystem::path >( &strSourceDir ), "Source directory")
            ("build_dir",   po::value< boost::filesystem::path >( &strBuildDir ), "Build Directory" )
            ("full",        po::bool_switch( &bFullRebuild ), "Full rebuild - do not reuse previous objects or precompiled headers" )
            ("flags",       po::value< std::vector< std::string > >( &flags ), "C++ Compilation Flags" )
            ("names",       po::value< std::string >( &names ), "eg source file names ( no extension, semicolon delimited )" )
        ;
    }

    po::positional_options_description p;
    p.add( "dir", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        if( strBuildCommand.empty() )
        {
            std::cout << "Missing build command type" << std::endl;
            return;
        }

        std::string strCompilationFlags;
        {
            std::ostringstream osFlags;
            for( const auto& str : flags )
                osFlags << str << " ";
            strCompilationFlags = osFlags.str();
        }

        if( strCompilationFlags.empty() )
        {
            //use defaults
            std::cout << "Warning using default compiler flags as none specified" << std::endl;
            strCompilationFlags = "-O3 -mllvm -polly -MD -DNDEBUG -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH -DWIN32_LEAN_AND_MEAN -D_DLL -DNOMINMAX -DBOOST_ALL_NO_LIB -D_CRT_SECURE_NO_WARNINGS -DBOOST_USE_WINDOWS_H -D_WIN32_WINNT=0x0601 -DWIN32 -D_WINDOWS -Xclang -std=c++17 -Xclang -fcoroutines-ts -Xclang -flto-visibility-public-std -Xclang -Wno-deprecated -Xclang -fexceptions -Xclang -Wno-inconsistent-missing-override";
        }


        //tokenize semi colon delimited names
        std::vector< std::string > megaFileNames;
        {
            using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
            boost::char_separator< char > sep( ";" );
            Tokeniser tokens( names, sep );
            for ( Tokeniser::iterator i = tokens.begin(); i != tokens.end(); ++i )
                megaFileNames.push_back( *i );
        }
        for( const std::string& str : megaFileNames )
        {
            std::cout << "eg source name: " << str << std::endl;
        }

        /*
        const boost::filesystem::path projectDirectory =
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strDirectory ) );

        if( !boost::filesystem::exists( projectDirectory ) )
        {
            THROW_RTE( "Specified directory does not exist: " << projectDirectory.generic_string() );
        }
        else if( !boost::filesystem::is_directory( projectDirectory ) )
        {
            THROW_RTE( "Specified path is not a directory: " << projectDirectory.generic_string() );
        }

        if( bFullRebuild )
        {
            Environment environment;
            ProjectTree projectTree( environment, projectDirectory, strProject );
            if( boost::filesystem::exists( projectTree.getInterfaceFolder() ) )
            {
                std::cout << "Removing: " << projectTree.getInterfaceFolder().generic_string() << std::endl;
                boost::filesystem::remove_all( projectTree.getInterfaceFolder() );
            }
            if( boost::filesystem::exists( projectTree.getImplFolder() ) )
            {
                std::cout << "Removing: " << projectTree.getImplFolder().generic_string() << std::endl;
                boost::filesystem::remove_all( projectTree.getImplFolder() );
            }
            if( boost::filesystem::exists( projectTree.getStashFolder() ) )
            {
                std::cout << "Removing: " << projectTree.getStashFolder().generic_string() << std::endl;
                boost::filesystem::remove_all( projectTree.getStashFolder() );
            }
        }

        if( strCoordinator.empty() && strHost.empty() )
        {
            build::Interface::build_interface( projectDirectory, strProject, strCompilationFlags );
        }
        else
        {
            //tokenize semi colon delimited names
            std::vector< std::string > megaFileNames;
            {
                using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
                boost::char_separator< char > sep( ";" );
                Tokeniser tokens( names, sep );
                for ( Tokeniser::iterator i = tokens.begin(); i != tokens.end(); ++i )
                    megaFileNames.push_back( *i );
            }
            //for( const std::string& str : megaFileNames )
            //{
            //    std::cout << "eg source name: " << str << std::endl;
            //}

            VERIFY_RTE_MSG( !strCoordinator.empty(), "Missing Coordinator" );
            VERIFY_RTE_MSG( !strHost.empty(), "Missing Host Name" );

            build::Implementation::build_implementation(
                projectDirectory, strCoordinator, strHost, strProject, strCompilationFlags, megaFileNames, binPath );
        }*/
    }

}
