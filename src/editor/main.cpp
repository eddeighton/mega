
#include "mainWindow.hpp"

#include <QApplication>
#include <QTimer>


#ifndef Q_MOC_RUN

#include "service/tool.hpp"
#include "service/network/network.hpp"

#include "boost/asio/spawn.hpp"

#include <iostream>

#endif

namespace editor
{
void MainWindow::OnIdle()
{
    boost::asio::post( m_yield_ctx );
    queueIdleTimer();
}
} // namespace editor

int runGui( int argc, char* argv[], boost::asio::yield_context& yield_ctx )
{
    QApplication application( argc, argv );

    editor::MainWindow mainWindow( yield_ctx );
    mainWindow.show();
    mainWindow.queueIdleTimer();

    return application.exec();
}

int main( int argc, char* argv[] )
{
    try
    {
        mega::service::Tool tool( mega::network::MegaDaemonPort() );

        mega::service::Tool::Functor functor = [ argc, argv ]( boost::asio::yield_context& yield_ctx ) -> int
        { return runGui( argc, argv, yield_ctx ); };

        tool.run( functor );
    }
    catch( std::exception& ex )
    {
        std::cout << "Exception: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
