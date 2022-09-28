

#ifndef SESSION_10_MAY_2022
#define SESSION_10_MAY_2022

#include "database/types/modes.hpp"
#include "database/types/operation.hpp"
#include "database/types/sources.hpp"

#include "database/common/environment_build.hpp"

#include "clang/AST/ASTContext.h"
#include "clang/Sema/Sema.h"

#include "boost/filesystem/path.hpp"

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
