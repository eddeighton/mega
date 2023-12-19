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

// disable clang warnings

#include "common/clang_warnings.hpp"

#include "parser/parser.hpp"
#include "clang.hpp"

#include "mega/values/compilation/cardinality.hpp"
#include "mega/values/compilation/ownership.hpp"
#include "mega/common_strings.hpp"

#include "database/sources.hpp"

#include "database/ParserStage.hxx"
#include "database/environment.hxx"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include "clang/Basic/LLVM.h"
#include "clang/Basic/TokenKinds.h"

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/file_status.hpp>
#include <boost/tokenizer.hpp>

#include "clang/Basic/FileManager.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/BitmaskEnum.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/LangOptions.h"
// #include "clang/Basic/MemoryBufferCache.h"
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

#include <unordered_map>

using namespace ParserStage;
using namespace ParserStage::Parser;

// cannibalised version of clang parser for parsing eg source code
class MegaParser : public ::Parser
{
    using SymbolMap = std::unordered_map< std::string, Symbol* >;
    Database& m_database;
    SymbolMap m_symbols;

public:
    MegaParser( Database& database, Stuff& stuff, llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnostics )
        : Parser( stuff, pDiagnostics )
        , m_database( database )
    {
    }

    SourceRange* getSourceRange( clang::SourceLocation startLoc, clang::SourceLocation endLoc )
    {
        return m_database.construct< SourceRange >( SourceRange::Args{
            sm.getFilename( startLoc ).str(),    // value< std::string > source_file;
            sm.getSpellingLineNumber( startLoc ) // value< mega::U64 >   line_number;
        } );
    }

    bool tokIsAt() const
    {
        using namespace std::string_literals;
        return Tok.is( clang::tok::at ) || ( Tok.is( clang::tok::unknown ) && ( PP.getSpelling( Tok ) == "@"s ) );
    }

    bool tokIsHat() const { return Tok.is( clang::tok::caret ); }

    bool tokIsAtOrHat() const { return tokIsAt() || tokIsHat(); }

    void parse_semicolon()
    {
        if( !TryConsumeToken( clang::tok::semi ) )
        {
            // Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
            MEGA_PARSER_ERROR( "Expected semicolon" );
        }
    }

    void parse_comment()
    {
        while( Tok.is( clang::tok::comment ) )
        {
            ConsumeToken();
        }
    }

    Symbol* getSymbol( const std::string& strSymbol )
    {
        auto iFind = m_symbols.find( strSymbol );
        if( iFind != m_symbols.end() )
        {
            return iFind->second;
        }
        else
        {
            auto pSymbol = m_database.construct< Symbol >( Symbol::Args{ strSymbol } );
            m_symbols.insert( { strSymbol, pSymbol } );
            return pSymbol;
        }
    }

    Identifier* parse_identifier()
    {
        std::string str;
        auto        startLoc = Tok.getLocation();

        std::vector< Symbol* > symbols;
        while( Tok.is( clang::tok::identifier ) )
        {
            const auto str = Tok.getIdentifierInfo()->getName().str();
            if( str == mega::EG_OWNER )
            {
                MEGA_PARSER_ERROR( "Invalid use of reserved symbol: " << mega::EG_OWNER );
            }
            if( str == mega::EG_STATE )
            {
                MEGA_PARSER_ERROR( "Invalid use of reserved symbol: " << mega::EG_STATE );
            }
            symbols.push_back( getSymbol( str ) );
            ConsumeToken();

            if( !Tok.is( clang::tok::coloncolon ) )
            {
                break;
            }
            else
            {
                ConsumeToken();
            }
        }
        return m_database.construct< Identifier >(
            Identifier::Args{ getSourceRange( startLoc, Tok.getLocation() ), symbols } );
    }

    /*
    Identifier* generate_unamedIdentifier( const char* pszPrefix )
    {
        const std::string strFileName  = sm.getFilename( Tok.getLocation() ).str();
        const mega::U64   szLineNumber = sm.getSpellingLineNumber( Tok.getLocation() );
        std::string strName;
        {
            std::ostringstream osName;
            osName << "_" << pszPrefix << "_" << szLineNumber;
            strName = osName.str();
            boost::replace_all( strName, "/", "_" );
            boost::replace_all( strName, ".", "_" );
            boost::replace_all( strName, " ", "_" );
        }
        return m_database.construct< Identifier >( Identifier::Args{ strName } );
    }*/

    Type::Variant* parse_type_variant()
    {
        if( Tok.is( clang::tok::identifier ) )
        {
            auto pSymbol = getSymbol( Tok.getIdentifierInfo()->getName().str() );
            ConsumeToken();
            return m_database.construct< Type::Variant >( Type::Variant::Args{ { pSymbol } } );
        }
        else
        {
            MEGA_PARSER_ERROR( "Expected Identifier" );
            return m_database.construct< Type::Variant >( Type::Variant::Args{ {} } );
        }
    }

    Type::Path* parse_type_path()
    {
        if( tokIsAtOrHat() )
        {
            const bool bAbsolute = tokIsHat();

            ConsumeToken();

            std::vector< Type::Variant* > variants;
            while( Tok.is( clang::tok::identifier ) )
            {
                variants.push_back( parse_type_variant() );
                if( !Tok.is( clang::tok::period ) )
                {
                    break;
                }
                else
                {
                    ConsumeToken();
                }
            }
            if( bAbsolute )
            {
                return m_database.construct< Type::Absolute >(
                    Type::Absolute::Args{ Type::Path::Args{ Type::Fragment::Args{}, variants } } );
            }
            else

            {
                return m_database.construct< Type::Deriving >(
                    Type::Deriving::Args{ Type::Path::Args{ Type::Fragment::Args{}, variants } } );
            }
        }
        else
        {
            MEGA_PARSER_ERROR( "Expected @ or ^" );
            return m_database.construct< Type::Path >( Type::Path::Args{ Type::Fragment::Args{}, {} } );
        }
    }

    Type::PathSequence* parse_type_path_sequence()
    {
        if( tokIsAtOrHat() )
        {
            std::vector< Type::Path* > typePaths;
            while( true )
            {
                typePaths.push_back( parse_type_path() );
                parse_comment();
                if( !Tok.is( clang::tok::comma ) )
                {
                    break;
                }
                else
                {
                    ConsumeToken();
                }
            }
            return m_database.construct< Type::PathSequence >( Type::PathSequence::Args{ typePaths } );
        }
        else
        {
            return m_database.construct< Type::PathSequence >( Type::PathSequence::Args{ {} } );
        }
    }

    Type::NamedPath* parse_named_path()
    {
        Type::Path* pTypePath = parse_type_path();
        if( Tok.is( clang::tok::identifier ) )
        {
            return m_database.construct< Type::NamedPath >( Type::NamedPath::Args{
                pTypePath, std::optional< std::string >( Tok.getIdentifierInfo()->getName().str() ) } );
        }
        else
        {
            return m_database.construct< Type::NamedPath >(
                Type::NamedPath::Args{ pTypePath, std::optional< std::string >{} } );
        }
    }

    Type::NamedPathSequence* parse_named_path_sequence()
    {
        if( tokIsAtOrHat() )
        {
            std::vector< Type::NamedPath* > typePaths;
            while( true )
            {
                typePaths.push_back( parse_named_path() );
                parse_comment();
                if( !Tok.is( clang::tok::comma ) )
                {
                    break;
                }
                else
                {
                    ConsumeToken();
                }
            }
            return m_database.construct< Type::NamedPathSequence >( Type::NamedPathSequence::Args{ typePaths } );
        }
        else
        {
            MEGA_PARSER_ERROR( "Expected @ or ^" );
            return m_database.construct< Type::NamedPathSequence >( Type::NamedPathSequence::Args{ {} } );
        }
    }

    Type::CPP* parse_cpp_declaration()
    {
        std::vector< Type::Fragment* > fragments;

        const unsigned short startParenCount = ParenCount, startBracketCount = BracketCount,
                             startBraceCount = BraceCount;

        // could be:
        //      dimension type -> semi
        //      return type    -> semi OR l_brace OR equal
        //      using type     -> semi

        // clang-format off
        while( !isEofOrEom() 
                && !Tok.is( clang::tok::l_brace )
                && !Tok.is( clang::tok::equal ) 
                && !Tok.is( clang::tok::semi )
               // clang-format on
        )
        {
            if( tokIsAtOrHat() )
            {
                Type::Path* pPath = parse_type_path();
                fragments.push_back( pPath );
            }
            else
            {
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();

                // clang-format off
                while( !isEofOrEom() 
                        && !tokIsAtOrHat()
                        && !Tok.is( clang::tok::l_brace )
                        && !Tok.is( clang::tok::equal ) 
                        && !Tok.is( clang::tok::semi )

                       // clang-format on
                )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                std::string strFragment;
                if( !getSourceText( startLoc, endLoc, strFragment ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing fragment" );
                }
                Type::CPPOpaque* pOpaque = m_database.construct< Type::CPPOpaque >(
                    Type::CPPOpaque::Args{ Type::Fragment::Args{}, strFragment } );
                fragments.push_back( pOpaque );
            }
        }

        return m_database.construct< Type::CPP >( Type::CPP::Args{ fragments } );
    }

    std::vector< Type::Fragment* > parse_cpp_block_fragments()
    {
        const unsigned short startParenCount = ParenCount, startBraceCount = BraceCount;

        std::vector< Type::Fragment* > fragments;

        // could be:
        //      Arguments       -> closing paren
        //      Body            -> closing r_brace

        while( !isEofOrEom() &&
               // clang-format off
                !( ( ParenCount == startParenCount ) && ( Tok.is( clang::tok::r_paren ) ) ) && 
                !( ( BraceCount == startBraceCount ) && ( Tok.is( clang::tok::r_brace ) ) )
               // clang-format on

        )
        {
            if( tokIsAtOrHat() )
            {
                Type::Path* pPath = parse_type_path();
                fragments.push_back( pPath );
            }
            else
            {
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();

                while( !isEofOrEom() &&
                       // clang-format off
                    !tokIsAtOrHat() && 
                
                    !( ( ParenCount == startParenCount ) && ( Tok.is( clang::tok::r_paren ) ) ) && 
                    !( ( BraceCount == startBraceCount ) && ( Tok.is( clang::tok::r_brace ) ) )
                       // clang-format on
                )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                std::string strFragment;
                if( !getSourceText( startLoc, endLoc, strFragment ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing fragment" );
                }
                Type::CPPOpaque* pOpaque = m_database.construct< Type::CPPOpaque >(
                    Type::CPPOpaque::Args{ Type::Fragment::Args{}, strFragment } );
                fragments.push_back( pOpaque );
            }
        }
        return fragments;
    }

    Type::CPP* parse_cpp_block()
    {
        return m_database.construct< Type::CPP >( Type::CPP::Args{ parse_cpp_block_fragments() } );
    }

    Size* parse_size()
    {
        auto startLoc = Tok.getLocation();

        std::string strSize;
        if( Tok.is( clang::tok::l_square ) )
        {
            BalancedDelimiterTracker T( *this, clang::tok::l_square );
            T.consumeOpen();

            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();
            ConsumeAnyToken();

            while( !isEofOrEom() && !Tok.is( clang::tok::r_square ) )
            {
                endLoc = Tok.getEndLoc();
                ConsumeAnyToken();
            }
            if( !getSourceText( startLoc, endLoc, strSize ) )
            {
                MEGA_PARSER_ERROR( "Error parsing size" );
            }
            T.consumeClose();
        }
        return m_database.construct< Size >( Size::Args{ getSourceRange( startLoc, Tok.getLocation() ), strSize } );
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    // Type Declarations

    TypeDecl::Return* parse_return()
    {
        auto       startLoc = Tok.getLocation();
        Type::CPP* pCPP     = parse_cpp_declaration();
        return m_database.construct< TypeDecl::Return >( TypeDecl::Return::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pCPP } );
    }

    TypeDecl::Arguments* parse_arguments()
    {
        auto startLoc = Tok.getLocation();

        Type::CPP* pCPP = nullptr;

        if( Tok.is( clang::tok::l_paren ) )
        {
            BalancedDelimiterTracker T( *this, clang::tok::l_paren );
            T.consumeOpen();
            pCPP = parse_cpp_block();
            T.consumeClose();
        }
        else
        {
            MEGA_PARSER_ERROR( "Error parsing argument list" );
            pCPP = m_database.construct< Type::CPP >( Type::CPP::Args{ {} } );
        }

        return m_database.construct< TypeDecl::Arguments >( TypeDecl::Arguments::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pCPP } );
    }

    std::optional< TypeDecl::Transitions* > parse_transitions()
    {
        auto startLoc = Tok.getLocation();

        bool bIsSuccessor   = false;
        bool bIsPredecessor = false;

        if( Tok.is( clang::tok::greater ) )
        {
            bIsSuccessor = true;
            ConsumeAnyToken();
        }
        else if( Tok.is( clang::tok::less ) )
        {
            bIsPredecessor = true;
            ConsumeAnyToken();
        }
        parse_comment();

        if( bIsSuccessor || bIsPredecessor )
        {
            Type::PathSequence* pTypePathSequence = parse_type_path_sequence();
            return m_database.construct< TypeDecl::Transitions >( TypeDecl::Transitions::Args{
                TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pTypePathSequence,
                bIsSuccessor, bIsPredecessor } );
        }
        else
        {
            return std::nullopt;
        }
    }

    TypeDecl::Events* parse_events()
    {
        auto                     startLoc           = Tok.getLocation();
        Type::NamedPathSequence* pNamedPathSequence = parse_named_path_sequence();
        return m_database.construct< TypeDecl::Events >( TypeDecl::Events::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pNamedPathSequence } );
    }

    std::optional< TypeDecl::Inheritance* > parse_inheritance()
    {
        if( Tok.is( clang::tok::colon ) )
        {
            ConsumeAnyToken();
            parse_comment();
            auto                startLoc          = Tok.getLocation();
            Type::PathSequence* pTypePathSequence = parse_type_path_sequence();
            return m_database.construct< TypeDecl::Inheritance >( TypeDecl::Inheritance::Args{
                TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pTypePathSequence } );
        }
        else
        {
            return {};
        }
    }

    TypeDecl::Data* parse_data_type()
    {
        auto       startLoc = Tok.getLocation();
        Type::CPP* pCPP     = parse_cpp_declaration();
        return m_database.construct< TypeDecl::Data >( TypeDecl::Data::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pCPP } );
    }

    TypeDecl::Initialiser* parse_initialiser()
    {
        auto       startLoc = Tok.getLocation();
        Type::CPP* pCPP     = parse_cpp_declaration();
        return m_database.construct< TypeDecl::Initialiser >( TypeDecl::Initialiser::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pCPP } );
    }

    TypeDecl::Link* parse_link_type()
    {
        auto        startLoc  = Tok.getLocation();
        Type::Path* pTypePath = parse_type_path();
        return m_database.construct< TypeDecl::Link >( TypeDecl::Link::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pTypePath } );
    }

    TypeDecl::Alias* parse_alias_type()
    {
        auto        startLoc  = Tok.getLocation();
        Type::Path* pTypePath = parse_type_path();
        return m_database.construct< TypeDecl::Alias >( TypeDecl::Alias::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pTypePath } );
    }

    TypeDecl::Using* parse_using_type()
    {
        auto       startLoc = Tok.getLocation();
        Type::CPP* pCPP     = parse_cpp_declaration();
        return m_database.construct< TypeDecl::Using >( TypeDecl::Using::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pCPP } );
    }

    TypeDecl::Body* parse_body()
    {
        auto startLoc = Tok.getLocation();

        Type::CPP* pCPP = nullptr;

        if( Tok.is( clang::tok::r_brace ) )
        {
            BalancedDelimiterTracker T( *this, clang::tok::r_brace );
            T.consumeOpen();
            pCPP = parse_cpp_block();
            T.consumeClose();
        }
        else
        {
            MEGA_PARSER_ERROR( "Error parsing argument list" );
            pCPP = m_database.construct< Type::CPP >( Type::CPP::Args{ {} } );
        }

        return m_database.construct< TypeDecl::Body >( TypeDecl::Body::Args{
            TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, Tok.getLocation() ) }, pCPP } );
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////

    boost::filesystem::path resolveFilePath( const std::string& strFile )
    {
        if( clang::Optional< clang::FileEntryRef > includeFile = PP.LookupFile(

                clang::SourceLocation(),
                // Filename
                strFile,
                // isAngled
                false,
                // DirectoryLookup *FromDir
                nullptr,
                // FileEntry *FromFile
                //&fileEntry,
                nullptr,
                // ConstSearchDirIterator *CurDir,
                nullptr,
                // SmallVectorImpl<char> *SearchPath
                nullptr,
                // SmallVectorImpl<char> *RelativePath
                nullptr,
                // SuggestedModule
                nullptr,
                // IsMapped
                nullptr,
                // bool *IsFrameworkFound
                nullptr,
                // bool SkipCache = false
                false ) )
        {
            // otherwise hte file should have normal file path and exist
            const boost::filesystem::path filePath = boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( includeFile->getFileEntry().tryGetRealPathName().str() ) );
            if( !boost::filesystem::exists( filePath ) )
            {
                MEGA_PARSER_ERROR( "Cannot locate include file: " << filePath.string() );
            }
            return filePath;
        }
        else
        {
            MEGA_PARSER_ERROR( "Cannot locate include file: " << strFile );
            return boost::filesystem::path{};
        }
    }

    Include* parse_include()
    {
        Include* pResult = nullptr;

        // include name( file );
        // optional identifier
        Identifier* pIdentifier = nullptr;
        if( Tok.is( clang::tok::identifier ) )
        {
            pIdentifier = parse_identifier();
        }

        BalancedDelimiterTracker T( *this, clang::tok::l_paren );

        T.consumeOpen();

        clang::SourceLocation startLoc = Tok.getLocation();
        clang::SourceLocation endLoc   = Tok.getEndLoc();

        while( !Tok.is( clang::tok::r_paren ) )
        {
            endLoc = Tok.getEndLoc();
            ConsumeAnyToken();
        }

        std::string strFile;
        VERIFY_RTE( getSourceText( startLoc, endLoc, strFile ) );
        strFile.erase( std::remove( strFile.begin(), strFile.end(), '\"' ), strFile.end() );
        strFile.erase( std::remove( strFile.begin(), strFile.end(), ' ' ), strFile.end() );
        if( !strFile.empty() )
        {
            auto       pSourceRange = getSourceRange( startLoc, endLoc );
            const bool bIsAngled    = strFile[ 0 ] == '<';
            if( bIsAngled )
            {
                VERIFY_RTE( strFile.back() == '>' );
                strFile = std::string( strFile.begin() + 1, strFile.end() - 1 );

                if( pIdentifier )
                {
                    MEGA_PARSER_ERROR( "System include has identifier" );
                }
                pResult = m_database.construct< SystemInclude >(
                    SystemInclude::Args{ Include::Args{ pSourceRange }, strFile } );
            }
            else
            {
                const boost::filesystem::path filePath = resolveFilePath( strFile );
                if( filePath.extension() == mega::io::megaFilePath::extension() )
                {
                    if( pIdentifier )
                        pResult = m_database.construct< MegaIncludeNested >( MegaIncludeNested::Args{
                            MegaInclude::Args{ Include::Args{ pSourceRange }, filePath }, pIdentifier } );
                    else
                        pResult = m_database.construct< MegaIncludeInline >(
                            MegaIncludeInline::Args{ MegaInclude::Args{ Include::Args{ pSourceRange }, filePath } } );
                }
                else
                {
                    if( pIdentifier )
                    {
                        MEGA_PARSER_ERROR( "C++  include has identifier" );
                    }
                    pResult = m_database.construct< CPPInclude >(
                        CPPInclude::Args{ Include::Args{ pSourceRange }, filePath } );
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

    Part* parse_part()
    {
        const clang::SourceLocation startLoc = Tok.getLocation();
        clang::SourceLocation       endLoc   = Tok.getEndLoc();

        std::vector< std::string > names;
        {
            using namespace ParserStage::Parser;
            std::string strArguments;
            if( Tok.is( clang::tok::l_paren ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_paren );
                T.consumeOpen();

                if( !Tok.is( clang::tok::r_paren ) )
                {
                    auto startPartLoc = Tok.getLocation();
                    endLoc            = Tok.getEndLoc();
                    ConsumeAnyToken();

                    while( !isEofOrEom() && !Tok.is( clang::tok::r_paren ) )
                    {
                        endLoc = Tok.getEndLoc();
                        ConsumeAnyToken();
                    }
                    if( !getSourceText( startPartLoc, endLoc, strArguments ) )
                    {
                        MEGA_PARSER_ERROR( "Error parsing argument list" );
                    }
                }

                T.consumeClose();
            }

            // tokenise the comma delimited list of names
            {
                using TokenizerType = boost::tokenizer< boost::char_separator< char > >;
                const boost::char_separator< char > sep( "," );
                TokenizerType                       tok( strArguments, sep );
                for( TokenizerType::const_iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter )
                {
                    const std::string s = *tok_iter;
                    if( s.empty() )
                    {
                        MEGA_PARSER_ERROR( "Part has empty part name" );
                    }
                    names.push_back( s );
                }
            }
        }

        if( names.empty() )
        {
            MEGA_PARSER_ERROR( "Part does not specify any names" );
        }

        parse_semicolon();
        parse_comment();
        auto pSourceRange = getSourceRange( startLoc, endLoc );

        return m_database.construct< Part >( Part::Args{ pSourceRange, names } );
    }

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    // named elements ( ignoring named include )

    // begin of actual parsing routines for mega grammar
    Dimension* parse_dimension()
    {
        Identifier*     pIdentifier = parse_identifier();
        TypeDecl::Data* pData       = parse_data_type();

        std::optional< TypeDecl::Initialiser* > initialiserOpt;
        {
            if( Tok.is( clang::tok::equal ) )
            {
                ConsumeToken();
                initialiserOpt = parse_initialiser();
                parse_semicolon();
            }
            else
            {
                parse_semicolon();
            }
        }
        return m_database.construct< Dimension >(
            Dimension::Args{ Aggregate::Args{ pIdentifier }, pData, initialiserOpt } );
    }

    void parse_cardinality( mega::Cardinality& linker )
    {
        /*if( Tok.is( clang::tok::exclaim ) )
        {
            linker.setIsNullAllowed( true );
            ConsumeToken();
        }*/

        if( Tok.is( clang::tok::star ) )
        {
            linker.setMany();
            ConsumeToken();
        }
        else
        {
            int iNumber;
            {
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeToken();
                while( !isEofOrEom() && !Tok.is( clang::tok::r_square ) && !Tok.is( clang::tok::colon ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeToken();
                }
                std::string strInteger;
                if( !getSourceText( startLoc, endLoc, strInteger ) )
                {
                    MEGA_PARSER_ERROR( "Error link size" );
                }
                std::istringstream is( strInteger );
                is >> iNumber;
            }
            linker.setNumber( iNumber );
        }
    }

    Link* parse_link( bool bOwning )
    {
        // link Foo Type.Path.To.Somehere [ 0:* ];
        // link Foo Type.Path.To.Somehere;

        Identifier*     pIdentifier = parse_identifier();
        TypeDecl::Link* pType       = parse_link_type();

        parse_comment();

        // default cardinality of zero to one
        mega::CardinalityRange cardinality_range( mega::Cardinality::ZERO, mega::Cardinality::ONE );
        if( Tok.is( clang::tok::l_square ) )
        {
            BalancedDelimiterTracker T( *this, clang::tok::l_square );
            T.consumeOpen();

            mega::Cardinality minimum, maximum;
            {
                parse_cardinality( minimum );
                if( Tok.is( clang::tok::colon ) )
                {
                    ConsumeToken();
                    parse_cardinality( maximum );
                    parse_comment();
                }
                else
                {
                    MEGA_PARSER_ERROR( "Expected colon in link cardinality specifier" );
                }
                if( !Tok.is( clang::tok::r_square ) )
                {
                    MEGA_PARSER_ERROR( "Expected right square bracket for link relation specifier" );
                }
                T.consumeClose();
            }
            cardinality_range = mega::CardinalityRange{ minimum, maximum };
        }
        parse_comment();
        parse_semicolon();

        return m_database.construct< Link >(
            Link::Args{ Aggregate::Args{ pIdentifier }, pType, bOwning, cardinality_range } );
    }

    Alias* parse_alias()
    {
        Identifier*      pIdentifier = parse_identifier();
        TypeDecl::Alias* pType       = parse_alias_type();
        parse_comment();
        return m_database.construct< Alias >( Alias::Args{ Aggregate::Args{ pIdentifier }, pType } );
    }

    Using* parse_using()
    {
        Identifier*      pIdentifier = parse_identifier();
        TypeDecl::Using* pType       = parse_using_type();
        parse_comment();
        return m_database.construct< Using >( Using::Args{ Aggregate::Args{ pIdentifier }, pType } );
    }

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    // Containers

    Container::Args defaultContainer( Identifier* pIdentifier ) const
    {
        Container::Args body( pIdentifier,
                              std::vector< Container* >{},

                              std::vector< Aggregate* >{},
                              std::vector< Dimension* >{},
                              std::vector< Link* >{},
                              std::vector< Alias* >{},
                              std::vector< Using* >{},

                              std::vector< Include* >{},
                              std::vector< Dependency* >{},
                              std::vector< Part* >{},

                              std::nullopt );
        return body;
    }

    Namespace* parse_namespace()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected body in abstract" );
            }
        }

        return m_database.construct< Namespace >( Namespace::Args{ body } );
    }

    Abstract* parse_abstract()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        Size* pSize = parse_size();
        parse_comment();
        auto pInheritanceOpt = parse_inheritance();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected body in abstract" );
            }
        }

        return m_database.construct< Abstract >( Abstract::Args{ body, pSize, pInheritanceOpt } );
    }

    Event* parse_event()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        Size* pSize = parse_size();
        parse_comment();
        auto pInheritanceOpt = parse_inheritance();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return m_database.construct< Event >( Event::Args{ body, pSize, pInheritanceOpt } );
    }

    Interupt* parse_interupt()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        std::optional< TypeDecl::Events* > pEventsOpt = parse_events();
        parse_comment();
        std::optional< TypeDecl::Transitions* > pTransitionsOpt = parse_transitions();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return m_database.construct< Interupt >( Interupt::Args{ body, pEventsOpt, pTransitionsOpt } );
    }

    Requirement* parse_requirement()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        std::optional< TypeDecl::Events* > pEventsOpt = parse_events();
        parse_comment();
        std::optional< TypeDecl::Transitions* > pTransitionsOpt = parse_transitions();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return m_database.construct< Requirement >( Requirement::Args{ body, pEventsOpt, pTransitionsOpt } );
    }

    Function* parse_function()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        TypeDecl::Arguments* pArguments = parse_arguments();
        parse_comment();
        TypeDecl::Return* pReturn = parse_return();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return m_database.construct< Function >( Function::Args{ body, pArguments, pReturn } );
    }

    Decider* parse_decider()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        std::optional< TypeDecl::Events* > pEventsOpt = parse_events();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        if( !pEventsOpt.has_value() )
        {
            MEGA_PARSER_ERROR( "Expected event list for decider" );
        }

        return m_database.construct< Decider >( Decider::Args{ body, pEventsOpt.value() } );
    }

    Object* parse_object()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        Size* pSize = parse_size();
        parse_comment();
        auto pInheritanceOpt = parse_inheritance();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return m_database.construct< Object >( Object::Args{ body, pSize, pInheritanceOpt } );
    }

    State* parse_state()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        Size* pSize = parse_size();
        parse_comment();
        auto pInheritanceOpt = parse_inheritance();
        parse_comment();
        std::optional< TypeDecl::Transitions* > pTransitionsOpt = parse_transitions();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return m_database.construct< State >( State::Args{ body, pSize, pInheritanceOpt, pTransitionsOpt } );
    }

    Action* parse_action()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        Size* pSize = parse_size();
        parse_comment();
        auto pInheritanceOpt = parse_inheritance();
        parse_comment();
        std::optional< TypeDecl::Transitions* > pTransitionsOpt = parse_transitions();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return m_database.construct< Action >(
            Action::Args{ State::Args{ body, pSize, pInheritanceOpt, pTransitionsOpt } } );
    }

    Component* parse_component()
    {
        Identifier* pIdentifier = parse_identifier();
        parse_comment();
        Size* pSize = parse_size();
        parse_comment();
        auto pInheritanceOpt = parse_inheritance();
        parse_comment();
        std::optional< TypeDecl::Transitions* > pTransitionsOpt = parse_transitions();
        parse_comment();

        Container::Args body = defaultContainer( pIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_container( pIdentifier );
                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected semicolon" );
            }
        }

        return m_database.construct< Component >(
            Component::Args{ State::Args{ body, pSize, pInheritanceOpt, pTransitionsOpt } } );
    }

    Container::Args parse_container( Identifier* pIdentifier )
    {
        Container::Args bodyArgs = defaultContainer( pIdentifier );

        braceStack.push_back( BraceCount );

        bool bIsBodyDefinition = false;

        while( !isEofOrEom() )
        {
            if( Tok.is( clang::tok::kw_namespace ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_namespace() );
            }
            else if( Tok.is( clang::tok::kw_interface ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_abstract() );
            }
            else if( Tok.is( clang::tok::kw_event ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_event() );
            }
            else if( Tok.is( clang::tok::kw_interupt ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_interupt() );
            }
            else if( Tok.is( clang::tok::kw_requirement ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_requirement() );
            }
            else if( Tok.is( clang::tok::kw_function ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_function() );
            }
            else if( Tok.is( clang::tok::kw_decider ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_decider() );
            }
            else if( Tok.is( clang::tok::kw_action ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_action() );
            }
            else if( Tok.is( clang::tok::kw_state ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_state() );
            }
            else if( Tok.is( clang::tok::kw_component ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_component() );
            }
            else if( Tok.is( clang::tok::kw_object ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_object() );
            }
            else if( Tok.is( clang::tok::kw_dim ) )
            {
                ConsumeToken();
                Dimension* pDimension = parse_dimension();
                bodyArgs.dimensions.value().push_back( pDimension );
                bodyArgs.aggregates.value().push_back( pDimension );
            }
            else if( Tok.is( clang::tok::kw_mega ) )
            {
                ConsumeToken();
                Alias* pAlias = parse_alias();
                bodyArgs.aliases.value().push_back( pAlias );
                bodyArgs.aggregates.value().push_back( pAlias );
            }
            else if( Tok.is( clang::tok::kw_cpp ) )
            {
                ConsumeToken();
                Using* pUsing = parse_using();
                bodyArgs.usings.value().push_back( pUsing );
                bodyArgs.aggregates.value().push_back( pUsing );
            }
            else if( Tok.is( clang::tok::kw_link ) )
            {
                ConsumeToken();
                Link* pLink = parse_link( false );
                bodyArgs.links.value().push_back( pLink );
                bodyArgs.aggregates.value().push_back( pLink );
            }
            else if( Tok.is( clang::tok::kw_owns ) )
            {
                ConsumeToken();
                Link* pLink = parse_link( true );
                bodyArgs.links.value().push_back( pLink );
                bodyArgs.aggregates.value().push_back( pLink );
            }
            else if( Tok.is( clang::tok::kw_include ) )
            {
                ConsumeToken();
                Include* pInclude = parse_include();
                bodyArgs.includes.value().push_back( pInclude );
            }
            else if( Tok.is( clang::tok::kw_part ) )
            {
                ConsumeToken();
                Part* pPart = parse_part();
                bodyArgs.parts.value().push_back( pPart );
            }
            else if( Tok.is( clang::tok::r_brace ) && ( BraceCount == braceStack.back() ) )
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
                        !Tok.isOneOf
                        (
                            clang::tok::kw_action,
                            clang::tok::kw_component,
                            clang::tok::kw_cpp,
                            clang::tok::kw_decider,
                            clang::tok::kw_dim,
                            clang::tok::kw_event,
                            clang::tok::kw_function,
                            clang::tok::kw_include,
                            clang::tok::kw_interface,
                            clang::tok::kw_interupt,
                            clang::tok::kw_link,
                            clang::tok::kw_mega,
                            clang::tok::kw_namespace,
                            clang::tok::kw_object,
                            clang::tok::kw_owns,
                            clang::tok::kw_part,
                            clang::tok::kw_requirement,
                            clang::tok::kw_state
                            
                            // NOTE: cannot now using 'using namespace ...' in function bodies
                        )
                    ) &&
                    !(
                        ( BraceCount == braceStack.back() ) &&
                        Tok.is( clang::tok::r_brace )
                    )
                )
                // clang-format on
                {
                    if( !Tok.isOneOf(
                            clang::tok::comment, clang::tok::eof, clang::tok::eod, clang::tok::code_completion ) )
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
                    if( !strBodyPart.empty() && bIsBodyDefinition )
                    {
                        if( bodyArgs.body_type_opt.has_value() && bodyArgs.body_type_opt.value().has_value() )
                        {
                            auto                           pBody     = bodyArgs.body_type_opt.value().value();
                            auto                           pCPP      = pBody->get_cpp_fragments();
                            std::vector< Type::Fragment* > fragments = parse_cpp_block_fragments();
                            for( auto pFragment : fragments )
                            {
                                pCPP->push_back_elements( pFragment );
                            }
                        }
                        else
                        {
                            auto pCPP              = parse_cpp_block();
                            bodyArgs.body_type_opt = m_database.construct< TypeDecl::Body >(
                                { TypeDecl::TypeDeclaration::Args{ getSourceRange( startLoc, endLoc ) }, pCPP } );
                        }
                    }
                }
            }
        }

        VERIFY_RTE( BraceCount == braceStack.back() );
        braceStack.pop_back();

        return bodyArgs;
    }

    Container* parse_file()
    {
        auto            pSourceRange = getSourceRange( Tok.getLocation(), Tok.getLocation() );
        Identifier*     pID          = m_database.construct< Identifier >( Identifier::Args{ pSourceRange, {} } );
        Container::Args args         = parse_container( pID );
        return m_database.construct< Container >( args );
    }
};

struct EG_PARSER_IMPL : EG_PARSER_INTERFACE
{
    virtual Container* parseEGSourceFile( Database&                                     database,
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
        if( !boost::filesystem::exists( sourceFile ) )
        {
            THROW_RTE( "File not found: " << sourceFile.string() );
        }

        std::shared_ptr< clang::HeaderSearchOptions > headerSearchOptions
            = std::make_shared< clang::HeaderSearchOptions >();
        for( const boost::filesystem::path& includeDir : includeDirectories )
        {
            headerSearchOptions->AddPath( includeDir.string(), clang::frontend::Quoted, false, false );
        }

        Stuff stuff( headerSearchOptions, includeDirectories, pFileManager, pDiagnosticsEngine, sourceFile );

        MegaParser parser( database, stuff, pDiagnosticsEngine );
        parser.ConsumeAnyToken( true );

        return parser.parse_file();
    }
};

extern "C" BOOST_SYMBOL_EXPORT EG_PARSER_IMPL g_parserSymbol;
MEGA_PARSER_EXPORT EG_PARSER_IMPL             g_parserSymbol;

/*
std::unique_ptr< EG_PARSER_INTERFACE > getParser()
{
    return std::make_unique< EG_PARSER_IMPL >();
}*/
