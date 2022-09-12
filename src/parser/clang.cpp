
#include "clang.hpp"

#include "common/assert_verify.hpp"
#include "clang/Basic/DirectoryEntry.h"
#include "clang/Lex/DirectoryLookup.h"

class ParserDiagnosticSystem::EGDiagConsumer : public clang::DiagnosticConsumer
{
    std::ostream& m_errorStream;
    std::ostream& m_warnStream;

public:
    EGDiagConsumer( std::ostream& osError, std::ostream& osWarn )
        : m_errorStream( osError )
        , m_warnStream( osWarn )
    {
    }

    virtual void anchor() {}

    void HandleDiagnostic( clang::DiagnosticsEngine::Level DiagLevel, const clang::Diagnostic& Info ) override
    {
        llvm::SmallString< 100 > msg;
        switch ( DiagLevel )
        {
            case clang::DiagnosticsEngine::Ignored:
            case clang::DiagnosticsEngine::Note:
            case clang::DiagnosticsEngine::Remark:
            case clang::DiagnosticsEngine::Warning:
            {
                Info.FormatDiagnostic( msg );
                std::string str( msg.begin(), msg.end() );
                m_warnStream << str << "\n";
            }
            break;
            case clang::DiagnosticsEngine::Error:
            case clang::DiagnosticsEngine::Fatal:
            {
                Info.FormatDiagnostic( msg );
                std::string str( msg.begin(), msg.end() );
                m_errorStream << str << "\n";
            }
            break;
        }
    }
};

class ParserDiagnosticSystem::Pimpl
{
public:
    std::shared_ptr< clang::FileManager > m_pFileManager;

    llvm::IntrusiveRefCntPtr< clang::DiagnosticOptions > m_pDiagnosticOptions;
    llvm::IntrusiveRefCntPtr< clang::DiagnosticIDs >     m_pDiagnosticIDs;
    llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > m_pDiagnosticsEngine;

    Pimpl( const boost::filesystem::path& currentPath, std::ostream& osError, std::ostream& osWarn );
};

ParserDiagnosticSystem::Pimpl::Pimpl( const boost::filesystem::path& currentPath, std::ostream& osError,
                                      std::ostream& osWarn )
    : m_pFileManager( std::make_shared< clang::FileManager >( clang::FileSystemOptions{ currentPath.string() } ) )
    , m_pDiagnosticOptions( new clang::DiagnosticOptions() )
    , m_pDiagnosticIDs( new clang::DiagnosticIDs() )
    , m_pDiagnosticsEngine( new clang::DiagnosticsEngine(
          m_pDiagnosticIDs, m_pDiagnosticOptions, new ParserDiagnosticSystem::EGDiagConsumer( osError, osWarn ) ) )
{
}

ParserDiagnosticSystem::ParserDiagnosticSystem( const boost::filesystem::path& currentPath, std::ostream& osError,
                                                std::ostream& osWarn )
    : m_pImpl( new Pimpl( currentPath, osError, osWarn ) )
{
}

std::shared_ptr< clang::FileManager > ParserDiagnosticSystem::get_clang_fileManager()
{
    return m_pImpl->m_pFileManager;
}

llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > ParserDiagnosticSystem::get_llvm_diagnosticEngine()
{
    return m_pImpl->m_pDiagnosticsEngine;
}
std::shared_ptr< clang::FileManager > get_clang_fileManager( ParserDiagnosticSystem& diagnosticSystem )
{
    return diagnosticSystem.get_clang_fileManager();
}

llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine >
get_llvm_diagnosticEngine( ParserDiagnosticSystem& diagnosticSystem )
{
    return diagnosticSystem.get_llvm_diagnosticEngine();
}

Stuff::Stuff( std::shared_ptr< clang::HeaderSearchOptions > headerSearchOptions,
              const std::vector< boost::filesystem::path >& includeDirectories,
              std::shared_ptr< clang::FileManager >
                  pFileManager,
              llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine >
                                             pDiagnosticsEngine,
              const boost::filesystem::path& sourceFile )

    : pSourceManager( std::make_unique< clang::SourceManager >( *pDiagnosticsEngine, *pFileManager ) )
    , headerSearchOptions( headerSearchOptions )
    , languageOptions( createEGLangOpts() )
    , pHeaderSearch( std::make_unique< clang::HeaderSearch >(
          headerSearchOptions, *pSourceManager, *pDiagnosticsEngine, languageOptions, nullptr ) )
    , pModuleLoader( std::make_unique< clang::TrivialModuleLoader >() )
    , pPreprocessorOptions( std::make_shared< clang::PreprocessorOptions >() )
    , pPreprocessor( std::make_shared< clang::Preprocessor >( pPreprocessorOptions,
                                                              *pDiagnosticsEngine,
                                                              languageOptions,
                                                              *pSourceManager,
                                                              *pHeaderSearch,
                                                              *pModuleLoader,
                                                              // PTHMgr
                                                              nullptr,
                                                              // OwnsHeaderSearch
                                                              false,
                                                              clang::TU_Complete ) ) // U_Prefix
    , pTargetOptions( getTargetOptions() )
    , pTargetInfo( clang::TargetInfo::CreateTargetInfo( *pDiagnosticsEngine, pTargetOptions ) )
{
    if ( auto f = pFileManager->getFile( llvm::StringRef( sourceFile.string() ), true, false ) )
    {
        pFileEntry           = f.get();
        clang::FileID fileID = pSourceManager->getOrCreateFileID( pFileEntry, clang::SrcMgr::C_User );
        pSourceManager->setMainFileID( fileID );
        pPreprocessor->SetCommentRetentionState( true, true );
        pPreprocessor->Initialize( *pTargetInfo );

        for ( const boost::filesystem::path& includeDir : includeDirectories )
        {
            if ( auto f = pFileManager->getDirectoryRef( includeDir.native(), false ) )
            {
                auto dirLookup = clang::DirectoryLookup( f.get(), clang::SrcMgr::C_System, false );
                pHeaderSearch->AddSearchPath( dirLookup, false );
            }
        }
    }
}

clang::LangOptions Stuff::createEGLangOpts()
{
    clang::LangOptions LangOpts;
    LangOpts.CPlusPlus   = 1;
    LangOpts.CPlusPlus11 = 1;
    LangOpts.CPlusPlus14 = 1;
    LangOpts.CPlusPlus17 = 1;
    // LangOpts.CPlusPlus2a      = 1;
    LangOpts.LineComment      = 1;
    LangOpts.CXXOperatorNames = 1;
    LangOpts.Bool             = 1;
    // LangOpts.ObjC = 1;
    // LangOpts.MicrosoftExt = 1;    // To get kw___try, kw___finally.
    LangOpts.DeclSpecKeyword = 1; // To get __declspec.
    LangOpts.WChar           = 1; // To get wchar_t
    LangOpts.EG              = 1; // enable eg
    return LangOpts;
}

std::shared_ptr< clang::TargetOptions > Stuff::getTargetOptions()
{
    std::shared_ptr< clang::TargetOptions > pTargetOptions = std::make_shared< clang::TargetOptions >();

    llvm::Triple triple;
    triple.setArch( llvm::Triple::x86 );
    triple.setVendor( llvm::Triple::PC );
    triple.setOS( llvm::Triple::Linux );

    pTargetOptions->Triple = triple.normalize();
    return pTargetOptions;
}

// Parser
bool Parser::getSourceText( clang::SourceLocation startLoc, clang::SourceLocation endLoc, std::string& str )
{
    bool                     bInvalid = false;
    const clang::SourceRange range( startLoc, endLoc );
    clang::CharSourceRange   charRange = clang::CharSourceRange::getCharRange( range );
    str                                = clang::Lexer::getSourceText( charRange, sm, languageOptions, &bInvalid );
    // sort out carriage returns
    boost::replace_all( str, "\r\n", "\n" );

    return !bInvalid;
}

Parser::Parser( Stuff& stuff, llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > Diags )
    : fileEntry( *stuff.pFileEntry )
    , PP( *stuff.pPreprocessor )
    , sm( *stuff.pSourceManager )
    , languageOptions( stuff.languageOptions )
    , headerSearch( *stuff.pHeaderSearch )
    , Diags( Diags )
{
    PP.EnterMainSourceFile();
}

const clang::Token& Parser::getCurToken() const { return Tok; }

const clang::Token& Parser::NextToken() { return PP.LookAhead( 0 ); }

clang::SourceLocation Parser::ConsumeToken()
{
    assert( !isTokenSpecial() && "Should consume special tokens with Consume*Token" );
    PrevTokLocation = Tok.getLocation();
    PP.Lex( Tok );
    return PrevTokLocation;
}

bool Parser::TryConsumeToken( clang::tok::TokenKind Expected )
{
    if ( Tok.isNot( Expected ) )
        return false;
    assert( !isTokenSpecial() && "Should consume special tokens with Consume*Token" );
    PrevTokLocation = Tok.getLocation();
    PP.Lex( Tok );
    return true;
}

clang::SourceLocation Parser::ConsumeAnyToken( bool ConsumeCodeCompletionTok /*= false*/ )
{
    if ( isTokenParen() )
        return ConsumeParen();
    if ( isTokenBracket() )
        return ConsumeBracket();
    if ( isTokenBrace() )
        return ConsumeBrace();
    if ( isTokenStringLiteral() )
        return ConsumeStringToken();
    // if (Tok.is(clang::tok::code_completion))
    //   return ConsumeCodeCompletionTok ? ConsumeCodeCompletionToken()
    //                                   : handleUnexpectedCodeCompletionToken();
    if ( Tok.isAnnotation() )
        return ConsumeAnnotationToken();
    return ConsumeToken();
}

bool Parser::isTokenParen() const { return Tok.isOneOf( clang::tok::l_paren, clang::tok::r_paren ); }
/// isTokenBracket - Return true if the cur token is '[' or ']'.
bool Parser::isTokenBracket() const { return Tok.isOneOf( clang::tok::l_square, clang::tok::r_square ); }
/// isTokenBrace - Return true if the cur token is '{' or '}'.
bool Parser::isTokenBrace() const { return Tok.isOneOf( clang::tok::l_brace, clang::tok::r_brace ); }
/// isTokenStringLiteral - True if this token is a string-literal.
bool Parser::isTokenStringLiteral() const { return clang::tok::isStringLiteral( Tok.getKind() ); }
/// isTokenSpecial - True if this token requires special consumption methods.
bool Parser::isTokenSpecial() const
{
    return isTokenStringLiteral() || isTokenParen() || isTokenBracket() || isTokenBrace()
           || Tok.is( clang::tok::code_completion ) || Tok.isAnnotation();
}

void Parser::UnconsumeToken( clang::Token& Consumed )
{
    clang::Token Next = Tok;
    PP.EnterToken( Consumed, true );
    PP.Lex( Tok );
    PP.EnterToken( Next, true );
}

clang::SourceLocation Parser::ConsumeAnnotationToken()
{
    assert( Tok.isAnnotation() && "wrong consume method" );
    clang::SourceLocation Loc = Tok.getLocation();
    PrevTokLocation           = Tok.getAnnotationEndLoc();
    PP.Lex( Tok );
    return Loc;
}

/// ConsumeParen - This consume method keeps the paren count up-to-date.
///
clang::SourceLocation Parser::ConsumeParen()
{
    assert( isTokenParen() && "wrong consume method" );
    if ( Tok.getKind() == clang::tok::l_paren )
        ++ParenCount;
    else if ( ParenCount )
    {
        AngleBrackets.clear( *this );
        --ParenCount; // Don't let unbalanced )'s drive the count negative.
    }
    PrevTokLocation = Tok.getLocation();
    PP.Lex( Tok );
    return PrevTokLocation;
}

clang::SourceLocation Parser::ConsumeBracket()
{
    assert( isTokenBracket() && "wrong consume method" );
    if ( Tok.getKind() == clang::tok::l_square )
        ++BracketCount;
    else if ( BracketCount )
    {
        AngleBrackets.clear( *this );
        --BracketCount; // Don't let unbalanced ]'s drive the count negative.
    }

    PrevTokLocation = Tok.getLocation();
    PP.Lex( Tok );
    return PrevTokLocation;
}

/// ConsumeBrace - This consume method keeps the brace count up-to-date.
///
clang::SourceLocation Parser::ConsumeBrace()
{
    assert( isTokenBrace() && "wrong consume method" );
    if ( Tok.getKind() == clang::tok::l_brace )
        ++BraceCount;
    else if ( BraceCount )
    {
        AngleBrackets.clear( *this );
        --BraceCount; // Don't let unbalanced }'s drive the count negative.
    }

    PrevTokLocation = Tok.getLocation();
    PP.Lex( Tok );
    return PrevTokLocation;
}

clang::SourceLocation Parser::ConsumeStringToken()
{
    assert( isTokenStringLiteral() && "Should only consume string literals with this method" );
    PrevTokLocation = Tok.getLocation();
    PP.Lex( Tok );
    return PrevTokLocation;
}

clang::SourceLocation Parser::ConsumeCodeCompletionToken()
{
    assert( Tok.is( clang::tok::code_completion ) );
    PrevTokLocation = Tok.getLocation();
    PP.Lex( Tok );
    return PrevTokLocation;
}

bool Parser::isEofOrEom()
{
    clang::tok::TokenKind Kind = Tok.getKind();
    return Kind == clang::tok::eof || Kind == clang::tok::annot_module_begin || Kind == clang::tok::annot_module_end
           || Kind == clang::tok::annot_module_include;
}

class Parser::TentativeParsingAction
{
    Parser&        P;
    clang::Token   PrevTok;
    size_t         PrevTentativelyDeclaredIdentifierCount;
    unsigned short PrevParenCount, PrevBracketCount, PrevBraceCount;
    bool           isActive;

public:
    explicit TentativeParsingAction( Parser& p )
        : P( p )
    {
        PrevTok                                = P.Tok;
        PrevTentativelyDeclaredIdentifierCount = P.TentativelyDeclaredIdentifiers.size();
        PrevParenCount                         = P.ParenCount;
        PrevBracketCount                       = P.BracketCount;
        PrevBraceCount                         = P.BraceCount;
        P.PP.EnableBacktrackAtThisPos();
        isActive = true;
    }
    void Commit()
    {
        assert( isActive && "Parsing action was finished!" );
        P.TentativelyDeclaredIdentifiers.resize( PrevTentativelyDeclaredIdentifierCount );
        P.PP.CommitBacktrackedTokens();
        isActive = false;
    }
    void Revert()
    {
        assert( isActive && "Parsing action was finished!" );
        P.PP.Backtrack();
        P.Tok = PrevTok;
        P.TentativelyDeclaredIdentifiers.resize( PrevTentativelyDeclaredIdentifierCount );
        P.ParenCount   = PrevParenCount;
        P.BracketCount = PrevBracketCount;
        P.BraceCount   = PrevBraceCount;
        isActive       = false;
    }
    ~TentativeParsingAction() { assert( !isActive && "Forgot to call Commit or Revert!" ); }
};

class Parser::RevertingTentativeParsingAction : private Parser::TentativeParsingAction
{
public:
    RevertingTentativeParsingAction( Parser& P )
        : Parser::TentativeParsingAction( P )
    {
    }
    ~RevertingTentativeParsingAction() { Revert(); }
};

Parser::BalancedDelimiterTracker::BalancedDelimiterTracker( Parser& p, clang::tok::TokenKind k,
                                                            clang::tok::TokenKind FinalToken /*= clang::tok::semi*/ )
    : GreaterThanIsOperatorScope( p.GreaterThanIsOperator, true )
    , P( p )
    , Kind( k )
    , FinalToken( FinalToken )
{
    switch ( Kind )
    {
        default:
            llvm_unreachable( "Unexpected balanced token" );
        case clang::tok::l_brace:
            Close    = clang::tok::r_brace;
            Consumer = &Parser::ConsumeBrace;
            break;
        case clang::tok::l_paren:
            Close    = clang::tok::r_paren;
            Consumer = &Parser::ConsumeParen;
            break;

        case clang::tok::l_square:
            Close    = clang::tok::r_square;
            Consumer = &Parser::ConsumeBracket;
            break;
    }
}

bool                  Parser::BalancedDelimiterTracker::diagnoseMissingClose() { THROW_RTE( "diagnoseMissingClose" ); }
clang::SourceLocation Parser::BalancedDelimiterTracker::getOpenLocation() const { return LOpen; }
clang::SourceLocation Parser::BalancedDelimiterTracker::getCloseLocation() const { return LClose; }
clang::SourceRange    Parser::BalancedDelimiterTracker::getRange() const { return clang::SourceRange( LOpen, LClose ); }

bool Parser::BalancedDelimiterTracker::consumeOpen()
{
    if ( !P.Tok.is( Kind ) )
        return true;

    // if (getDepth() < P.getLangOpts().BracketDepth)
    {
        LOpen = ( P.*Consumer )();
        return false;
    }

    // return diagnoseOverflow();
}

bool Parser::BalancedDelimiterTracker::consumeClose()
{
    if ( P.Tok.is( Close ) )
    {
        LClose = ( P.*Consumer )();
        return false;
    }
    else if ( P.Tok.is( clang::tok::semi ) && P.NextToken().is( Close ) )
    {
        clang::SourceLocation SemiLoc = P.ConsumeToken();
        P.Diag( SemiLoc, clang::diag::err_unexpected_semi )
            << Close << clang::FixItHint::CreateRemoval( clang::SourceRange( SemiLoc, SemiLoc ) );
        LClose = ( P.*Consumer )();
        return false;
    }

    return diagnoseMissingClose();
}

bool Parser::SkipUntil( clang::tok::TokenKind T, SkipUntilFlags Flags /*= static_cast< SkipUntilFlags >( 0 )*/ )
{
    return SkipUntil( llvm::makeArrayRef( T ), Flags );
}
bool Parser::SkipUntil( clang::tok::TokenKind T1, clang::tok::TokenKind T2,
                        SkipUntilFlags Flags /*= static_cast< SkipUntilFlags >( 0 )*/ )
{
    clang::tok::TokenKind TokArray[] = { T1, T2 };
    return SkipUntil( TokArray, Flags );
}
bool Parser::SkipUntil( clang::tok::TokenKind T1, clang::tok::TokenKind T2, clang::tok::TokenKind T3,
                        SkipUntilFlags Flags /*= static_cast< SkipUntilFlags >( 0 )*/ )
{
    clang::tok::TokenKind TokArray[] = { T1, T2, T3 };
    return SkipUntil( TokArray, Flags );
}

bool Parser::HasFlagsSet( Parser::SkipUntilFlags L, Parser::SkipUntilFlags R )
{
    return ( static_cast< unsigned >( L ) & static_cast< unsigned >( R ) ) != 0;
}
bool Parser::SkipUntil( llvm::ArrayRef< clang::tok::TokenKind > Toks,
                        SkipUntilFlags                          Flags /*= static_cast< SkipUntilFlags >( 0 )*/ )
{
    using namespace clang;
    // We always want this function to skip at least one token if the first token
    // isn't T and if not at EOF.
    bool isFirstTokenSkipped = true;
    while ( 1 )
    {
        // If we found one of the tokens, stop and return true.
        for ( unsigned i = 0, NumToks = Toks.size(); i != NumToks; ++i )
        {
            if ( Tok.is( Toks[ i ] ) )
            {
                if ( HasFlagsSet( Flags, StopBeforeMatch ) )
                {
                    // Noop, don't consume the token.
                }
                else
                {
                    ConsumeAnyToken();
                }
                return true;
            }
        }

        // Important special case: The caller has given up and just wants us to
        // skip the rest of the file. Do this without recursing, since we can
        // get here precisely because the caller detected too much recursion.
        if ( Toks.size() == 1 && Toks[ 0 ] == tok::eof && !HasFlagsSet( Flags, StopAtSemi )
             && !HasFlagsSet( Flags, StopAtCodeCompletion ) )
        {
            while ( Tok.isNot( tok::eof ) )
                ConsumeAnyToken();
            return true;
        }

        switch ( Tok.getKind() )
        {
            case tok::eof:
                // Ran out of tokens.
                return false;

            case tok::annot_pragma_openmp:
            case tok::annot_pragma_openmp_end:
                // Stop before an OpenMP pragma boundary.
            case tok::annot_module_begin:
            case tok::annot_module_end:
            case tok::annot_module_include:
                // Stop before we change submodules. They generally indicate a "good"
                // place to pick up parsing again (except in the special case where
                // we're trying to skip to EOF).
                return false;

                // case tok::code_completion:
                //   if (!HasFlagsSet(Flags, StopAtCodeCompletion))
                //     handleUnexpectedCodeCompletionToken();
                //   return false;

            case tok::l_paren:
                // Recursively skip properly-nested parens.
                ConsumeParen();
                if ( HasFlagsSet( Flags, StopAtCodeCompletion ) )
                    SkipUntil( tok::r_paren, StopAtCodeCompletion );
                else
                    SkipUntil( tok::r_paren );
                break;
            case tok::l_square:
                // Recursively skip properly-nested square brackets.
                ConsumeBracket();
                if ( HasFlagsSet( Flags, StopAtCodeCompletion ) )
                    SkipUntil( tok::r_square, StopAtCodeCompletion );
                else
                    SkipUntil( tok::r_square );
                break;
            case tok::l_brace:
                // Recursively skip properly-nested braces.
                ConsumeBrace();
                if ( HasFlagsSet( Flags, StopAtCodeCompletion ) )
                    SkipUntil( tok::r_brace, StopAtCodeCompletion );
                else
                    SkipUntil( tok::r_brace );
                break;

                // Okay, we found a ']' or '}' or ')', which we think should be balanced.
                // Since the user wasn't looking for this token (if they were, it would
                // already be handled), this isn't balanced.  If there is a LHS token at a
                // higher level, we will assume that this matches the unbalanced token
                // and return it.  Otherwise, this is a spurious RHS token, which we skip.
            case tok::r_paren:
                if ( ParenCount && !isFirstTokenSkipped )
                    return false; // Matches something.
                ConsumeParen();
                break;
            case tok::r_square:
                if ( BracketCount && !isFirstTokenSkipped )
                    return false; // Matches something.
                ConsumeBracket();
                break;
            case tok::r_brace:
                if ( BraceCount && !isFirstTokenSkipped )
                    return false; // Matches something.
                ConsumeBrace();
                break;

            case tok::semi:
                if ( HasFlagsSet( Flags, StopAtSemi ) )
                    return false;
                LLVM_FALLTHROUGH;
            default:
                // Skip this token.
                ConsumeAnyToken();
                break;
        }
        isFirstTokenSkipped = false;
    }
}
