

#include "session.hpp"
#include "clang_utils.hpp"

#include "database/model/OperationsStage.hxx"

#include "database/types/invocation_id.hpp"
#include "invocation/invocation.hpp"

#include "mega/common_strings.hpp"
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

#pragma warning( pop )

namespace clang
{

class OperationsSession : public AnalysisSession
{
    OperationsStage::Database                                    m_database;
    std::map< std::string, ::OperationsStage::Symbols::Symbol* > m_symbols;

    using InvocationsMap = std::map< mega::invocation::ID, ::OperationsStage::Operations::Invocation* >;
    InvocationsMap m_invocationsMap;

protected:
    bool m_bError = false;

public:
    using Ptr = std::unique_ptr< Session >;
    OperationsSession( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir,
                       const char* strSourceFile )
        : AnalysisSession( pASTContext, pSema, strSrcDir, strBuildDir, strSourceFile )
        , m_database( m_environment, m_sourceFile )
    {
        using namespace OperationsStage;
        using namespace OperationsStage::Interface;

        Symbols::SymbolTable* pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        m_symbols = pSymbolTable->get_symbols();
    }

    virtual bool isPossibleEGTypeIdentifier( const std::string& strIdentifier ) const
    {
        if ( mega::getOperationName( strIdentifier ) != mega::HIGHEST_OPERATION_TYPE )
        {
            return true;
        }

        if ( m_symbols.find( strIdentifier ) != m_symbols.end() )
            return true;
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

    virtual bool getInvocationResultType( const clang::SourceLocation& loc, const clang::QualType& type,
                                          clang::QualType& resultType )
    {
        bool bFound = false;

        std::cout << "Hello world!" << std::endl;

        resultType = getVoidType( pASTContext );

        if ( type.getTypePtrOrNull() )
        {
            if ( !type->isDependentType() )
            {
                if ( const clang::IdentifierInfo* pBaseTypeID = type.getBaseTypeIdentifier() )
                {
                    if ( pBaseTypeID == pASTContext->getEGInvocationTypeName() )
                    {
                        if ( const clang::TemplateSpecializationType* pTemplateType
                             = type->getAs< clang::TemplateSpecializationType >() )
                        {
                            if ( pTemplateType->getNumArgs() == 3U )
                            {
                                try
                                {
                                    clang::QualType context       = pTemplateType->getArg( 0U ).getAsType();
                                    clang::QualType typePath      = pTemplateType->getArg( 1U ).getAsType();
                                    clang::QualType operationType = pTemplateType->getArg( 2U ).getAsType();

                                    std::vector< mega::SymbolID > contextSymbolIDs;
                                    if ( !clang::getContextSymbolIDs( pASTContext, context, contextSymbolIDs ) )
                                    {
                                        std::ostringstream os;
                                        os << "Invalid context for invocation of type: "
                                                  + context.getCanonicalType().getAsString();
                                        pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                                            << os.str();
                                        m_bError = true;
                                        return false;
                                    }

                                    std::vector< mega::SymbolID > typePathSymbolIDs;
                                    if ( !clang::getTypePathSymbolIDs( pASTContext, typePath, typePathSymbolIDs ) )
                                    {
                                        pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                                            << "Invalid type path for invocation";
                                        m_bError = true;
                                        return false;
                                    }

                                    std::optional< mega::SymbolID > operationTypeIDOpt
                                        = getEGSymbolID( pASTContext, operationType );
                                    if ( !operationTypeIDOpt )
                                    {
                                        pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                                            << "Invalid operation for invocation";
                                        m_bError = true;
                                        return false;
                                    }
                                    mega::OperationID operationTypeID
                                        = static_cast< mega::OperationID >( operationTypeIDOpt.value() );

                                    using namespace OperationsStage;

                                    const mega::invocation::ID id{
                                        contextSymbolIDs, typePathSymbolIDs, operationTypeID };

                                    Operations::Invocation* pInvocation = nullptr;
                                    {
                                        // auto iFind = m_invocationsMap.find( id );
                                        // if ( iFind != m_invocationsMap.end() )
                                        //{
                                        //     // existing invocation solution exists
                                        //     pInvocation = iFind->second;
                                        // }
                                        // else
                                        //{
                                        //     pInvocation = mega::invocation::construct(
                                        //         m_environment, id, m_database, m_sourceFile );
                                        //     m_invocationsMap.insert( std::make_pair( id, pInvocation ) );
                                        // }
                                    }

                                    // pInvocation

                                    // mega::invocation::Solution

                                    /*const InvocationSolution::InvocationID invocationID
                                        = InvocationSolution::invocationIDFromTypeIDs(
                                            g_pOperationsSession->getObjects( IndexedObject::MASTER_FILE ),
                                            g_pOperationsSession->getIdentifiers(), contextTypes, typePathTypes,
                                            operationTypeID );

                                    try
                                    {
                                        if ( const InvocationSolution* pSolution
                                            = g_pOperationsSession->getInvocation( invocationID, typePathTypes ) )
                                        {
                                            calculateReturnType( pSolution, resultType );
                                        }
                                    }
                                    catch ( mega::NameResolutionException& nameResolutionException )
                                    {
                                        pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                                            << nameResolutionException.what();
                                        m_bError = true;
                                        return false;
                                    }
                                    catch ( mega::InvocationException& invocationException )
                                    {
                                        pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                                            << invocationException.what();
                                        m_bError = true;
                                        return false;
                                    }*/
                                }
                                catch ( std::exception& ex )
                                {
                                    pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                                        << ex.what();
                                    m_bError = true;
                                    return false;
                                }

                                return true;
                            }
                        }
                    }
                }
            }
        }
        return bFound;
    }
    virtual void runFinalAnalysis()
    {
        bool bSuccess = true;

        using namespace OperationsStage;
        try
        {
            m_database.construct< Operations::Invocations >( Operations::Invocations::Args{ m_invocationsMap } );
        }
        catch ( std::exception& ex )
        {
            pASTContext->getDiagnostics().Report( SourceLocation(), clang::diag::err_mega_generic_error ) << ex.what();
            m_bError = true;
            bSuccess = false;
        }

        if ( bSuccess )
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
