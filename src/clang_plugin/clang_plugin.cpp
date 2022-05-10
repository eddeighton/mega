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

#include "clang_plugin/clang_plugin.hpp"

//#include "eg_compiler/identifiers.hpp"
//#include "eg_compiler/eg.hpp"
//#include "eg_compiler/codegen/codegen.hpp"
//#include "eg_compiler/name_resolution.hpp"
//#include "eg_compiler/invocation.hpp"

#include "session.hpp"

#include "common/backtrace.hpp"

#include "clang_utils.hpp"

#include "clang/Sema/Lookup.h"

#include "boost/dll.hpp"
#include <boost/dll/runtime_symbol_info.hpp>

#include <sstream>
#include <cstdlib>
#include <memory>
//#include <iostream>

namespace clang
{
extern Session::Ptr make_interface_session( clang::ASTContext* pASTContext, clang::Sema* pSema, const char* strSrcDir,
                                            const char* strBuildDir, const char* strSourceFile );
}

namespace
{
clang::Session::Ptr g_pSession;
clang::ASTContext*  g_pASTContext = nullptr;
clang::Sema*        g_pSema       = nullptr;
} // namespace

namespace mega
{
struct EG_PLUGIN_INTERFACE_IMPL : EG_PLUGIN_INTERFACE
{
    virtual void initialise( clang::ASTContext* pASTContext, clang::Sema* pSema )
    {
        //std::cout << "EG_PLUGIN_INTERFACE_IMPL::initialise: " << std::endl;
        g_pASTContext = pASTContext;
        g_pSema       = pSema;
        VERIFY_RTE( g_pASTContext );
        VERIFY_RTE( g_pSema );
        VERIFY_RTE( g_pSession );
        g_pSession->setContext( g_pASTContext, g_pSema );
    }

    virtual void setMode( const char* strMode, const char* strSrcDir, const char* strBuildDir,
                          const char* strSourceFile )
    {
        //std::cout << "EG_PLUGIN_INTERFACE_IMPL::setMode: " << strMode << " : " << strSrcDir << " : " << strBuildDir
        //          << " : " << strSourceFile << std::endl;
        
        switch ( mega::fromStr( strMode ) )
        {
            case mega::eInterface:
                g_pSession
                    = clang::make_interface_session( g_pASTContext, g_pSema, strSrcDir, strBuildDir, strSourceFile );
                break;
            case mega::eOperations:
            case mega::eImplementation:
            case mega::eCPP:
            case mega::TOTAL_COMPILATION_MODES:
            default:
                g_pSession = std::make_unique< clang::Session >( g_pASTContext, g_pSema );
                return;
        }

        // Common::debug_break();
    }

    virtual void runFinalAnalysis()
    {
        if ( g_pSession )
        {
            g_pSession->runFinalAnalysis();
        }
    }

    virtual bool isEGEnabled()
    {
        if ( g_pSession )
        {
            return g_pSession->isAnalysis();
        }
        else
        {
            return false;
        }
    }

    virtual bool isEGType( const clang::QualType& type )
    {
        if ( g_pSession )
        {
            if ( clang::getEGTypeID( g_pSession->getASTContext(), type ) )
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }

    virtual bool isPossibleEGType( const clang::QualType& type )
    {
        if ( type.getTypePtrOrNull() )
        {
            if ( type->isDependentType() )
            {
                return true;
            }
            else
            {
                if ( g_pSession )
                {
                    if ( clang::getEGTypeID( g_pSession->getASTContext(), type ) )
                        return true;
                    else
                        return false;
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

    virtual bool isPossibleEGTypeIdentifier( const clang::Token& token )
    {
        if ( g_pSession )
        {
            if ( token.is( clang::tok::identifier ) )
            {
                const clang::IdentifierInfo* pIdentifierInfo = token.getIdentifierInfo();
                return g_pSession->isPossibleEGTypeIdentifier( pIdentifierInfo->getName().str() );
            }
            else if ( token.is( clang::tok::annot_template_id ) )
            {
                // const clang::TemplateIdAnnotation* Annot
                //     = static_cast< const clang::TemplateIdAnnotation* >( token.getAnnotationValue() );
            }
            else if ( token.is( clang::tok::annot_typename ) )
            {
                clang::ParsedType parsedType = clang::ParsedType::getFromOpaquePtr( token.getAnnotationValue() );
                if ( g_pSession )
                {
                    return isPossibleEGType( g_pSession->getSema()->GetTypeFromParser( parsedType ) );
                }
                else
                {
                    return false;
                }
            }
        }

        return false;
    }

    virtual int isPossibleEGTypeIdentifierDecl( const clang::Token& token, bool bIsTypePathParsing )
    {
        if ( isPossibleEGTypeIdentifier( token ) )
        {
            if ( !bIsTypePathParsing )
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 2; // ambiguous
        }
    }

    virtual bool getInvocationOperationType( const clang::SourceLocation& loc, const clang::QualType& typePathType,
                                             bool bHasArguments, clang::QualType& operationType )
    {
        // determine the operation type...
        if ( g_pSession )
        {
            if ( const clang::IdentifierInfo* pIdentifierInfo
                 = clang::getOperationID( g_pSession->getASTContext(), typePathType, bHasArguments ) )
            {
                // return the type
                clang::TagDecl* operatorClassDecl = nullptr;
                {
                    clang::LookupResult Result( *g_pSession->getSema(), pIdentifierInfo, clang::SourceLocation(),
                                                clang::Sema::LookupOrdinaryName );
                    if ( g_pSession->getSema()->LookupName( Result, g_pSession->getSema()->getCurScope() ) )
                    {
                        operatorClassDecl = llvm::dyn_cast< clang::TagDecl >( Result.getFoundDecl() );
                    }
                }
                if ( operatorClassDecl )
                {
                    operationType = g_pSession->getASTContext()->getTagDeclType( operatorClassDecl );
                    return true;
                }
                else
                {
                    // diagnostic
                }
            }
            else
            {
            }
        }
        return false;
    }

    /*std::optional< clang::QualType > buildActionReturnType( const InvocationSolution::Context& returnTypes,
        clang::DeclContext* pDeclContext, clang::SourceLocation loc )
    {
        bool bIsFunctionReturnType = false;
        {
            std::set< std::string > functionReturnTypes;
            bool bNonFunction = false;
            for( const interface::Element* pReturnType : returnTypes )
            {
                if( const interface::Function* pFunctionCall =
                    dynamic_cast< const interface::Function* >( pReturnType ) )
                {
                    functionReturnTypes.insert( pFunctionCall->getReturnType() );
                }
                else
                {
                    bNonFunction = true;
                }
            }
            if( !functionReturnTypes.empty() )
            {
                if( bNonFunction || ( functionReturnTypes.size() != 1U ) )
                    return std::optional< clang::QualType >();
                //return the function return type
                bIsFunctionReturnType = true;
            }
        }

        if( ( returnTypes.size() == 1U ) || bIsFunctionReturnType )
        {
            const interface::Element* pTarget = returnTypes.front();
            clang::DeclContext* pDeclContextIter = pDeclContext;
            const std::vector< const interface::Element* > path = getPath( pTarget );
            for( const interface::Element* pElementElement : path )
            {
                if( pElementElement == path.back() )
                {
                    if( const interface::Function* pFunctionCall =
                        dynamic_cast< const interface::Function* >( pElementElement ) )
                    {
                        clang::DeclLocType result = getNestedDeclContext( g_pASTContext, g_pSema,
                            pDeclContextIter, loc, ::mega::getInterfaceType( pElementElement->getIdentifier() ), true );
                        if( result.pContext )
                        {
                            const std::string strBaseType = ::mega::getBaseTraitType( 0 );
                            clang::DeclLocType linkResult = getNestedDeclContext( g_pASTContext, g_pSema,
                                result.pContext, result.loc, strBaseType, true );
                            if( linkResult.pContext )
                            {
                                //attempt to get the Type Alias
                                clang::QualType typeType = getTypeTrait( g_pASTContext, g_pSema, linkResult.pContext,
    linkResult.loc, "Type" ); return typeType.getCanonicalType();
                            }
                        }
                    }
                    else
                    {
                        return clang::getType( g_pASTContext, g_pSema,
                            getInterfaceType( pElementElement->getIdentifier() ), "void",
                            pDeclContextIter, loc, true );
                    }

                }
                else
                {
                    clang::getType( g_pASTContext, g_pSema,
                        getInterfaceType( pElementElement->getIdentifier() ), "void",
                        pDeclContextIter, loc, false );
                    if( !pDeclContextIter ) break;
                }
            }
        }
        else
        {
            std::vector< clang::QualType > types;
            for( const interface::Element* pTarget : returnTypes )
            {
                clang::DeclContext* pDeclContextIter = g_pASTContext->getTranslationUnitDecl();
                const std::vector< const interface::Element* > path = getPath( pTarget );
                for( const interface::Element* pElementElement : path )
                {
                    if( pElementElement == path.back() )
                    {
                        clang::QualType variantType = clang::getType( g_pASTContext, g_pSema,
                            getInterfaceType( pElementElement->getIdentifier() ), "void",
                            pDeclContextIter, loc, true );
                        types.push_back( variantType );
                    }
                    else
                    {
                        clang::getType( g_pASTContext, g_pSema,
                            getInterfaceType( pElementElement->getIdentifier() ), "void",
                            pDeclContextIter, loc, false );
                        if( !pDeclContextIter ) break;
                    }
                }
            }
            //construct the variant result type
            clang::SourceLocation loc;
            return clang::getVariantType( g_pASTContext, g_pSema,
                g_pASTContext->getTranslationUnitDecl(), loc, types );
        }
        return std::optional< clang::QualType >();
    }*/

    /*
    void calculateReturnType( const InvocationSolution* pSolution, clang::QualType& resultType )
    {
        //establish the return type
        clang::DeclContext* pDeclContext = g_pASTContext->getTranslationUnitDecl();
        const InvocationSolution::Context& returnTypes = pSolution->getReturnTypes();

        clang::SourceLocation loc;
        const mega::OperationID operationTypeID = pSolution->getOperation();
        switch( operationTypeID )
        {
            case id_Imp_NoParams    :
            case id_Imp_Params      :
                {
                    if( !pSolution->isReturnTypeDimensions() )
                    {
                        if( std::optional< clang::QualType > resultOpt =
                                buildActionReturnType( returnTypes, pDeclContext, loc ) )
                        {
                            resultType = resultOpt.value();
                        }
                    }
                    else if( operationTypeID == id_Imp_NoParams )
                    {
                        const interface::Element* pTarget = returnTypes.front();
                        clang::DeclContext* pDeclContextIter = pDeclContext;
                        const std::vector< const interface::Element* > path = getPath( pTarget );
                        for( const interface::Element* pElementElement : path )
                        {
                            clang::getType( g_pASTContext, g_pSema,
                                getInterfaceType( pElementElement->getIdentifier() ), "void",
                                pDeclContextIter, loc, false );
                            if( !pDeclContextIter ) break;
                        }
                        if( pDeclContextIter )
                            resultType = clang::getTypeTrait( g_pASTContext, g_pSema, pDeclContextIter, loc, "Read" );
                    }
                    else if( operationTypeID == id_Imp_Params )
                    {
                        //resultType = clang::getVoidType( g_pASTContext );

                        if( std::optional< clang::QualType > resultOpt =
                                buildActionReturnType( returnTypes, pDeclContext, loc ) )
                        {
                            resultType = resultOpt.value();
                        }

                        //if( pSolution->isReturnTypeDimensions() )
                        //{
                        //    ASSERT( returnTypes.size() == 1 );
                        //    const interface::Element* pTarget = returnTypes.front();
                        //    clang::DeclContext* pDeclContextIter = pDeclContext;
                        //    const std::vector< const interface::Element* > path = getPath( pTarget );
                        //    for( const interface::Element* pElementElement : path )
                        //    {
                        //        clang::getType( g_pASTContext, g_pSema,
                        //            getInterfaceType( pElementElement->getIdentifier() ), "void",
                        //            pDeclContextIter, loc, false );
                        //        if( !pDeclContextIter ) break;
                        //    }
                        //    if( pDeclContextIter )
                        //        resultType = clang::getTypeTrait( g_pASTContext, g_pSema, pDeclContextIter, loc, "Get"
    );
                        //}
                        //else
                        //{
                        //}


                    }
                    else
                    {
                        //error
                    }
                }
                break;
            case id_Start        :
                {
                    if( std::optional< clang::QualType > resultOpt =
                            buildActionReturnType( returnTypes, pDeclContext, loc ) )
                    {
                        resultType = resultOpt.value();
                    }
                    else
                    {
                        //error
                    }
                }
                break;
            case id_Stop       :
                {
                    resultType = clang::getVoidType( g_pASTContext );
                }
                break;
            case id_Pause      :
                {
                    resultType = clang::getVoidType( g_pASTContext );
                }
                break;
            case id_Resume     :
                {
                    resultType = clang::getVoidType( g_pASTContext );
                }
                break;
            case id_Wait       :
                {
                    if( pSolution->isReturnTypeDimensions() )
                    {
                        ASSERT( returnTypes.size() == 1 );
                        const interface::Element* pTarget = returnTypes.front();
                        clang::DeclContext* pDeclContextIter = pDeclContext;
                        const std::vector< const interface::Element* > path = getPath( pTarget );
                        for( const interface::Element* pElementElement : path )
                        {
                            clang::getType( g_pASTContext, g_pSema,
                                getInterfaceType( pElementElement->getIdentifier() ), "void",
                                pDeclContextIter, loc, false );
                            if( !pDeclContextIter ) break;
                        }
                        if( pDeclContextIter )
                            resultType = clang::getTypeTrait( g_pASTContext, g_pSema, pDeclContextIter, loc, "Read" );
                    }
                    else
                    {
                        if( std::optional< clang::QualType > resultOpt =
                                buildActionReturnType( returnTypes, pDeclContext, loc ) )
                        {
                            resultType = resultOpt.value();
                        }
                    }
                }
                break;
            case id_Get        :
                {
                    if( pSolution->isReturnTypeDimensions() )
                    {
                        ASSERT( returnTypes.size() == 1 );
                        const interface::Element* pTarget = returnTypes.front();
                        clang::DeclContext* pDeclContextIter = pDeclContext;
                        const std::vector< const interface::Element* > path = getPath( pTarget );
                        for( const interface::Element* pElementElement : path )
                        {
                            clang::getType( g_pASTContext, g_pSema,
                                getInterfaceType( pElementElement->getIdentifier() ), "void",
                                pDeclContextIter, loc, false );
                            if( !pDeclContextIter ) break;
                        }
                        if( pDeclContextIter )
                            resultType = clang::getTypeTrait( g_pASTContext, g_pSema, pDeclContextIter, loc, "Get" );
                    }
                    else
                    {
                        if( std::optional< clang::QualType > resultOpt =
                                buildActionReturnType( returnTypes, pDeclContext, loc ) )
                        {
                            resultType = resultOpt.value();
                        }
                    }
                }
                break;
            case id_Done     :
                {
                    resultType = clang::getBooleanType( g_pASTContext );
                }
                break;
            case id_Range      :
                if( !returnTypes.empty() )
                {
                    if( pSolution->getRoot()->getMaxRanges() == 1 )
                    {
                        if( std::optional< clang::QualType > resultOpt =
                                buildActionReturnType( returnTypes, pDeclContext, loc ) )
                        {
                            resultType = clang::getIteratorRangeType( g_pASTContext, g_pSema,
                                g_pASTContext->getTranslationUnitDecl(),
                                loc, resultOpt.value(), mega::EG_REFERENCE_ITERATOR_TYPE );
                        }
                    }
                    else
                    {
                        if( std::optional< clang::QualType > resultOpt =
                                buildActionReturnType( returnTypes, pDeclContext, loc ) )
                        {
                            resultType = clang::getMultiIteratorRangeType( g_pASTContext, g_pSema,
                                g_pASTContext->getTranslationUnitDecl(),
                                loc, resultOpt.value(), pSolution->getRoot()->getMaxRanges(),
    mega::EG_REFERENCE_ITERATOR_TYPE );
                        }
                    }
                }
                break;
            case id_Raw      :
                if( !returnTypes.empty() )
                {
                    if( pSolution->getRoot()->getMaxRanges() == 1 )
                    {
                        if( std::optional< clang::QualType > resultOpt =
                                buildActionReturnType( returnTypes, pDeclContext, loc ) )
                        {
                            resultType = clang::getIteratorRangeType( g_pASTContext, g_pSema,
                                g_pASTContext->getTranslationUnitDecl(),
                                loc, resultOpt.value(), mega::EG_REFERENCE_RAW_ITERATOR_TYPE );
                        }
                    }
                    else
                    {
                        if( std::optional< clang::QualType > resultOpt =
                                buildActionReturnType( returnTypes, pDeclContext, loc ) )
                        {
                            resultType = clang::getMultiIteratorRangeType( g_pASTContext, g_pSema,
                                g_pASTContext->getTranslationUnitDecl(),
                                loc, resultOpt.value(), pSolution->getRoot()->getMaxRanges(),
    mega::EG_REFERENCE_RAW_ITERATOR_TYPE );
                        }
                    }
                }
                break;
            default:
                break;
        }
    }*/

    // warn_eg_generic_warning
    // err_eg_generic_error
    virtual bool getInvocationResultType( const clang::SourceLocation& loc, const clang::QualType& type,
                                          clang::QualType& resultType )
    {
        /*if( g_pOperationsSession )
        {
            if( type.getTypePtrOrNull()  )
            {
                if( !type->isDependentType() )
                {
                    if( const clang::IdentifierInfo* pBaseTypeID = type.getBaseTypeIdentifier() )
                    {
                        if( pBaseTypeID == g_pASTContext->getEGInvocationTypeName() )
                        {
                            if( const clang::TemplateSpecializationType* pTemplateType =
                                type->getAs< clang::TemplateSpecializationType >() )
                            {
                                if( pTemplateType->getNumArgs() == 3U )
                                {
                                    clang::QualType context         = pTemplateType->getArg( 0U ).getAsType();
                                    clang::QualType typePath        = pTemplateType->getArg( 1U ).getAsType();
                                    clang::QualType operationType   = pTemplateType->getArg( 2U ).getAsType();

                                    std::vector< mega::TypeID > contextTypes;
                                    if( !clang::getContextTypes( g_pASTContext, context, contextTypes ) )
                                    {
                                        std::ostringstream os;
                                        os << "Invalid context for invocation of type: " +
        context.getCanonicalType().getAsString(); g_pASTContext->getDiagnostics().Report( loc,
        clang::diag::err_eg_generic_error ) << os.str(); g_bError = true; return false;
                                    }

                                    std::vector< mega::TypeID > typePathTypes;
                                    if( !clang::getTypePathTypes( g_pASTContext, typePath, typePathTypes ) )
                                    {
                                        g_pASTContext->getDiagnostics().Report( loc, clang::diag::err_eg_generic_error )
        << "Invalid type path for invocation"; g_bError = true; return false;
                                    }

                                    std::optional< mega::TypeID > operationTypeIDOpt = getEGTypeID( g_pASTContext,
        operationType ); if( !operationTypeIDOpt )
                                    {
                                        g_pASTContext->getDiagnostics().Report( loc, clang::diag::err_eg_generic_error )
        << "Invalid operation for invocation"; g_bError = true; return false;
                                    }
                                    mega::OperationID operationTypeID = static_cast< mega::OperationID >(
        operationTypeIDOpt.value() );

                                    const InvocationSolution::InvocationID invocationID =
                                        InvocationSolution::invocationIDFromTypeIDs(
                                            g_pOperationsSession->getObjects( IndexedObject::MASTER_FILE ),
                                            g_pOperationsSession->getIdentifiers(),
                                            contextTypes, typePathTypes, operationTypeID );

                                    try
                                    {
                                        if( const InvocationSolution* pSolution =
                                                g_pOperationsSession->getInvocation( invocationID, typePathTypes ) )
                                        {
                                            calculateReturnType( pSolution, resultType );
                                        }
                                    }
                                    catch( mega::NameResolutionException& nameResolutionException )
                                    {
                                        g_pASTContext->getDiagnostics().Report( loc, clang::diag::err_eg_generic_error )
        << nameResolutionException.what(); g_bError = true; return false;
                                    }
                                    catch( mega::InvocationException& invocationException )
                                    {
                                        g_pASTContext->getDiagnostics().Report( loc, clang::diag::err_eg_generic_error )
        << invocationException.what(); g_bError = true; return false;
                                    }

                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }*/
        return false;
    }
};
} // namespace mega

void* GET_EG_PLUGIN_INTERFACE()
{
    static ::mega::EG_PLUGIN_INTERFACE_IMPL impl;
    return &impl;
}
