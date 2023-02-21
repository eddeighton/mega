
#include "mainWindow.hpp"

#include <QApplication>

//megastructure includes
//#include "megastructureprotocol/protocol_helpers.hpp"
//#include "megastructure/component.hpp"
//#include "megastructure/mega.hpp"
//#include "egcomponent/egcomponent.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    /*Environment environment( "." );
    megastructure::Component component( 
        environment,
        megastructure::MEGA_PORT,
        megastructure::EG_PORT,
        megastructure::getHostProgramName(),
        nullptr );*/
            
    QApplication application( argc, argv );
    
    editor::MainWindow mainWindow;
    mainWindow.show();
        
    return application.exec();
}
