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

#include "database/model/InterfaceAnalysisStage.hxx"

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
using namespace InterfaceAnalysisStage;
using namespace InterfaceAnalysisStage::Interface;

class InterfaceSession : public AnalysisSession
{
    InterfaceAnalysisStage::Database m_database;

    using SymbolTypeIDMap    = std::map< mega::TypeID, Symbols::SymbolTypeID* >;
    using InterfaceTypeIDMap = std::map< mega::TypeID, Symbols::InterfaceTypeID* >;
    using MangleMap          = std::unordered_map< std::string, MegaMangle::Mangle* >;

    std::map< std::string, Symbols::SymbolTypeID* > m_symbols;
    SymbolTypeIDMap                                 m_symbolIDs;
    InterfaceTypeIDMap                              m_interfaceTypeIDs;
    MangleMap                                       m_mangleMap;

public:
    InterfaceSession( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir,
                      const char* strSourceFile )
        : AnalysisSession( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile )
        , m_database( m_environment, m_sourceFile )
    {
        Symbols::SymbolTable* pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        m_symbols          = pSymbolTable->get_symbol_names();
        m_symbolIDs        = pSymbolTable->get_symbol_type_ids();
        m_interfaceTypeIDs = pSymbolTable->get_interface_type_ids();

        for( auto pMangle : m_database.many< MegaMangle::Mangle >( m_environment.project_manifest() ) )
        {
            m_mangleMap.insert( { pMangle->get_canon(), pMangle } );
        }
    }

    virtual bool isPossibleEGTypeIdentifier( const std::string& strIdentifier ) const
    {
        if( AnalysisSession::isPossibleEGTypeIdentifier( strIdentifier ) )
        {
            return true;
        }

        if( m_symbols.find( strIdentifier ) != m_symbols.end() )
            return true;
        return false;
    }

    template < typename TContextType >
    bool argumentAnalysis( TContextType* pContext, SourceLocation loc, CXXRecordDecl* pCXXRecordDecl )
    {
        CXXMethodDecl* pApplicationOperator = nullptr;
        for( CXXRecordDecl::method_iterator i = pCXXRecordDecl->method_begin(), iEnd = pCXXRecordDecl->method_end();
             i != iEnd; ++i )
        {
            CXXMethodDecl* pMethod = *i;
            if( pMethod->getNameInfo().getName().getAsString() == "operator()" )
            {
                pApplicationOperator = pMethod;
                break;
            }
        }

        if( !pApplicationOperator )
        {
            // std::ostringstream os;
            // os << "Failed to locate interface member operator() for " << pContext->getFriendlyName();
            // pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            return false;
        }
        else
        {
            std::vector< std::string > canonicalTypes;
            for( auto param : pApplicationOperator->parameters() )
            {
                canonicalTypes.push_back( param->getType().getCanonicalType().getAsString() );
            }
            m_database.construct< Interface::ArgumentListTrait >(
                Interface::ArgumentListTrait::Args{ pContext->get_arguments_trait(), canonicalTypes } );
        }
        return true;
    }

    template < typename TContextType >
    bool returnTypeAnalysis( TContextType* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        DeclLocType returnTypeResult
            = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, ::mega::EG_FUNCTION_TRAIT_TYPE );
        if( !returnTypeResult.pDeclContext )
        {
            return false;
        }
        else
        {
            // determine the type
            QualType typeType
                = getTypeTrait( pASTContext, pSema, returnTypeResult.pDeclContext, returnTypeResult.loc, "Type" );
            QualType typeTypeCanonical = typeType.getCanonicalType();

            m_database.construct< Interface::ReturnTypeTrait >( Interface::ReturnTypeTrait::Args{
                pContext->get_return_type_trait(), std::string{ typeTypeCanonical.getAsString() } } );
        }

        return true;
    }
    template < typename TContextType >
    bool dimensionAnalysis( TContextType* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        for( Interface::DimensionTrait* pDimensionTrait : pContext->get_dimension_traits() )
        {
            DeclLocType dimensionResult
                = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, pDimensionTrait->get_id()->get_str() );
            if( dimensionResult.pDeclContext )
            {
                // determine the type
                std::string                           strCanonicalType;
                std::vector< Symbols::SymbolTypeID* > symbols;
                MegaMangle::Mangle*                   pMangle = nullptr;
                {
                    QualType typeType
                        = getTypeTrait( pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc, "Type" );
                    QualType typeTypeCanonical = typeType.getCanonicalType();
                    // std::vector< mega::TypeID > dimensionTypes;

                    strCanonicalType = typeTypeCanonical.getAsString();

                    auto iFind = m_mangleMap.find( strCanonicalType );
                    VERIFY_RTE_MSG( iFind != m_mangleMap.end(),
                                    "Failed to locate mangle for canonical type: "
                                        << strCanonicalType
                                        << " for dimension: " << pDimensionTrait->get_interface_id() );
                    pMangle = iFind->second;

                    // only attempt this is it has a base type identifier
                    /*if( typeTypeCanonical.getBaseTypeIdentifier() )
                    {
                        getContextSymbolIDs( pASTContext, typeTypeCanonical, dimensionTypes );
                    }
                    if( dimensionTypes.empty()
                        || ( ( dimensionTypes.size() == 1U ) && ( dimensionTypes.front() == 0 ) ) )
                    {
                        strCanonicalType = typeTypeCanonical.getAsString();
                    }
                    else if( !dimensionTypes.empty() )
                    {
                        for( mega::TypeID symbolID : dimensionTypes )
                        {
                            auto iFind = m_symbolIDs.find( symbolID );
                            if( iFind != m_symbolIDs.end() )
                            {
                                Symbols::SymbolTypeID* pSymbol = iFind->second;
                                symbols.push_back( pSymbol );
                            }
                            else
                            {
                                // may be Interface reference type
                                auto iFind2 = m_interfaceTypeIDs.find( symbolID );
                                if( iFind2 != m_interfaceTypeIDs.end() )
                                {
                                    Symbols::InterfaceTypeID* pInterfaceTypeID = iFind2->second;
                                    auto iFind3 = m_symbolIDs.find( pInterfaceTypeID->get_symbol_ids().front() );
                                    if( iFind3 != m_symbolIDs.end() )
                                    {
                                        Symbols::SymbolTypeID* pSymbol = iFind3->second;
                                        symbols.push_back( pSymbol );
                                    }
                                    // const auto typeIDSeq = pInterfaceTypeID->get_symbol_ids();
                                    //  symbols.push_back( typeIDSeq.front() );
                                }
                                else
                                {
                                    // diag
                                    std::ostringstream os;
                                    os << "Invalid dimension type: " << pDimensionTrait->get_id()->get_str();
                                    pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error )
                                        << os.str();
                                    return false;
                                }
                            }
                        }
                    }*/
                }
                if( strCanonicalType.empty() && symbols.empty() )
                {
                    THROW_RTE( "Failed to determine dimension type" );
                }

                // determine the erased type
                std::string strErasedType;
                {
                    QualType typeType = getTypeTrait(
                        pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc, "Erased" );
                    QualType typeTypeCanonical = typeType.getCanonicalType();
                    strErasedType              = typeTypeCanonical.getAsString();
                }
                if( strErasedType.empty() )
                {
                    THROW_RTE( "Failed to determine erased type" );
                }

                // determine the size
                ::mega::U64 szSize = 0U;
                if( std::optional< ::mega::U64 > sizeOpt
                    = getConstant( pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc,
                                   ::mega::EG_TRAITS_SIZE ) )
                {
                    szSize = static_cast< ::mega::U64 >( sizeOpt.value() );
                }
                else
                {
                    THROW_RTE( "Error attempting to record dimension size" );
                }

                // determine the alignment
                ::mega::U64 szAlignment = 0U;
                if( std::optional< ::mega::U64 > alignmentOpt
                    = getConstant( pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc,
                                   ::mega::EG_TRAITS_ALIGNMENT ) )
                {
                    szAlignment = static_cast< ::mega::U64 >( alignmentOpt.value() );
                }
                else
                {
                    THROW_RTE( "Error attempting to record dimension size" );
                }

                // determine if the type is simple
                bool bIsSimple = true;
                if( std::optional< ::mega::U64 > sizeOpt
                    = getConstant( pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc,
                                   ::mega::EG_TRAITS_SIMPLE ) )
                {
                    bIsSimple = static_cast< bool >( sizeOpt.value() );
                }
                else
                {
                    THROW_RTE( "Error attempting to record if dimension is simple" );
                }

                m_database.construct< Interface::DimensionTrait >(
                    Interface::DimensionTrait::Args{ pDimensionTrait, strCanonicalType, pMangle, strErasedType, szSize,
                                                     szAlignment, bIsSimple, symbols } );
            }
            else
            {
                THROW_RTE( "Error attempting to resolve dimension type" );
            }
        }
        return true;
    }

    template < typename TContextType >
    bool sizeAnalysis( TContextType* pContext, Interface::SizeTrait* pSizeTrait, SourceLocation loc,
                       DeclContext* pDeclContext )
    {
        DeclLocType dimensionResult
            = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, ::mega::EG_SIZE_PREFIX_TRAIT_TYPE );
        if( dimensionResult.pDeclContext )
        {
            if( std::optional< ::mega::U64 > sizeOpt
                = getConstant( pASTContext, pSema, dimensionResult.pDeclContext, loc, ::mega::EG_TRAITS_SIZE ) )
            {
                m_database.construct< Interface::SizeTrait >(
                    Interface::SizeTrait::Args{ pSizeTrait, sizeOpt.value() } );
                return true;
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

    template < typename TContextType >
    bool inheritanceAnalysis( TContextType* pContext, InheritanceTrait* pInheritanceTrait, SourceLocation loc,
                              DeclContext* pDeclContext )
    {
        VERIFY_RTE( pInheritanceTrait );

        std::vector< IContext* > inheritedContexts;

        int iCounter = 0;
        for( const std::string& strBaseType : pInheritanceTrait->get_strings() )
        {
            std::string strTraitsType;
            {
                std::ostringstream os;
                os << mega::EG_BASE_PREFIX_TRAIT_TYPE << iCounter++;
                strTraitsType = os.str();
            }

            DeclLocType traitDecl = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, strTraitsType );
            if( traitDecl.pDeclContext )
            {
                DeclContext* pTypeDeclContext = traitDecl.pDeclContext;
                QualType     typeType = getTypeTrait( pASTContext, pSema, pTypeDeclContext, traitDecl.loc, "Type" );
                if( pTypeDeclContext )
                {
                    QualType typeTypeCanonical = typeType.getCanonicalType();
                    if( std::optional< mega::TypeID > inheritanceTypeIDOpt
                        = getMegaTypeID( pASTContext, typeTypeCanonical ) )
                    {
                        auto iFind = m_interfaceTypeIDs.find( inheritanceTypeIDOpt.value() );
                        if( iFind != m_interfaceTypeIDs.end() )
                        {
                            if( !iFind->second->get_context().has_value() )
                            {
                                std::ostringstream os;
                                os << "Invalid inherited non-context type " << pContext->get_identifier() << "("
                                   << pContext->get_interface_id() << ") of: " << typeTypeCanonical.getAsString();
                                pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                                return false;
                            }
                            inheritedContexts.push_back( iFind->second->get_context().value() );
                        }
                        else
                        {
                            // error invalid inheritance type
                            std::ostringstream os;
                            os << "Invalid inheritance type for " << pContext->get_identifier() << "("
                               << pContext->get_interface_id() << ") of: " << typeTypeCanonical.getAsString();
                            pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                            return false;
                        }
                    }
                    else
                    {
                        std::ostringstream os;
                        os << "Failed to resolve inheritance for " << pContext->get_identifier() << "("
                           << pContext->get_interface_id() << ") of: " << strBaseType;
                        pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                        return false;
                    }
                }
                else
                {
                    std::ostringstream os;
                    os << "Failed to resolve inheritance target type for " << pContext->get_identifier() << "("
                       << pContext->get_interface_id() << ") of: " << strBaseType;
                    pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                    return false;
                }
            }
            else
            {
                std::ostringstream os;
                os << "Failed to resolve inheritance for " << pContext->get_identifier() << "("
                   << pContext->get_interface_id() << ") of: " << strBaseType;
                pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                return false;
            }
        }

        pInheritanceTrait = m_database.construct< InheritanceTrait >(
            InheritanceTrait::Args( pInheritanceTrait, inheritedContexts ) );

        return true;
    }

    template < typename TContextType >
    bool usingAnalysis( TContextType* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        // determine the using types
        /*std::vector< ::mega::TypeIDinterface::Using* > usings;
        pContext->getUsings( usings );
        for ( ::mega::TypeIDinterface::Using* pUsing : usings )
        {
            QualType typeType
                = getTypeTrait( pASTContext, pSema, result.pDeclContext, result.loc, pUsing->getIdentifier() );

            QualType typeTypeCanonical = typeType.getCanonicalType();

            AbstractMutator::setCanonicalType( *pUsing, typeTypeCanonical.getAsString() );
        }*/
        return true;
    }

    bool interfaceAnalysis( IContext* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        DeclLocType result = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, pContext->get_identifier() );
        if( nullptr == result.pDeclContext )
        {
            std::ostringstream os;
            os << "Unknown context type: " << pContext->get_identifier() << "(" << pContext->get_interface_id() << ")";
            pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            return false;
        }
        bool bProcess = false;
        {
            if( auto pNamespace = db_cast< Namespace >( pContext ) )
            {
                if( !pNamespace->get_is_global() )
                {
                    dimensionAnalysis( pNamespace, result.loc, result.pDeclContext );
                }
                bProcess = true;
            }
        }
        {
            if( auto pAbstract = db_cast< Abstract >( pContext ) )
            {
                dimensionAnalysis( pAbstract, result.loc, result.pDeclContext );

                if( std::optional< InheritanceTrait* > inheritanceOpt = pAbstract->get_inheritance_trait() )
                {
                    if( !inheritanceAnalysis( pAbstract, inheritanceOpt.value(), result.loc, result.pDeclContext ) )
                        return false;
                }
                if( std::optional< Interface::SizeTrait* > sizeOpt = pAbstract->get_size_trait() )
                {
                    if( !sizeAnalysis( pAbstract, sizeOpt.value(), result.loc, result.pDeclContext ) )
                        return false;
                }
                bProcess = true;
            }
        }
        {
            if( auto pAction = db_cast< Action >( pContext ) )
            {
                dimensionAnalysis( pAction, result.loc, result.pDeclContext );

                if( std::optional< InheritanceTrait* > inheritanceOpt = pAction->get_inheritance_trait() )
                {
                    if( !inheritanceAnalysis( pAction, inheritanceOpt.value(), result.loc, result.pDeclContext ) )
                        return false;
                }
                if( std::optional< Interface::SizeTrait* > sizeOpt = pAction->get_size_trait() )
                {
                    if( !sizeAnalysis( pAction, sizeOpt.value(), result.loc, result.pDeclContext ) )
                        return false;
                }
                bProcess = true;
            }
        }
        {
            if( auto pEvent = db_cast< Event >( pContext ) )
            {
                dimensionAnalysis( pEvent, result.loc, result.pDeclContext );

                if( std::optional< InheritanceTrait* > inheritanceOpt = pEvent->get_inheritance_trait() )
                {
                    if( !inheritanceAnalysis( pEvent, inheritanceOpt.value(), result.loc, result.pDeclContext ) )
                        return false;
                }
                if( std::optional< Interface::SizeTrait* > sizeOpt = pEvent->get_size_trait() )
                {
                    if( !sizeAnalysis( pEvent, sizeOpt.value(), result.loc, result.pDeclContext ) )
                        return false;
                }
                bProcess = true;
            }
        }
        {
            if( auto pInterupt = db_cast< Interupt >( pContext ) )
            {
                

                bProcess = true;
            }
        }
        {
            if( auto pFunction = db_cast< Function >( pContext ) )
            {
                auto pCXXRecordDecl = dyn_cast< CXXRecordDecl >( result.pDeclContext );
                if( !pCXXRecordDecl )
                {
                    std::ostringstream os;
                    os << "Invalid function context type: " << pContext->get_identifier() << "("
                       << pContext->get_interface_id() << ")";
                    pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                    return false;
                }
                argumentAnalysis( pFunction, result.loc, pCXXRecordDecl );
                returnTypeAnalysis( pFunction, result.loc, pCXXRecordDecl );
                bProcess = true;
            }
        }
        {
            if( auto pObject = db_cast< Object >( pContext ) )
            {
                dimensionAnalysis( pObject, result.loc, result.pDeclContext );

                if( std::optional< InheritanceTrait* > inheritanceOpt = pObject->get_inheritance_trait() )
                {
                    if( !inheritanceAnalysis( pObject, inheritanceOpt.value(), result.loc, result.pDeclContext ) )
                        return false;
                }
                if( std::optional< Interface::SizeTrait* > sizeOpt = pObject->get_size_trait() )
                {
                    if( !sizeAnalysis( pObject, sizeOpt.value(), result.loc, result.pDeclContext ) )
                        return false;
                }
                bProcess = true;
            }
        }
        {
            if( auto pLink = db_cast< Link >( pContext ) )
            {
                if( !inheritanceAnalysis( pLink, pLink->get_link_interface(), result.loc, result.pDeclContext ) )
                    return false;
                bProcess = true;
            }
        }

        if( !bProcess )
        {
            std::ostringstream os;
            os << "Unknown context type: " << pContext->get_identifier() << "(" << pContext->get_interface_id() << ")";
            pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            return false;
        }

        for( IContext* pContextChild : pContext->get_children() )
        {
            if( !interfaceAnalysis( pContextChild, result.loc, result.pDeclContext ) )
                return false;
        }

        return true;
    }

    virtual void runFinalAnalysis()
    {
        bool bSuccess = true;
        {
            SourceLocation loc;
            DeclContext*   pDeclContext = pASTContext->getTranslationUnitDecl();

            Interface::Root* pRoot = m_database.one< Interface::Root >( m_sourceFile );
            for( IContext* pContext : pRoot->get_children() )
            {
                if( !interfaceAnalysis( pContext, loc, pDeclContext ) )
                {
                    bSuccess = false;
                    break;
                }
            }
        }

        if( bSuccess )
        {
            const mega::io::CompilationFilePath interfaceAnalysisFile
                = m_environment.InterfaceAnalysisStage_Clang( m_sourceFile );
            m_database.save_Clang_to_temp();
            m_environment.temp_to_real( interfaceAnalysisFile );
        }
    }
};

Session::Ptr make_interface_session( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir,
                                     const char* strBuildDir, const char* strSourceFile )
{
    return std::make_unique< InterfaceSession >( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile );
}

} // namespace clang
