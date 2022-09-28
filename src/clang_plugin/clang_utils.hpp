

#ifndef CLANG_UTILS_19_04_2019
#define CLANG_UTILS_19_04_2019

#include "mega/common.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"
#include "common/requireSemicolon.hpp"

#include <boost/current_function.hpp>

#include "clang/AST/Type.h"
#include "clang/Sema/Sema.h"

#pragma warning( pop )

#include <optional>
#include <iostream>

//#define CLANG_PLUGIN_LOG( msg )                                                                              \
//    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::cout << "FILE " << __FILE__ << " FUNC: " << BOOST_CURRENT_FUNCTION \
//                                               << " LINE:" << __LINE__ << " MSG: " << msg << std::endl; )

#define CLANG_PLUGIN_LOG( msg )

namespace clang
{
std::optional< mega::TypeID > getEGSymbolID( ASTContext* pASTContext, QualType type );

const IdentifierInfo* getOperationID( ASTContext* pASTContext, QualType ty, bool bHasParameters );

bool getContextSymbolIDs( ASTContext* pASTContext, QualType contextType, std::vector< mega::SymbolID >& contextTypes );

bool getTypePathSymbolIDs( ASTContext* pASTContext, QualType typePath, std::vector< mega::SymbolID >& typePathTypes );

std::optional< ::mega::U64 > getConstant( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext,
                                          const SourceLocation& loc, const std::string& strConstantName );

QualType getVariantType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                         const std::vector< QualType >& typeParameters );

// QualType getIteratorType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
//     const clang::QualType& interfaceType, const char* pszIteratorTypeName );

QualType getIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                               const clang::QualType& interfaceType, const char* pszIteratorTypeName );

QualType getMultiIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                                    const clang::QualType& interfaceType, ::mega::U64 szTargetTypes,
                                    const char* pszIteratorTypeName );

QualType getVoidType( ASTContext* pASTContext );
QualType getBooleanType( ASTContext* pASTContext );
QualType getIntType( ASTContext* pASTContext );
QualType getUIntType( ASTContext* pASTContext );

QualType getTypeTrait( ASTContext* pASTContext, Sema* pSema, DeclContext*& pDeclContext, const SourceLocation& loc,
                       const std::string& strTypeName );
                       /*
QualType getType( ASTContext* pASTContext, Sema* pSema, const std::string& strTypeName, DeclContext*& pDeclContext,
                  SourceLocation& loc, bool bLast );*/

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