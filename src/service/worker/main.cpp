

#include "service/worker.hpp"
#include "service/network/log.hpp"

#include "pipeline/pipeline.hpp"

#include "common/assert_verify.hpp"

//#include "common/file.hpp"
//#include "common/stash.hpp"

#include "boost/program_options.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

int main( int argc, const char* argv[] )
{
    using NumThreadsType = decltype( std::thread::hardware_concurrency() );

    // boost::filesystem::path logDir      = boost::filesystem::current_path(), stashDir, pipelineXML;

    NumThreadsType          uiNumThreads       = std::thread::hardware_concurrency();
    std::string             strIP              = "localhost";
    boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
    std::string             strConsoleLogLevel = "warn", strLogFileLevel = "warn";
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",    po::bool_switch( &bShowHelp ),                      "Show Command Line Help" )
        ( "ip",      po::value< std::string >( &strIP ),                 "Root IP Address" )
        ( "threads", po::value< NumThreadsType >( &uiNumThreads ),       "Max number of threads" )
        ( "log",     po::value< boost::filesystem::path >( &logFolder ), "Logging folder" )
        ( "console", po::value< std::string >( &strConsoleLogLevel ),    "Console logging level" )
        ( "level", po::value< std::string >( &strLogFileLevel ),         "Log file logging level" )

       /* ( "log_dir",            po::value< boost::filesystem::path >( &logDir ),        "Build log directory" )
        ( "stash_dir",          po::value< boost::filesystem::path >( &stashDir ),      "Stash directory" )
        ( "configuration",      po::value< boost::filesystem::path >( &pipelineXML ),   "Pipeline Configuration XML File" )*/
        ;
        // clang-format on

        po::parsed_options parsedOptions = po::command_line_parser( argc, argv ).options( options ).run();

        po::variables_map vm;
        po::store( parsedOptions, vm );
        po::notify( vm );

        if ( bShowHelp )
        {
            std::cout << options << "\n";
            return 0;
        }

        if ( strIP.empty() )
        {
            std::cerr << "Missing IP Address" << std::endl;
            return -1;
        }
        uiNumThreads = std::min( std::max( 1U, uiNumThreads ), std::thread::hardware_concurrency() );
    }

     std::cout << "Connecting to: " << strIP << std::endl;

    try
    {
        /*{
            mega::pipeline::Configuration pipelineConfig;
            boost::filesystem::loadAsciiFile( pipelineXML, pipelineConfig.data() );

            // if ( bRunLocally )
            {
                VERIFY_RTE_MSG( !stashDir.empty(), "Local pipeline execution requires stash directry" );
                task::Stash stash( stashDir );

                mega::pipeline::Pipeline::Ptr pPipeline = mega::pipeline::Registry::getPipeline( pipelineConfig );

                bool bContinue = true;
                struct ProgressReport : public mega::pipeline::Progress
                {
                    bool& m_bContinue;
                    ProgressReport( bool& bContinue )
                        : m_bContinue( bContinue )
                    {
                    }
                    virtual void onStarted( const std::string& strMsg ) { SPDLOG_INFO( strMsg ); }
                    virtual void onProgress( const std::string& strMsg ) { SPDLOG_INFO( strMsg ); }
                    virtual void onFailed( const std::string& strMsg )
                    {
                        SPDLOG_INFO( strMsg );
                        m_bContinue = false;
                    }
                    virtual void onCompleted( const std::string& strMsg ) { SPDLOG_INFO( strMsg ); }
                } progressReporter( bContinue );

                struct StashImpl : public mega::pipeline::Stash
                {
                    task::Stash& m_stash;
                    StashImpl( task::Stash& stash )
                        : m_stash( stash )
                    {
                    }

                    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath )
                    {
                        return m_stash.getBuildHashCode( filePath );
                    }
                    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode )
                    {
                        m_stash.setBuildHashCode( filePath, hashCode );
                    }
                    virtual void stash( const boost::filesystem::path& file, task::DeterminantHash code )
                    {
                        m_stash.stash( file, code );
                    }
                    virtual bool restore( const boost::filesystem::path& file, task::DeterminantHash code )
                    {
                        return m_stash.restore( file, code );
                    }
                } stashImpl( stash );

                mega::pipeline::Schedule schedule = pPipeline->getSchedule();
                while ( !schedule.isComplete() && bContinue )
                {
                    bool bProgress = false;
                    for ( const mega::pipeline::TaskDescriptor& task : schedule.getReady() )
                    {
                        pPipeline->execute( task, progressReporter, stashImpl );
                        if ( !bContinue )
                            break;
                        schedule.complete( task );
                        bProgress = true;
                    }
                    VERIFY_RTE_MSG( bProgress, "Failed to make progress executing pipeline" );
                }
            }
        }*/

        auto logThreads
            = mega::network::configureLog( logFolder, "worker", mega::network::fromStr( strConsoleLogLevel ),
                                           mega::network::fromStr( strLogFileLevel ) );

        boost::asio::io_context ioContext;

        mega::service::Worker worker( ioContext, uiNumThreads );

        std::vector< std::thread > threads;
        for ( int i = 0; i < uiNumThreads; ++i )
        {
            threads.emplace_back( std::move( std::thread( [ &ioContext ]() { ioContext.run(); } ) ) );
        }
        for ( std::thread& thread : threads )
        {
            thread.join();
        }
    }
    catch ( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
