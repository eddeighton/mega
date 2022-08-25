//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#include "clang_plugin/clang_plugin.hpp"

#include "database/common/exception.hpp"

#include "session.hpp"

#include "common/assert_verify.hpp"

#include "clang_utils.hpp"

#include "clang/Lex/Pragma.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Lookup.h"
#include "clang/Basic/DiagnosticParse.h"

#include <sstream>
#include <cstdlib>
#include <memory>

namespace
{
clang::Session::Ptr g_pSession;
clang::ASTContext*  g_pASTContext = nullptr;
clang::Sema*        g_pSema       = nullptr;
std::string         g_error;
bool                g_bMegaEnabled = false;
} // namespace

namespace clang
{
extern Session::Ptr make_interface_session( clang::ASTContext* pASTContext, clang::Sema* pSema, const char* strSrcDir,
                                            const char* strBuildDir, const char* strSourceFile );

extern Session::Ptr make_operations_session( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir,
                                             const char* strBuildDir, const char* strSourceFile );

Session::Ptr make_library_session( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir,
                                     const char* strBuildDir, const char* strSourceFile )
{
    return std::make_unique< AnalysisSession >( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile );
}

class TestPragmaHandler : public clang::PragmaHandler
{
public:
    TestPragmaHandler()
        : PragmaHandler( "mega" )
    {
    }
    void HandlePragma( Preprocessor& PP, PragmaIntroducer Introducer, Token& PragmaTok )
    {
        std::cout << "GOT PRAGMA!!!" << std::endl;
        g_bMegaEnabled = !g_bMegaEnabled;
    }
};

static PragmaHandlerRegistry::Add< TestPragmaHandler > Y( "mega", "mega pragma description" );

} // namespace clang

namespace mega
{

struct EG_PLUGIN_INTERFACE_IMPL : EG_PLUGIN_INTERFACE
{
    virtual void initialise( clang::ASTContext* pASTContext, clang::Sema* pSema )
    {
        // std::cout << "EG_PLUGIN_INTERFACE_IMPL::initialise: " << std::endl;
        g_pASTContext = pASTContext;
        g_pSema       = pSema;
        VERIFY_RTE( g_pASTContext );
        VERIFY_RTE( g_pSema );
        if ( g_pSession )
        {
            g_pSession->setContext( g_pASTContext, g_pSema );
        }
    }

    virtual void setMode( const char* strMode, const char* strSrcDir, const char* strBuildDir,
                          const char* strSourceFile )
    {
        try
        {
            const mega::CompilationMode compilationMode = mega::CompilationMode::fromStr( strMode );
            switch ( compilationMode.get() )
            {
                case mega::CompilationMode::eInterface:
                    g_bMegaEnabled = true;
                    g_pSession = clang::make_interface_session(
                        g_pASTContext, g_pSema, strSrcDir, strBuildDir, strSourceFile );
                    break;
                case mega::CompilationMode::eLibrary:
                    g_bMegaEnabled = true;
                    g_pSession = clang::make_library_session(
                        g_pASTContext, g_pSema, strSrcDir, strBuildDir, strSourceFile );
                    break;
                case mega::CompilationMode::eOperations:
                    g_bMegaEnabled = true;
                    g_pSession = clang::make_operations_session(
                        g_pASTContext, g_pSema, strSrcDir, strBuildDir, strSourceFile );
                    break;
                case mega::CompilationMode::eCPP:
                    g_bMegaEnabled = false;
                    g_pSession = clang::make_operations_session(
                        g_pASTContext, g_pSema, strSrcDir, strBuildDir, strSourceFile );
                    break;
                case mega::CompilationMode::eNormal:
                case mega::CompilationMode::TOTAL_COMPILATION_MODES:
                default:
                    g_bMegaEnabled = false;
                    g_pSession = std::make_unique< clang::Session >( g_pASTContext, g_pSema );
                    return;
            }
        }
        catch ( mega::io::DatabaseVersionException& ex )
        {
            g_pSession = nullptr;
            std::ostringstream os;
            os << "DatabaseVersion Exception constructing session for: " << strSourceFile << " : " << ex.what();
            if ( g_pASTContext )
            {
                g_pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            }
            else
            {
                g_error = os.str();
            }
        }
        catch ( std::exception& ex )
        {
            g_pSession = nullptr;
            std::ostringstream os;
            os << "Exception for: " << strSourceFile << " constructing session: " << ex.what();
            if ( g_pASTContext )
            {
                g_pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            }
            else
            {
                g_error = os.str();
            }
        }
    }

    virtual void runFinalAnalysis()
    {
        if ( !g_error.empty() )
        {
            std::ostringstream os;
            os << "Error in clang_plugin::runFinalAnalysis: " << g_error;
            g_pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
        }
        else if ( g_pSession )
        {
            try
            {
                g_pSession->runFinalAnalysis();
            }
            catch ( std::exception& ex )
            {
                std::ostringstream os;
                os << "Error in clang_plugin::runFinalAnalysis: " << ex.what();
                g_pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            }
        }
    }

    virtual bool isEGEnabled()
    {
        if ( g_pSession )
        {
            return g_bMegaEnabled;
        }
        else
        {
            return false;
        }
    }

    virtual bool isEGType( const clang::QualType& type )
    {
        if ( g_pSession )
        {
            if ( clang::getEGSymbolID( g_pSession->getASTContext(), type ) )
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }

    virtual bool isPossibleEGType( const clang::QualType& type )
    {
        if ( type.getTypePtrOrNull() )
        {
            if ( type->isDependentType() )
            {
                return true;
            }
            else
            {
                if ( g_pSession )
                {
                    if ( clang::getEGSymbolID( g_pSession->getASTContext(), type ) )
                        return true;
                    else
                        return false;
                }
                else
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }

    virtual bool isPossibleEGTypeIdentifier( const clang::Token& token )
    {
        if ( g_pSession )
        {
            if ( token.is( clang::tok::identifier ) )
            {
                const clang::IdentifierInfo* pIdentifierInfo = token.getIdentifierInfo();
                return g_pSession->isPossibleEGTypeIdentifier( pIdentifierInfo->getName().str() );
            }
            else if ( token.is( clang::tok::annot_template_id ) )
            {
                // const clang::TemplateIdAnnotation* Annot
                //     = static_cast< const clang::TemplateIdAnnotation* >( token.getAnnotationValue() );
            }
            else if ( token.is( clang::tok::annot_typename ) )
            {
                clang::ParsedType parsedType = clang::ParsedType::getFromOpaquePtr( token.getAnnotationValue() );
                if ( g_pSession )
                {
                    return isPossibleEGType( g_pSession->getSema()->GetTypeFromParser( parsedType ) );
                }
                else
                {
                    return false;
                }
            }
        }

        return false;
    }

    virtual int isPossibleEGTypeIdentifierDecl( const clang::Token& token, bool bIsTypePathParsing )
    {
        if ( isPossibleEGTypeIdentifier( token ) )
        {
            if ( !bIsTypePathParsing )
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 2; // ambiguous
        }
    }

    virtual bool getInvocationOperationType( const clang::SourceLocation& loc, const clang::QualType& typePathType,
                                             bool bHasArguments, clang::QualType& operationType )
    {
        // determine the operation type...
        if ( g_pSession )
        {
            if ( const clang::IdentifierInfo* pIdentifierInfo
                 = clang::getOperationID( g_pSession->getASTContext(), typePathType, bHasArguments ) )
            {
                std::cout << "getInvocationOperationType: " << pIdentifierInfo->getName().str() << std::endl;
                // return the type
                clang::TagDecl* operatorClassDecl = nullptr;
                {
                    clang::LookupResult Result( *g_pSession->getSema(), pIdentifierInfo, clang::SourceLocation(),
                                                clang::Sema::LookupOrdinaryName );
                    if ( g_pSession->getSema()->LookupName( Result, g_pSession->getSema()->getCurScope() ) )
                    {
                        operatorClassDecl = llvm::dyn_cast< clang::TagDecl >( Result.getFoundDecl() );
                    }
                }
                if ( operatorClassDecl )
                {
                    operationType = g_pSession->getASTContext()->getTagDeclType( operatorClassDecl );
                    return true;
                }
                else
                {
                    // diagnostic
                }
            }
            else
            {
            }
        }
        return false;
    }

    // warn_eg_generic_warning
    // err_eg_generic_error
    virtual bool getInvocationResultType( const clang::SourceLocation& loc, const clang::QualType& type,
                                          clang::QualType& resultType )
    {
        if ( g_pSession )
        {
            return g_pSession->getInvocationResultType( loc, type, resultType );
        }

        return false;
    }
};
} // namespace mega

void* GET_EG_PLUGIN_INTERFACE()
{
    static ::mega::EG_PLUGIN_INTERFACE_IMPL impl;
    return &impl;
}
