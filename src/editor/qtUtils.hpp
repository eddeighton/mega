
#ifndef QT_UTILS_21_FEB_2021
#define QT_UTILS_21_FEB_2021

#include "common/requireSemicolon.hpp"

#define CMD_CONNECT( action, command ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        QObject::connect( m_pMainWindow->getUI()->action, SIGNAL( triggered() ), this, SLOT( command() )); \
        m_pMainWindow->addActionRef( m_pMainWindow->getUI()->action ); \
    )

#define CMD_DISCONNECT( action, command ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        QObject::disconnect( m_pMainWindow->getUI()->action, SIGNAL(triggered()), this, SLOT( command() )); \
        m_pMainWindow->decActionRef( m_pMainWindow->getUI()->action ); \
    )
    
#endif //QT_UTILS_21_FEB_2021