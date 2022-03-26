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

#include "environment.hpp"

#include "database/io/source_listing.hpp"

#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace list
{
    class BaseTask : public task::Task
    {
    public:
        BaseTask( const Environment& environment, const RawPtrSet& dependencies )
            : task::Task( dependencies )
            , m_environment( environment )
        {
        }

    protected:
        const Environment& m_environment;
    };

    class Task_SourceListingToManifest : public BaseTask
    {
    public:
        Task_SourceListingToManifest( const Environment&                            environment,
                                      const std::vector< boost::filesystem::path >& inputMegaSourceFiles,
                                      bool                                          bIsComponent )
            : BaseTask( environment, {} )
            , m_sourceListing( inputMegaSourceFiles, bIsComponent )
        {
        }
        virtual void run( task::Progress& taskProgress )
        {
            // inputMegaSourceFiles
            const Environment::Path sourceListPath = m_environment.source_list();

            taskProgress.start( "Task_SourceListingToManifest",
                                m_environment.sourceDir(),
                                sourceListPath );

            m_sourceListing.save( sourceListPath );

            taskProgress.succeeded();
        }

    private:
        mega::io::SourceListing m_sourceListing;
    };

    void command( bool bHelp, const std::vector< std::string >& args )
    {
        boost::filesystem::path rootSourceDir, rootBuildDir, sourceDir, buildDir;
        bool                    bIsComponent;
        std::string             objectSourceFiles;

        namespace po = boost::program_options;
        po::options_description commandOptions( " List input mega source files" );
        {
            // clang-format off
            commandOptions.add_options()
                ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ),     "Root source directory" )
                ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),      "Root build directory" )
                ( "src_dir",        po::value< boost::filesystem::path >( &sourceDir ),         "Source directory" )
                ( "build_dir",      po::value< boost::filesystem::path >( &buildDir ),          "Build Directory" )
                ( "is_component",   po::value< bool >( &bIsComponent ),                         "Is this the start of a mega component" )
                ( "names",          po::value< std::string >( &objectSourceFiles ),             "Mega source file names ( semicolon delimited )" )
                ;
            // clang-format on
        }

        po::positional_options_description p;
        p.add( "dir", -1 );

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
            std::vector< boost::filesystem::path > inputSourceFiles;
            {
                std::vector< std::string > megaFileNames;
                {
                    using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
                    boost::char_separator< char > sep( ";" );
                    Tokeniser                     tokens( objectSourceFiles, sep );
                    for ( Tokeniser::iterator i = tokens.begin(); i != tokens.end(); ++i )
                        megaFileNames.push_back( *i );
                }
                for ( const std::string& strFileName : megaFileNames )
                {
                    const boost::filesystem::path sourceFilePath = boost::filesystem::edsCannonicalise(
                        boost::filesystem::absolute( sourceDir / strFileName ) );
                    VERIFY_RTE_MSG( boost::filesystem::exists( sourceFilePath ),
                                    common::COLOUR_RED_BEGIN << "ERROR: Failed to locate: " << sourceFilePath.string() << common::COLOUR_END );
                    inputSourceFiles.push_back( sourceFilePath );
                }
            }

            Environment environment( rootSourceDir, rootBuildDir, sourceDir, buildDir );

            task::Task::PtrVector tasks;

            Task_SourceListingToManifest* pTask = new Task_SourceListingToManifest( environment, inputSourceFiles, bIsComponent );
            tasks.push_back( task::Task::Ptr( pTask ) );

            task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
            task::run( pSchedule, std::cout );
        }
    }
} // namespace list
} // namespace driver
