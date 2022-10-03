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



#ifndef SESSION_10_MAY_2022
#define SESSION_10_MAY_2022

#include "database/types/modes.hpp"
#include "database/types/operation.hpp"
#include "database/types/sources.hpp"

#include "database/common/environment_build.hpp"

#include "clang/AST/ASTContext.h"
#include "clang/Sema/Sema.h"

#include <boost/filesystem/path.hpp>

#include <memory>
#include <string>

namespace clang
{
class Session
{
protected:
    ASTContext* pASTContext = nullptr;
    Sema*       pSema       = nullptr;

public:
    using Ptr = std::unique_ptr< Session >;
    Session( ASTContext* pASTContext, Sema* pSema )
        : pASTContext( pASTContext )
        , pSema( pSema )
    {
    }
    virtual ~Session() {}

    void setContext( ASTContext* _pASTContext, Sema* _pSema )
    {
        pASTContext = _pASTContext;
        pSema       = _pSema;
    }

    ASTContext* getASTContext() const { return pASTContext; }
    Sema*       getSema() const { return pSema; }

    virtual bool isPossibleEGTypeIdentifier( const std::string& strIdentifier ) const { return false; }

    virtual void runFinalAnalysis()
    {
        // do nothing
    }

    virtual bool getInvocationResultType( const clang::SourceLocation& loc, const clang::QualType& type,
                                          clang::QualType& resultType )
    {
        THROW_RTE( "Unimplemented" );
    }
};

class AnalysisSession : public Session
{
protected:
    const boost::filesystem::path m_srcDir, m_buildDir;
    mega::compiler::Directories   m_directories;
    mega::io::BuildEnvironment    m_environment;
    const mega::io::megaFilePath  m_sourceFile;

public:
    using Ptr = std::unique_ptr< Session >;
    AnalysisSession( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir,
                     const char* strSourceFile )
        : Session( pASTContext, pSema )
        , m_srcDir( strSrcDir )
        , m_buildDir( strBuildDir )
        , m_directories{ m_srcDir, m_buildDir, "", "" }
        , m_environment( m_directories )
        , m_sourceFile( m_environment.megaFilePath_fromPath( boost::filesystem::path( strSourceFile ) ) )
    {
    }

    virtual bool isPossibleEGTypeIdentifier( const std::string& strIdentifier ) const
    {
        if ( mega::getOperationName( strIdentifier ) != mega::HIGHEST_OPERATION_TYPE )
        {
            return true;
        }

        return false;
    }
};

} // namespace clang

#endif // SESSION_10_MAY_2022
