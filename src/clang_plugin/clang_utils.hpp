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

#ifndef CLANG_UTILS_19_04_2019
#define CLANG_UTILS_19_04_2019

#include "mega/values/compilation/interface/symbol_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"
#include "mega/values/compilation/invocation_id.hpp"

#include "common/requireSemicolon.hpp"

#include <boost/current_function.hpp>

#include "common/clang_warnings_begin.hpp"
#include "clang/AST/Type.h"
#include "clang/Sema/Sema.h"
#include "common/clang_warnings_end.hpp"

#include <optional>
#include <iostream>

// #define ENABLE_CLANG_PLUGIN_LOG 1

#ifdef ENABLE_CLANG_PLUGIN_LOG
#define CLANG_PLUGIN_LOG( msg )                                                                              \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::cout << "FILE " << __FILE__ << " FUNC: " << BOOST_CURRENT_FUNCTION \
                                               << " LINE:" << __LINE__ << " MSG: " << msg << std::endl; )
#else
#define CLANG_PLUGIN_LOG( msg )
#endif

namespace clang
{
/*
std::string getCanonicalTypeStr( QualType type );

std::optional< mega::interface::TypeID > getMegaTypeID( ASTContext* pASTContext, QualType type );



bool getSymbolVariant( ASTContext* pASTContext, QualType typePath, mega::InvocationID::SymbolIDArray& typePathTypes );

using SymbolID                      = mega::interface::SymbolID;
using SymbolIDVariant               = std::vector< SymbolID >;
using SymbolIDVariantSequence       = std::vector< SymbolIDVariant >;
using SymbolIDVariantSequenceVector = std::vector< SymbolIDVariantSequence >;
bool getSymbolIDVariantSequenceVector( ASTContext* pASTContext, QualType typePath,
                                   SymbolIDVariantSequenceVector& result );

std::optional< ::mega::U64 > getConstant( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext,
                                      const SourceLocation& loc, const std::string& strConstantName );

QualType getVariantType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                     const std::vector< QualType >& typeParameters );

QualType getVectorConstRefType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                            const QualType& valueType );

QualType getIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                           const clang::QualType& interfaceType, const char* pszIteratorTypeName );

QualType getMultiIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                                const clang::QualType& interfaceType, ::mega::U64 szTargetTypes,
                                const char* pszIteratorTypeName );

QualType getBooleanType( ASTContext* pASTContext );
QualType getIntType( ASTContext* pASTContext );
QualType getUIntType( ASTContext* pASTContext );

                   */

/*
QualType getType( ASTContext* pASTContext, Sema* pSema, const std::string& strTypeName, DeclContext*& pDeclContext,
SourceLocation& loc, bool bLast );*/

QualType makeVoidType( ASTContext* pASTContext );
QualType makeUIntType( ASTContext* pASTContext );
QualType makeMegaPointerType( ASTContext* pASTContext, Sema* pSema, const std::vector< mega::interface::TypeID >& typeIDs );

bool getContextTypeIDs( ASTContext* pASTContext, QualType contextType, mega::InvocationID::TypeIDArray& contextTypes );

bool getSymbolPath( ASTContext* pASTContext, QualType contextType, mega::InvocationID::SymbolIDArray& symbolPath );

const IdentifierInfo* getOperationID( ASTContext* pASTContext, QualType ty, bool bHasParameters );

QualType getTypeTrait( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, const SourceLocation& loc,
                       const std::string& strTypeName );

struct DeclLocType
{
    DeclContext*   pDeclContext = nullptr;
    SourceLocation loc;
    QualType       type;
};
DeclLocType getNestedDeclContext( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                                  const std::string& strIdentifier );

} // namespace clang

#endif // CLANG_UTILS_19_04_2019