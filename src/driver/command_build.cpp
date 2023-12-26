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

#include "service/terminal.hpp"

#include "environment/environment.hpp"
#include "environment/environment_build.hpp"
#include "environment/environment_archive.hpp"

#include "log/log.hpp"

#include "common/process.hpp"
#include "common/stash.hpp"
#include "common/string.hpp"
#include "common/file.hpp"

#include <spdlog/stopwatch.h>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#pragma warning( push )
#pragma warning( disable : 4996 ) // iterator thing
#pragma warning( disable : 4244 ) // conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#pragma warning( pop )

#include <vector>
#include <string>
#include <iostream>

namespace driver::build
{
bool runBuild( const mega::service::Project& project, const mega::io::Directories& directories )
{
    using namespace mega::service;

    bool bWasError = false;

    std::cout << "Building project: '" << project << "' in directory: " << directories.buildDir.string() << std::endl;

    const auto buildCmd = Environment::buildCmd();

    std::future< std::string > output, error;
    {
        namespace bp = boost::process;
        boost::asio::io_service ios;
        std::error_code         ec;
        bp::child c( buildCmd, bp::start_dir = directories.buildDir, bp::std_in.close(), bp::std_out > output,
                     bp::std_err > error, bp::error_code( ec ), ios );
        ios.run();
    }

    // ninja does NOT produce error code or error output
    std::string strOutput = output.get();
    std::string strError  = error.get();
    if( !strError.empty() )
    {
        bWasError = true;
        std::cout << "Error: " << strError << std::endl;
    }
    else
    {
        using namespace std::string_literals;
        const std::string strFAILED = "FAILED:"s;

        bWasError
            = std::search( strOutput.begin(), strOutput.end(), strFAILED.begin(), strFAILED.end() ) != strOutput.end();

        if( bWasError )
        {
            std::cout << strOutput << std::endl;
        }
    }
    return bWasError;
}

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    std::string projectName;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Build Commands" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "name", po::value< std::string >( &projectName ), "Project Name" )

        ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "name", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        using namespace mega::service;

        Project project( projectName );
        VERIFY_RTE_MSG( !project.empty(), "Invalid project name in build command: " << projectName );

        const mega::io::Directories directories{ Environment::srcDir( project ), Environment::buildDir( project ) };

        if( runBuild( project, directories ) )
        {
            return;
        }

        mega::io::BuildEnvironment buildEnvironment( directories );

        const auto workBinPath = Environment::workBin();
        if( !boost::filesystem::exists( workBinPath ) )
        {
            boost::filesystem::create_directories( workBinPath );
        }

        bool                    bNewDatabase = false;
        boost::filesystem::path databaseHashFile;
        {
            boost::filesystem::path  databaseArchivePath = directories.buildDir / "database.archive";
            const mega::io::Manifest manifest( buildEnvironment, buildEnvironment.project_manifest() );
            mega::io::ReadArchive::compile_archive(
                databaseArchivePath, manifest, directories.srcDir, directories.buildDir );

            const task::FileHash fileHash( databaseArchivePath );

            std::ostringstream osFileName;
            osFileName << fileHash.toHexString() << ".db";

            databaseHashFile = workBinPath / osFileName.str();

            if( boost::filesystem::copyFileIfChanged( databaseArchivePath, databaseHashFile ) )
            {
                bNewDatabase = true;
                std::cout << "Generated db at: " << databaseHashFile.string() << std::endl;
            }
        }

        bool                                          bNewComponent = false;
        std::vector< ProgramManifest::ComponentHash > components;
        {
            using namespace FinalStage;
            Database database( buildEnvironment );

            for( Components::Component* pComponent :
                 database.many< Components::Component >( buildEnvironment.project_manifest() ) )
            {
                const auto           componentFilePath = buildEnvironment.FilePath( pComponent->get_file_path() );
                const task::FileHash fileHash( componentFilePath );
                components.push_back( ProgramManifest::ComponentHash{ pComponent->get_name(), fileHash } );

                std::ostringstream osFileName;
                osFileName << fileHash.toHexString() << ".so";
                const boost::filesystem::path soHashFile = workBinPath / osFileName.str();

                if( boost::filesystem::copyFileIfChanged( componentFilePath, soHashFile ) )
                {
                    bNewComponent = true;
                    std::cout << "Generated Component: " << pComponent->get_name() << " : " << pComponent->get_type()
                              << " : " << soHashFile.string() << std::endl;
                }
            }
        }

        bool bLatestIsSame = false;
        auto latestOpt     = Program::latest( project, workBinPath );
        {
            // is the latest the same ?
            if( latestOpt.has_value() )
            {
                const auto latest = Environment::load( latestOpt.value() );
                if( ( latest.getComponents() == components ) && ( latest.getDatabase() == databaseHashFile ) )
                {
                    bLatestIsSame = true;
                }
            }
        }

        if( bLatestIsSame )
        {
            std::cout << "Found existing program: " << latestOpt.value() << std::endl;
        }
        else
        {
            const Program         program = Program::next( project, workBinPath );
            const ProgramManifest programManifest{ program, databaseHashFile, components };
            auto pFileStream = boost::filesystem::createBinaryOutputFileStream( Environment::prog( program ) );
            boost::archive::xml_oarchive xml( *pFileStream );
            xml&                         boost::serialization::make_nvp( "program_manifest", programManifest );

            std::cout << "Created new program: " << program << std::endl;
        }
    }
}
} // namespace driver::build
