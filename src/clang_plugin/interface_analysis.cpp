

#include "session.hpp"
#include "clang_utils.hpp"

#include "database/model/InterfaceAnalysisStage.hxx"

#include "mega/common_strings.hpp"

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

#pragma warning( pop )

#include <map>

namespace clang
{

class InterfaceSession : public AnalysisSession
{
    InterfaceAnalysisStage::Database                                          m_database;
    std::map< std::string, ::InterfaceAnalysisStage::Symbols::Symbol* >       m_symbols;
    std::map< int32_t, ::InterfaceAnalysisStage::Interface::Context* >        m_contextTypeIDs;
    std::map< int32_t, ::InterfaceAnalysisStage::Interface::DimensionTrait* > m_dimensionTypeIDs;

public:
    InterfaceSession( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir,
                      const char* strSourceFile )
        : AnalysisSession( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile )
        , m_database( m_environment, m_sourceFile )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        Symbols::SymbolTable* pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        m_symbols          = pSymbolTable->get_symbols();
        m_contextTypeIDs   = pSymbolTable->get_context_type_ids();
        m_dimensionTypeIDs = pSymbolTable->get_dimension_type_ids();
    }

    virtual bool isPossibleEGTypeIdentifier( const std::string& strIdentifier ) const
    {
        if ( AnalysisSession::isPossibleEGTypeIdentifier( strIdentifier ) )
        {
            return true;
        }

        if ( m_symbols.find( strIdentifier ) != m_symbols.end() )
            return true;
        return false;
    }

    template < typename TContextType >
    bool argumentListAnalysis( TContextType* pContext, SourceLocation loc, CXXRecordDecl* pCXXRecordDecl )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        CXXMethodDecl* pApplicationOperator = nullptr;
        for ( CXXRecordDecl::method_iterator i = pCXXRecordDecl->method_begin(), iEnd = pCXXRecordDecl->method_end();
              i != iEnd; ++i )
        {
            CXXMethodDecl* pMethod = *i;
            if ( pMethod->getNameInfo().getName().getAsString() == "operator()" )
            {
                pApplicationOperator = pMethod;
                break;
            }
        }

        if ( !pApplicationOperator )
        {
            // std::ostringstream os;
            // os << "Failed to locate interface member operator() for " << pContext->getFriendlyName();
            // pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            return false;
        }
        else
        {
            // AbstractMutator::setParameters( *pContext, pApplicationOperator->parameters() );
        }

        return true;
    }

    template < typename TContextType >
    bool dimensionAnalysis( TContextType* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;
        for ( Interface::DimensionTrait* pDimension : pContext->get_dimension_traits() )
        {
            /*DeclLocType dimensionResult
                = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, pDimension->get_id()->get_str(), true );
            if ( dimensionResult.pDeclContext )
            {
                // determine the type
                {
                    QualType typeType
                        = getTypeTrait( pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc, "Type" );

                    QualType typeTypeCanonical = typeType.getCanonicalType();

                    std::vector< mega::TypeID > dimensionTypes;

                    // only attempt this is it has a base type identifier
                    if ( typeTypeCanonical.getBaseTypeIdentifier() )
                    {
                        getContextTypes( pASTContext, typeTypeCanonical, dimensionTypes );
                    }

                    if ( dimensionTypes.empty()
                            || ( ( dimensionTypes.size() == 1U ) && ( dimensionTypes.front() == 0 ) ) )
                    {
                        //AbstractMutator::setCanonicalType( *pDimension, typeTypeCanonical.getAsString() );
                    }
                    else if ( !dimensionTypes.empty() )
                    {
                        for ( mega::index : dimensionTypes )
                        {
                            if ( index > 0 && index < static_cast< ::mega::TypeID >( objects.size() ) )
                            {
                                ::mega::TypeIDIndexedObject* pObject = objects[ index ];
                                if ( ::mega::TypeIDinterface::Context* pContext
                                        = dynamic_cast< ::mega::TypeIDinterface::Context* >( pObject ) )
                                {
                                    //AbstractMutator::appendActionTypes( *pDimension, pContext );
                                }
                            }
                            else
                            {
                                // diag
                                std::ostringstream os;
                                os << "Invalid dimension type: " << pDimension->get_id()->get_str();
                                pASTContext->getDiagnostics().Report( clang::diag::err_eg_generic_error )
                                    << os.str();
                                return false;
                            }
                        }
                    }
                }
                // determine the size
                if ( std::optional< std::size_t > sizeOpt
                        = getConstant( pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc,
                                    ::mega::TypeIDEG_TRAITS_SIZE ) )
                {
                    //AbstractMutator::setSize( *pDimension, static_cast< std::size_t >( sizeOpt.value() ) );
                }
                else
                {
                    THROW_RTE( "Error attempting to record dimension size" );
                }

                // determine if the type is simple
                if ( std::optional< std::size_t > sizeOpt
                        = getConstant( pASTContext, pSema, dimensionResult.pDeclContext, dimensionResult.loc,
                                    ::mega::TypeIDEG_TRAITS_SIMPLE ) )
                {
                    //AbstractMutator::setSimple( *pDimension, static_cast< std::size_t >( sizeOpt.value() ) );
                }
            }*/
        }
        return true;
    }

    template < typename TContextType >
    bool sizeAnalysis( TContextType* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        /*if ( std::optional< std::size_t > sizeOpt
             = getConstant( pASTContext, pSema, pDeclContext, loc, ::mega::EG_TRAITS_SIZE ) )
        {
            // AbstractMutator::setSize( *pContext, static_cast< std::size_t >( sizeOpt.value() ) );
        }*/

        return true;
    }

    template < typename TContextType >
    bool returnTypeAnalysis( TContextType* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        /*if ( std::optional< std::size_t > sizeOpt
             = getConstant( pASTContext, pSema, pDeclContext, loc, ::mega::EG_TRAITS_SIZE ) )
        {
            // AbstractMutator::setSize( *pContext, static_cast< std::size_t >( sizeOpt.value() ) );
        }*/

        return true;
    }

    template < typename TContextType >
    bool inheritanceAnalysis( TContextType* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        std::optional< InheritanceTrait* > pInheritanceOpt = pContext->get_inheritance_trait();
        if ( pInheritanceOpt.has_value() )
        {
            InheritanceTrait* pInheritanceTrait = pInheritanceOpt.value();
            VERIFY_RTE( pInheritanceTrait );

            std::vector< Context* > inheritedContexts;

            int iCounter = 0;
            for ( const std::string& strBaseType : pInheritanceTrait->get_strings() )
            {
                std::string strTraitsType;
                {
                    std::ostringstream os;
                    os << mega::EG_BASE_PREFIX_TRAIT_TYPE << iCounter++;
                    strTraitsType = os.str();
                }

                DeclLocType traitDecl = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, strTraitsType );
                if ( traitDecl.pDeclContext )
                {
                    QualType typeType
                        = getTypeTrait( pASTContext, pSema, traitDecl.pDeclContext, traitDecl.loc, "Type" );
                    QualType typeTypeCanonical = typeType.getCanonicalType();
                    if ( std::optional< mega::TypeID > inheritanceTypeIDOpt
                         = getEGTypeID( pASTContext, typeTypeCanonical ) )
                    {
                        auto iFind = m_contextTypeIDs.find( inheritanceTypeIDOpt.value() );
                        if ( iFind != m_contextTypeIDs.end() )
                        {
                            inheritedContexts.push_back( iFind->second );
                        }
                        else
                        {
                            // error invalid inheritance type
                            std::ostringstream os;
                            os << "Invalid inheritance type for " << pContext->get_identifier() << "("
                               << pContext->get_type_id() << ") of: " << typeTypeCanonical.getAsString();
                            pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                            return false;
                        }
                    }
                    else
                    {
                        std::ostringstream os;
                        os << "Failed to resolve inheritance for " << pContext->get_identifier() << "("
                           << pContext->get_type_id() << ") of: " << strBaseType;
                        pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                        return false;
                    }
                }
                else
                {
                    std::ostringstream os;
                    os << "Failed to resolve inheritance for " << pContext->get_identifier() << "("
                       << pContext->get_type_id() << ") of: " << strBaseType;
                    pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                    return false;
                }
            }

            pInheritanceTrait = m_database.construct< InheritanceTrait >(
                InheritanceTrait::Args( pInheritanceTrait, inheritedContexts ) );
        }
        return true;
    }

    template < typename TContextType >
    bool usingAnalysis( TContextType* pContext, SourceLocation loc, DeclContext* pDeclContext )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;
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

    bool interfaceAnalysis( InterfaceAnalysisStage::Interface::Context* pContext, SourceLocation loc,
                            DeclContext* pDeclContext )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        DeclLocType result = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, pContext->get_identifier() );
        if ( nullptr == result.pDeclContext )
        {
            std::ostringstream os;
            os << "Unknown context type: " << pContext->get_identifier() << "(" << pContext->get_type_id() << ")";
            pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            return false;
        }

        if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
        {
            if ( !pNamespace->get_is_global() )
            {
                dimensionAnalysis( pNamespace, result.loc, result.pDeclContext );
            }
        }
        else if ( Abstract* pAbstract = dynamic_database_cast< Abstract >( pContext ) )
        {
            dimensionAnalysis( pAbstract, result.loc, result.pDeclContext );
            inheritanceAnalysis( pAbstract, result.loc, result.pDeclContext );
        }
        else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
        {
            dimensionAnalysis( pAction, result.loc, pDeclContext );
            inheritanceAnalysis( pAction, result.loc, result.pDeclContext );
            sizeAnalysis( pAction, result.loc, result.pDeclContext );
        }
        else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
        {
            dimensionAnalysis( pEvent, result.loc, pDeclContext );
            inheritanceAnalysis( pEvent, result.loc, result.pDeclContext );
            sizeAnalysis( pEvent, result.loc, result.pDeclContext );
        }
        else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
        {
            CXXRecordDecl* pCXXRecordDecl = dyn_cast< CXXRecordDecl >( result.pDeclContext );
            if ( !pCXXRecordDecl )
            {
                std::ostringstream os;
                os << "Invalid function context type: " << pContext->get_identifier() << "(" << pContext->get_type_id()
                   << ")";
                pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
                return false;
            }
            argumentListAnalysis( pFunction, result.loc, pCXXRecordDecl );
            returnTypeAnalysis( pFunction, result.loc, result.pDeclContext );
        }
        else if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
        {
            dimensionAnalysis( pObject, result.loc, pDeclContext );
            inheritanceAnalysis( pObject, result.loc, result.pDeclContext );
        }
        else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
        {
        }
        else
        {
            std::ostringstream os;
            os << "Unknown context type: " << pContext->get_identifier() << "(" << pContext->get_type_id() << ")";
            pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) << os.str();
            return false;
        }

        for ( Context* pContextChild : pContext->get_children() )
        {
            if ( !interfaceAnalysis( pContextChild, result.loc, result.pDeclContext ) )
                return false;
        }

        return true;
    }

    virtual void runFinalAnalysis()
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        bool bSuccess = true;
        {
            SourceLocation loc;
            DeclContext*   pDeclContext = pASTContext->getTranslationUnitDecl();

            Interface::Root* pRoot = m_database.one< Interface::Root >( m_sourceFile );
            for ( Context* pContext : pRoot->get_children() )
            {
                if ( !interfaceAnalysis( pContext, loc, pDeclContext ) )
                {
                    bSuccess = false;
                    break;
                }
            }
        }

        if ( bSuccess )
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
