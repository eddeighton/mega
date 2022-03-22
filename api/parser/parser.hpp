
#ifndef PARSER_14_SEPT_2020
#define PARSER_14_SEPT_2020

#include "database/stages/parser.hpp"

#include "boost/filesystem/path.hpp"
#include <ostream>

#if defined(_MSC_VER)

// Microsoft
#ifdef EG_DATABASE_SHARED_MODULE
    #define EGDB_EXPORT __declspec( dllexport )
#else
    #define EGDB_EXPORT __declspec( dllimport )
#endif

#elif defined(__GNUC__)

// GCC
#ifdef EG_DATABASE_SHARED_MODULE
    #define EGDB_EXPORT __attribute__((visibility("default")))
#else
    #define EGDB_EXPORT
#endif

#endif

//EG Database Interface
namespace eg
{
    namespace input
    {
        class Context;
        class Export;
    }
    
    struct EG_PARSER_CALLBACK
    {
        virtual void contextBody( const eg::input::Context* pContext, const char* pszBodyText ) = 0;
        virtual void exportBody( const eg::input::Export* pExport, const char* pszBodyText ) = 0;
    };
    
    struct EGDB_EXPORT EG_PARSER_INTERFACE
    {
        virtual void parseEGSourceFile( EG_PARSER_CALLBACK* pCallback,
                    const boost::filesystem::path& egSourceFile,
                    const boost::filesystem::path& cwdPath, std::ostream& osError,
                    Stages::Parser& session, input::Root* pRoot ) = 0;
    };
}

#endif //PARSER_14_SEPT_2020