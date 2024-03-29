
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

#include "model.hpp"

#include "common/assert_verify.hpp"

////////////////////////////////////////////
#include "common/clang_warnings_begin.hpp"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/ASTConsumer.h"

#include "clang/Basic/Thunk.h"
#include "clang/AST/Mangle.h"

#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Tooling.h"

#include "common/clang_warnings_end.hpp"
////////////////////////////////////////////

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

#include <string>
#include <iostream>

namespace il_gen
{

namespace
{
using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace std::string_literals;

struct ToolDB : public clang::tooling::CompilationDatabase
{
    std::vector< clang::tooling::CompileCommand > m_commands;
    std::vector< std::string >                    m_files;

    ToolDB( const boost::filesystem::path& filePath )
    {
        m_files.push_back( filePath.string() );

        clang::tooling::CompileCommand cmd;

        cmd.Directory = filePath.parent_path().string();

        /// The source file associated with the command.
        cmd.Filename = filePath.filename().string();

        /// The command line that was executed.
        cmd.CommandLine = std::vector< std::string >{

            "/build/linux_gcc_static_release/llvm/install/bin/clang-15"s,

            "-resource-dir"s,
            "/build/linux_gcc_static_release/llvm/install/lib/clang/15.0.0"s,

            "-I"s,
            "/workspace/root/src/common/src/api/"s,
            "-I"s,
            "/workspace/root/src/mega/src/include/"s,
            "-I"s,
            "/build/linux_gcc_shared_debug/boost/install/include/"s,

            "-std=c++20"s,
            "-fcoroutines-ts"s,
            "-fgnuc-version=4.2.1"s,
            "-fno-implicit-modules"s,
            "-fcxx-exceptions"s,
            "-fexceptions"s,
            "-fcolor-diagnostics"s,
            "-faddrsig"s,
            "-D__GCC_HAVE_DWARF2_CFI_ASM=1"s,

            "-Wno-unused-command-line-argument"s,

            "-x"s,
            "c++"s,
            filePath.string(),

        };

        /// The output file associated with the command.
        cmd.Output = ""s;

        /// If this compile command was guessed rather than read from an authoritative
        /// source, a short human-readable explanation.
        /// e.g. "inferred from foo/bar.h".
        cmd.Heuristic = ""s;

        m_commands.push_back( cmd );
    }
    virtual std::vector< clang::tooling::CompileCommand > getCompileCommands( llvm::StringRef ) const
    {
        return m_commands;
    }
    virtual std::vector< std::string >    getAllFiles() const { return m_files; }
    virtual std::vector< CompileCommand > getAllCompileCommands() const { return m_commands; }
};

ValueType fromName( const std::string& strName )
{
    std::string tmp = strName;
    boost::replace_all( tmp, "_Bool", "bool" );
    boost::replace_all( tmp, "struct ", "" );
    boost::replace_all( tmp, "class ", "" );

    VERIFY_RTE_MSG( tmp.find( '*' ) == std::string::npos, "Type contains nested pointers: " << strName );
    VERIFY_RTE_MSG( tmp.find( '&' ) == std::string::npos, "Type contains nested Pointer: " << strName );
    static const std::string strConst = "const"s;
    VERIFY_RTE_MSG( std::search( tmp.begin(), tmp.end(), strConst.begin(), strConst.end() ) == tmp.end(),
                    "Type contains nested const: " << strName );

    return ValueType{ tmp };
}

Namespaces detectNamespace( const clang::DeclContext* pDeclContext )
{
    Namespaces namespaces;
    for( auto p = pDeclContext; p; p = p->getParent() )
    {
        if( auto pNamespace = llvm::dyn_cast< const clang::NamespaceDecl >( p ) )
        {
            namespaces.push_back( pNamespace->getNameAsString() );
        }
    }
    std::reverse( namespaces.begin(), namespaces.end() );
    return namespaces;
}

Type detectType( const clang::QualType& type )
{
    if( type->isPointerType() )
    {
        auto tmp = type->getPointeeType();
        if( tmp.isLocalConstQualified() )
        {
            tmp.removeLocalConst();
            VERIFY_RTE_MSG( !tmp.isLocalConstQualified(), "Failed to remove const" );
            return ConstPtr{ fromName( tmp.getAsString() ) };
        }
        else
        {
            return Ptr{ fromName( tmp.getAsString() ) };
        }
    }
    else if( type->isReferenceType() )
    {
        auto tmp = type.getNonReferenceType();
        if( tmp.isConstQualified() )
        {
            tmp.removeLocalConst();
            VERIFY_RTE_MSG( !tmp.isLocalConstQualified(), "Failed to remove const" );
            return ConstRef{ fromName( tmp.getAsString() ) };
        }
        else
        {
            return Ref{ fromName( tmp.getAsString() ) };
        }
    }
    else
    {
        if( type.isLocalConstQualified() )
        {
            auto tmp = type;
            tmp.removeLocalConst();
            VERIFY_RTE_MSG( !tmp.isLocalConstQualified(), "Failed to remove const" );
            return Const{ fromName( tmp.getAsString() ) };
        }
        else
        {
            return Mutable{ fromName( type.getAsString() ) };
        }
    }
}

std::optional< Type > getInlineSliceType( const Model& model, const QualType& type )
{
    if( const TagType* tagType = dyn_cast< TagType >( type ) )
    {
        if( const auto* pRecord = dyn_cast< CXXRecordDecl >( tagType->getDecl() ) )
        {
            for( const auto pBase : pRecord->bases() )
            {
                auto baseType = detectType( pBase.getType() );
                if( model.isInlineType( baseType ) )
                {
                    return baseType;
                }
            }
        }
    }

    return std::nullopt;
}

AdaptedType getAdaptedType( Model& model, const QualType& type )
{
    auto sliceType = getInlineSliceType( model, type );
    auto paramType = detectType( type );

    if( model.isNativeType( paramType ) )
    {
        return NativeType{ paramType };
    }
    else if( sliceType.has_value() )
    {
        return SlicedType{ paramType, sliceType.value() };
    }
    else if( model.isInlineType( paramType ) )
    {
        return InlineType{ paramType };
    }
    else
    {
        // add new extern type
        const Type nonQualified{ Mutable{ getDataType( paramType ) } };
        auto       iFind = std::find( model.externTypes.begin(), model.externTypes.end(), nonQualified );
        if( iFind == model.externTypes.end() )
        {
            model.externTypes.push_back( nonQualified );
        }

        return BoxedType{ paramType };
    }
}

class ExternFunctionCallback : public MatchFinder::MatchCallback
{
    Model& model;

public:
    ExternFunctionCallback( Model& _model )
        : model( _model )
    {
    }
    virtual void run( const MatchFinder::MatchResult& Result )
    {
        static const Namespaces expectedNamespaces = { "mega"s, "mangle"s };
        if( auto pFunctionDecl = Result.Nodes.getNodeAs< clang::FunctionDecl >( "functions" ) )
        {
            auto namespaces = detectNamespace( pFunctionDecl );
            if( namespaces == expectedNamespaces )
            {
                try
                {
                    std::vector< Variable > arguments;
                    {
                        for( auto i = 0U; i != pFunctionDecl->getNumParams(); ++i )
                        {
                            auto pParam      = pFunctionDecl->getParamDecl( i );
                            auto qualType    = pParam->getType().getCanonicalType();
                            auto adaptedType = getAdaptedType( model, qualType );
                            arguments.push_back( Variable{ adaptedType, pParam->getNameAsString() } );
                        }
                    }
                    auto returnType = getAdaptedType( model, pFunctionDecl->getReturnType().getCanonicalType() );
                    model.externFunctions.push_back(
                        Function{ arguments, returnType, pFunctionDecl->getNameAsString(), namespaces } );
                }
                catch( std::exception& ex )
                {
                    THROW_RTE( "Fail to analyse function type for: "
                               << pFunctionDecl->getNameAsString()
                               << " with type: " << pFunctionDecl->getType().getAsString() << " error: " << ex.what() );
                }
            }
        }
    }
};

class InlineFunctionCallback : public MatchFinder::MatchCallback
{
    Model& model;

public:
    InlineFunctionCallback( Model& _model )
        : model( _model )
    {
    }
    virtual void run( const MatchFinder::MatchResult& Result )
    {
        if( auto pFunctionDecl = Result.Nodes.getNodeAs< clang::FunctionDecl >( "functions" ) )
        {
            if( !pFunctionDecl->isCXXClassMember() )
            {
                auto namespaces = detectNamespace( pFunctionDecl );
                if( !namespaces.empty() && namespaces.front() == "mega" )
                {
                    try
                    {
                        std::vector< Variable > arguments;
                        {
                            for( auto i = 0U; i != pFunctionDecl->getNumParams(); ++i )
                            {
                                auto pParam = pFunctionDecl->getParamDecl( i );

                                auto adaptedType = getAdaptedType( model, pParam->getType().getCanonicalType() );

                                VERIFY_RTE_MSG( !std::get_if< BoxedType >( &adaptedType ),
                                                "Inline function uses extern type : "
                                                    << pFunctionDecl->getNameAsString()
                                                    << " with type: " << pFunctionDecl->getType().getAsString() );

                                arguments.push_back( Variable{ adaptedType, pParam->getNameAsString() } );
                            }
                        }

                        auto returnType = getAdaptedType( model, pFunctionDecl->getReturnType().getCanonicalType() );
                        VERIFY_RTE_MSG( !std::get_if< BoxedType >( &returnType ),
                                        "Inline function extern type : " << pFunctionDecl->getNameAsString()
                                                                         << " with type: "
                                                                         << pFunctionDecl->getType().getAsString() );

                        model.inlineFunctions.push_back(
                            Function{ arguments, returnType, pFunctionDecl->getNameAsString(), namespaces } );
                    }
                    catch( std::exception& ex )
                    {
                        THROW_RTE( "Fail to analyse function type for: "
                                   << pFunctionDecl->getNameAsString() << " with type: "
                                   << pFunctionDecl->getType().getAsString() << " error: " << ex.what() );
                    }
                }
            }
        }
    }
};

class InlineTypeCallback : public MatchFinder::MatchCallback
{
    Model& model;

public:
    InlineTypeCallback( Model& _model )
        : model( _model )
    {
    }
    virtual void run( const MatchFinder::MatchResult& Result )
    {
        if( auto pRecordDecl = Result.Nodes.getNodeAs< clang::RecordDecl >( "types" ) )
        {
            // auto namespaces = detectNamespace( pRecordDecl );
            // if( !namespaces.empty() && namespaces.front() == "mega" )
            {
                auto type = pRecordDecl->getASTContext().getTypeDeclType( pRecordDecl );
                try
                {
                    Type recordType{ Mutable{ fromName( type.getAsString() ) } };
                    auto iFind = std::find( model.inlineTypes.begin(), model.inlineTypes.end(), recordType );
                    if( iFind == model.inlineTypes.end() )
                    {
                        model.inlineTypes.push_back( recordType );
                    }
                }
                catch( std::exception& ex )
                {
                    THROW_RTE( "Fail to analyse function type for: " << pRecordDecl->getNameAsString() << " with type: "
                                                                     << type.getAsString() << " error: " << ex.what() );
                }
            }
        }
    }
};

class NativeTypeCallback : public MatchFinder::MatchCallback
{
    Model& model;

public:
    NativeTypeCallback( Model& _model )
        : model( _model )
    {
    }
    virtual void run( const MatchFinder::MatchResult& Result )
    {
        if( auto pTypeAliasDecl = Result.Nodes.getNodeAs< clang::TypeAliasDecl >( "types" ) )
        {
            auto namespaces = detectNamespace( pTypeAliasDecl->getDeclContext() );
            if( !namespaces.empty() && namespaces.front() == "mega" )
            {
                auto type = pTypeAliasDecl->getUnderlyingType().getCanonicalType();
                try
                {
                    Type recordType{ Mutable{ fromName( type.getAsString() ) } };
                    auto iFind = std::find( model.nativeTypes.begin(), model.nativeTypes.end(), recordType );
                    if( iFind == model.nativeTypes.end() )
                    {
                        model.nativeTypes.push_back( recordType );
                    }
                }
                catch( std::exception& ex )
                {
                    THROW_RTE( "Fail to analyse function type for: " << pTypeAliasDecl->getNameAsString()
                                                                     << " with type: " << type.getAsString()
                                                                     << " error: " << ex.what() );
                }
            }
        }
    }
};

class MaterialiserCallback : public MatchFinder::MatchCallback
{
    Model& model;

public:
    MaterialiserCallback( Model& _model )
        : model( _model )
    {
    }
    virtual void run( const MatchFinder::MatchResult& Result )
    {
        if( auto pCXXRecordDecl = Result.Nodes.getNodeAs< clang::CXXRecordDecl >( "materialisers" ) )
        {
            auto namespaces = detectNamespace( pCXXRecordDecl->getDeclContext() );

            if( ( namespaces.size() == 2 ) && ( namespaces.front() == "mega" )
                && ( namespaces.back() == "materialiser" ) )
            {
                try
                {
                    if( pCXXRecordDecl->getNameAsString() == "Name"
                        || pCXXRecordDecl->getNameAsString() == "Functions" )
                    {
                        return;
                    }

                    std::vector< Variable > arguments;
                    std::vector< Function > functions;

                    for( const auto* pMatChild : pCXXRecordDecl->decls() )
                    {
                        if( auto pStruct = llvm::dyn_cast< CXXRecordDecl >( pMatChild ) )
                        {
                            if( pStruct->getNameAsString() == "Name" )
                            {
                                for( const auto* pNamingChild : pStruct->decls() )
                                {
                                    if( auto pFunctionDecl = llvm::dyn_cast< FunctionDecl >( pNamingChild ) )
                                    {
                                        std::cout << "Found naming function: " << pFunctionDecl->getNameAsString()
                                                  << " with arguments: " << pFunctionDecl->getNumParams() << std::endl;
                                        VERIFY_RTE_MSG( arguments.empty(), "Found multiple naming functions" );
                                        for( auto i = 0U; i != pFunctionDecl->getNumParams(); ++i )
                                        {
                                            auto pParam = pFunctionDecl->getParamDecl( i );
                                            auto adaptedType
                                                = getAdaptedType( model, pParam->getType().getCanonicalType() );
                                            arguments.push_back( Variable{ adaptedType, pParam->getNameAsString() } );
                                        }
                                    }
                                }
                            }
                            else if( pStruct->getNameAsString() == "Functions" )
                            {
                                for( const auto* pFunctionsChild : pStruct->decls() )
                                {
                                    if( const auto* pFunctionDecl = llvm::dyn_cast< CXXMethodDecl >( pFunctionsChild ) )
                                    {
                                        std::vector< Variable > functionArgs;
                                        {
                                            for( auto i = 0U; i != pFunctionDecl->getNumParams(); ++i )
                                            {
                                                auto pParam = pFunctionDecl->getParamDecl( i );
                                                auto adaptedType
                                                    = getAdaptedType( model, pParam->getType().getCanonicalType() );
                                                functionArgs.push_back(
                                                    Variable{ adaptedType, pParam->getNameAsString() } );
                                            }
                                        }
                                        auto returnType = getAdaptedType(
                                            model, pFunctionDecl->getReturnType().getCanonicalType() );
                                        // generate the function as though in the namespace of the materialiser name
                                        functions.push_back( Function{ functionArgs,
                                                                       returnType,
                                                                       pFunctionDecl->getNameAsString(),
                                                                       { pCXXRecordDecl->getNameAsString() } } );
                                    }
                                }
                            }
                            else if( pStruct->getNameAsString() == pCXXRecordDecl->getNameAsString() )
                            {
                                // self Pointer
                            }
                            else
                            {
                                THROW_RTE( "Unknown struct found in materialiser: "
                                           << pCXXRecordDecl->getNameAsString() << " " << pStruct->getNameAsString() );
                            }
                        }
                        else if( auto pNamedDecl = llvm::dyn_cast< NamedDecl >( pMatChild ) )
                        {
                            THROW_RTE( "Found unexpected declaration in materialiser: "
                                       << pCXXRecordDecl->getNameAsString()
                                       << " of: " << pNamedDecl->getNameAsString() );
                        }
                        else
                        {
                            THROW_RTE(
                                "Found unnamed declaration in materialiser: " << pCXXRecordDecl->getNameAsString() );
                        }
                    }

                    bool bFound = false;
                    for( const auto& existing : model.materialisers )
                    {
                        if( existing.name == pCXXRecordDecl->getNameAsString() )
                        {
                            bFound = true;
                            break;
                        }
                    }
                    if( bFound )
                    {
                        VERIFY_RTE_MSG(
                            arguments.empty(), "Duplicate materialiser: " << pCXXRecordDecl->getNameAsString() );
                        VERIFY_RTE_MSG(
                            functions.empty(), "Duplicate materialiser: " << pCXXRecordDecl->getNameAsString() );
                    }
                    else
                    {
                        VERIFY_RTE_MSG( !arguments.empty(), "Failed to determine arguments for materialiser: "
                                                                << pCXXRecordDecl->getNameAsString() );
                        VERIFY_RTE_MSG( !functions.empty(), "Failed to determine functions for materialiser: "
                                                                << pCXXRecordDecl->getNameAsString() );

                        std::cout << "Found materialiser: " << pCXXRecordDecl->getNameAsString()
                                  << " decl kind: " << pCXXRecordDecl->getKindName().str() << std::endl;

                        Materialiser materialiser{ pCXXRecordDecl->getNameAsString(), arguments, functions };
                        model.materialisers.push_back( materialiser );
                    }
                }
                catch( std::exception& ex )
                {
                    THROW_RTE( "Fail to analyse materialiser: " << pCXXRecordDecl->getNameAsString()
                                                                << " error: " << ex.what() );
                }
            }
        }
    }
};

class StubFunctionCallback : public MatchFinder::MatchCallback
{
    Model& model;

    std::unique_ptr< ItaniumMangleContext > pMangle;

public:
    StubFunctionCallback( Model& _model )
        : model( _model )
    {
    }
    virtual void run( const MatchFinder::MatchResult& Result )
    {
        if( !pMangle )
        {
            VERIFY_RTE( Result.Context );
            pMangle.reset( ItaniumMangleContext::create( *Result.Context, Result.Context->getDiagnostics() ) );
        }

        if( auto pFunctionDecl = Result.Nodes.getNodeAs< clang::FunctionDecl >( "functions" ) )
        {
            try
            {
                auto namespaces = detectNamespace( pFunctionDecl );
                if( namespaces.size() == 1 )
                {
                    for( auto& mat : model.materialisers )
                    {
                        if( mat.name == namespaces.front() )
                        {
                            const auto functionName = pFunctionDecl->getNameAsString();
                            bool       bFound       = false;
                            for( auto& func : mat.functions )
                            {
                                if( functionName == func.name )
                                {
                                    // determine mangle for function
                                    llvm::raw_string_ostream os( func.mangle );
                                    pMangle->mangleName( pFunctionDecl, os );
                                    std::cout << "Mangle: " << func << " " << func.mangle << std::endl;
                                    bFound = true;
                                }
                            }
                            VERIFY_RTE_MSG( bFound, "Failed to locate function: " << functionName
                                                                                  << " in materialiser: " << mat.name );
                        }
                    }
                }
            }
            catch( std::exception& ex )
            {
                THROW_RTE( "Fail to analyse function type for: " << pFunctionDecl->getNameAsString() << " with type: "
                                                                 << pFunctionDecl->getType().getAsString()
                                                                 << " error: " << ex.what() );
            }
        }
    }
};

} // namespace

void parseNative( const boost::filesystem::path& filePath, Model& model )
{
    using namespace llvm;
    using namespace clang;
    using namespace clang::tooling;
    using namespace clang::ast_matchers;

    NativeTypeCallback callback( model );
    ToolDB             db( filePath );
    ClangTool          tool( db, { filePath.string() } );
    MatchFinder        finder;
    DeclarationMatcher matcher = typeAliasDecl().bind( "types" );
    finder.addMatcher( matcher, &callback );
    tool.run( newFrontendActionFactory( &finder ).get() );
}

void parseInline( const boost::filesystem::path& filePath, Model& model )
{
    using namespace llvm;
    using namespace clang;
    using namespace clang::tooling;
    using namespace clang::ast_matchers;

    ToolDB      db( filePath );
    ClangTool   tool( db, { filePath.string() } );
    MatchFinder finder;

    InlineFunctionCallback functionCallback( model );
    DeclarationMatcher     functionMatcher = functionDecl().bind( "functions" );
    finder.addMatcher( functionMatcher, &functionCallback );

    InlineTypeCallback typeCallback( model );
    DeclarationMatcher typeMatcher = recordDecl().bind( "types" );
    finder.addMatcher( typeMatcher, &typeCallback );

    tool.run( newFrontendActionFactory( &finder ).get() );
}

void parseExtern( const boost::filesystem::path& filePath, Model& model )
{
    using namespace llvm;
    using namespace clang;
    using namespace clang::tooling;
    using namespace clang::ast_matchers;

    ExternFunctionCallback callback( model );
    ToolDB                 db( filePath );
    ClangTool              tool( db, { filePath.string() } );
    MatchFinder            finder;
    DeclarationMatcher     matcher = functionDecl().bind( "functions" );
    finder.addMatcher( matcher, &callback );
    tool.run( newFrontendActionFactory( &finder ).get() );
}

void parseMaterialiser( const boost::filesystem::path& filePath, Model& model )
{
    using namespace llvm;
    using namespace clang;
    using namespace clang::tooling;
    using namespace clang::ast_matchers;

    MaterialiserCallback callback( model );
    ToolDB               db( filePath );
    ClangTool            tool( db, { filePath.string() } );
    MatchFinder          finder;
    DeclarationMatcher   matcher = cxxRecordDecl().bind( "materialisers" );
    finder.addMatcher( matcher, &callback );
    tool.run( newFrontendActionFactory( &finder ).get() );
}

void parseStubs( const boost::filesystem::path& filePath, Model& model )
{
    using namespace llvm;
    using namespace clang;
    using namespace clang::tooling;
    using namespace clang::ast_matchers;

    StubFunctionCallback callback( model );
    ToolDB               db( filePath );
    ClangTool            tool( db, { filePath.string() } );
    MatchFinder          finder;
    DeclarationMatcher   matcher = functionDecl().bind( "functions" );
    finder.addMatcher( matcher, &callback );
    tool.run( newFrontendActionFactory( &finder ).get() );
}

} // namespace il_gen
