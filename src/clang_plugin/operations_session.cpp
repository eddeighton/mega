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
#include "mega/operator_id.hpp"
#include "database/types/source_location.hpp"

#include <boost/algorithm/string.hpp>

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
        // CLANG_PLUGIN_LOG( "isPossibleEGTypeIdentifier: " << strIdentifier );
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

    bool buildDimensionReturnType( OperationsStage::Operations::ReturnTypes::Dimension* pDimensionReturnType,
                                   clang::QualType&                                     resultType )
    {
        using namespace OperationsStage;

        VERIFY_RTE_MSG(
            pDimensionReturnType->get_homogeneous(), "Non-homogeneous dimension return types NOT supported" );

        auto dimensions = pDimensionReturnType->get_dimensions();
        VERIFY_RTE_MSG( dimensions.size() == 1, "Non-homogeneous dimension return types NOT supported" );

        Interface::DimensionTrait* pTargetDimension = dimensions.front();

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

    bool buildFunctionReturnType( OperationsStage::Operations::ReturnTypes::Function* pFunctionReturnType,
                                  clang::QualType&                                    resultType )
    {
        using namespace OperationsStage;

        VERIFY_RTE_MSG( pFunctionReturnType->get_homogeneous(), "Non-homogeneous function return types NOT supported" );
        auto functions = pFunctionReturnType->get_functions();
        if( pFunctionReturnType->get_homogeneous() )
        {
            DeclLocType declLocType      = locateInterfaceContext( functions.front() );
            DeclLocType returnTypeResult = getNestedDeclContext(
                pASTContext, pSema, declLocType.pDeclContext, declLocType.loc, ::mega::EG_FUNCTION_TRAIT_TYPE );
            QualType typeType
                = getTypeTrait( pASTContext, pSema, returnTypeResult.pDeclContext, returnTypeResult.loc, "Type" );
            resultType = typeType.getCanonicalType();
            return true;
        }
        else
        {
            std::vector< clang::QualType > types;
            for( Interface::Function* pTarget : functions )
            {
                DeclLocType declLocType      = locateInterfaceContext( pTarget );
                DeclLocType returnTypeResult = getNestedDeclContext(
                    pASTContext, pSema, declLocType.pDeclContext, declLocType.loc, ::mega::EG_FUNCTION_TRAIT_TYPE );
                QualType typeType
                    = getTypeTrait( pASTContext, pSema, returnTypeResult.pDeclContext, returnTypeResult.loc, "Type" );
                types.push_back( typeType.getCanonicalType() );
            }
            clang::SourceLocation loc;
            auto                  variantType
                = clang::getVariantType( pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, types );
            resultType = variantType;
            return true;
        }
    }

    bool buildContextReturnType( OperationsStage::Operations::ReturnTypes::Context* pContextReturnType,
                                 clang::QualType&                                   resultType )
    {
        using namespace OperationsStage;

        auto returnTypes = pContextReturnType->get_contexts();

        if( returnTypes.size() == 1 )
        {
            DeclLocType declLocType = locateInterfaceContext( returnTypes.front() );
            resultType              = declLocType.type;
            return true;
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
            auto                  variantType
                = clang::getVariantType( pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, types );
            resultType = variantType;
            return true;
        }
    }

    bool buildRangeReturnType( OperationsStage::Operations::ReturnTypes::Range* pRangeReturnType,
                               clang::QualType&                                 resultType )
    {
        using namespace OperationsStage;

        auto returnTypes = pRangeReturnType->get_contexts();

        if( returnTypes.size() == 1 )
        {
            DeclLocType           declLocType = locateInterfaceContext( returnTypes.front() );
            clang::SourceLocation loc;
            resultType = clang::getVectorConstRefType(
                pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, declLocType.type );
            return true;
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
            auto                  variantType
                = clang::getVariantType( pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, types );
            resultType = clang::getVectorConstRefType(
                pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc, variantType );
            return true;
        }
    }

    bool buildReturnType( OperationsStage::Operations::Invocation* pInvocation, clang::QualType& resultType )
    {
        using namespace OperationsStage;

        auto pReturnType = pInvocation->get_return_type();
        if( auto pVoid = db_cast< Operations::ReturnTypes::Void >( pReturnType ) )
        {
            resultType = clang::getVoidType( pASTContext );
            return true;
        }
        else if( auto pDimension = db_cast< Operations::ReturnTypes::Dimension >( pReturnType ) )
        {
            return buildDimensionReturnType( pDimension, resultType );
        }
        else if( auto pFunction = db_cast< Operations::ReturnTypes::Function >( pReturnType ) )
        {
            return buildFunctionReturnType( pFunction, resultType );
        }
        else if( auto pRange = db_cast< Operations::ReturnTypes::Range >( pReturnType ) )
        {
            return buildRangeReturnType( pRange, resultType );
        }
        else if( auto pContext = db_cast< Operations::ReturnTypes::Context >( pReturnType ) )
        {
            return buildContextReturnType( pContext, resultType );
        }
        else
        {
            THROW_RTE( "Unknown return type" );
        }

        return false;
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
            CLANG_PLUGIN_LOG( "getInvocationResultType failed" );
            m_bError = true;
            return {};
        }

        mega::InvocationID::SymbolIDVector typePathSymbolIDs;
        if( !clang::getTypePathSymbolIDs( pASTContext, typePath, typePathSymbolIDs ) )
        {
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                << "Invalid type path for invocation";
            CLANG_PLUGIN_LOG( "getInvocationResultType failed" );
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
                CLANG_PLUGIN_LOG( "getInvocationResultType failed" );
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

    struct InvocationIDCanonicalTypes
    {
        std::string strContext, strTypePath, strOperation;
    };

    std::optional< mega::InvocationID > getInvocationID( const clang::SourceLocation& loc, const clang::QualType& type,
                                                         InvocationIDCanonicalTypes& canonicalTypes )
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

                                canonicalTypes.strContext   = context.getCanonicalType().getAsString();
                                canonicalTypes.strTypePath  = typePath.getCanonicalType().getAsString();
                                canonicalTypes.strOperation = operationType.getCanonicalType().getAsString();

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
            InvocationIDCanonicalTypes canonicalTypes;
            auto                       invocationID = getInvocationID( loc, type, canonicalTypes );
            if( invocationID.has_value() )
            {
                using namespace OperationsStage;

                Operations::Invocation* pInvocation    = nullptr;
                bool                    bNewInvocation = false;
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
                        bNewInvocation = true;
                        using ::operator<<;
                        CLANG_PLUGIN_LOG( "Compiled invocation: " << invocationID.value() );
                    }
                }

                if( buildReturnType( pInvocation, resultType ) )
                {
                    if( bNewInvocation )
                    {
                        pInvocation->get_return_type()->set_canonical_type(
                            resultType.getCanonicalType().getAsString() );

                        if( auto pWrite = db_cast< Operations::Write >( pInvocation ) )
                        {
                            pWrite->get_parameter_type()->set_canonical_type( "" );
                        }

                        // need to strip struct or class from type
                        boost::replace_all( canonicalTypes.strContext, "struct ", " " );
                        boost::replace_all( canonicalTypes.strTypePath, "struct ", " " );
                        boost::replace_all( canonicalTypes.strOperation, "struct ", " " );

                        pInvocation->set_canonical_context( canonicalTypes.strContext );
                        pInvocation->set_canonical_type_path( canonicalTypes.strTypePath );
                        pInvocation->set_canonical_operation( canonicalTypes.strOperation );
                    }
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

        CLANG_PLUGIN_LOG( "getInvocationResultType failed" );
        return false;
    }

    void recordInvocationLocs( const clang::SourceLocation& beginLoc, const clang::SourceLocation& endLoc,
                               const clang::UnaryTransformType* pUnaryTransformType )
    {
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
                        CLANG_PLUGIN_LOG( "Found invocation id: " << invocationID.value() );
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
                    else
                    {
                        CLANG_PLUGIN_LOG( "Failed to determine invocation ID" );
                        pASTContext->getDiagnostics().Report( beginLoc, clang::diag::err_mega_generic_error )
                            << "Failed to determine invocation ID";
                        m_bError = true;
                    }
                }
                else
                {
                    CLANG_PLUGIN_LOG( "Wrong number of template parameters in EGResultType" );
                    pASTContext->getDiagnostics().Report( beginLoc, clang::diag::err_mega_generic_error )
                        << "Wrong number of template parameters in EGResultType";
                    m_bError = true;
                }
            }
            else
            {
                CLANG_PLUGIN_LOG( "Failed to locate EGResultType" );
                pASTContext->getDiagnostics().Report( beginLoc, clang::diag::err_mega_generic_error )
                    << "Failed to locate invocation EGResultType";
                m_bError = true;
            }
        }
    }

    void recordInvocationLocations()
    {
        // ensure ALL invocations are matched and file offset recorded

        using namespace OperationsStage;
        using namespace clang;
        using namespace clang::ast_matchers;

        for( auto de : pASTContext->getTranslationUnitDecl()->noload_decls() )
        {
            if( auto pMethod = llvm::dyn_cast< clang::CXXMethodDecl >( de ) )
            {
                // CLANG_PLUGIN_LOG( "Method: " << pMethod->getThisType().getAsString() );
                {
                    auto results
                        = match( cxxMethodDecl( hasDescendant( findAll( cxxMemberCallExpr().bind( "invocation" ) ) ) ),
                                 *pMethod, *pASTContext );
                    for( auto result : results )
                    {
                        if( auto pCall = result.getNodeAs< clang::CXXMemberCallExpr >( "invocation" ) )
                        {
                            CLANG_PLUGIN_LOG(
                                "Found member function call: " << pCall->getMethodDecl()->getNameAsString() );
                            if( pCall->getMethodDecl()->getNameAsString() == mega::EG_INVOKE_MEMBER_FUNCTION_NAME )
                            {
                                if( auto pElaboratedType
                                    = pCall->getCallReturnType( *pASTContext )->getAs< clang::ElaboratedType >() )
                                {
                                    if( auto pTypeDefType
                                        = pElaboratedType->getNamedType()->getAs< clang::TypedefType >() )
                                    {
                                        if( auto pTypeAliasDecl
                                            = llvm::dyn_cast< clang::TypeAliasDecl >( pTypeDefType->getDecl() ) )
                                        {
                                            if( auto pUnaryTransformType = pTypeAliasDecl->getUnderlyingType()
                                                                               ->getAs< clang::UnaryTransformType >() )
                                            {
                                                auto pMemberExpr = llvm::dyn_cast< MemberExpr >( pCall->getCallee() );
                                                VERIFY_RTE_MSG(
                                                    pMemberExpr, "Failed to resolve member expression for invocation" );
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
    }

    std::map< mega::OperatorID, OperationsStage::Operations::Operator* > detectOperatorInstantiations()
    {
        using namespace OperationsStage;
        std::map< mega::OperatorID, Operations::Operator* > operators;

        using namespace std::string_literals;
        static const std::array< std::pair< mega::Operator, std::string >, mega::HIGHEST_OPERATOR_TYPE > operatorNames
            = { std::pair< mega::Operator, std::string >{ mega::op_new, "mega_new"s },
                std::pair< mega::Operator, std::string >{ mega::op_delete, "mega_delete"s },
                std::pair< mega::Operator, std::string >{ mega::op_cast, "mega_cast"s },
                std::pair< mega::Operator, std::string >{ mega::op_active, "mega_active"s },
                std::pair< mega::Operator, std::string >{ mega::op_enabled, "mega_enabled"s } };

        using namespace clang;
        using namespace clang::ast_matchers;

        for( const auto& [ operatorType, strOpName ] : operatorNames )
        {
            for( auto de : pASTContext->getTranslationUnitDecl()->noload_decls() )
            {
                if( auto pMethod = llvm::dyn_cast< clang::CXXMethodDecl >( de ) )
                {
                    // CLANG_PLUGIN_LOG( "Method: " << pMethod->getThisType().getAsString() );
                    {
                        auto results = match( cxxMethodDecl( hasDescendant( findAll(
                                                  callExpr( callee( functionDecl().bind( "operator" ) ) ) ) ) ),
                                              *pMethod, *pASTContext );

                        for( auto result : results )
                        {
                            if( auto pFunctionDecl = result.getNodeAs< FunctionDecl >( "operator" ) )
                            {
                                if( pFunctionDecl->getNameAsString() == strOpName )
                                {
                                    CLANG_PLUGIN_LOG( "Found operator instance: "
                                                      << pFunctionDecl->getNameAsString()
                                                      << " in function: " << pMethod->getThisType().getAsString() );

                                    if( pFunctionDecl->getTemplatedKind()
                                        != FunctionDecl::TK_FunctionTemplateSpecialization )
                                    {
                                        pASTContext->getDiagnostics().Report(
                                            pFunctionDecl->getLocation(), clang::diag::err_mega_generic_error )
                                            << "Unexpected template kind for operator: " << strOpName << " in "
                                            << pMethod->getThisType().getAsString();
                                        m_bError = true;
                                    }
                                    else
                                    {
                                        auto pTempalteArgs = pFunctionDecl->getTemplateSpecializationArgs();

                                        if( pTempalteArgs->size() != 1 )
                                        {
                                            pASTContext->getDiagnostics().Report(
                                                pFunctionDecl->getLocation(), clang::diag::err_mega_generic_error )
                                                << "Unexpected number of template arguemnts for operator: " << strOpName
                                                << " in " << pMethod->getThisType().getAsString();
                                            m_bError = true;
                                        }
                                        else
                                        {
                                            auto pTemplateArg    = pTempalteArgs->get( 0 );
                                            auto templateArgType = pTemplateArg.getAsType();

                                            auto megaType = getMegaTypeID( pASTContext, templateArgType );
                                            if( !megaType.has_value() )
                                            {
                                                pASTContext->getDiagnostics().Report(
                                                    pFunctionDecl->getLocation(), clang::diag::err_mega_generic_error )
                                                    << "Failed to resolve mega type for operator template argument: "
                                                    << strOpName << " in " << pMethod->getThisType().getAsString();
                                                m_bError = true;
                                            }
                                            else
                                            {
                                                const mega::OperatorID operatorID{ operatorType, megaType.value() };

                                                auto iFind = operators.find( operatorID );
                                                if( iFind == operators.end() )
                                                {
                                                    Operations::Operator* pOperator = nullptr;
                                                    switch( operatorType )
                                                    {
                                                        case mega::op_new:
                                                        {
                                                            pOperator = m_database.construct< Operations::New >(
                                                                Operations::New::Args{
                                                                    Operations::Operator::Args{ operatorID } } );
                                                        }
                                                        break;
                                                        case mega::op_delete:
                                                        {
                                                            pOperator = m_database.construct< Operations::Delete >(
                                                                Operations::Delete::Args{
                                                                    Operations::Operator::Args{ operatorID } } );
                                                        }
                                                        break;
                                                        case mega::op_cast:
                                                        {
                                                            pOperator = m_database.construct< Operations::Cast >(
                                                                Operations::Cast::Args{
                                                                    Operations::Operator::Args{ operatorID } } );
                                                        }
                                                        break;
                                                        case mega::op_active:
                                                        {
                                                            pOperator = m_database.construct< Operations::Active >(
                                                                Operations::Active::Args{
                                                                    Operations::Operator::Args{ operatorID } } );
                                                        }
                                                        break;
                                                        case mega::op_enabled:
                                                        {
                                                            pOperator = m_database.construct< Operations::Enabled >(
                                                                Operations::Enabled::Args{
                                                                    Operations::Operator::Args{ operatorID } } );
                                                        }
                                                        break;
                                                        default:
                                                        case mega::HIGHEST_OPERATOR_TYPE:
                                                            break;
                                                    }
                                                    VERIFY_RTE( pOperator );
                                                    operators.insert( { operatorID, pOperator } );
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
        }

        return operators;
    }

    virtual void runFinalAnalysis()
    {
        bool bSuccess = !m_bError;

        using namespace OperationsStage;
        if( bSuccess )
        {
            try
            {
                recordInvocationLocations();

                // check found ALL invocations
                for( const auto& [ id, pInvocation ] : m_invocationsMap )
                {
                    try
                    {
                        // will throw exception if NOT set at all
                        pInvocation->get_file_offsets();
                    }
                    catch( std::exception& ex )
                    {
                        using ::           operator<<;
                        std::ostringstream osError;
                        osError << "Failed to determine file offsets for invocation: " << id;
                        pASTContext->getDiagnostics().Report( SourceLocation(), clang::diag::err_mega_generic_error )
                            << osError.str();
                        bSuccess = false;
                    }
                }

                auto operators = detectOperatorInstantiations();

                m_database.construct< Operations::Invocations >(
                    Operations::Invocations::Args{ m_invocationsMap, operators } );
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
