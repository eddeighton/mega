//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef DATABASE_HEADER_15_04_2019
#define DATABASE_HEADER_15_04_2019

#include "mega/common_strings.hpp"

#if defined( _WIN32 )
#ifdef MEGA_CLANG_PLUGIN_MODULE
#define MEGA_CLANG_PLUGIN_EXPORT __declspec( dllexport )
#else
#define MEGA_CLANG_PLUGIN_EXPORT __declspec( dllimport )
#endif
#elif defined( __GNUC__ )
#ifdef MEGA_CLANG_PLUGIN_MODULE
#define MEGA_CLANG_PLUGIN_EXPORT __attribute__( ( visibility( "default" ) ) )
#else
#define MEGA_CLANG_PLUGIN_EXPORT
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

struct MEGA_CLANG_PLUGIN_EXPORT EG_PLUGIN_INTERFACE
{
    virtual void initialise( clang::ASTContext* pASTContext, clang::Sema* pSema ) = 0;
    virtual void onMegaPragma()                                                   = 0;
    virtual void setMode( const char* strMode, const char* strSrcDir, const char* strBuildDir,
                          const char* strSourceFile )
        = 0;
    virtual void runFinalAnalysis() = 0;
    virtual bool isEGEnabled()      = 0;

    virtual bool isPtr( const clang::QualType& type )     = 0;
    virtual unsigned int  getSymbolID( const clang::Token& token ) = 0;

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
    MEGA_CLANG_PLUGIN_EXPORT void* GET_EG_PLUGIN_INTERFACE();
}

#endif // DATABASE_HEADER_15_04_2019
