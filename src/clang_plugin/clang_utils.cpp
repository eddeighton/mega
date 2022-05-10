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

#include "clang_utils.hpp"

#include "database/types/operation.hpp"

#include "mega/common_strings.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/Sema/Ownership.h"
#include "clang/Sema/ParsedTemplate.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/Lookup.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Type.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/Lex/Token.h"
#include "clang/Basic/DiagnosticParse.h"

#include "llvm/ADT/APSInt.h"

#pragma warning( pop )

#include <iostream>

namespace clang
{

std::optional< mega::TypeID > getEGTypeID( ASTContext* pASTContext, QualType type )
{
    std::optional< mega::TypeID > result;

    if ( type.getTypePtrOrNull() && !type->isDependentType() )
    {
        QualType canonicalType = type.getCanonicalType();

        if ( const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier() )
        {
            if ( pBaseTypeID == pASTContext->getEGTypePathName() )
            {
                return mega::id_TypePath;
            }
        }

        if ( const CXXRecordDecl* pRecordDecl = canonicalType->getAsCXXRecordDecl() )
        {
            if ( pRecordDecl->hasAttr< EGTypeIDAttr >() )
            {
                if ( EGTypeIDAttr* pAttr = pRecordDecl->getAttr< EGTypeIDAttr >() )
                {
                    return pAttr->getId();
                }
            }
        }

        /*
            if( canonicalType->hasAttr< EGTypeIDAttr >() )
            {
                if( EGTypeIDAttr* pAttr = canonicalType->getAttr< EGTypeIDAttr >() )
                {
                    return pAttr->getId();
                }
            }
            */

        /*
        if( const AttributedType* pAttributedType = canonicalType->getAs< AttributedType >() )
        {
            if( pAttributedType->getAttrKind() == ParsedAttr::AT_EGTypeID )
            {

            }
        }*/
    }

    return result;
}

const IdentifierInfo* getOperationIdentifier( ASTContext* pASTContext, const std::string& strName )
{
    return &pASTContext->Idents.get( strName.c_str() );
}
const IdentifierInfo* getImplicitNoParamsOperation( ASTContext* pASTContext )
{
    return getOperationIdentifier( pASTContext, mega::getOperationString( mega::id_Imp_NoParams ) );
}
const IdentifierInfo* getImplicitParamsOperation( ASTContext* pASTContext )
{
    return getOperationIdentifier( pASTContext, mega::getOperationString( mega::id_Imp_Params ) );
}

const IdentifierInfo* getOperationID( ASTContext* pASTContext, QualType ty, bool bHasParameters )
{
    QualType              canonicalType = ty.getCanonicalType();
    const IdentifierInfo* pBaseTypeID   = canonicalType.getBaseTypeIdentifier();

    if ( !pBaseTypeID )
        return nullptr;

    if ( pBaseTypeID == pASTContext->getEGTypePathName() )
    {
        const Type* pType = canonicalType.getTypePtr();

        if ( const TemplateSpecializationType* pTemplateType = canonicalType->getAs< TemplateSpecializationType >() )
        {
            if ( pTemplateType->getNumArgs() == 0U )
                return nullptr;

            const TemplateArgument& lastTemplateArg = pTemplateType->getArg( pTemplateType->getNumArgs() - 1U );
            QualType                t               = lastTemplateArg.getAsType();
            return getOperationID( pASTContext, t, bHasParameters );
        }
        else if ( const DependentTemplateSpecializationType* pDependentTemplateType
                  = llvm::dyn_cast< const DependentTemplateSpecializationType >( pType ) )
        {
            if ( pTemplateType->getNumArgs() == 0U )
                return nullptr;
            const TemplateArgument& lastTemplateArg = pTemplateType->getArg( pTemplateType->getNumArgs() - 1U );
            QualType                t               = lastTemplateArg.getAsType();
            return getOperationID( pASTContext, t, bHasParameters );
        }
        else if ( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
        {
            const CXXRecordDecl* pRecordDecl = llvm::dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
            if ( !pRecordDecl )
                return nullptr;

            const auto* Spec = llvm::dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
            if ( !Spec )
                return nullptr;

            const TemplateArgumentList& Args = Spec->getTemplateInstantiationArgs();
            if ( Args.size() == 0U )
                return nullptr;

            const TemplateArgument& lastTemplateArg = Args[ Args.size() - 1U ];

            if ( lastTemplateArg.getKind() == TemplateArgument::Pack )
            {
                const TemplateArgument& lastTemplatePackArg = lastTemplateArg.pack_elements().back();
                QualType                t                   = lastTemplatePackArg.getAsType();
                return getOperationID( pASTContext, t, bHasParameters );
            }
            else if ( lastTemplateArg.getKind() == TemplateArgument::Type )
            {
                QualType t = lastTemplateArg.getAsType();
                return getOperationID( pASTContext, t, bHasParameters );
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }
    }

    if ( ::mega::getOperationName( pBaseTypeID->getName().str() ) != ::mega::HIGHEST_OPERATION_TYPE )
    {
        return getOperationIdentifier( pASTContext, pBaseTypeID->getName().str() );
    }
    else if ( bHasParameters )
    {
        return getImplicitParamsOperation( pASTContext );
    }
    else
    {
        return getImplicitNoParamsOperation( pASTContext );
    }

    return nullptr;
}

bool getContextTypes( ASTContext* pASTContext, QualType contextType, std::vector< mega::TypeID >& contextTypes )
{
    QualType canonicalType = contextType.getCanonicalType();
    if ( const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier() )
    {
        if ( pBaseTypeID == pASTContext->getEGVariantName() )
        {
            const Type* pType = canonicalType.getTypePtr();

            if ( const TemplateSpecializationType* pTemplateType
                 = canonicalType->getAs< TemplateSpecializationType >() )
            {
                if ( pTemplateType->getNumArgs() == 0U )
                    return false;

                bool bSuccess = false;
                for ( TemplateSpecializationType::iterator pIter    = pTemplateType->begin(),
                                                           pIterEnd = pTemplateType->end();
                      pIter != pIterEnd;
                      ++pIter )
                {
                    if ( !getContextTypes( pASTContext, pIter->getAsType(), contextTypes ) )
                        return false;
                    else
                        bSuccess = true;
                }
                return bSuccess;
            }
            else if ( const DependentTemplateSpecializationType* pDependentTemplateType
                      = llvm::dyn_cast< const DependentTemplateSpecializationType >( pType ) )
            {
                return false;
            }
            else if ( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
            {
                const CXXRecordDecl* pRecordDecl = llvm::dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
                if ( !pRecordDecl )
                    return false;

                const auto* Spec = llvm::dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
                if ( !Spec )
                    return false;

                bool                        bSuccess = false;
                const TemplateArgumentList& Args     = Spec->getTemplateInstantiationArgs();
                for ( unsigned i = 0; i < Args.size(); ++i )
                {
                    const TemplateArgument& arg = Args[ i ];
                    if ( arg.getKind() == TemplateArgument::Pack )
                    {
                        for ( TemplateArgument::pack_iterator j = arg.pack_begin(), jEnd = arg.pack_end(); j != jEnd;
                              ++j )
                        {
                            const TemplateArgument& packArg = *j;
                            if ( !getContextTypes( pASTContext, packArg.getAsType(), contextTypes ) )
                                return false;
                            else
                                bSuccess = true;
                        }
                    }
                    else if ( arg.getKind() == TemplateArgument::Type )
                    {
                        if ( !getContextTypes( pASTContext, arg.getAsType(), contextTypes ) )
                            return false;
                        else
                            bSuccess = true;
                    }
                    else
                    {
                        return false;
                    }
                }
                return bSuccess;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if ( std::optional< mega::TypeID > egTypeID = getEGTypeID( pASTContext, canonicalType ) )
            {
                contextTypes.push_back( egTypeID.value() );
                return true;
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

bool getTypePathTypes( ASTContext* pASTContext, QualType typePath, std::vector< mega::TypeID >& typePathTypes )
{
    QualType              canonicalType = typePath.getCanonicalType();
    const IdentifierInfo* pBaseTypeID   = canonicalType.getBaseTypeIdentifier();
    if ( !pBaseTypeID )
        return false;

    if ( pBaseTypeID == pASTContext->getEGTypePathName() )
    {
        const Type* pType = canonicalType.getTypePtr();

        if ( const TemplateSpecializationType* pTemplateType = canonicalType->getAs< TemplateSpecializationType >() )
        {
            if ( pTemplateType->getNumArgs() == 0U )
                return false;

            bool bSuccess = false;
            for ( TemplateSpecializationType::iterator pIter = pTemplateType->begin(), pIterEnd = pTemplateType->end();
                  pIter != pIterEnd; ++pIter )
            {
                if ( !getTypePathTypes( pASTContext, pIter->getAsType(), typePathTypes ) )
                    return false;
                else
                    bSuccess = true;
            }
            return bSuccess;
        }
        else if ( const DependentTemplateSpecializationType* pDependentTemplateType
                  = dyn_cast< const DependentTemplateSpecializationType >( pType ) )
        {
            return false;
        }
        else if ( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
        {
            const CXXRecordDecl* pRecordDecl = dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
            if ( !pRecordDecl )
                return false;

            const auto* Spec = dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
            if ( !Spec )
                return false;

            bool                        bSuccess = false;
            const TemplateArgumentList& Args     = Spec->getTemplateInstantiationArgs();
            for ( unsigned i = 0; i < Args.size(); ++i )
            {
                const TemplateArgument& arg = Args[ i ];
                if ( arg.getKind() == TemplateArgument::Pack )
                {
                    for ( TemplateArgument::pack_iterator j = arg.pack_begin(), jEnd = arg.pack_end(); j != jEnd; ++j )
                    {
                        const TemplateArgument& packArg = *j;
                        if ( !getTypePathTypes( pASTContext, packArg.getAsType(), typePathTypes ) )
                            return false;
                        else
                            bSuccess = true;
                    }
                }
                else if ( arg.getKind() == TemplateArgument::Type )
                {
                    if ( !getTypePathTypes( pASTContext, arg.getAsType(), typePathTypes ) )
                        return false;
                    else
                        bSuccess = true;
                }
                else
                {
                    return false;
                }
            }
            return bSuccess;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if ( std::optional< mega::TypeID > egTypeID = getEGTypeID( pASTContext, canonicalType ) )
        {
            typePathTypes.push_back( egTypeID.value() );
            return true;
        }
        else
        {
            return false;
        }
    }
}

std::optional< std::size_t > getConstant( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext,
                                          const SourceLocation& loc, const std::string& strConstantName )
{
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( strConstantName );
    LookupResult    lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
    if ( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
    {
        NamedDecl* pDecl = lookupResult.getFoundDecl();
        if ( VarDecl* pVarDecl = dyn_cast< VarDecl >( pDecl ) )
        {
            if ( const clang::Expr* pInitialisationExpr = pVarDecl->getInit() )
            {
                clang::Expr::EvalResult result;
                if ( pInitialisationExpr->EvaluateAsInt( result, *pASTContext ) )
                {
                    return result.Val.getInt().getExtValue();
                }
            }
        }
    }
    return std::optional< std::size_t >();
}

QualType getVariantType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                         const std::vector< QualType >& typeParameters )
{
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( mega::EG_VARIANT_TYPE );
    LookupResult    result( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
    if ( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if ( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
        {
            loc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( loc, loc );

            for ( QualType qt : typeParameters )
            {
                TemplateArgs.addArgument(
                    TemplateArgumentLoc( TemplateArgument( qt ), pASTContext->getTrivialTypeSourceInfo( qt, loc ) ) );
            }

            TemplateName templateName( pDecl );
            return pSema->CheckTemplateIdType( templateName, loc, TemplateArgs );
        }
    }
    return QualType();
}

QualType getIteratorType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                          const clang::QualType& interfaceType, const char* pszIteratorTypeName )
{
    SourceLocation  iterLoc;
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( pszIteratorTypeName );
    LookupResult    result( *pSema, &identifierInfo, iterLoc, Sema::LookupAnyName );
    if ( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if ( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
        {
            iterLoc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( iterLoc, iterLoc );

            TemplateArgumentLocInfo tali;
            {
                TemplateArgs.addArgument( TemplateArgumentLoc(
                    TemplateArgument( interfaceType ), pASTContext->getTrivialTypeSourceInfo( interfaceType, loc ) ) );
            }

            TemplateName templateName( pDecl );
            return pSema->CheckTemplateIdType( templateName, iterLoc, TemplateArgs );
        }
    }
    return QualType();
}

namespace mega
{
static const char* EG_RANGE_TYPE          = "__eg_Range";
static const char* EG_MULTI_ITERATOR_TYPE = "__eg_MultiIterator";
} // namespace mega

QualType getIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                               const clang::QualType& interfaceType, const char* pszIteratorTypeName )
{
    QualType iteratorType
        = getIteratorType( pASTContext, pSema, pDeclContext, loc, interfaceType, pszIteratorTypeName );

    SourceLocation  rangeLoc;
    IdentifierInfo& rangeIdentifierInfo = pASTContext->Idents.get( mega::EG_RANGE_TYPE );
    LookupResult    result( *pSema, &rangeIdentifierInfo, rangeLoc, Sema::LookupAnyName );
    if ( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if ( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
        {
            rangeLoc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( rangeLoc, rangeLoc );

            TemplateArgs.addArgument( TemplateArgumentLoc(
                TemplateArgument( iteratorType ), pASTContext->getTrivialTypeSourceInfo( iteratorType, loc ) ) );
            TemplateName templateName( pDecl );
            return pSema->CheckTemplateIdType( templateName, rangeLoc, TemplateArgs );
        }
    }

    return QualType();
}

QualType getMultiIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                                    const clang::QualType& interfaceType, std::size_t szTargetTypes,
                                    const char* pszIteratorTypeName )
{
    QualType iteratorType
        = getIteratorType( pASTContext, pSema, pDeclContext, loc, interfaceType, pszIteratorTypeName );

    SourceLocation  iterLoc;
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( mega::EG_MULTI_ITERATOR_TYPE );
    LookupResult    result( *pSema, &identifierInfo, iterLoc, Sema::LookupAnyName );
    if ( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if ( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
        {
            iterLoc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( iterLoc, iterLoc );

            TemplateArgumentLocInfo tali;
            {
                TemplateArgs.addArgument( TemplateArgumentLoc(
                    TemplateArgument( iteratorType ), pASTContext->getTrivialTypeSourceInfo( iteratorType, loc ) ) );
            }
            {
                const llvm::APInt  actualValue( 8U * 4U, static_cast< uint64_t >( szTargetTypes ), false );
                const llvm::APSInt compileTimeIntValue( actualValue );

                TemplateArgumentLocInfo tali;
                TemplateArgs.addArgument( TemplateArgumentLoc(
                    TemplateArgument( *pASTContext, compileTimeIntValue, getUIntType( pASTContext ) ), tali ) );
            }

            TemplateName templateName( pDecl );
            QualType     multiIteratorType = pSema->CheckTemplateIdType( templateName, iterLoc, TemplateArgs );

            SourceLocation  rangeLoc;
            IdentifierInfo& rangeIdentifierInfo = pASTContext->Idents.get( mega::EG_RANGE_TYPE );
            LookupResult    result( *pSema, &rangeIdentifierInfo, rangeLoc, Sema::LookupAnyName );
            if ( pSema->LookupQualifiedName( result, pDeclContext ) )
            {
                if ( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
                {
                    rangeLoc = pDecl->getTemplatedDecl()->getBeginLoc();
                    TemplateArgumentListInfo TemplateArgs( rangeLoc, rangeLoc );

                    TemplateArgs.addArgument(
                        TemplateArgumentLoc( TemplateArgument( multiIteratorType ),
                                             pASTContext->getTrivialTypeSourceInfo( multiIteratorType, loc ) ) );
                    TemplateName templateName( pDecl );
                    return pSema->CheckTemplateIdType( templateName, rangeLoc, TemplateArgs );
                }
            }
        }
    }
    return QualType();
}

QualType getVoidType( ASTContext* pASTContext ) { return pASTContext->VoidTy; }
QualType getBooleanType( ASTContext* pASTContext ) { return pASTContext->BoolTy; }
QualType getIntType( ASTContext* pASTContext ) { return pASTContext->IntTy; }
QualType getUIntType( ASTContext* pASTContext ) { return pASTContext->UnsignedIntTy; }

QualType getTypeTrait( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, const SourceLocation& loc,
                       const std::string& strTypeName )
{
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( strTypeName );
    LookupResult    lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
    if ( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
    {
        if ( TypeAliasDecl* pTypeAliasDecl = llvm::dyn_cast< TypeAliasDecl >( lookupResult.getFoundDecl() ) )
        {
            return pASTContext->getTypeDeclType( pTypeAliasDecl );
        }
    }
    pDeclContext = nullptr;
    return QualType();
}

QualType getType( ASTContext* pASTContext, Sema* pSema, const std::string& strTypeName, const std::string& strTypeParam,
                  DeclContext*& pDeclContext, SourceLocation& loc, bool bLast )
{
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( strTypeName );
    LookupResult    Result( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
    if ( pSema->LookupQualifiedName( Result, pDeclContext ) )
    {
        if ( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( Result.getFoundDecl() ) )
        {
            loc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( loc, loc );
            TemplateArgs.addArgument(
                TemplateArgumentLoc( TemplateArgument( pASTContext->VoidTy ),
                                     pASTContext->getTrivialTypeSourceInfo( pASTContext->VoidTy, loc ) ) );

            TemplateName templateName( pDecl );
            QualType     templateSpecializationType = pSema->CheckTemplateIdType( templateName, loc, TemplateArgs );
            if ( bLast )
            {
                return templateSpecializationType;
            }
            else
            {
                void*                              InsertPos = nullptr;
                SmallVector< TemplateArgument, 4 > Converted;
                Converted.push_back( TemplateArgument( pASTContext->VoidTy ) );
                if ( ClassTemplateSpecializationDecl* Decl = pDecl->findSpecialization( Converted, InsertPos ) )
                {
                    pDeclContext = Decl;
                    return templateSpecializationType;
                }
            }
        }
    }
    pDeclContext = nullptr;
    return QualType();
}

DeclLocType getNestedDeclContext( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                                  const std::string& str, bool bIsTemplate )
{
    DeclLocType result;

    IdentifierInfo& identifierInfo = pASTContext->Idents.get( str );
    LookupResult    lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
    if ( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
    {
        if ( bIsTemplate )
        {
            ClassTemplateDecl* pDecl = dyn_cast< ClassTemplateDecl >( lookupResult.getFoundDecl() );
            result.loc               = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( result.loc, result.loc );
            TemplateArgs.addArgument(
                TemplateArgumentLoc( TemplateArgument( pASTContext->VoidTy ),
                                     pASTContext->getTrivialTypeSourceInfo( pASTContext->VoidTy, result.loc ) ) );

            TemplateName templateName( pDecl );
            result.type = pSema->CheckTemplateIdType( templateName, result.loc, TemplateArgs );

            void*                              InsertPos = nullptr;
            SmallVector< TemplateArgument, 4 > Converted;
            Converted.push_back( TemplateArgument( pASTContext->VoidTy ) );
            if ( ClassTemplateSpecializationDecl* pClassSpecialisationDeclaration
                 = pDecl->findSpecialization( Converted, InsertPos ) )
            {
                result.pDeclContext = pClassSpecialisationDeclaration;
            }
        }
        else
        {
            if ( CXXRecordDecl* pRecordDecl = dyn_cast< CXXRecordDecl >( lookupResult.getFoundDecl() ) )
            {
                result.pDeclContext = pRecordDecl;
                result.loc          = pRecordDecl->getBeginLoc();
                result.type         = pASTContext->getTypeDeclType( pRecordDecl );
            }
        }
    }
    return result;
}

} // namespace clang
