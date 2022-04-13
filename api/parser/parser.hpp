
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

namespace ParserStage
{
    class Database;

    namespace Parser
    {
        class ContextDef;
    }
} // namespace ParserStage

struct EGDB_EXPORT EG_PARSER_INTERFACE
{
    virtual ParserStage::Parser::ContextDef* parseEGSourceFile( ParserStage::Database&                        database,
                                                                const boost::filesystem::path&                sourceFilePath,
                                                                const std::vector< boost::filesystem::path >& includeDirectories,
                                                                std::ostream&                                 osError,
                                                                std::ostream&                                 osWarn )
        = 0;
};

#endif // PARSER_14_SEPT_2020