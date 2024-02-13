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

#include "database/ClangAnalysisStage.hxx"

#include "mega/common_strings.hpp"

#include "common/requireSemicolon.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "llvm/Support/Casting.h"

#include "clang/AST/Type.h"
#include "clang/Basic/DiagnosticParse.h"
#include "clang/AST/Mangle.h"

#pragma warning( pop )

#include <map>

namespace ClangAnalysisStage
{
#include "compiler/interface.hpp"
#include "compiler/interface_visitor.hpp"
#include "compiler/interface_printer.hpp"
} // namespace ClangAnalysisStage

#define REPORT_ERROR( _msg )                                                                                     \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _os; _os << _msg;                                        \
                                     pASTContext->getDiagnostics().Report( clang::diag::err_mega_generic_error ) \
                                     << _os.str(); )

namespace clang
{
using namespace ClangAnalysisStage;
using namespace ClangAnalysisStage::Interface;

class TraitsSession : public SymbolSession
{
    Database m_database;

    using SymbolTypeIDMap    = std::map< mega::interface::SymbolID, Symbols::SymbolID* >;
    using InterfaceTypeIDMap = std::map< mega::interface::TypeID, Symbols::InterfaceTypeID* >;

    std::map< std::string, Symbols::SymbolID* > m_symbols;

public:
    TraitsSession( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir )
        : SymbolSession( pASTContext, pSema, strSrcDir, strBuildDir )
        , m_database( m_environment, m_environment.project_manifest() )
    {
        Symbols::SymbolTable* pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );
        m_symbols                          = pSymbolTable->get_symbol_names();
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

    using TypeInfoMap = std::map< std::string, Interface::CPP::TypeInfo* >;

    struct Visitor : Interface::Visitor
    {
        Database&             database;
        ASTContext*           pASTContext;
        Sema*                 pSema;
        ItaniumMangleContext& mangle;
        TypeInfoMap&          typeInfoMap;

        DeclContext*   pDeclContext = nullptr;
        SourceLocation loc;

        Visitor( Database&             database,
                 ASTContext*           pASTContext,
                 Sema*                 pSema,
                 ItaniumMangleContext& mangle,
                 TypeInfoMap&          typeInfoMap )
            : database( database )
            , pASTContext( pASTContext )
            , pSema( pSema )
            , mangle( mangle )
            , typeInfoMap( typeInfoMap )
        {
        }

        Interface::CPP::TypeInfo* getOrCreateTypeInfo( QualType traitType ) const
        {
            const std::string strCanonical = traitType.getCanonicalType().getAsString();

            auto iFind = typeInfoMap.find( strCanonical );
            if( iFind != typeInfoMap.end() )
            {
                return iFind->second;
            }
            else
            {
                std::string strMangle;
                {
                    llvm::raw_string_ostream os( strMangle );
                    mangle.mangleTypeName( traitType, os );
                }

                std::ostringstream osTraitName;
                {
                    osTraitName << '_' << typeInfoMap.size();
                }

                auto pTypeInfo = database.construct< Interface::CPP::TypeInfo >(
                    Interface::CPP::TypeInfo::Args{ strCanonical, strMangle, osTraitName.str() } );

                typeInfoMap.insert( { strCanonical, pTypeInfo } );

                return pTypeInfo;
            }
        }

        virtual bool visit( Interface::UserDimension* pNode ) const
        {
            QualType traitType
                = getTypeTrait( pASTContext, pSema, pDeclContext, loc, pNode->get_symbol()->get_token() );

            database.construct< Interface::CPP::DataType >(
                Interface::CPP::DataType::Args{ pNode->get_cpp_data_type(), getOrCreateTypeInfo( traitType ) } );

            return true;
        }

        virtual bool visit( Interface::UserUsing* pNode ) const
        {
            QualType traitType
                = getTypeTrait( pASTContext, pSema, pDeclContext, loc, pNode->get_symbol()->get_token() );

            database.construct< Interface::CPP::DataType >(
                Interface::CPP::DataType::Args{ pNode->get_cpp_data_type(), getOrCreateTypeInfo( traitType ) } );

            return true;
        }

        virtual bool visit( Interface::Aggregate* pNode ) const { return true; }

        virtual bool visit( Interface::Function* pNode ) const
        {
            QualType traitType
                = getTypeTrait( pASTContext, pSema, pDeclContext, loc, pNode->get_symbol()->get_token() );
            VERIFY_RTE( traitType->isFunctionPointerType() );

            auto pClangFunctionType = traitType->getPointeeType()->getAs< clang::FunctionProtoType >();
            VERIFY_RTE( pClangFunctionType );

            Interface::CPP::TypeInfo* pReturnTypeInfo = getOrCreateTypeInfo( pClangFunctionType->getReturnType() );

            std::vector< Interface::CPP::TypeInfo* > parameterTypeInfos;
            {
                for( auto paramType : pClangFunctionType->getParamTypes() )
                {
                    parameterTypeInfos.push_back( getOrCreateTypeInfo( paramType.getCanonicalType() ) );
                }
            }

            database.construct< Interface::CPP::FunctionType >( Interface::CPP::FunctionType::Args{
                pNode->get_cpp_function_type(), pReturnTypeInfo, parameterTypeInfos } );

            return true;
        }
        virtual bool visit( Interface::IContext* pNode ) const { return true; }
    };

    bool recurse( Interface::Node*      pNode,
                  DeclContext*          pDeclContext,
                  ItaniumMangleContext& mangle,
                  SourceLocation        loc,
                  TypeInfoMap&          typeInfoMap )
    {
        Visitor visitor{ m_database, pASTContext, pSema, mangle, typeInfoMap };

        visitor.pDeclContext = pDeclContext;
        visitor.loc          = loc;
        Interface::visit( visitor, pNode );

        // recurse following pattern used in compiler/task_clang_traits.cpp:213
        const bool bIsContext  = db_cast< Interface::IContext >( pNode );
        const bool bIsFunction = db_cast< Interface::Function >( pNode );

        if( bIsContext && !bIsFunction )
        {
            DeclLocType result
                = getNestedDeclContext( pASTContext, pSema, pDeclContext, loc, pNode->get_symbol()->get_token() );
            if( nullptr == result.pDeclContext )
            {
                REPORT_ERROR( "Failed to resolve trait for: " << Interface::getKind( pNode ) << " "
                                                              << Interface::fullTypeName( pNode ) );
                return false;
            }

            for( Interface::Node* pChildNode : pNode->get_children() )
            {
                if( !recurse( pChildNode, result.pDeclContext, mangle, result.loc, typeInfoMap ) )
                {
                    return false;
                }
            }
        }
        return true;
    }

    virtual void runFinalAnalysis() override
    {
        bool bSuccess = true;
        {
            SourceLocation                          loc;
            DeclContext*                            pDeclContext = pASTContext->getTranslationUnitDecl();
            std::unique_ptr< ItaniumMangleContext > pMangle(
                ItaniumMangleContext::create( *pASTContext, pASTContext->getDiagnostics() ) );

            TypeInfoMap typeInfoMap;

            Interface::Root* pRoot = m_database.one< Interface::Root >( m_environment.project_manifest() );
            for( Interface::Node* pNode : pRoot->get_children() )
            {
                if( !recurse( pNode, pDeclContext, *pMangle, loc, typeInfoMap ) )
                {
                    bSuccess = false;
                    break;
                }
            }
        }

        if( bSuccess )
        {
            const mega::io::CompilationFilePath interfaceAnalysisFile
                = m_environment.ClangAnalysisStage_Analysis( m_environment.project_manifest() );
            m_database.save_Analysis_to_temp();
            m_environment.temp_to_real( interfaceAnalysisFile );
        }
    }
};

Session::Ptr make_traits_session( ASTContext* pASTContext, Sema* pSema, const char* strSrcDir, const char* strBuildDir )
{
    return std::make_unique< TraitsSession >( pASTContext, pSema, strSrcDir, strBuildDir );
}

} // namespace clang
