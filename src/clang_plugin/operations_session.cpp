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

#include "database/model/OperationsStage.hxx"

#include "invocation/invocation.hpp"

#include "mega/invocation_io.hpp"
#include "mega/common_strings.hpp"
#include "mega/invocation_id.hpp"
#include "database/types/source_location.hpp"

#include "clang/Basic/SourceLocation.h"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "llvm/Support/Casting.h"

#include "clang/Sema/Ownership.h"
#include "clang/Sema/ParsedTemplate.h"
#include "clang/Sema/Lookup.h"
#include "clang/AST/Type.h"
#include "clang/Lex/Token.h"
#include "clang/Basic/DiagnosticParse.h"
#include "clang/Basic/DiagnosticSema.h"
#include "clang/Basic/SourceManager.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/ASTConsumer.h"

#include <iostream>

#pragma warning( pop )

namespace clang
{

class OperationsSession : public AnalysisSession
{
    OperationsStage::Database              m_database;
    OperationsStage::Symbols::SymbolTable* m_pSymbolTable;
    mega::invocation::SymbolTables         m_symbolTables;

    using InvocationsMap = std::map< mega::InvocationID, ::OperationsStage::Operations::Invocation* >;
    InvocationsMap m_invocationsMap;

    using SymbolMap = std::map< std::string, OperationsStage::Symbols::SymbolTypeID* >;
    SymbolMap m_symbols;

protected:
    bool m_bError = false;

public:
    using Ptr = std::unique_ptr< Session >;
    OperationsSession( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir,
                       const char* strSourceFile )
        : AnalysisSession( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile )
        , m_database( m_environment, m_sourceFile )
        , m_pSymbolTable( m_database.one< OperationsStage::Symbols::SymbolTable >( m_environment.project_manifest() ) )
        , m_symbolTables( m_pSymbolTable )
    {
        using namespace OperationsStage;
        using namespace OperationsStage::Interface;

        m_symbols = m_pSymbolTable->get_symbol_names();
    }

    virtual bool isPossibleEGTypeIdentifier( const std::string& strIdentifier ) const
    {
        if( mega::getOperationName( strIdentifier ) != mega::HIGHEST_OPERATION_TYPE )
        {
            return true;
        }

        if( m_symbols.find( strIdentifier ) != m_symbols.end() )
            return true;
        // std::cout << "isPossibleEGTypeIdentifier: " << strIdentifier << std::endl;
        return false;
    }

    void getRootPath( OperationsStage::Interface::ContextGroup*                 pContextGroup,
                      std::vector< OperationsStage::Interface::ContextGroup* >& path )
    {
        using namespace OperationsStage;
        Interface::ContextGroup* pIter = pContextGroup;
        while( pIter )
        {
            path.push_back( pIter );
            if( auto pContext = db_cast< Interface::IContext >( pIter ) )
            {
                pIter = pContext->get_parent();
            }
            else
            {
                pIter = nullptr;
            }
        }
        std::reverse( path.begin(), path.end() );
    }

    DeclLocType locateInterfaceContext( OperationsStage::Interface::IContext* pInterfaceContext )
    {
        using namespace OperationsStage;

        std::vector< Interface::ContextGroup* > path;
        getRootPath( pInterfaceContext, path );

        DeclLocType declLocType;
        declLocType.pDeclContext = pASTContext->getTranslationUnitDecl();

        for( Interface::ContextGroup* pContextGroup : path )
        {
            if( auto pContext = db_cast< Interface::IContext >( pContextGroup ) )
            {
                declLocType = clang::getNestedDeclContext(
                    pASTContext, pSema, declLocType.pDeclContext, declLocType.loc, pContext->get_identifier() );
                if( !declLocType.pDeclContext )
                {
                    std::ostringstream osFullType;
                    bool               bFirst = true;
                    for( auto i : path )
                    {
                        if( auto pContext = db_cast< Interface::IContext >( i ) )
                        {
                            if( bFirst )
                                bFirst = false;
                            else
                                osFullType << "::";
                            osFullType << pContext->get_identifier();
                        }
                    }
                    CLANG_PLUGIN_LOG(
                        "buildDimensionReturnType failed to resolve interface type: " << osFullType.str() );
                    THROW_RTE( "Failed to resolve interface context: " << osFullType.str() );
                }
            }
        }
        return declLocType;
    }

    bool
    buildDimensionReturnType( const std::vector< OperationsStage::Interface::DimensionTrait* >& returnTypesDimensions,
                              const char* pTraitName, clang::QualType& resultType )
    {
        using namespace OperationsStage;

        Interface::DimensionTrait* pTargetDimension = returnTypesDimensions.front();

        DeclLocType declLocType = locateInterfaceContext( pTargetDimension->get_parent() );

        DeclLocType dimensionResult = getNestedDeclContext(
            pASTContext, pSema, declLocType.pDeclContext, declLocType.loc, pTargetDimension->get_id()->get_str() );
        if( dimensionResult.pDeclContext )
        {
            clang::QualType type
                = clang::getTypeTrait( pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc, "Read" );
            if( type != QualType() )
            {
                resultType = type;
                return true;
            }
        }
        return false;
    }

    std::optional< clang::QualType >
    buildContextReturnType( const std::vector< OperationsStage::Interface::IContext* >& returnTypes,
                            bool                                                        bIsSingular,
                            bool                                                        bIsFunctionCall,
                            clang::DeclContext*                                         pDeclContext,
                            clang::SourceLocation                                       loc )
    {
        using namespace OperationsStage;

        if( bIsFunctionCall )
        {
            if( !bIsSingular )
            {
                CLANG_PLUGIN_LOG( "buildContextReturnType got function call with non-singular return type" );
                return {};
            }
            if( returnTypes.size() > 1 )
            {
                CLANG_PLUGIN_LOG( "buildContextReturnType got function call with variant return type" );
                return {};
            }
            DeclLocType declLocType      = locateInterfaceContext( returnTypes.front() );
            DeclLocType returnTypeResult = getNestedDeclContext(
                pASTContext, pSema, declLocType.pDeclContext, declLocType.loc, ::mega::EG_FUNCTION_TRAIT_TYPE );
            QualType typeType
                = getTypeTrait( pASTContext, pSema, returnTypeResult.pDeclContext, returnTypeResult.loc, "Type" );
            QualType typeTypeCanonical = typeType.getCanonicalType();
            return typeType.getCanonicalType();
        }
        else
        {
            if( returnTypes.size() == 1 )
            {
                DeclLocType declLocType = locateInterfaceContext( returnTypes.front() );
                if( !bIsSingular )
                {
                    clang::SourceLocation loc;
                    return clang::getVectorConstRefType(
                        pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, declLocType.type );
                }
                else
                {
                    return declLocType.type;
                }
            }
            else
            {
                if( !bIsSingular )
                {
                    std::vector< clang::QualType > types;
                    for( Interface::IContext* pTarget : returnTypes )
                    {
                        DeclLocType declLocType = locateInterfaceContext( pTarget );
                        types.push_back( declLocType.type );
                    }
                    clang::SourceLocation loc;
                    auto                  variantType = clang::getVariantType(
                        pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, types );
                    return clang::getVectorConstRefType(
                        pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, variantType );
                }
                else
                {
                    std::vector< clang::QualType > types;
                    for( Interface::IContext* pTarget : returnTypes )
                    {
                        DeclLocType declLocType = locateInterfaceContext( pTarget );
                        types.push_back( declLocType.type );
                    }
                    clang::SourceLocation loc;
                    return clang::getVariantType(
                        pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, types );
                }
            }
        }
    }

    bool buildReturnType( OperationsStage::Operations::Invocation* pInvocation, clang::QualType& resultType )
    {
        using namespace OperationsStage;

        THROW_TODO;

        /* std::vector< Interface::IContext* >       returnTypesContext   = pInvocation->get_return_type_contexts();
         std::vector< Interface::DimensionTrait* > returnTypesDimension = pInvocation->get_return_type_dimensions();
         bool                                      bIsSingular          = pInvocation->get_singular();
         if( pInvocation->get_explicit_operation() == mega::id_exp_Write_Link )
         {
             bIsSingular = true;
         }

         // establish the return type
         clang::DeclContext*   pDeclContext = pASTContext->getTranslationUnitDecl();
         clang::SourceLocation loc;

         switch( pInvocation->get_operation() )
         {
             case mega::id_Imp_NoParams:
             case mega::id_Imp_Params:
             {
                 if( !returnTypesContext.empty() )
                 {
                     CLANG_PLUGIN_LOG( "buildContextReturnType: " << pInvocation->get_name() );
                     if( std::optional< clang::QualType > resultOpt = buildContextReturnType(
                             returnTypesContext, bIsSingular, pInvocation->get_is_function_call(), pDeclContext, loc ) )
                     {
                         resultType = resultOpt.value();
                         return true;
                     }
                 }
                 else if( pInvocation->get_operation() == mega::id_Imp_NoParams )
                 {
                     CLANG_PLUGIN_LOG( "buildDimensionReturnType: " << pInvocation->get_name() );
                     return buildDimensionReturnType( returnTypesDimension, "Read", resultType );
                 }
                 else if( pInvocation->get_operation() == mega::id_Imp_Params )
                 {
                     // resultType = clang::getVoidType( pASTContext );

                     if( std::optional< clang::QualType > resultOpt = buildContextReturnType(
                             returnTypesContext, bIsSingular, pInvocation->get_is_function_call(), pDeclContext, loc ) )
                     {
                         resultType = resultOpt.value();
                         return true;
                     }
                 }
             }
             break;
             case mega::id_Start:
             {
                 if( std::optional< clang::QualType > resultOpt = buildContextReturnType(
                         returnTypesContext, bIsSingular, pInvocation->get_is_function_call(), pDeclContext, loc ) )
                 {
                     resultType = resultOpt.value();
                     return true;
                 }
             }
             break;
             case mega::id_Stop:
             {
                 resultType = clang::getVoidType( pASTContext );
                 return true;
             }
             break;
             case mega::id_Move:
             {
                 resultType = clang::getVoidType( pASTContext );
                 return true;
             }
             break;
             case mega::id_Get:
             {
                 if( !returnTypesDimension.empty() )
                 {
                     return buildDimensionReturnType( returnTypesDimension, "Get", resultType );
                 }
                 else
                 {
                     if( std::optional< clang::QualType > resultOpt = buildContextReturnType(
                             returnTypesContext, bIsSingular, pInvocation->get_is_function_call(), pDeclContext, loc ) )
                     {
                         resultType = resultOpt.value();
                         return true;
                     }
                 }
             }
             break;
             case mega::id_Range:
                 if ( !returnTypes.empty() )
                 {
                     if ( pSolution->getRoot()->getMaxRanges() == 1 )
                     {
                         if ( std::optional< clang::QualType > resultOpt
                              = buildContextReturnType( returnTypes, bIsSingular, pInvocation->get_is_function_call(),
                 pDeclContext, loc ) )
                         {
                             resultType = clang::getIteratorRangeType(
                                 pASTContext, g_pSema, pASTContext->getTranslationUnitDecl(), loc, resultOpt.value(),
                                 mega::EG_REFERENCE_ITERATOR_TYPE );
                         }
                     }
                     else
                     {
                         if ( std::optional< clang::QualType > resultOpt
                              = buildContextReturnType( returnTypes, bIsSingular, pInvocation->get_is_function_call(),
                 pDeclContext, loc ) )
                         {
                             resultType = clang::getMultiIteratorRangeType(
                                 pASTContext, g_pSema, pASTContext->getTranslationUnitDecl(), loc, resultOpt.value(),
                                 pSolution->getRoot()->getMaxRanges(), mega::EG_REFERENCE_ITERATOR_TYPE );
                         }
                     }
                 }
                 break;
         }
         return false;*/
    }

    std::optional< mega::InvocationID > getInvocationID( const clang::SourceLocation& loc, clang::QualType context,
                                                         clang::QualType typePath, clang::QualType operationType )
    {
        mega::InvocationID::SymbolIDVector contextSymbolIDs;
        if( !clang::getContextSymbolIDs( pASTContext, context, contextSymbolIDs ) )
        {
            std::ostringstream os;
            os << "Invalid context for invocation of type: " + context.getCanonicalType().getAsString();
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error ) << os.str();
            std::cout << "getInvocationResultType failed" << std::endl;
            m_bError = true;
            return {};
        }

        mega::InvocationID::SymbolIDVector typePathSymbolIDs;
        if( !clang::getTypePathSymbolIDs( pASTContext, typePath, typePathSymbolIDs ) )
        {
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                << "Invalid type path for invocation";
            std::cout << "getInvocationResultType failed" << std::endl;
            m_bError = true;
            return {};
        }

        mega::OperationID operationTypeID;
        {
            std::optional< mega::TypeID > operationTypeIDOpt = getMegaTypeID( pASTContext, operationType );
            if( !operationTypeIDOpt )
            {
                pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                    << "Invalid operation for invocation";
                std::cout << "getInvocationResultType failed" << std::endl;
                m_bError = true;
                return {};
            }
            operationTypeID = static_cast< mega::OperationID >( operationTypeIDOpt.value().getSymbolID() );
            VERIFY_RTE_MSG( ( static_cast< int >( operationTypeID ) >= mega::TypeID::LOWEST_SYMBOL_ID )
                                && ( static_cast< int >( operationTypeID ) < mega::HIGHEST_OPERATION_TYPE ),
                            "Invalid operation ID" );
        }

        return mega::InvocationID{ contextSymbolIDs, typePathSymbolIDs, operationTypeID };
    }

    std::optional< mega::InvocationID > getInvocationID( const clang::SourceLocation& loc, const clang::QualType& type )
    {
        if( type.getTypePtrOrNull() )
        {
            if( !type->isDependentType() )
            {
                if( const clang::IdentifierInfo* pBaseTypeID = type.getBaseTypeIdentifier() )
                {
                    if( pBaseTypeID == pASTContext->getEGInvocationTypeName() )
                    {
                        if( const clang::TemplateSpecializationType* pTemplateType
                            = type->getAs< clang::TemplateSpecializationType >() )
                        {
                            if( pTemplateType->getNumArgs() == 3U )
                            {
                                clang::QualType context       = pTemplateType->getArg( 0U ).getAsType();
                                clang::QualType typePath      = pTemplateType->getArg( 1U ).getAsType();
                                clang::QualType operationType = pTemplateType->getArg( 2U ).getAsType();
                                return getInvocationID( loc, context, typePath, operationType );
                            }
                        }
                    }
                }
            }
        }

        return {};
    }

    virtual bool getInvocationResultType( const clang::SourceLocation& loc, const clang::QualType& type,
                                          clang::QualType& resultType )
    {
        try
        {
            auto invocationID = getInvocationID( loc, type );
            if( invocationID.has_value() )
            {
                using namespace OperationsStage;

                Operations::Invocation* pInvocation = nullptr;
                {
                    auto iFind = m_invocationsMap.find( invocationID.value() );
                    if( iFind != m_invocationsMap.end() )
                    {
                        pInvocation = iFind->second;
                    }
                    else
                    {
                        pInvocation
                            = mega::invocation::compileInvocation( m_database, m_symbolTables, invocationID.value() );
                        m_invocationsMap.insert( std::make_pair( invocationID.value(), pInvocation ) );
                    }
                }

                if( buildReturnType( pInvocation, resultType ) )
                {
                    return true;
                }
                else
                {
                    pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                        << "Failed to resolve invocation";
                    m_bError = true;
                }
            }
        }
        /*catch( mega::invocation::NameResolutionException& nameResolutionException )
        {
            CLANG_PLUGIN_LOG( "NameResolutionException: " << nameResolutionException.what() );
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                << nameResolutionException.what();
            m_bError = true;
        }*/
        catch( mega::invocation::Exception& invocationException )
        {
            CLANG_PLUGIN_LOG( "invocation::Exception: " << invocationException.what() );
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                << invocationException.what();
            m_bError = true;
        }
        catch( std::exception& ex )
        {
            CLANG_PLUGIN_LOG( "std::exception: " << ex.what() );
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error ) << ex.what();
            m_bError = true;
        }

        std::cout << "getInvocationResultType failed" << std::endl;
        return false;
    }

    void recordInvocationLocs( const clang::SourceLocation& beginLoc, const clang::SourceLocation& endLoc,
                               const clang::UnaryTransformType* pUnaryTransformType )
    {
        THROW_TODO;
        /*
        if( pUnaryTransformType->getUTTKind() == clang::UnaryTransformType::EGResultType )
        {
            if( const clang::TemplateSpecializationType* pTemplateType
                = pUnaryTransformType->getBaseType()->getAs< clang::TemplateSpecializationType >() )
            {
                if( pTemplateType->getNumArgs() == 3U )
                {
                    clang::QualType context       = pTemplateType->getArg( 0U ).getAsType();
                    clang::QualType typePath      = pTemplateType->getArg( 1U ).getAsType();
                    clang::QualType operationType = pTemplateType->getArg( 2U ).getAsType();
                    auto            invocationID  = getInvocationID( beginLoc, context, typePath, operationType );
                    if( invocationID.has_value() )
                    {
                        using ::operator<<;
                        // std::cout << "Found invocation id: " << invocationID.value() << std::endl;
                        using namespace OperationsStage;

                        auto iFind = m_invocationsMap.find( invocationID.value() );
                        if( iFind != m_invocationsMap.end() )
                        {
                            Operations::Invocation* pInvocation = iFind->second;
                            const auto beginFileOffset = pASTContext->getSourceManager().getFileOffset( beginLoc );
                            const auto endFileOffset   = pASTContext->getSourceManager().getFileOffset( endLoc );
                            pInvocation->push_back_file_offsets(
                                mega::SourceLocation( beginFileOffset, endFileOffset ) );
                        }
                        else
                        {
                            CLANG_PLUGIN_LOG( "Failed to locate invocation id: " << invocationID.value() );
                            pASTContext->getDiagnostics().Report( beginLoc, clang::diag::err_mega_generic_error )
                                << "Failed to locate invocation";
                            m_bError = true;
                        }
                    }
                }
            }
        }*/
    }

    virtual void runFinalAnalysis()
    {
        bool bSuccess = !m_bError;

        using namespace OperationsStage;
        if( bSuccess )
        {
            try
            {
                // ensure ALL invocations are matched and file offset recorded
                using namespace clang;
                using namespace clang::ast_matchers;

                for( auto de : pASTContext->getTranslationUnitDecl()->noload_decls() )
                {
                    if( clang::CXXMethodDecl* pMethod = llvm::dyn_cast< clang::CXXMethodDecl >( de ) )
                    {
                        // std::cout << "Method: " << pMethod->getThisType().getAsString() << std::endl;
                        {
                            auto results = match(
                                cxxMethodDecl( hasDescendant( findAll( cxxMemberCallExpr().bind( "invocation" ) ) ) ),
                                *pMethod, *pASTContext );
                            for( auto result : results )
                            {
                                if( auto pCall = result.getNodeAs< clang::CXXMemberCallExpr >( "invocation" ) )
                                {
                                    if( pCall->getMethodDecl()->getNameAsString() == "invoke" )
                                    {
                                        if( const clang::ElaboratedType* pElaboratedType
                                            = pCall->getCallReturnType( *pASTContext )
                                                  ->getAs< clang::ElaboratedType >() )
                                        {
                                            if( const clang::TypedefType* pTypeDefType
                                                = pElaboratedType->getNamedType()->getAs< clang::TypedefType >() )
                                            {
                                                if( const clang::TypeAliasDecl* pTypeAliasDecl
                                                    = llvm::dyn_cast< clang::TypeAliasDecl >(
                                                        pTypeDefType->getDecl() ) )
                                                {
                                                    if( const clang::UnaryTransformType* pUnaryTransformType
                                                        = pTypeAliasDecl->getUnderlyingType()
                                                              ->getAs< clang::UnaryTransformType >() )
                                                    {
                                                        const clang::MemberExpr* pMemberExpr
                                                            = llvm::dyn_cast< MemberExpr >( pCall->getCallee() );
                                                        VERIFY_RTE_MSG(
                                                            pMemberExpr,
                                                            "Failed to resolve member expression for invocation" );
                                                        recordInvocationLocs( pMemberExpr->getMemberLoc(),
                                                                              pCall->getEndLoc(),
                                                                              pUnaryTransformType );
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                m_database.construct< Operations::Invocations >( Operations::Invocations::Args{ m_invocationsMap } );
            }
            catch( std::exception& ex )
            {
                pASTContext->getDiagnostics().Report( SourceLocation(), clang::diag::err_mega_generic_error )
                    << ex.what();
                bSuccess = false;
            }
        }

        if( bSuccess )
        {
            const mega::io::CompilationFilePath operationsSession
                = m_environment.OperationsStage_Operations( m_sourceFile );
            m_database.save_Operations_to_temp();
            m_environment.temp_to_real( operationsSession );
        }
    }
};

Session::Ptr make_operations_session( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir,
                                      const char* strBuildDir, const char* strSourceFile )
{
    return std::make_unique< OperationsSession >( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile );
}

} // namespace clang
