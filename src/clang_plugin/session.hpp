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

#include "common/clang_warnings.hpp"

#include "mega/values/compilation/modes.hpp"

#include "database/sources.hpp"

#include "environment/environment_build.hpp"

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
    virtual ~Session() = default;

    void setContext( ASTContext* _pASTContext, Sema* _pSema )
    {
        pASTContext = _pASTContext;
        pSema       = _pSema;
    }

    ASTContext* getASTContext() const { return pASTContext; }
    Sema*       getSema() const { return pSema; }

    virtual unsigned int getSymbolID( const std::string& strIdentifier ) const { return 0U; }

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

} // namespace clang

#endif // SESSION_10_MAY_2022
