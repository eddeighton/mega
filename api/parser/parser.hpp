
#ifndef PARSER_14_SEPT_2020
#define PARSER_14_SEPT_2020

#include "boost/filesystem/path.hpp"
#include <ostream>

#if defined( _MSC_VER )

// Microsoft
#ifdef EG_DATABASE_SHARED_MODULE
#define EGDB_EXPORT __declspec( dllexport )
#else
#define EGDB_EXPORT __declspec( dllimport )
#endif

#elif defined( __GNUC__ )

// GCC
#ifdef EG_DATABASE_SHARED_MODULE
#define EGDB_EXPORT __attribute__( ( visibility( "default" ) ) )
#else
#define EGDB_EXPORT
#endif

#endif

// EG Database Interface
namespace mega
{
namespace io
{
    class FileInfo;
    namespace stage
    {
        class Stage_ObjectParse;
    }
    template < typename T >
    class Database;
} // namespace io
/*
namespace input
{
    class Root;
}*/

struct EGDB_EXPORT EG_PARSER_INTERFACE
{
    virtual void
    parseEGSourceFile( mega::io::Database< mega::io::stage::Stage_ObjectParse >& database,
                       //mega::input::Root*                                        pRoot,
                       const std::vector< boost::filesystem::path >&             includeDirectories,
                       std::ostream&                                             osError,
                       std::ostream&                                             osWarn )
        = 0;
};
} // namespace mega

#endif // PARSER_14_SEPT_2020