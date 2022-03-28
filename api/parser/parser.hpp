
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
}

struct EGDB_EXPORT EG_PARSER_INTERFACE
{
    virtual void parseEGSourceFile( const boost::filesystem::path& sourceDir,
                                    const boost::filesystem::path& buildDir,
                                    const mega::io::FileInfo&      sourceFile,
                                    const mega::io::FileInfo&      objectASTFile,
                                    const mega::io::FileInfo&      objectBodyFile,
                                    std::ostream&                  osError )
        = 0;
};
} // namespace mega

#endif // PARSER_14_SEPT_2020