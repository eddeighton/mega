

#ifndef DATABASE_HEADER_15_04_2019
#define DATABASE_HEADER_15_04_2019

#include "mega/common_strings.hpp"

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

namespace clang
{
class QualType;
class ASTContext;
class Sema;
class Token;
class SourceLocation;
} // namespace clang

// EG Database Interface
namespace mega
{
using EGChar = char;

struct EGDB_EXPORT EG_PLUGIN_INTERFACE
{
    virtual void initialise( clang::ASTContext* pASTContext, clang::Sema* pSema ) = 0;
    virtual void setMode( const char* strMode, const char* strSrcDir, const char* strBuildDir,
                             const char* strSourceFile )
        = 0;
    virtual void runFinalAnalysis()                                                                   = 0;
    virtual bool isEGEnabled()                                                                        = 0;
    virtual bool isEGType( const clang::QualType& type )                                              = 0;
    virtual bool isPossibleEGType( const clang::QualType& type )                                      = 0;
    virtual bool isPossibleEGTypeIdentifier( const clang::Token& token )                              = 0;
    virtual int  isPossibleEGTypeIdentifierDecl( const clang::Token& token, bool bIsTypePathParsing ) = 0;
    virtual bool getInvocationOperationType( const clang::SourceLocation& loc, const clang::QualType& typePathType,
                                             bool bHasArguments, clang::QualType& operationType )
        = 0;
    virtual bool getInvocationResultType( const clang::SourceLocation& loc, const clang::QualType& baseType,
                                          clang::QualType& resultType )
        = 0;
};
} // namespace mega

typedef void* ( *EG_PLUGIN_INTERFACE_GETTER )();

extern "C"
{
    EGDB_EXPORT void* GET_EG_PLUGIN_INTERFACE();
}

#endif // DATABASE_HEADER_15_04_2019
