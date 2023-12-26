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

#include "session.hpp"
#include "clang_utils.hpp"

#include "mega/common_strings.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "llvm/Support/Casting.h"

// #include "clang/Sema/Ownership.h"
// #include "clang/Sema/ParsedTemplate.h"
// #include "clang/Sema/Lookup.h"
#include "clang/AST/Type.h"
// #include "clang/Lex/Token.h"
#include "clang/Basic/DiagnosticParse.h"
// #include "clang/Basic/DiagnosticSema.h"

#pragma warning( pop )

#include <map>

namespace clang
{
// using namespace FinalStage;
// using namespace FinalStage::Interface;

class LibrarySession : public AnalysisSession
{
    // FinalStage::Database                            m_database;
    //std::map< std::string, Symbols::SymbolTypeID* > m_symbols;

public:
    LibrarySession( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir,
                   const char* strSourceFile )
        : AnalysisSession( pASTContext, pSema, strSrcDir, strBuildDir )
        //, m_database( m_environment, m_environment.megaFilePath_fromPath( boost::filesystem::path( strSourceFile ) ) )
    {
        // Symbols::SymbolTable* pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );
        // m_symbols                          = pSymbolTable->get_symbol_names();
    }

    virtual bool isPossibleEGTypeIdentifier( const std::string& strIdentifier ) const override
    {
        if( AnalysisSession::isPossibleEGTypeIdentifier( strIdentifier ) )
        {
            return true;
        }

        THROW_TODO;
        // if( m_symbols.find( strIdentifier ) != m_symbols.end() )
        //     return true;
        return false;
    }
};

Session::Ptr make_library_session( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir,
                                  const char* strSourceFile )
{
    return std::make_unique< LibrarySession >( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile );
}

} // namespace clang
