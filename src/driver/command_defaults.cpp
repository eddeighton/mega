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

#include "environment/environment_archive.hpp"
#include "database/FinalStage.hxx"

#include "mega/defaults.hpp"
#include "mega/values/compilation/type_id.hpp"
#include "mega/values/runtime/reference_io.hpp"

#include "service/network/log.hpp"

#include "mega/values/service/project.hpp"

#include "service/network/log.hpp"
#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/serialization.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace driver::defaults
{

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path projectPath, outputFilePath;

    namespace po = boost::program_options;

    po::options_description commandOptions( " Generate retail archive" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "project_install",   po::value< boost::filesystem::path >( &projectPath ), "Path to Megastructure Project" )
            ( "output",  po::value< boost::filesystem::path >( &outputFilePath ),        "Defaults file to generate" )
            ;
        // clang-format on
    }

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        THROW_TODO;
        /*const mega::Project project( projectPath );

        VERIFY_RTE_MSG( boost::filesystem::exists( project.getProjectDatabase() ),
                        "Failed to locate project database at: " << project.getProjectDatabase().string() );

        mega::io::ArchiveEnvironment environment( project.getProjectDatabase() );

        using namespace FinalStage;
        Database database( environment, environment.project_manifest() );

        mega::Defaults defaults;

        for( Components::Component* pComponent :
             database.many< Components::Component >( environment.project_manifest() ) )
        {
            if( pComponent->get_type().get() == mega::ComponentType::eInterface )
            {
                const mega::io::ComponentFilePath& componentBuildPath = pComponent->get_init_file_path();
                const boost::filesystem::path      componentPath
                    = project.getProjectBin() / componentBuildPath.path().filename();
                VERIFY_RTE_MSG( boost::filesystem::exists( componentPath ),
                                "Failed to locate init module at: " << componentPath.string() );

                boost::dll::fs::path defaultsProviderLib( componentPath );
                SPDLOG_TRACE( "Loaded module: {}", componentPath.string() );

                for( const auto& src : pComponent->get_mega_source_files() )
                {
                    std::string strFileID = src.path().string();
                    boost::replace_all( strFileID, "/", "_" );
                    boost::replace_all( strFileID, ".", "_" );

                    boost::shared_ptr< mega::DefaultsProvider > pDefaultsProvider
                        = boost::dll::import_symbol< mega::DefaultsProvider >( defaultsProviderLib, strFileID );

                    pDefaultsProvider->getDefaults( defaults );
                    SPDLOG_TRACE( "Got defaults for: {}", strFileID );
                }
            }
        }

        // outputFilePath
        {
            static constexpr auto boostXMLArchiveFlags = boost::archive::no_header | boost::archive::no_codecvt
                                                         | boost::archive::no_xml_tag_checking
                                                         | boost::archive::no_tracking;
            auto pOutputFile = boost::filesystem::createNewFileStream( outputFilePath );
            {
                boost::archive::xml_oarchive oa( *pOutputFile, boostXMLArchiveFlags );
                oa&                          boost::serialization::make_nvp( "defaults", defaults );
            }
        }
        SPDLOG_INFO( "Generated defaults to: {}", outputFilePath.string() );*/
    }
}

} // namespace driver::defaults
