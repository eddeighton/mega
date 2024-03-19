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

#include "symbol_session.hpp"
#include "clang_utils.hpp"

#include "database/ObjectStage.hxx"

#include "invocation/invocation.hpp"

#include "mega/values/clang/attribute_id.hpp"

#include "mega/values/compilation/invocation_id.hpp"
#include "mega/values/compilation/operator_id.hpp"
#include "mega/values/compilation/source_location.hpp"

#include "mega/common_strings.hpp"

#include <boost/algorithm/string.hpp>

////////////////////////////////////////
#include "common/clang_warnings_begin.hpp"

#include "clang/Basic/SourceLocation.h"

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

#include "common/clang_warnings_end.hpp"
////////////////////////////////////////

#include <iostream>

namespace ObjectStage
{
#include "compiler/interface_printer.hpp"
}

using namespace ObjectStage;

namespace clang
{

class InvocationsSession : public SymbolSession
{
    mega::io::cppFilePath          m_sourceFile;
    Database                       m_database;
    Symbols::SymbolTable*          m_pSymbolTable;
    mega::invocation::SymbolTables m_symbolTables;

    using InvocationsMap = std::map< mega::InvocationID, Functions::Invocation* >;
    InvocationsMap m_invocationsMap;

    using SymbolMap = std::map< std::string, Symbols::SymbolID* >;
    SymbolMap m_symbols;

protected:
    bool m_bError = false;

public:
    using Ptr = std::unique_ptr< Session >;
    InvocationsSession( ASTContext* pASTContext_, Sema* pSema_, const char* strSrcDir, const char* strBuildDir,
                        const char* strSourceFile )
        : SymbolSession( pASTContext_, pSema_, strSrcDir, strBuildDir )
        , m_sourceFile( m_environment.cppFilePath_fromPath( strSourceFile ) )
        , m_database( m_environment, m_sourceFile )
        , m_pSymbolTable( m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() ) )
        , m_symbolTables( m_pSymbolTable )
    {
        m_symbols = m_pSymbolTable->get_symbol_names();
    }

    virtual unsigned int getSymbolID( const std::string& strIdentifier ) const override
    {
        auto result = SymbolSession::getSymbolID( strIdentifier );
        if( 0U == result )
        {
            auto iFind = m_symbols.find( strIdentifier );
            if( iFind == m_symbols.end() )
            {
                return 0;
            }
            else
            {
                return iFind->second->get_id().getValue();
            }
        }
        else
        {
            return result;
        }
    }

    bool locateMegaTypeTrait( Interface::CPP::TypeInfo* pType, clang::QualType& resultType )
    {
        DeclLocType declLocType;
        declLocType.pDeclContext = pASTContext->getTranslationUnitDecl();

        declLocType = clang::getNestedDeclContext(
            pASTContext, pSema, declLocType.pDeclContext, declLocType.loc, mega::MEGA_TRAITS );
        if( declLocType.pDeclContext )
        {
            clang::QualType type = clang::getTypeTrait(
                pASTContext, pSema, declLocType.pDeclContext, declLocType.loc, pType->get_trait_name() );
            if( type != QualType() )
            {
                resultType = type;
                return true;
            }
        }
        return false;
    }

    bool buildDimensionReturnType( Functions::ReturnTypes::Dimension* pDimensionReturnType, clang::QualType& resultType,
                                   const clang::SourceLocation& loc )
    {
        auto dimensions = pDimensionReturnType->get_dimensions();
        VERIFY_RTE_MSG( dimensions.size() != 0, "Invalid dimension return type" );

        std::set< Interface::CPP::TypeInfo* > dataTypes;
        {
            for( auto pDim : dimensions )
            {
                dataTypes.insert( pDim->get_cpp_data_type()->get_type_info() );
            }
        }

        if( dataTypes.size() == 1 )
        {
            if( locateMegaTypeTrait( *dataTypes.begin(), resultType ) )
            {
                return true;
            }
        }
        else
        {
            // non-homogeneous case - could generate variant ??
            std::ostringstream os;
            os << "Dimension return type has non-homogeneous types";
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error ) << os.str();
        }

        return false;
    }

    bool buildFunctionReturnType( Functions::ReturnTypes::Function* pFunctionReturnType, clang::QualType& resultType,
                                  const clang::SourceLocation& loc )
    {
        std::set< Interface::CPP::TypeInfo* > dataTypes;
        for( auto pFunction : pFunctionReturnType->get_functions() )
        {
            dataTypes.insert( pFunction->get_cpp_function_type()->get_return_type_info() );
        }

        if( dataTypes.size() == 1 )
        {
            if( locateMegaTypeTrait( *dataTypes.begin(), resultType ) )
            {
                return true;
            }
        }
        else
        {
            // non-homogeneous case - could generate variant ??
            std::ostringstream os;
            os << "Function return type has non-homogeneous types";
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error ) << os.str();

            return false;

            /*
                        std::vector< clang::QualType > types;
                        {
                            for( auto pType : dataTypes )
                            {
                                clang::QualType dataTypeType;
                                if( locateMegaTypeTrait( pType, dataTypeType ) )
                                {
                                    return true;
                                }
                                types.push_back( dataTypeType.getCanonicalType() );
                            }
                        }

                        clang::SourceLocation loc;
                        auto                  variantType
                            = clang::getVariantType( pASTContext, pSema, pASTContext->getTranslationUnitDecl(), loc,
               types ); resultType = variantType; return true;*/
        }
        return false;
    }

    bool buildContextReturnType( Functions::ReturnTypes::Context* pContextReturnType, clang::QualType& resultType,
                                 const clang::SourceLocation& loc )
    {
        std::vector< mega::interface::TypeID > typeIDs;
        for( auto pContext : pContextReturnType->get_contexts() )
        {
            typeIDs.push_back( pContext->get_interface_id()->get_type_id() );
        }

        resultType = clang::makeMegaPointerType( pASTContext, pSema, typeIDs );

        if( resultType != QualType() )
        {
            return true;
        }
        else
        {
            std::ostringstream os;
            os << "Failed to convert invocation return type";
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error ) << os.str();
            return false;
        }
    }

    bool buildRangeReturnType( Functions::ReturnTypes::Range*, clang::QualType&, const clang::SourceLocation& loc )
    {
        // non-homogeneous case - could generate variant ??
        std::ostringstream os;
        os << "Context range return types not supported yet";
        pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error ) << os.str();

        return false;
    }

    bool buildReturnType( Functions::Invocation* pInvocation, clang::QualType& resultType,
                          const clang::SourceLocation& loc )
    {
        auto pReturnType = pInvocation->get_return_type();

        if( db_cast< Functions::ReturnTypes::Void >( pReturnType ) )
        {
            resultType = clang::makeVoidType( pASTContext );
            return true;
        }
        else if( auto pDimension = db_cast< Functions::ReturnTypes::Dimension >( pReturnType ) )
        {
            return buildDimensionReturnType( pDimension, resultType, loc );
        }
        else if( auto pFunction = db_cast< Functions::ReturnTypes::Function >( pReturnType ) )
        {
            return buildFunctionReturnType( pFunction, resultType, loc );
        }
        else if( auto pRange = db_cast< Functions::ReturnTypes::Range >( pReturnType ) )
        {
            return buildRangeReturnType( pRange, resultType, loc );
        }
        else if( auto pContext = db_cast< Functions::ReturnTypes::Context >( pReturnType ) )
        {
            return buildContextReturnType( pContext, resultType, loc );
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
        mega::InvocationID::TypeIDArray contextSymbolIDs;
        if( !clang::getContextTypeIDs( pASTContext, context, contextSymbolIDs ) )
        {
            std::ostringstream os;
            os << "Invalid context for invocation of type: " + context.getCanonicalType().getAsString();
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error ) << os.str();
            CLANG_PLUGIN_LOG( "getInvocationID failed" );
            m_bError = true;
            return {};
        }

        mega::InvocationID::SymbolIDArray symbolPath;
        if( !clang::getSymbolPath( pASTContext, typePath, symbolPath ) )
        {
            pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                << "Invalid type path for invocation";
            CLANG_PLUGIN_LOG( "getInvocationID failed" );
            m_bError = true;
            return {};
        }

        bool bHasParams = false;
        {
            std::optional< bool > hasParamsOpt;
            if( const CXXRecordDecl* pRecordDecl = operationType.getCanonicalType()->getAsCXXRecordDecl() )
            {
                if( pRecordDecl->hasAttr< EGTypeIDAttr >() )
                {
                    if( auto pAttr = pRecordDecl->getAttr< EGTypeIDAttr >() )
                    {
                        switch( pAttr->getId() )
                        {
                            case mega::id_OPERATION_IMP_NOPARAMS:
                                hasParamsOpt = false;
                                break;
                            case mega::id_OPERATION_IMP_PARAMS:
                                hasParamsOpt = true;
                                break;
                        }
                    }
                }
            }
            if( !hasParamsOpt )
            {
                pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                    << "Invalid operation type for invocation";
                CLANG_PLUGIN_LOG( "getInvocationID failed" );
                m_bError = true;
                return {};
            }
            bHasParams = hasParamsOpt.value();
        }

        return mega::InvocationID{ contextSymbolIDs, symbolPath, bHasParams };
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
                                          clang::QualType& resultType ) override
    {
        try
        {
            InvocationIDCanonicalTypes canonicalTypes;
            auto                       invocationID = getInvocationID( loc, type, canonicalTypes );
            if( invocationID.has_value() )
            {
                Functions::Invocation* pInvocation    = nullptr;
                bool                   bNewInvocation = false;
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
                        CLANG_PLUGIN_LOG( "Compiled invocation: " << invocationID.value() );
                    }
                }

                if( buildReturnType( pInvocation, resultType, loc ) )
                {
                    if( bNewInvocation )
                    {
                        pInvocation->get_return_type()->set_canonical_type(
                            resultType.getCanonicalType().getAsString() );

                        if( auto pWrite = db_cast< Functions::Write >( pInvocation ) )
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
                        << "Failed to build invocation return type";
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
                        CLANG_PLUGIN_LOG( "Found invocation id: " << invocationID.value() );

                        auto iFind = m_invocationsMap.find( invocationID.value() );
                        if( iFind != m_invocationsMap.end() )
                        {
                            Functions::Invocation* pInvocation = iFind->second;
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

    void recurseDecls( clang::DeclContext* pDeclContext )
    {
        using namespace clang;
        using namespace clang::ast_matchers;

        if( auto pMethod = llvm::dyn_cast< clang::CXXMethodDecl >( pDeclContext ) )
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
                        // CLANG_PLUGIN_LOG(
                        //     "Found member function call: " << pCall->getMethodDecl()->getNameAsString() );
                        if( pCall->getMethodDecl()->getNameAsString() == mega::MEGA_INVOKE_MEMBER_FUNCTION_NAME )
                        {
                            if( auto pElaboratedType
                                = pCall->getCallReturnType( *pASTContext )->getAs< clang::ElaboratedType >() )
                            {
                                if( auto pTypeDefType = pElaboratedType->getNamedType()->getAs< clang::TypedefType >() )
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
                                            recordInvocationLocs(
                                                pMemberExpr->getMemberLoc(), pCall->getEndLoc(), pUnaryTransformType );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        for( auto pChildDecl : pDeclContext->decls() )
        {
            if( auto pChildDeclContext = llvm::dyn_cast< clang::DeclContext >( pChildDecl ) )
            {
                recurseDecls( pChildDeclContext );
            }
        }
    }

    void recordInvocationLocations()
    {
        using namespace clang;
        using namespace clang::ast_matchers;

        for( auto de : pASTContext->getTranslationUnitDecl()->noload_decls() )
        {
            if( auto pChildDeclContext = llvm::dyn_cast< clang::DeclContext >( de ) )
            {
                recurseDecls( pChildDeclContext );
            }
        }
    }
    /*
        std::map< mega::OperatorID, Functions::Operator* > detectOperatorInstantiations()
        {
            std::map< mega::OperatorID, Functions::Operator* > operators;

            using namespace clang;
            using namespace clang::ast_matchers;

            for( const auto& [ operatorType, strOpName ] : mega::OperatorID::names )
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
                                        // CLANG_PLUGIN_LOG( "Found operator instance: "
                                        //                   << pFunctionDecl->getNameAsString()
                                        //                   << " in function: " << pMethod->getThisType().getAsString()
       );

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
                                                    << "Unexpected number of template arguemnts for operator: " <<
       strOpName
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
                                                        pFunctionDecl->getLocation(),
       clang::diag::err_mega_generic_error )
                                                        << "Failed to resolve mega type for operator template argument:
       "
                                                        << strOpName << " in " << pMethod->getThisType().getAsString();
                                                    m_bError = true;
                                                }
                                                else
                                                {
                                                    const mega::OperatorID operatorID{ operatorType, megaType.value() };

                                                    auto iFind = operators.find( operatorID );
                                                    if( iFind == operators.end() )
                                                    {
                                                        Functions::Operator* pOperator = nullptr;
                                                        switch( operatorType )
                                                        {
                                                            case mega::OperatorID::op_new:
                                                            {
                                                                pOperator = m_database.construct< Functions::New >(
                                                                    Functions::New::Args{
                                                                        Functions::Operator::Args{ operatorID } } );
                                                            }
                                                            break;
                                                            case mega::OperatorID::op_remote_new:
                                                            {
                                                                pOperator = m_database.construct< Functions::RemoteNew
       >( Functions::RemoteNew::Args{ Functions::Operator::Args{ operatorID } } );
                                                            }
                                                            break;
                                                            case mega::OperatorID::op_delete:
                                                            {
                                                                pOperator = m_database.construct< Functions::Delete >(
                                                                    Functions::Delete::Args{
                                                                        Functions::Operator::Args{ operatorID } } );
                                                            }
                                                            break;
                                                            case mega::OperatorID::op_cast:
                                                            {
                                                                pOperator = m_database.construct< Functions::Cast >(
                                                                    Functions::Cast::Args{
                                                                        Functions::Operator::Args{ operatorID } } );
                                                            }
                                                            break;
                                                            case mega::OperatorID::op_active:
                                                            {
                                                                pOperator = m_database.construct< Functions::Active >(
                                                                    Functions::Active::Args{
                                                                        Functions::Operator::Args{ operatorID } } );
                                                            }
                                                            break;
                                                            case mega::OperatorID::op_enabled:
                                                            {
                                                                pOperator = m_database.construct< Functions::Enabled >(
                                                                    Functions::Enabled::Args{
                                                                        Functions::Operator::Args{ operatorID } } );
                                                            }
                                                            break;
                                                            default:
                                                            case mega::OperatorID::HIGHEST_OPERATOR_TYPE:
                                                                THROW_RTE( "Unknown operator type" );
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
    */
    virtual void runFinalAnalysis() override
    {
        bool bSuccess = !m_bError;

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
                        std::ostringstream osError;
                        osError << "Failed to determine file offsets for invocation: " << id;
                        pASTContext->getDiagnostics().Report( SourceLocation(), clang::diag::err_mega_generic_error )
                            << osError.str();
                        bSuccess = false;
                    }
                }

                std::map< mega::OperatorID, Functions::Operator* > operators;
                // auto operators = detectOperatorInstantiations();

                m_database.construct< Functions::Invocations >(
                    Functions::Invocations::Args{ m_invocationsMap, operators } );
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
            const mega::io::CompilationFilePath invocationsSession = m_environment.ObjectStage_Obj( m_sourceFile );
            m_database.save_Obj_to_temp();
            m_environment.temp_to_real( invocationsSession );
        }
    }
};

Session::Ptr make_invocations_session( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir,
                                       const char* strBuildDir, const char* strSourceFile )
{
    return std::make_unique< InvocationsSession >( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile );
}

} // namespace clang
