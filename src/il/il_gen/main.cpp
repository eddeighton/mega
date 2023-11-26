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

#include "analysis.hpp"

#include "compiler/cmake.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/ASTConsumer.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
// #include "llvm/Support/CommandLine.h"

#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>
#include <iomanip>

namespace
{
using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

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
        using namespace std::string_literals;
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
    virtual std::vector< clang::tooling::CompileCommand > getCompileCommands( llvm::StringRef FilePath ) const
    {
        return m_commands;
    }
    virtual std::vector< std::string >    getAllFiles() const { return m_files; }
    virtual std::vector< CompileCommand > getAllCompileCommands() const { return m_commands; }
};

using namespace il_gen;

ValueType fromName( const std::string& strName )
{
    std::string tmp = strName;
    boost::replace_all( tmp, "_Bool", "bool" );
    boost::replace_all( tmp, "struct ", "" );
    boost::replace_all( tmp, "class ", "" );

    VERIFY_RTE_MSG( tmp.find( '*' ) == std::string::npos, "Type contains nested pointers: " << strName );
    VERIFY_RTE_MSG( tmp.find( '&' ) == std::string::npos, "Type contains nested reference: " << strName );
    using namespace std::string_literals;
    static const std::string strConst = "const"s;
    VERIFY_RTE_MSG( std::search( tmp.begin(), tmp.end(), strConst.begin(), strConst.end() ) == tmp.end(),
                    "Type contains nested const: " << strName );

    return ValueType{ tmp };
}

il_gen::Type detectType( const clang::QualType& type )
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

class MegaMangleFunctionCallback : public MatchFinder::MatchCallback
{
    std::vector< il_gen::Function >& externFunctions;

public:
    MegaMangleFunctionCallback( std::vector< il_gen::Function >& externFunctions )
        : externFunctions( externFunctions )
    {
    }
    virtual void run( const MatchFinder::MatchResult& Result )
    {
        if( auto pFunctionDecl = Result.Nodes.getNodeAs< clang::FunctionDecl >( "functions" ) )
        {
            if( auto pMangleNamespace = llvm::dyn_cast< clang::NamespaceDecl >( pFunctionDecl->getParent() ) )
            {
                if( pMangleNamespace->getNameAsString() == "mangle" )
                {
                    if( auto pMegaNamespace = llvm::dyn_cast< clang::NamespaceDecl >( pMangleNamespace->getParent() ) )
                    {
                        if( pMegaNamespace->getNameAsString() == "mega" )
                        {
                            try
                            {
                                std::vector< Variable > arguments;
                                {
                                    for( int i = 0; i != pFunctionDecl->getNumParams(); ++i )
                                    {
                                        auto pParam = pFunctionDecl->getParamDecl( i );
                                        arguments.push_back(
                                            Variable{ detectType( pParam->getType().getCanonicalType() ),
                                                      pParam->getNameAsString() } );
                                    }
                                }

                                externFunctions.push_back( Function{
                                    arguments, detectType( pFunctionDecl->getReturnType().getCanonicalType() ),
                                    pFunctionDecl->getNameAsString() } );
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
        }
    }
};
} // namespace

void parseExtern( const boost::filesystem::path& filePath, std::vector< il_gen::Function >& externFunctions )
{
    using namespace llvm;
    using namespace clang;
    using namespace clang::tooling;
    using namespace clang::ast_matchers;

    MegaMangleFunctionCallback callback( externFunctions );
    ToolDB                     db( filePath );
    ClangTool                  tool( db, { filePath.string() } );
    MatchFinder                finder;
    DeclarationMatcher         matcher = functionDecl().bind( "functions" );
    finder.addMatcher( matcher, &callback );
    tool.run( newFrontendActionFactory( &finder ).get() );
}

int main( int argc, const char* argv[] )
{
    bool bWait = false;

    {
        namespace po = boost::program_options;
        po::variables_map vm;
        bool              bGeneralWait = false;

        po::options_description genericOptions( " General" );
        {
            // clang-format off
            genericOptions.add_options()
            ( "help,?",                                                         "Produce general or command help message" )
            ( "wait",       po::bool_switch( &bGeneralWait ),                   "Wait at startup for attaching a debugger" );
            // clang-format on
        }

        std::string inlineFiles, externFiles, templateFiles;

        namespace po = boost::program_options;
        po::options_description commandOptions( " Execute inja template" );
        {
            // clang-format off
            commandOptions.add_options()
                ( "inline",         po::value< std::string >( &inlineFiles ),   "CMake file list describing IL inline types and functions" )
                ( "extern",         po::value< std::string >( &externFiles ),   "CMake file list describing extern runtime support functions" )
                ( "template",       po::value< std::string >( &templateFiles ), "CMake file list describing materialised functions" )
                ;
            // clang-format on
        }

        po::options_description visibleOptions( "Allowed options" );
        visibleOptions.add( genericOptions ).add( commandOptions );

        po::parsed_options parsedOptions = po::command_line_parser( argc, argv ).options( visibleOptions ).run();
        po::store( parsedOptions, vm );
        po::notify( vm );

        if( vm.count( "help" ) )
        {
            std::cout << visibleOptions << "\n";
            return 0;
        }

        try
        {
            if( bGeneralWait )
            {
                std::cout << "Waiting for input..." << std::endl;
                char c;
                std::cin >> c;
            }

            const std::vector< boost::filesystem::path > inlineFilePaths
                = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( inlineFiles, " " ) );
            const std::vector< boost::filesystem::path > externFilePaths
                = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( externFiles, " " ) );
            const std::vector< boost::filesystem::path > templateFilePaths
                = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( templateFiles, " " ) );

            for( const auto& f : inlineFilePaths )
            {
                std::cout << "Inline file: " << f.string() << std::endl;
            }

            std::vector< il_gen::Function > externFunctions;
            {
                for( const auto& f : externFilePaths )
                {
                    std::cout << "Extern file: " << f.string() << std::endl;
                    parseExtern( f, externFunctions );
                    for( const auto& e : externFunctions )
                    {
                        std::cout << "Mega mangle extern: " << e << std::endl;
                    }
                }
            }

            for( const auto& f : templateFilePaths )
            {
                std::cout << "Template file: " << f.string() << std::endl;
            }

            std::cout << "It works!" << std::endl;

            return 0;
        }
        catch( std::exception& e )
        {
            std::cout << "Exception: " << e.what() << std::endl;
            return 1;
        }
        catch( ... )
        {
            std::cout << "Unknown error" << std::endl;
            return 1;
        }
    }

    return 0;
}
