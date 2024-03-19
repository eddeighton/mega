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

#include "clang_utils.hpp"

#include "mega/values/compilation/reserved_symbols.hpp"

#include "mega/common_strings.hpp"

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <boost/algorithm/string.hpp>

#include "common/clang_warnings_begin.hpp"

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

#include "common/clang_warnings_end.hpp"

namespace clang
{
/*
bool isVariant( ASTContext* pASTContext, QualType typePath )
{
    if( const IdentifierInfo* pBaseTypeID = typePath.getBaseTypeIdentifier() )
    {
        return pBaseTypeID == pASTContext->getEGVariantName();
    }
    return false;
}

bool isTypePath( ASTContext* pASTContext, QualType typePath )
{
    if( const IdentifierInfo* pBaseTypeID = typePath.getBaseTypeIdentifier() )
    {
        return pBaseTypeID == pASTContext->getEGSymbolPathName();
    }
    return false;
}

std::string getCanonicalTypeStr( QualType type )
{
    std::string strCanonicalType = type.getCanonicalType().getAsString();
    boost::replace_all( strCanonicalType, "_Bool", "bool" );
    return strCanonicalType;
}

std::optional< mega::interface::TypeID > getMegaTypeID( ASTContext* pASTContext, QualType type )
{
    if( type.getTypePtrOrNull() && !type->isDependentType() )
    {
        QualType canonicalType = type.getCanonicalType();

        if( const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier() )
        {
            if( pBaseTypeID == pASTContext->getEGSymbolPathName() )
            {
                //return mega::interface::TypeID( mega::id_SymbolPath );
            }
        }

        if( const CXXRecordDecl* pRecordDecl = canonicalType->getAsCXXRecordDecl() )
        {
            if( pRecordDecl->hasAttr< EGTypeIDAttr >() )
            {
                if( EGTypeIDAttr* pAttr = pRecordDecl->getAttr< EGTypeIDAttr >() )
                {
                    return mega::interface::TypeID( static_cast< mega::interface::TypeID::ValueType >( pAttr->getId() )
);
                }
            }
        }

        // if( canonicalType->hasAttr< EGTypeIDAttr >() )
        // {
        //     if( EGTypeIDAttr* pAttr = canonicalType->getAttr< EGTypeIDAttr >() )
        //     {
        //         return pAttr->getId();
        //     }
        // }
        // if( const AttributedType* pAttributedType = canonicalType->getAs< AttributedType >() )
        // {
        //     if( pAttributedType->getAttrKind() == ParsedAttr::AT_EGTypeID )
        //     {
        //     }
        // }
    }

    // CLANG_PLUGIN_LOG( "No symbol id for: "  << type.getAsString() );

    return std::optional< mega::interface::TypeID >();
}




bool getSymbol( ASTContext* pASTContext, QualType typePath, SymbolID& symbol )
{
    QualType              canonicalType = typePath.getCanonicalType();
    const IdentifierInfo* pBaseTypeID   = canonicalType.getBaseTypeIdentifier();
    if( !pBaseTypeID )
        return false;

    if( auto typeIDOpt = getMegaTypeID( pASTContext, canonicalType ) )
    {
        THROW_TODO;
        // symbol = typeIDOpt.value();
        return true;
    }
    else
    {
        return false;
    }
}

bool getSymbolVariant( ASTContext* pASTContext, QualType typePath, SymbolIDVariant& symbolVariant )
{
    QualType              canonicalType = typePath.getCanonicalType();
    const IdentifierInfo* pBaseTypeID   = canonicalType.getBaseTypeIdentifier();
    if( !pBaseTypeID )
        return false;

    if( pBaseTypeID == pASTContext->getEGSymbolPathName() )
    {
        const Type* pType = canonicalType.getTypePtr();

        if( const TemplateSpecializationType* pTemplateType = canonicalType->getAs< TemplateSpecializationType >() )
        {
            if( pTemplateType->getNumArgs() == 0U )
                return false;

            bool bSuccess = false;
            for( TemplateSpecializationType::iterator pIter = pTemplateType->begin(), pIterEnd = pTemplateType->end();
                 pIter != pIterEnd; ++pIter )
            {
                SymbolID symbolID;
                if( getSymbol( pASTContext, pIter->getAsType(), symbolID ) )
                {
                    symbolVariant.push_back( symbolID );
                    bSuccess = true;
                }
                else
                {
                    return false;
                }
            }
            return bSuccess;
        }
        else if( auto pDependentTemplateType = dyn_cast< const DependentTemplateSpecializationType >( pType ) )
        {
            return false;
        }
        else if( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
        {
            const CXXRecordDecl* pRecordDecl = dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
            if( !pRecordDecl )
                return false;

            const auto* Spec = dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
            if( !Spec )
                return false;

            bool                        bSuccess = false;
            const TemplateArgumentList& Args     = Spec->getTemplateInstantiationArgs();
            for( unsigned i = 0; i < Args.size(); ++i )
            {
                const TemplateArgument& arg = Args[ i ];
                if( arg.getKind() == TemplateArgument::Pack )
                {
                    for( TemplateArgument::pack_iterator j = arg.pack_begin(), jEnd = arg.pack_end(); j != jEnd; ++j )
                    {
                        const TemplateArgument& packArg = *j;

                        SymbolID symbolID;
                        if( getSymbol( pASTContext, packArg.getAsType(), symbolID ) )
                        {
                            symbolVariant.push_back( symbolID );
                            bSuccess = true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else if( arg.getKind() == TemplateArgument::Type )
                {
                    SymbolID symbolID;
                    if( getSymbol( pASTContext, arg.getAsType(), symbolID ) )
                    {
                        symbolVariant.push_back( symbolID );
                        bSuccess = true;
                    }
                    else
                    {
                        return false;
                    }
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
        SymbolID symbolID;
        if( getSymbol( pASTContext, canonicalType, symbolID ) )
        {
            symbolVariant.push_back( symbolID );
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool getSymbolVariantSequence( ASTContext* pASTContext, QualType typePath,
                               SymbolIDVariantSequence& symbolIDVariantSequence )
{
    if( isTypePath( pASTContext, typePath ) )
    {
        if( const TemplateSpecializationType* pTemplateType = typePath->getAs< TemplateSpecializationType >() )
        {
            if( pTemplateType->getNumArgs() == 0U )
                return false;

            for( TemplateSpecializationType::iterator pIter = pTemplateType->begin(), pIterEnd = pTemplateType->end();
                 pIter != pIterEnd; ++pIter )
            {
                SymbolIDVariant symbolVariant;
                if( !getSymbolVariant( pASTContext, pIter->getAsType(), symbolVariant ) )
                    return false;
                symbolIDVariantSequence.emplace_back( std::move( symbolVariant ) );
            }
            return true;
        }
        else if( auto pDependentTemplateType
                 = dyn_cast< const DependentTemplateSpecializationType >( typePath.getTypePtr() ) )
        {
            return false;
        }
        else if( const RecordType* pRecordType = typePath->getAs< RecordType >() )
        {
            if( const CXXRecordDecl* pRecordDecl = dyn_cast< CXXRecordDecl >( pRecordType->getDecl() ) )
            {
                if( const auto* Spec = dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl ) )
                {
                    bool                        bSuccess = false;
                    const TemplateArgumentList& Args     = Spec->getTemplateInstantiationArgs();
                    for( unsigned i = 0; i < Args.size(); ++i )
                    {
                        const TemplateArgument& arg = Args[ i ];

                        if( arg.getKind() == TemplateArgument::Pack )
                        {
                            for( TemplateArgument::pack_iterator j = arg.pack_begin(), jEnd = arg.pack_end(); j != jEnd;
                                 ++j )
                            {
                                const TemplateArgument& packArg = *j;
                                SymbolIDVariant         symbolVariant;
                                if( !getSymbolVariant(
                                        pASTContext, packArg.getAsType().getCanonicalType(), symbolVariant ) )
                                {
                                    return false;
                                }
                                symbolIDVariantSequence.emplace_back( std::move( symbolVariant ) );
                            }
                        }
                        else if( arg.getKind() == TemplateArgument::Type )
                        {
                            SymbolIDVariant symbolVariant;
                            if( !getSymbolVariant( pASTContext, arg.getAsType().getCanonicalType(), symbolVariant ) )
                            {
                                return false;
                            }
                            symbolIDVariantSequence.emplace_back( std::move( symbolVariant ) );
                        }
                        else
                        {
                            return false;
                        }
                    }
                    return true;
                }
            }
        }
    }
    else
    {
        // std::vector< mega::interface::TypeID > typePathTypes;
        // if( !getSymbolVariant( pASTContext, typePath, typePathTypes ) )
        //     return false;
        // symbolIDVariantSequence.emplace_back( std::move( typePathTypes ) );
        return false;
    }
    return false;
}

bool getSymbolIDVariantSequenceVector( ASTContext* pASTContext, QualType typePath,
                                       SymbolIDVariantSequenceVector& symbolIDVariantSequenceVector )
{
    QualType              canonicalType = typePath.getCanonicalType();
    const IdentifierInfo* pBaseTypeID   = canonicalType.getBaseTypeIdentifier();
    if( !pBaseTypeID )
        return false;

    if( pBaseTypeID == pASTContext->getEGSymbolPathName() )
    {
        if( const TemplateSpecializationType* pTemplateType = canonicalType->getAs< TemplateSpecializationType >() )
        {
            for( TemplateSpecializationType::iterator pOuterTypePathIter    = pTemplateType->begin(),
                                                      pOuterTypePathIterEnd = pTemplateType->end();
                 pOuterTypePathIter != pOuterTypePathIterEnd;
                 ++pOuterTypePathIter )
            {
                SymbolIDVariantSequence symbolIDVariantPath;
                if( !getSymbolVariantSequence(
                        pASTContext, pOuterTypePathIter->getAsType().getCanonicalType(), symbolIDVariantPath ) )
                {
                    return false;
                }
                symbolIDVariantSequenceVector.emplace_back( std::move( symbolIDVariantPath ) );
            }
            return true;
        }
        else if( auto pDependentTemplateType
                 = dyn_cast< const DependentTemplateSpecializationType >( canonicalType.getTypePtr() ) )
        {
            return false;
        }
        else if( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
        {
            const CXXRecordDecl* pRecordDecl = dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
            if( !pRecordDecl )
                return false;

            const auto* Spec = dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
            if( !Spec )
                return false;

            {
                bool                        bSuccess = false;
                const TemplateArgumentList& Args     = Spec->getTemplateInstantiationArgs();
                for( unsigned i = 0; i < Args.size(); ++i )
                {
                    const TemplateArgument& arg = Args[ i ];

                    if( arg.getKind() == TemplateArgument::Pack )
                    {
                        for( TemplateArgument::pack_iterator j = arg.pack_begin(), jEnd = arg.pack_end(); j != jEnd;
                             ++j )
                        {
                            const TemplateArgument& packArg = *j;
                            SymbolIDVariantSequence symbolIDVariantPath;
                            if( !getSymbolVariantSequence(
                                    pASTContext, packArg.getAsType().getCanonicalType(), symbolIDVariantPath ) )
                            {
                                return false;
                            }
                            symbolIDVariantSequenceVector.emplace_back( std::move( symbolIDVariantPath ) );
                        }
                    }
                    else if( arg.getKind() == TemplateArgument::Type )
                    {
                        SymbolIDVariantSequence symbolIDVariantPath;
                        if( !getSymbolVariantSequence(
                                pASTContext, arg.getAsType().getCanonicalType(), symbolIDVariantPath ) )
                        {
                            return false;
                        }
                        symbolIDVariantSequenceVector.emplace_back( std::move( symbolIDVariantPath ) );
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    return false;
}

std::optional< ::mega::U64 > getConstant( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext,
                                          const SourceLocation& loc, const std::string& strConstantName )
{
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( strConstantName );
    LookupResult    lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
    if( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
    {
        NamedDecl* pDecl = lookupResult.getFoundDecl();
        if( VarDecl* pVarDecl = dyn_cast< VarDecl >( pDecl ) )
        {
            if( const clang::Expr* pInitialisationExpr = pVarDecl->getInit() )
            {
                clang::Expr::EvalResult result;
                if( pInitialisationExpr->EvaluateAsInt( result, *pASTContext ) )
                {
                    return result.Val.getInt().getExtValue();
                }
            }
        }
    }
    return std::optional< ::mega::U64 >();
}

QualType getVariantType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                         const std::vector< QualType >& typeParameters )
{
    IdentifierInfo* pIdentifierInfo = pASTContext->getEGVariantName();
    LookupResult    result( *pSema, pIdentifierInfo, loc, Sema::LookupAnyName );
    if( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
        {
            loc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( loc, loc );

            for( QualType qt : typeParameters )
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

QualType getVectorConstRefType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                                const QualType& valueType )
{
    SourceLocation  iterLoc;
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( "__mega_vector" );
    LookupResult    result( *pSema, &identifierInfo, iterLoc, Sema::LookupAnyName );
    if( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
        {
            iterLoc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( iterLoc, iterLoc );

            TemplateArgumentLocInfo tali;
            {
                TemplateArgs.addArgument( TemplateArgumentLoc(
                    TemplateArgument( valueType ), pASTContext->getTrivialTypeSourceInfo( valueType, loc ) ) );
            }

            TemplateName templateName( pDecl );
            QualType     result = pSema->CheckTemplateIdType( templateName, iterLoc, TemplateArgs );
            return pASTContext->getLValueReferenceType( pASTContext->getConstType( result ) );
        }
        else if( TypeAliasTemplateDecl* pDecl = llvm::dyn_cast< TypeAliasTemplateDecl >( result.getFoundDecl() ) )
        {
            iterLoc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( iterLoc, iterLoc );

            TemplateArgumentLocInfo tali;
            {
                TemplateArgs.addArgument( TemplateArgumentLoc(
                    TemplateArgument( valueType ), pASTContext->getTrivialTypeSourceInfo( valueType, loc ) ) );
            }

            TemplateName templateName( pDecl );
            QualType     result = pSema->CheckTemplateIdType( templateName, iterLoc, TemplateArgs );
            return pASTContext->getLValueReferenceType( pASTContext->getConstType( result ) );
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
    if( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
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
} // namespace mega

QualType getIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                               const clang::QualType& interfaceType, const char* pszIteratorTypeName )
{
    QualType iteratorType
        = getIteratorType( pASTContext, pSema, pDeclContext, loc, interfaceType, pszIteratorTypeName );

    SourceLocation  rangeLoc;
    IdentifierInfo& rangeIdentifierInfo = pASTContext->Idents.get( mega::EG_RANGE_TYPE );
    LookupResult    result( *pSema, &rangeIdentifierInfo, rangeLoc, Sema::LookupAnyName );
    if( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
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
                                    const clang::QualType& interfaceType, ::mega::U64 szTargetTypes,
                                    const char* pszIteratorTypeName )
{
    QualType iteratorType
        = getIteratorType( pASTContext, pSema, pDeclContext, loc, interfaceType, pszIteratorTypeName );

    SourceLocation  iterLoc;
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( mega::EG_MULTI_ITERATOR_TYPE );
    LookupResult    result( *pSema, &identifierInfo, iterLoc, Sema::LookupAnyName );
    if( pSema->LookupQualifiedName( result, pDeclContext ) )
    {
        if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
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
            if( pSema->LookupQualifiedName( result, pDeclContext ) )
            {
                if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
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

QualType getBooleanType( ASTContext* pASTContext )
{
    return pASTContext->BoolTy;
}
QualType getIntType( ASTContext* pASTContext )
{
    return pASTContext->IntTy;
}

*/
/*
QualType getType( ASTContext* pASTContext, Sema* pSema, const std::string& strTypeName,
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
*/

QualType makeVoidType( ASTContext* pASTContext )
{
    return pASTContext->VoidTy;
}
QualType makeUIntType( ASTContext* pASTContext )
{
    return pASTContext->UnsignedIntTy;
}

QualType makeMegaPointerType( ASTContext* pASTContext, Sema* pSema,
                              const std::vector< mega::interface::TypeID >& typeIDs )
{
    IdentifierInfo* pIdentifierInfo = pASTContext->getEGPointerName();

    SourceLocation loc;
    LookupResult   result( *pSema, pIdentifierInfo, loc, Sema::LookupAnyName );

    if( pSema->LookupQualifiedName( result, pASTContext->getTranslationUnitDecl() ) )
    {
        if( auto pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
        {
            loc = pDecl->getTemplatedDecl()->getBeginLoc();
            TemplateArgumentListInfo TemplateArgs( loc, loc );

            for( const auto& typeID : typeIDs )
            {
                const llvm::APInt integerValue = llvm::APInt( 32, typeID.getValue() );

                // auto pExpr = IntegerLiteral::Create( *pASTContext, integerValue,
                //                                     pASTContext->UnsignedIntTy,
                //                                     loc );

                // ParsedTemplateArgument Arg(ParsedTemplateArgument::NonType, pExpr, loc );

                // if (Arg.isInvalid() ||
                //     (Arg.getKind() != ParsedTemplateArgument::NonType))
                // {
                //     return QualType();
                // }

                const llvm::APSInt intValue = llvm::APSInt( integerValue );
                TemplateArgument   templateArgument( *pASTContext, intValue, pASTContext->UnsignedIntTy );
                TemplateArgs.addArgument( TemplateArgumentLoc(
                    templateArgument, pASTContext->getTrivialTypeSourceInfo( pASTContext->UnsignedIntTy, loc ) ) );
            }

            // for( QualType qt : typeParameters )
            // {
            //     TemplateArgs.addArgument(
            //         TemplateArgumentLoc( TemplateArgument( qt ), pASTContext->getTrivialTypeSourceInfo( qt, loc ) )
            //         );
            // }

            TemplateName templateName( pDecl );
            return pSema->CheckTemplateIdType( templateName, loc, TemplateArgs );
        }
    }
    return {};
}

bool getContextTypeIDs( ASTContext* pASTContext, QualType contextType, mega::InvocationID::TypeIDArray& contextTypes )
{
    std::size_t szIndex = 0;

    QualType canonicalType = contextType.getCanonicalType();
    if( const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier() )
    {
        auto strName = pBaseTypeID->getName();
        if( pBaseTypeID == pASTContext->getEGPointerName() )
        {
            const Type* pType = canonicalType.getTypePtr();

            if( const TemplateSpecializationType* pTemplateType = canonicalType->getAs< TemplateSpecializationType >() )
            {
                if( pTemplateType->getNumArgs() == 0U )
                    return false;

                bool bSuccess = false;
                for( TemplateSpecializationType::iterator pIter    = pTemplateType->begin(),
                                                          pIterEnd = pTemplateType->end();
                     pIter != pIterEnd;
                     ++pIter )
                {
                    return false;
                }
                return bSuccess;
            }
            else if( auto pDependentTemplateType
                     = llvm::dyn_cast< const DependentTemplateSpecializationType >( pType ) )
            {
                return false;
            }
            else if( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
            {
                const CXXRecordDecl* pRecordDecl = llvm::dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
                if( !pRecordDecl )
                    return false;

                const auto* Spec = llvm::dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
                if( !Spec )
                    return false;

                bool                        bSuccess = false;
                const TemplateArgumentList& Args     = Spec->getTemplateInstantiationArgs();
                for( unsigned i = 0; i < Args.size(); ++i )
                {
                    const TemplateArgument& arg = Args[ i ];
                    if( arg.getKind() == TemplateArgument::Pack )
                    {
                        for( TemplateArgument::pack_iterator j = arg.pack_begin(), jEnd = arg.pack_end(); j != jEnd;
                             ++j )
                        {
                            const TemplateArgument& packArg = *j;

                            VERIFY_RTE_MSG(
                                szIndex < mega::InvocationID::MAX_ELEMENTS,
                                "More than " << mega::InvocationID::MAX_ELEMENTS << " elements in context" );

                            switch( packArg.getKind() )
                            {
                                case TemplateArgument::Integral:
                                {
                                    const auto value = static_cast< mega::interface::TypeID::ValueType >(
                                        packArg.getAsIntegral().getExtValue() );

                                    contextTypes[ szIndex ] = mega::interface::TypeID( value );

                                    ++szIndex;
                                    bSuccess = true;
                                }
                                break;
                                case TemplateArgument::Null:
                                case TemplateArgument::Type:
                                case TemplateArgument::Declaration:
                                case TemplateArgument::NullPtr:
                                case TemplateArgument::Template:
                                case TemplateArgument::TemplateExpansion:
                                case TemplateArgument::Expression:
                                case TemplateArgument::Pack:
                                {
                                    THROW_TODO;
                                }
                                break;
                            }
                        }
                    }
                    else if( arg.getKind() == TemplateArgument::Type )
                    {
                        return false;
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
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool getSymbolPath( ASTContext* pASTContext, QualType contextType, mega::InvocationID::SymbolIDArray& symbolPath )
{
    std::size_t szIndex = 0;

    QualType canonicalType = contextType.getCanonicalType();
    if( const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier() )
    {
        auto strName = pBaseTypeID->getName();
        if( pBaseTypeID == pASTContext->getEGSymbolPathName() )
        {
            const Type* pType = canonicalType.getTypePtr();

            if( const TemplateSpecializationType* pTemplateType = canonicalType->getAs< TemplateSpecializationType >() )
            {
                if( pTemplateType->getNumArgs() == 0U )
                    return false;

                bool bSuccess = false;
                for( TemplateSpecializationType::iterator pIter    = pTemplateType->begin(),
                                                          pIterEnd = pTemplateType->end();
                     pIter != pIterEnd;
                     ++pIter )
                {
                    THROW_TODO;
                }
                return bSuccess;
            }
            else if( auto pDependentTemplateType
                     = llvm::dyn_cast< const DependentTemplateSpecializationType >( pType ) )
            {
                return false;
            }
            else if( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
            {
                const CXXRecordDecl* pRecordDecl = llvm::dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
                if( !pRecordDecl )
                    return false;

                const auto* Spec = llvm::dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
                if( !Spec )
                    return false;

                bool                        bSuccess = false;
                const TemplateArgumentList& Args     = Spec->getTemplateInstantiationArgs();
                for( unsigned i = 0; i < Args.size(); ++i )
                {
                    const TemplateArgument& arg = Args[ i ];
                    if( arg.getKind() == TemplateArgument::Pack )
                    {
                        for( TemplateArgument::pack_iterator j = arg.pack_begin(), jEnd = arg.pack_end(); j != jEnd;
                             ++j )
                        {
                            const TemplateArgument& packArg = *j;

                            VERIFY_RTE_MSG(
                                szIndex < mega::InvocationID::MAX_ELEMENTS,
                                "More than " << mega::InvocationID::MAX_ELEMENTS << " elements in context" );

                            switch( packArg.getKind() )
                            {
                                case TemplateArgument::Integral:
                                {
                                    const auto value = static_cast< mega::interface::SymbolID::ValueType >(
                                        packArg.getAsIntegral().getExtValue() );

                                    symbolPath[ szIndex ] = mega::interface::SymbolID( value );

                                    ++szIndex;
                                    bSuccess = true;
                                }
                                break;
                                case TemplateArgument::Null:
                                case TemplateArgument::Type:
                                case TemplateArgument::Declaration:
                                case TemplateArgument::NullPtr:
                                case TemplateArgument::Template:
                                case TemplateArgument::TemplateExpansion:
                                case TemplateArgument::Expression:
                                case TemplateArgument::Pack:
                                {
                                    THROW_TODO;
                                }
                                break;
                            }
                        }
                    }
                    else if( arg.getKind() == TemplateArgument::Type )
                    {
                        THROW_TODO;
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
            THROW_TODO;
        }
    }
    else
    {
        return false;
    }
}

namespace
{
const IdentifierInfo* getOperationIdentifier( ASTContext* pASTContext, const std::string& strName )
{
    return &pASTContext->Idents.get( strName.c_str() );
}
const IdentifierInfo* getImplicitNoParamsOperation( ASTContext* pASTContext )
{
    return getOperationIdentifier(
        pASTContext, mega::interface::getReservedSymbol( mega::interface::OPERATION_IMP_NOPARAMS ) );
}
const IdentifierInfo* getImplicitParamsOperation( ASTContext* pASTContext )
{
    return getOperationIdentifier(
        pASTContext, mega::interface::getReservedSymbol( mega::interface::OPERATION_IMP_PARAMS ) );
}
} // namespace

const IdentifierInfo* getOperationID( ASTContext* pASTContext, QualType ty, bool bHasParameters )
{
    if( bHasParameters )
    {
        return getImplicitParamsOperation( pASTContext );
    }
    else
    {
        return getImplicitNoParamsOperation( pASTContext );
    }

    return nullptr;
}

QualType getTypeTrait( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, const SourceLocation& loc,
                       const std::string& strTypeName )
{
    IdentifierInfo& identifierInfo = pASTContext->Idents.get( strTypeName );
    LookupResult    lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
    if( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
    {
        if( auto pTypeAliasDecl = llvm::dyn_cast< TypeAliasDecl >( lookupResult.getFoundDecl() ) )
        {
            return pASTContext->getTypeDeclType( pTypeAliasDecl );
        }
    }
    return {};
}

DeclLocType getNestedDeclContext( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc,
                                  const std::string& strIdentifier )
{
    DeclLocType result;

    IdentifierInfo& identifierInfo = pASTContext->Idents.get( strIdentifier );
    LookupResult    lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
    if( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
    {
        if( NamedDecl* pNamedDecl = lookupResult.getFoundDecl() )
        {
            if( CXXRecordDecl* pRecordDecl = dyn_cast< CXXRecordDecl >( pNamedDecl ) )
            {
                result.pDeclContext = pRecordDecl;
                result.loc          = pRecordDecl->getBeginLoc();
                result.type         = pASTContext->getTypeDeclType( pRecordDecl );
            }
            else if( NamespaceDecl* pNamespaceDecl = dyn_cast< NamespaceDecl >( pNamedDecl ) )
            {
                result.pDeclContext = pNamespaceDecl;
                result.loc          = pNamespaceDecl->getBeginLoc();
            }
        }
    }
    return result;
}

} // namespace clang
