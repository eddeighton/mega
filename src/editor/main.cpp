
#include "mainWindow.hpp"

#include <QApplication>
#include <QTimer>

#ifndef Q_MOC_RUN

#include "service/tool.hpp"
#include "service/network/network.hpp"

#include "boost/asio/spawn.hpp"
#include <boost/program_options.hpp>

#include <iostream>

#endif

namespace editor
{
void MainWindow::OnIdle()
{
    if( m_pYield_ctx )
    {
        boost::asio::post( *m_pYield_ctx );
        queueIdleTimer();
    }
}
} // namespace editor

int runGuiInService( int argc, char* argv[], boost::asio::yield_context& yield_ctx )
{
    QApplication       application( argc, argv );
    editor::MainWindow mainWindow( &yield_ctx );
    mainWindow.show();
    mainWindow.queueIdleTimer();
    return application.exec();
}
int runGui( int argc, char* argv[] )
{
    QApplication       application( argc, argv );
    editor::MainWindow mainWindow( nullptr );
    mainWindow.show();
    return application.exec();
}

int main( int argc, char* argv[] )
{
    std::string             strIP              = "localhost";
    short                   daemonPortNumber   = mega::network::MegaDaemonPort();
    boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
    std::string             strConsoleLogLevel = "warn", strLogFileLevel = "warn";
    bool                    bConnect = false;
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",    po::bool_switch( &bShowHelp ),                                                 "Show Command Line Help" )
        ( "log",     po::value< boost::filesystem::path >( &logFolder ),                            "Logging folder" )
        ( "console", po::value< std::string >( &strConsoleLogLevel ),                               "Console logging level" )
        ( "level",   po::value< std::string >( &strLogFileLevel ),                                  "Log file logging level" )
        ( "port",    po::value< short >( &daemonPortNumber )->default_value( daemonPortNumber ),    "Daemon port number" )
        ( "connect", po::bool_switch( &bConnect ),                                                  "Connect to Megastructure" )
        ;
        // clang-format on

        po::parsed_options parsedOptions = po::command_line_parser( argc, argv ).options( options ).run();

        po::variables_map vm;
        po::store( parsedOptions, vm );
        po::notify( vm );

        if( bShowHelp )
        {
            std::cout << options << "\n";
            return 0;
        }
    }

    try
    {
        auto log = mega::network::configureLog(
            mega::network::Log::Config{ logFolder, "editor", mega::network::fromStr( strConsoleLogLevel ),
                                        mega::network::fromStr( strLogFileLevel ) } );

        if( bConnect )
        {
            mega::service::Tool tool( daemonPortNumber, log );

            mega::service::Tool::Functor functor = [ argc, argv ]( boost::asio::yield_context& yield_ctx ) -> int
            { return runGuiInService( argc, argv, yield_ctx ); };

            tool.run( functor );
        }
        else
        {
            runGui( argc, argv );
        }
    }
    catch( std::exception& ex )
    {
        std::cout << "Exception: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
