#ifndef CLANG_PARSER_13_APRIL_2022
#define CLANG_PARSER_13_APRIL_2022

#include "clang/Basic/LLVM.h"

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/file_status.hpp>

// disable clang warnings

#ifdef __clang__
#elif __GNUC__
#pragma GCC diagnostic push
#elif _MSC_VER
#pragma warning( push )
#endif

#include "common/clang_warnings.hpp"

#include "clang/Basic/FileManager.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/BitmaskEnum.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/LangOptions.h"
//#include "clang/Basic/MemoryBufferCache.h"
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

#ifdef __clang__
#elif __GNUC__
#pragma GCC diagnostic pop
#elif _MSC_VER
#pragma warning( pop )
#endif

class ParserDiagnosticSystem
{
public:
    ParserDiagnosticSystem( const boost::filesystem::path& currentPath, std::ostream& osError, std::ostream& osWarn );

    std::shared_ptr< clang::FileManager >                get_clang_fileManager();
    llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > get_llvm_diagnosticEngine();

private:
    class EGDiagConsumer;
    class Pimpl;
    std::shared_ptr< Pimpl > m_pImpl;
};

std::shared_ptr< clang::FileManager > get_clang_fileManager( ParserDiagnosticSystem& diagnosticSystem );

llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > get_llvm_diagnosticEngine( ParserDiagnosticSystem& diagnosticSystem );

#define MEGA_PARSER_ERROR( _msg )                                                                                          \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _os; _os << Tok.getLocation().printToString( sm ) << " " << _msg;  \
                                     Diags->Report( Tok.getLocation(), clang::diag::err_mega_generic_error ) << _os.str(); \
                                      )
// THROW_RTE( "Parser error: " << _os.str() );

#define MEGA_PARSER_WARNING( _msg )                                                                                       \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _os; _os << Tok.getLocation().printToString( sm ) << " " << _msg; \
                                     Diags->Report( Tok.getLocation(), clang::diag::warn_mega_generic_warning ) << _os.str(); )

struct Stuff
{
    friend class Parser;

    Stuff( std::shared_ptr< clang::HeaderSearchOptions > headerSearchOptions,
           const std::vector< boost::filesystem::path >& includeDirectories,
           std::shared_ptr< clang::FileManager >
               pFileManager,
           llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine >
                                          pDiagnosticsEngine,
           const boost::filesystem::path& sourceFile );

    clang::LangOptions                             createEGLangOpts();
    inline std::shared_ptr< clang::TargetOptions > getTargetOptions();

private:
    std::unique_ptr< clang::SourceManager >              pSourceManager;
    std::shared_ptr< clang::HeaderSearchOptions >        headerSearchOptions;
    clang::LangOptions                                   languageOptions;
    std::unique_ptr< clang::HeaderSearch >               pHeaderSearch;
    std::unique_ptr< clang::TrivialModuleLoader >        pModuleLoader;
    std::shared_ptr< clang::PreprocessorOptions >        pPreprocessorOptions;
    std::shared_ptr< clang::Preprocessor >               pPreprocessor;
    std::shared_ptr< clang::TargetOptions >              pTargetOptions;
    std::shared_ptr< clang::TargetInfo >                 pTargetInfo;
    const clang::FileEntry*                              pFileEntry = nullptr;
};

// cannibalised version of clang parser for parsing eg source code
class Parser
{
protected:
    const clang::FileEntry&                              fileEntry;
    clang::Preprocessor&                                 PP;
    clang::SourceManager&                                sm;
    clang::LangOptions&                                  languageOptions;
    clang::HeaderSearch&                                 headerSearch;
    llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > Diags;

    clang::DiagnosticBuilder Diag( clang::SourceLocation Loc, unsigned DiagID ) { return Diags->Report( Loc, DiagID ); }
    clang::DiagnosticBuilder Diag( const clang::Token& Tok, unsigned DiagID ) { return Diag( Tok.getLocation(), DiagID ); }
    clang::DiagnosticBuilder Diag( unsigned DiagID ) { return Diag( Tok, DiagID ); }

    bool getSourceText( clang::SourceLocation startLoc, clang::SourceLocation endLoc, std::string& str );

    /// Tok - The current token we are peeking ahead.  All parsing methods assume
    /// that this is valid.
    clang::Token Tok;

    // PrevTokLocation - The location of the token we previously
    // consumed. This token is used for diagnostics where we expected to
    // see a token following another token (e.g., the ';' at the end of
    // a statement).
    clang::SourceLocation PrevTokLocation;

    unsigned short ParenCount = 0, BracketCount = 0, BraceCount = 0;
    unsigned short MisplacedModuleBeginCount = 0;

    std::vector< unsigned short > braceStack;
    /// ScopeCache - Cache scopes to reduce malloc traffic.
    // enum { ScopeCacheSize = 16 };
    // unsigned NumCachedScopes;
    // Scope *ScopeCache[ScopeCacheSize];

    struct AngleBracketTracker
    {
        /// Flags used to rank candidate template names when there is more than one
        /// '<' in a scope.
        enum Priority : unsigned short
        {
            /// A non-dependent name that is a potential typo for a template name.
            PotentialTypo = 0x0,
            /// A dependent name that might instantiate to a template-name.
            DependentName = 0x2,

            /// A space appears before the '<' token.
            SpaceBeforeLess = 0x0,
            /// No space before the '<' token
            NoSpaceBeforeLess = 0x1,

            LLVM_MARK_AS_BITMASK_ENUM( DependentName )
        };

        struct Loc
        {
            clang::SourceLocation         LessLoc;
            AngleBracketTracker::Priority Priority;
            unsigned short                ParenCount, BracketCount, BraceCount;

            bool isActive( Parser& P ) const
            {
                return P.ParenCount == ParenCount && P.BracketCount == BracketCount && P.BraceCount == BraceCount;
            }

            bool isActiveOrNested( Parser& P ) const
            {
                return isActive( P ) || P.ParenCount > ParenCount || P.BracketCount > BracketCount || P.BraceCount > BraceCount;
            }
        };

        llvm::SmallVector< Loc, 8 > Locs;

        /// Add an expression that might have been intended to be a template name.
        /// In the case of ambiguity, we arbitrarily select the innermost such
        /// expression, for example in 'foo < bar < baz', 'bar' is the current
        /// candidate. No attempt is made to track that 'foo' is also a candidate
        /// for the case where we see a second suspicious '>' token.
        void add( Parser& P, clang::SourceLocation LessLoc, Priority Prio )
        {
            if ( !Locs.empty() && Locs.back().isActive( P ) )
            {
                if ( Locs.back().Priority <= Prio )
                {
                    Locs.back().LessLoc = LessLoc;
                    Locs.back().Priority = Prio;
                }
            }
            else
            {
                Locs.push_back( { LessLoc, Prio, P.ParenCount, P.BracketCount, P.BraceCount } );
            }
        }

        /// Mark the current potential missing template location as having been
        /// handled (this happens if we pass a "corresponding" '>' or '>>' token
        /// or leave a bracket scope).
        void clear( Parser& P )
        {
            while ( !Locs.empty() && Locs.back().isActiveOrNested( P ) )
                Locs.pop_back();
        }

        /// Get the current enclosing expression that might hve been intended to be
        /// a template name.
        Loc* getCurrent( Parser& P )
        {
            if ( !Locs.empty() && Locs.back().isActive( P ) )
                return &Locs.back();
            return nullptr;
        }
    };
    AngleBracketTracker AngleBrackets;

public:
    Parser( Stuff& stuff, llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > Diags );

    const clang::Token& getCurToken() const;

    const clang::Token& NextToken();

    clang::SourceLocation ConsumeToken();

    bool TryConsumeToken( clang::tok::TokenKind Expected );

    clang::SourceLocation ConsumeAnyToken( bool ConsumeCodeCompletionTok = false );

    bool isTokenParen() const;
    /// isTokenBracket - Return true if the cur token is '[' or ']'.
    bool isTokenBracket() const;
    /// isTokenBrace - Return true if the cur token is '{' or '}'.
    bool isTokenBrace() const;
    /// isTokenStringLiteral - True if this token is a string-literal.
    bool isTokenStringLiteral() const;
    /// isTokenSpecial - True if this token requires special consumption methods.
    bool isTokenSpecial() const;

    void UnconsumeToken( clang::Token& Consumed );

    clang::SourceLocation ConsumeAnnotationToken();

    /// ConsumeParen - This consume method keeps the paren count up-to-date.
    ///
    clang::SourceLocation ConsumeParen();

    /// ConsumeBracket - This consume method keeps the bracket count up-to-date.
    ///
    clang::SourceLocation ConsumeBracket();

    /// ConsumeBrace - This consume method keeps the brace count up-to-date.
    ///
    clang::SourceLocation ConsumeBrace();

    /// ConsumeStringToken - Consume the current 'peek token', lexing a new one
    /// and returning the token kind.  This method is specific to strings, as it
    /// handles string literal concatenation, as per C99 5.1.1.2, translation
    /// phase #6.
    clang::SourceLocation ConsumeStringToken();

    /// Consume the current code-completion token.
    ///
    /// This routine can be called to consume the code-completion token and
    /// continue processing in special cases where \c cutOffParsing() isn't
    /// desired, such as token caching or completion with lookahead.
    clang::SourceLocation ConsumeCodeCompletionToken();

    bool isEofOrEom();

    /// Identifiers which have been declared within a tentative parse.
    llvm::SmallVector< clang::IdentifierInfo*, 8 > TentativelyDeclaredIdentifiers;

    class TentativeParsingAction;

    /// A TentativeParsingAction that automatically reverts in its destructor.
    /// Useful for disambiguation parses that will always be reverted.
    class RevertingTentativeParsingAction;

    bool GreaterThanIsOperator;
    class GreaterThanIsOperatorScope
    {
        bool& GreaterThanIsOperator;
        bool  OldGreaterThanIsOperator;

    public:
        GreaterThanIsOperatorScope( bool& GTIO, bool Val )
            : GreaterThanIsOperator( GTIO )
            , OldGreaterThanIsOperator( GTIO )
        {
            GreaterThanIsOperator = Val;
        }
        ~GreaterThanIsOperatorScope() { GreaterThanIsOperator = OldGreaterThanIsOperator; }
    };

    class BalancedDelimiterTracker : public GreaterThanIsOperatorScope
    {
    public:
        BalancedDelimiterTracker( Parser& p, clang::tok::TokenKind k, clang::tok::TokenKind FinalToken = clang::tok::semi );

        bool diagnoseMissingClose();

        clang::SourceLocation getOpenLocation() const;
        clang::SourceLocation getCloseLocation() const;
        clang::SourceRange    getRange() const;
        bool consumeOpen();
        bool consumeClose();

        // unsigned short &getDepth()
        //{
        //     switch( Kind )
        //     {
        //         case clang::tok::l_brace: return P.BraceCount;
        //         case clang::tok::l_square: return P.BracketCount;
        //         case clang::tok::l_paren: return P.ParenCount;
        //         default: llvm_unreachable( "Wrong token kind" );
        //     }
        // }

        // bool diagnoseOverflow();
        //void skipToEnd();

        //bool expectAndConsume( unsigned              DiagID = clang::diag::err_expected,
        //                        const char*           Msg = "",
        //                        clang::tok::TokenKind SkipToTok = clang::tok::unknown );
    private:
        Parser&               P;
        clang::tok::TokenKind Kind, Close, FinalToken;
        clang::SourceLocation ( Parser::*Consumer )();
        clang::SourceLocation LOpen, LClose;

    };

    /// Control flags for SkipUntil functions.
    enum SkipUntilFlags
    {
        StopAtSemi = 1 << 0, ///< Stop skipping at semicolon
        /// Stop skipping at specified token, but don't skip the token itself
        StopBeforeMatch = 1 << 1,
        StopAtCodeCompletion = 1 << 2 ///< Stop at code completion
    };

    friend constexpr SkipUntilFlags operator|( SkipUntilFlags L, SkipUntilFlags R )
    {
        return static_cast< SkipUntilFlags >( static_cast< unsigned >( L ) | static_cast< unsigned >( R ) );
    }

    /// SkipUntil - Read tokens until we get to the specified token, then consume
    /// it (unless StopBeforeMatch is specified).  Because we cannot guarantee
    /// that the token will ever occur, this skips to the next token, or to some
    /// likely good stopping point.  If Flags has StopAtSemi flag, skipping will
    /// stop at a ';' character.
    ///
    /// If SkipUntil finds the specified token, it returns true, otherwise it
    /// returns false.
    bool        SkipUntil( clang::tok::TokenKind T, SkipUntilFlags Flags = static_cast< SkipUntilFlags >( 0 ) );
    bool        SkipUntil( clang::tok::TokenKind T1, clang::tok::TokenKind T2, SkipUntilFlags Flags = static_cast< SkipUntilFlags >( 0 ) );
    bool        SkipUntil( clang::tok::TokenKind T1, clang::tok::TokenKind T2, clang::tok::TokenKind T3,
                           SkipUntilFlags Flags = static_cast< SkipUntilFlags >( 0 ) );
    static bool HasFlagsSet( Parser::SkipUntilFlags L, Parser::SkipUntilFlags R );
    bool        SkipUntil( llvm::ArrayRef< clang::tok::TokenKind > Toks, SkipUntilFlags Flags = static_cast< SkipUntilFlags >( 0 ) );
};

#endif // CLANG_PARSER_13_APRIL_2022
