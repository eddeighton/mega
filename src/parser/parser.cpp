
#include "parser/parser.hpp"
#include "clang.hpp"

#include "database/common/sources.hpp"

#include "database/model/ParserStage.hxx"
#include "database/model/environment.hxx"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include "clang/Basic/LLVM.h"
#include "clang/Basic/TokenKinds.h"

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/file_status.hpp>

// disable clang warnings
#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/Basic/FileManager.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/BitmaskEnum.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/MemoryBufferCache.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Lex/Token.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/ModuleLoader.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseDiagnostic.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/DiagnosticOptions.h"

#pragma warning( pop )

using namespace ParserStage;
using namespace ParserStage::Parser;

// cannibalised version of clang parser for parsing eg source code
class MegaParser : public ::Parser
{
public:
    MegaParser( Stuff& stuff, llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnostics )
        : Parser( stuff, pDiagnostics )
    {
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    // high level parsing utility functions
    Identifier* parse_identifier( Database& database )
    {
        std::string str;
        if ( Tok.is( clang::tok::identifier ) )
        {
            clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
            str                                = pIdentifier->getName();
            ConsumeToken();
        }
        else
        {
            MEGA_PARSER_ERROR( "Expected identifier" );
        }
        return database.construct< Identifier >( Identifier::Args{ str } );
    }

    ScopedIdentifier* parse_scopedIdentifier( Database& database )
    {
        const std::string strFileName  = sm.getFilename( Tok.getLocation() );
        const std::size_t szLineNumber = sm.getSpellingLineNumber( Tok.getLocation() );

        // const std::string strLocation = Tok.getLocation().printToString( sm );

        std::vector< Identifier* > identifiers;
        identifiers.push_back( parse_identifier( database ) );
        while ( Tok.is( clang::tok::coloncolon ) )
        {
            ConsumeToken();

            if ( Tok.is( clang::tok::identifier ) )
            {
                identifiers.push_back( parse_identifier( database ) );
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected identifier" );
            }
        }
        return database.construct< ScopedIdentifier >( ScopedIdentifier::Args{ identifiers, strFileName, szLineNumber } );
    }
    // void parse_visibility( Database& session, input::Visibility* pVisibility )
    //{
    //     if ( Tok.is( clang::tok::kw_public ) )
    //     {
    //         pVisibility->m_visibility = mega::eVisPublic;
    //         ConsumeToken();
    //     }
    //     else if ( Tok.is( clang::tok::kw_private ) )
    //     {
    //         pVisibility->m_visibility = mega::eVisPrivate;
    //         ConsumeToken();
    //     }
    //     else
    //     {
    //         MEGA_PARSER_ERROR( "Expected public or private token" );
    //     }
    //     if ( !TryConsumeToken( clang::tok::colon ) )
    //     {
    //         // Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
    //         MEGA_PARSER_ERROR( "Expected colon" );
    //     }
    // }

    void parse_semicolon()
    {
        if ( !TryConsumeToken( clang::tok::semi ) )
        {
            // Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
            MEGA_PARSER_ERROR( "Expected semicolon" );
        }
    }

    ArgumentList* parse_argumentList( Database& database )
    {
        using namespace ParserStage::Parser;
        std::string strArguments;
        if ( Tok.is( clang::tok::l_paren ) )
        {
            BalancedDelimiterTracker T( *this, clang::tok::l_paren );
            T.consumeOpen();

            if ( !Tok.is( clang::tok::r_paren ) )
            {
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();

                while ( !isEofOrEom() && !Tok.is( clang::tok::r_paren ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                if ( !getSourceText( startLoc, endLoc, strArguments ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing argument list" );
                }
            }

            T.consumeClose();
        }
        return database.construct< ArgumentList >( ArgumentList::Args( strArguments ) );
    }

    void parse_comment()
    {
        while ( Tok.is( clang::tok::comment ) )
        {
            ConsumeToken();
        }
    }

    ReturnType* parse_returnType( Database& database )
    {
        std::string str;
        if ( Tok.is( clang::tok::colon ) )
        {
            ConsumeAnyToken();

            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();
            ConsumeAnyToken();

            while ( !isEofOrEom() && !Tok.isOneOf( clang::tok::semi, clang::tok::comma, clang::tok::l_brace ) )
            {
                endLoc = Tok.getEndLoc();
                ConsumeAnyToken();
            }

            {
                if ( !getSourceText( startLoc, endLoc, str ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing return type" );
                }
            }
        }
        return database.construct< ReturnType >( ReturnType::Args{ str } );
    }

    Inheritance* parse_inheritance( Database& database )
    {
        std::vector< std::string > strings;
        if ( Tok.is( clang::tok::colon ) )
        {
            bool bFoundComma = true;
            while ( bFoundComma )
            {
                ConsumeAnyToken();
                bFoundComma = false;

                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();

                while ( !isEofOrEom() && !Tok.isOneOf( clang::tok::semi, clang::tok::comma, clang::tok::l_brace ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }

                {
                    std::string str;
                    if ( !getSourceText( startLoc, endLoc, str ) )
                    {
                        MEGA_PARSER_ERROR( "Error parsing inheritance" );
                    }
                    strings.push_back( str );
                }

                if ( Tok.is( clang::tok::comma ) )
                    bFoundComma = true;
            }
        }

        return database.construct< Inheritance >( Inheritance::Args{ strings } );
    }

    Size* parse_size( Database& database )
    {
        // parse optional size specifier
        std::string strSize;
        if ( Tok.is( clang::tok::l_square ) )
        {
            BalancedDelimiterTracker T( *this, clang::tok::l_square );
            T.consumeOpen();

            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();
            ConsumeAnyToken();

            while ( !isEofOrEom() && !Tok.is( clang::tok::r_square ) )
            {
                endLoc = Tok.getEndLoc();
                ConsumeAnyToken();
            }

            {
                if ( !getSourceText( startLoc, endLoc, strSize ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing size" );
                }
            }

            T.consumeClose();
        }
        return database.construct< Size >( Size::Args{ strSize } );
    }

    // begin of actual parsing routines for eg grammar
    Dimension* parse_dimension( Database& database, bool bIsConst )
    {
        Dimension::Args args;
        args.isConst = bIsConst;
        {
            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();

            clang::Token next = NextToken();
            while ( !next.is( clang::tok::semi ) )
            {
                endLoc = Tok.getEndLoc();
                ConsumeToken();
                next = NextToken();
            }

            std::string strType;
            VERIFY_RTE( getSourceText( startLoc, endLoc, strType ) );
            args.type = strType;
        }
        args.id = parse_identifier( database );
        parse_semicolon();

        return database.construct< Dimension >( args );
    }

    boost::filesystem::path resolveFilePath( const std::string& strFile )
    {
        const clang::DirectoryLookup* CurDir;

        if ( const clang::FileEntry* pIncludeFile = PP.LookupFile( clang::SourceLocation(),
                                                                   // Filename
                                                                   strFile,
                                                                   // isAngled
                                                                   false,
                                                                   // DirectoryLookup *FromDir
                                                                   nullptr,
                                                                   // FileEntry *FromFile
                                                                   //&fileEntry,
                                                                   nullptr,
                                                                   // DirectoryLookup *&CurDir
                                                                   CurDir,
                                                                   // SmallVectorImpl<char> *SearchPath
                                                                   nullptr,
                                                                   // SmallVectorImpl<char> *RelativePath
                                                                   nullptr,
                                                                   // SuggestedModule
                                                                   nullptr,
                                                                   // IsMapped
                                                                   nullptr ) )
        {
            // otherwise hte file should have normal file path and exist
            const boost::filesystem::path filePath
                = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( pIncludeFile->tryGetRealPathName().str() ) );
            if ( !boost::filesystem::exists( filePath ) )
            {
                MEGA_PARSER_ERROR( "Cannot locate include file: " << filePath.string() );
            }
            return filePath;
        }
        else
        {
            MEGA_PARSER_ERROR( "Cannot locate include file: " << strFile );
        }
    }

    Include* parse_include( Database& database )
    {
        Include* pResult = nullptr;

        // include name( file );
        // optional identifier
        ScopedIdentifier* pIdentifier = nullptr;
        if ( Tok.is( clang::tok::identifier ) )
        {
            pIdentifier = parse_scopedIdentifier( database );
        }

        BalancedDelimiterTracker T( *this, clang::tok::l_paren );

        T.consumeOpen();

        clang::SourceLocation startLoc = Tok.getLocation();
        clang::SourceLocation endLoc   = Tok.getEndLoc();

        while ( !Tok.is( clang::tok::r_paren ) )
        {
            endLoc = Tok.getEndLoc();
            ConsumeAnyToken();
        }

        std::string strFile;
        VERIFY_RTE( getSourceText( startLoc, endLoc, strFile ) );
        strFile.erase( std::remove( strFile.begin(), strFile.end(), '\"' ), strFile.end() );
        if ( !strFile.empty() )
        {
            const bool bIsAngled = strFile[ 0 ] == '<';
            if ( bIsAngled )
            {
                VERIFY_RTE( strFile.back() == '>' );
                strFile = std::string( strFile.begin() + 1, strFile.end() - 1 );

                if ( pIdentifier )
                {
                    MEGA_PARSER_ERROR( "System include has identifier" );
                }
                pResult = database.construct< SystemInclude >( SystemInclude::Args{ Include::Args{}, strFile } );
            }
            else
            {
                const boost::filesystem::path filePath = resolveFilePath( strFile );
                if ( boost::filesystem::extension( filePath ) == mega::io::megaFilePath::extension() )
                {
                    if ( pIdentifier )
                        pResult = database.construct< MegaIncludeNested >(
                            MegaIncludeNested::Args{ MegaInclude::Args{ Include::Args{}, filePath }, pIdentifier } );
                    else
                        pResult = database.construct< MegaIncludeInline >(
                            MegaIncludeInline::Args{ MegaInclude::Args{ Include::Args{}, filePath } } );
                }
                else
                {
                    if ( pIdentifier )
                    {
                        MEGA_PARSER_ERROR( "C++  include has identifier" );
                    }
                    pResult = database.construct< CPPInclude >( CPPInclude::Args{ Include::Args{}, filePath } );
                }
            }
        }
        else
        {
            MEGA_PARSER_ERROR( "Include file is empty" );
        }

        T.consumeClose();

        parse_semicolon();

        VERIFY_RTE( pResult );
        return pResult;
    }

    Dependency* parse_dependency( Database& database )
    {
        BalancedDelimiterTracker T( *this, clang::tok::l_paren );

        T.consumeOpen();

        clang::SourceLocation startLoc = Tok.getLocation();
        clang::SourceLocation endLoc   = Tok.getEndLoc();

        while ( !Tok.is( clang::tok::r_paren ) )
        {
            endLoc = Tok.getEndLoc();
            ConsumeAnyToken();
        }

        std::string strDependency;
        VERIFY_RTE( getSourceText( startLoc, endLoc, strDependency ) );

        T.consumeClose();

        parse_semicolon();

        return database.construct< Dependency >( Dependency::Args{ strDependency } );
    }

    ContextDef::Args defaultBody( ScopedIdentifier* pScopedIdentifier ) const
    {
        ContextDef::Args body( pScopedIdentifier,
                               std::vector< ContextDef* >{},
                               std::vector< Dimension* >{},
                               std::vector< Include* >{},
                               std::vector< Dependency* >{},
                               std::string{} );
        return body;
    }

    NamespaceDef* parse_namespace( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if ( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
                T.consumeClose();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected body in abstract" );
            }
        }

        return database.construct< NamespaceDef >( NamespaceDef::Args{ body } );
    }

    AbstractDef* parse_abstract( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if ( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
                T.consumeClose();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected body in abstract" );
            }
        }

        return database.construct< AbstractDef >( AbstractDef::Args{ body, pInheritance } );
    }

    EventDef* parse_event( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Size* pSize = parse_size( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if ( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
                T.consumeClose();
            }
            else if ( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return database.construct< EventDef >( EventDef::Args{ body, pSize, pInheritance } );
    }

    FunctionDef* parse_function( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        ArgumentList* pArgumentList = parse_argumentList( database );
        parse_comment();
        ReturnType* pReturnType = parse_returnType( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if ( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
                T.consumeClose();
            }
            else if ( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return database.construct< FunctionDef >( FunctionDef::Args{ body, pArgumentList, pReturnType } );
    }

    ObjectDef* parse_object( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if ( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
                T.consumeClose();
            }
            else if ( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return database.construct< ObjectDef >( ObjectDef::Args{ body, pInheritance } );
    }

    LinkDef* parse_link( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if ( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
                T.consumeClose();
            }
            else if ( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return database.construct< LinkDef >( LinkDef::Args{ body, pInheritance } );
    }

    ActionDef* parse_action( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Size* pSize = parse_size( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if ( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
                T.consumeClose();
            }
            else if ( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return database.construct< ActionDef >( ActionDef::Args{ body, pSize, pInheritance } );
    }

    ContextDef::Args parse_context_body( Database& database, ScopedIdentifier* pScopedIdentifier )
    {
        ContextDef::Args bodyArgs = defaultBody( pScopedIdentifier );

        braceStack.push_back( BraceCount );

        bool bIsBodyDefinition = false;

        while ( !isEofOrEom() )
        {
            if ( Tok.is( clang::tok::kw_namespace ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_namespace( database ) );
            }
            else if ( Tok.is( clang::tok::kw_abstract ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_abstract( database ) );
            }
            else if ( Tok.is( clang::tok::kw_event ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_event( database ) );
            }
            else if ( Tok.is( clang::tok::kw_function ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_function( database ) );
            }
            else if ( Tok.is( clang::tok::kw_action ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_action( database ) );
            }
            else if ( Tok.is( clang::tok::kw_link ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_link( database ) );
            }
            else if ( Tok.is( clang::tok::kw_object ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_object( database ) );
            }
            else if ( ( Tok.is( clang::tok::kw_const ) && NextToken().is( clang::tok::kw_dim ) ) || Tok.is( clang::tok::kw_dim ) )
            {
                bool bIsConst = false;
                if ( Tok.is( clang::tok::kw_const ) )
                {
                    bIsConst = true;
                    ConsumeToken();
                }
                ConsumeToken();
                Dimension* pDimension = parse_dimension( database, bIsConst );
                bodyArgs.dimensions.value().push_back( pDimension );
            }
            else if ( Tok.is( clang::tok::kw_include ) )
            {
                ConsumeToken();
                Include* pInclude = parse_include( database );
                bodyArgs.includes.value().push_back( pInclude );
            }
            else if ( Tok.is( clang::tok::kw_dependency ) )
            {
                ConsumeToken();
                Dependency* pDependency = parse_dependency( database );
                bodyArgs.dependencies.value().push_back( pDependency );
            }
            else if ( Tok.is( clang::tok::r_brace ) && ( BraceCount == braceStack.back() ) )
            {
                // leave the r_brace to be consumed by parent
                break;
            }
            else
            {
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();

                while
                    // clang-format off
                (
                    !isEofOrEom() &&
                    (
                        !(
                            Tok.is( clang::tok::kw_const ) &&
                            NextToken().is( clang::tok::kw_dim )
                        ) &&
                        !Tok.isOneOf
                        (
                            clang::tok::kw_action,
                            clang::tok::kw_object,
                            clang::tok::kw_function,
                            clang::tok::kw_event,
                            clang::tok::kw_abstract,
                            clang::tok::kw_dim,
                            clang::tok::kw_link,
                            clang::tok::kw_include,
                            clang::tok::kw_dependency
                        )
                    ) &&
                    !(
                        ( BraceCount == braceStack.back() ) &&
                        Tok.is( clang::tok::r_brace )
                    )
                )
                // clang-format on
                {
                    if ( !Tok.isOneOf( clang::tok::comment, clang::tok::eof, clang::tok::eod, clang::tok::code_completion ) )
                    {
                        bIsBodyDefinition = true;
                    }

                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }

                {
                    std::string strBodyPart;
                    VERIFY_RTE( getSourceText( startLoc, endLoc, strBodyPart ) );

                    // capture body if allowed
                    if ( !strBodyPart.empty() && bIsBodyDefinition )
                    {
                        bodyArgs.body = strBodyPart;
                    }
                }
            }
        }

        VERIFY_RTE( BraceCount == braceStack.back() );
        braceStack.pop_back();

        return bodyArgs;
    }

    ContextDef* parse_file( Database& database )
    {
        const std::string strLocation = Tok.getLocation().printToString( sm );
        ScopedIdentifier* pID
            = database.construct< ScopedIdentifier >( ScopedIdentifier::Args{ std::vector< Identifier* >{}, strLocation, 0U } );
        ContextDef::Args args = parse_context_body( database, pID );
        return database.construct< ContextDef >( args );
    }
};

struct EG_PARSER_IMPL : EG_PARSER_INTERFACE
{
    virtual ContextDef* parseEGSourceFile( Database&                                     database,
                                           const boost::filesystem::path&                sourceFile,
                                           const std::vector< boost::filesystem::path >& includeDirectories,
                                           std::ostream&                                 osError,
                                           std::ostream&                                 osWarn )

    {
        boost::filesystem::path sourceDir = sourceFile;
        sourceDir.remove_filename();

        ParserDiagnosticSystem                               pds( sourceDir, osError, osWarn );
        std::shared_ptr< clang::FileManager >                pFileManager       = get_clang_fileManager( pds );
        llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnosticsEngine = get_llvm_diagnosticEngine( pds );

        // check file exists
        if ( !boost::filesystem::exists( sourceFile ) )
        {
            THROW_RTE( "File not found: " << sourceFile.string() );
        }

        std::shared_ptr< clang::HeaderSearchOptions > headerSearchOptions = std::make_shared< clang::HeaderSearchOptions >();
        for ( const boost::filesystem::path& includeDir : includeDirectories )
        {
            headerSearchOptions->AddPath( includeDir.native(), clang::frontend::Quoted, false, false );
        }

        Stuff stuff( headerSearchOptions, includeDirectories, pFileManager, pDiagnosticsEngine, sourceFile );

        MegaParser parser( stuff, pDiagnosticsEngine );
        parser.ConsumeAnyToken( true );

        return parser.parse_file( database );
    }
};
extern "C" BOOST_SYMBOL_EXPORT EG_PARSER_IMPL g_parserSymbol;
EG_PARSER_IMPL                                g_parserSymbol;
