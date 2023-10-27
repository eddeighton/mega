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

#include "mega/values/compilation/arguments.hpp"
#include "mega/values/compilation/cardinality.hpp"
#include "mega/values/compilation/ownership.hpp"
#include "mega/values/compilation/ownership.hpp"
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
        if( Tok.is( clang::tok::identifier ) )
        {
            clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
            str                                = pIdentifier->getName().str();
            ConsumeToken();
        }
        else
        {
            MEGA_PARSER_ERROR( "Expected identifier" );
        }
        return database.construct< Identifier >( Identifier::Args{ str } );
    }

    Identifier* generate_unamedIdentifier( Database& database )
    {
        const std::string strFileName  = sm.getFilename( Tok.getLocation() ).str();
        const mega::U64   szLineNumber = sm.getSpellingLineNumber( Tok.getLocation() );

        std::string strName;
        {
            std::ostringstream osName;
            osName << "_anon_" << szLineNumber;
            strName = osName.str();
            boost::replace_all( strName, "/", "_" );
            boost::replace_all( strName, ".", "_" );
            boost::replace_all( strName, " ", "_" );
        }

        return database.construct< Identifier >( Identifier::Args{ strName } );
    }

    ScopedIdentifier* parse_scopedIdentifier( Database& database )
    {
        if( !Tok.is( clang::tok::identifier ) )
        {
            MEGA_PARSER_ERROR( "Expected identifier" );
        }

        const std::string strFileName  = sm.getFilename( Tok.getLocation() ).str();
        const mega::U64   szLineNumber = sm.getSpellingLineNumber( Tok.getLocation() );

        // const std::string strLocation = Tok.getLocation().printToString( sm );

        std::vector< Identifier* > identifiers;
        identifiers.push_back( parse_identifier( database ) );
        while( Tok.is( clang::tok::coloncolon ) )
        {
            ConsumeToken();

            if( Tok.is( clang::tok::identifier ) )
            {
                identifiers.push_back( parse_identifier( database ) );
            }
            else
            {
                MEGA_PARSER_ERROR( "Expected identifier" );
            }
        }
        return database.construct< ScopedIdentifier >(
            ScopedIdentifier::Args{ identifiers, strFileName, szLineNumber } );
    }

    ScopedIdentifier* generate_ScopeIdentifier( Database& database, Identifier* pIdentifier )
    {
        const std::string strFileName  = sm.getFilename( Tok.getLocation() ).str();
        const mega::U64   szLineNumber = sm.getSpellingLineNumber( Tok.getLocation() );

        std::vector< Identifier* > identifiers;
        identifiers.push_back( pIdentifier );
        return database.construct< ScopedIdentifier >(
            ScopedIdentifier::Args{ identifiers, strFileName, szLineNumber } );
    }

    ScopedIdentifier* generate_unamedScopeIdentifier( Database& database )
    {
        const std::string strFileName  = sm.getFilename( Tok.getLocation() ).str();
        const mega::U64   szLineNumber = sm.getSpellingLineNumber( Tok.getLocation() );

        std::vector< Identifier* > identifiers;
        identifiers.push_back( generate_unamedIdentifier( database ) );
        return database.construct< ScopedIdentifier >(
            ScopedIdentifier::Args{ identifiers, strFileName, szLineNumber } );
    }

    void parse_semicolon()
    {
        if( !TryConsumeToken( clang::tok::semi ) )
        {
            // Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
            MEGA_PARSER_ERROR( "Expected semicolon" );
        }
    }

    TypeList* parse_typeList( Database& database )
    {
        using namespace ParserStage::Parser;
        std::string strArguments;
        if( Tok.is( clang::tok::l_paren ) )
        {
            BalancedDelimiterTracker T( *this, clang::tok::l_paren );
            T.consumeOpen();

            if( !Tok.is( clang::tok::r_paren ) )
            {
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();

                while( !isEofOrEom() && !Tok.is( clang::tok::r_paren ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                if( !getSourceText( startLoc, endLoc, strArguments ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing argument list" );
                }
            }

            T.consumeClose();
        }

        mega::Type::Vector args;
        mega::parse( strArguments, args );

        return database.construct< TypeList >( TypeList::Args( args ) );
    }

    ArgumentList* parse_argumentList( Database& database )
    {
        using namespace ParserStage::Parser;
        std::string strArguments;
        if( Tok.is( clang::tok::l_paren ) )
        {
            BalancedDelimiterTracker T( *this, clang::tok::l_paren );
            T.consumeOpen();

            if( !Tok.is( clang::tok::r_paren ) )
            {
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();

                while( !isEofOrEom() && !Tok.is( clang::tok::r_paren ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                if( !getSourceText( startLoc, endLoc, strArguments ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing argument list" );
                }
            }

            T.consumeClose();
        }

        mega::TypeName::Vector args;
        mega::parse( strArguments, args );

        return database.construct< ArgumentList >( ArgumentList::Args( args ) );
    }

    void parse_comment()
    {
        while( Tok.is( clang::tok::comment ) )
        {
            ConsumeToken();
        }
    }

    ReturnType* parse_returnType( Database& database )
    {
        std::string str;
        if( Tok.is( clang::tok::colon ) )
        {
            parse_comment();
            ConsumeAnyToken();

            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();
            parse_comment();
            ConsumeAnyToken();

            while( !isEofOrEom() && !Tok.isOneOf( clang::tok::semi, clang::tok::comma, clang::tok::l_brace ) )
            {
                endLoc = Tok.getEndLoc();
                parse_comment();
                ConsumeAnyToken();
            }

            {
                if( !getSourceText( startLoc, endLoc, str ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing return type" );
                }
            }
        }
        return database.construct< ReturnType >( ReturnType::Args{ str } );
    }

    Transition* parse_transition( Database& database )
    {
        std::string str;

        bool isSuccessor   = false;
        bool isPredecessor = false;

        parse_comment();

        if( Tok.is( clang::tok::greater ) )
        {
            isSuccessor = true;
            ConsumeAnyToken();
        }
        else if( Tok.is( clang::tok::less ) )
        {
            isPredecessor = true;
            ConsumeAnyToken();
        }

        parse_comment();

        if( isSuccessor || isPredecessor )
        {
            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();
            parse_comment();
            ConsumeAnyToken();

            while( !isEofOrEom() && !Tok.isOneOf( clang::tok::semi, clang::tok::l_brace ) )
            {
                endLoc = Tok.getEndLoc();
                parse_comment();
                ConsumeAnyToken();
            }
            {
                if( !getSourceText( startLoc, endLoc, str ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing return type" );
                }
            }
        }
        return database.construct< Transition >( Transition::Args{ isSuccessor, isPredecessor, str } );
    }

    Inheritance* parse_inheritance( Database& database, bool bSkipColon = false )
    {
        std::vector< std::string > strings;
        if( bSkipColon || Tok.is( clang::tok::colon ) )
        {
            bool bFoundComma = true;
            bool bFirst      = true;
            while( bFoundComma )
            {
                if( bFirst )
                {
                    bFirst = false;
                    if( !bSkipColon )
                    {
                        ConsumeAnyToken();
                    }
                }
                else
                {
                    ConsumeAnyToken();
                }
                bFoundComma = false;

                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();

                while( !isEofOrEom() && !Tok.isOneOf( clang::tok::semi, clang::tok::comma, clang::tok::l_brace ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }

                {
                    std::string str;
                    if( !getSourceText( startLoc, endLoc, str ) )
                    {
                        MEGA_PARSER_ERROR( "Error parsing inheritance" );
                    }
                    strings.push_back( str );
                }

                if( Tok.is( clang::tok::comma ) )
                    bFoundComma = true;
            }
        }

        return database.construct< Inheritance >( Inheritance::Args{ strings } );
    }

    Size* parse_size( Database& database )
    {
        // parse optional size specifier
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

            {
                if( !getSourceText( startLoc, endLoc, strSize ) )
                {
                    MEGA_PARSER_ERROR( "Error parsing size" );
                }
            }

            T.consumeClose();
        }
        return database.construct< Size >( Size::Args{ strSize } );
    }

    // begin of actual parsing routines for mega grammar
    Dimension* parse_dimension( Database& database, bool bIsConst )
    {
        Dimension::Args args;
        args.isConst = bIsConst;
        {
            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();

            clang::Token next = NextToken();
            while( !next.is( clang::tok::semi ) && !next.is( clang::tok::equal ) )
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

        // parse optional initialiser
        if( Tok.is( clang::tok::equal ) )
        {
            ConsumeToken();

            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();

            while( !Tok.is( clang::tok::semi ) )
            {
                endLoc = Tok.getEndLoc();
                ConsumeToken();
            }

            std::string strInitialiser;
            VERIFY_RTE( getSourceText( startLoc, endLoc, strInitialiser ) );

            // std::cout << "Got initialiser: " << strInitialiser << " from: " << startLoc.printToString( sm )
            //           << " to: " << endLoc.printToString( sm ) << " from offset: " << sm.getFileOffset( startLoc )
            //           << " to offset: " << sm.getFileOffset( endLoc ) << std::endl;

            Initialiser::Args initArgs = { strInitialiser, sm.getFileOffset( startLoc ), sm.getFileOffset( endLoc ) };

            args.initialiser = database.construct< Initialiser >( initArgs );

            parse_semicolon();
        }
        else
        {
            args.initialiser = std::optional< Initialiser* >();
            parse_semicolon();
        }

        return database.construct< Dimension >( args );
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

    mega::Type::Vector parse_typeList()
    {
        clang::SourceLocation startLoc = Tok.getLocation();
        clang::SourceLocation endLoc   = Tok.getEndLoc();

        while( !Tok.is( clang::tok::l_square ) && !Tok.is( clang::tok::semi ) && !Tok.is( clang::tok::equal ) )
        {
            endLoc = Tok.getEndLoc();
            ConsumeAnyToken();
        }

        std::string strTypeList;
        VERIFY_RTE( getSourceText( startLoc, endLoc, strTypeList ) );
        strTypeList.erase( std::remove( strTypeList.begin(), strTypeList.end(), ' ' ), strTypeList.end() );

        mega::Type::Vector typeList;
        mega::parse( strTypeList, typeList );

        return typeList;
    }

    Link* parse_link( Database& database, bool bOwning )
    {
        // link Type.Path.To.Somehere [ 0:* ];
        // link Type.Path.To.Somehere;
        // Interface::TypeName [ 1:1 ];
        // link Foo = Type.Path.To.Somehere [ 0:* ];
        // link Foo = Type.Path.To.Somehere;

        Identifier* pID       = nullptr;
        TypeList*   pTypeList = nullptr;

        mega::Type::Vector typeList = parse_typeList();
        parse_comment();

        if( Tok.is( clang::tok::equal ) )
        {
            if( typeList.size() != 1 )
            {
                MEGA_PARSER_ERROR( "Link alias must be a single symbol" );
            }
            const auto& identifier = typeList.front();
            {
                auto iFind = std::find( identifier.get().begin(), identifier.get().end(), '.' );
                if( iFind != identifier.get().end() )
                {
                    MEGA_PARSER_ERROR( "Link alias cannot be a type path" );
                }
            }

            pID = database.construct< Identifier >( Identifier::Args{ identifier.get() } );

            ConsumeToken();
            parse_comment();

            mega::Type::Vector actulTypeList = parse_typeList();
            if( actulTypeList.size() == 0 )
            {
                MEGA_PARSER_ERROR( "Link must contain a symbol" );
            }
            pTypeList = database.construct< TypeList >( TypeList::Args( actulTypeList ) );
        }
        else
        {
            if( typeList.size() == 0 )
            {
                MEGA_PARSER_ERROR( "Link must contain a symbol" );
            }

            const std::string& strLast   = typeList.back().get();
            auto               iFindLast = std::find( strLast.rbegin(), strLast.rend(), '.' );
            if( iFindLast != strLast.rend() )
            {
                pID = database.construct< Identifier >(
                    Identifier::Args{ std::string( iFindLast.base(), strLast.end() ) } );
            }
            else
            {
                pID = database.construct< Identifier >( Identifier::Args{ typeList.back().get() } );
            }

            pTypeList = database.construct< TypeList >( TypeList::Args( typeList ) );
        }

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

        return database.construct< Link >( Link::Args{ pID, pTypeList, bOwning, cardinality_range } );
    }

    boost::filesystem::path resolveFilePath( const std::string& strFile )
    {
        if( clang::Optional< clang::FileEntryRef > includeFile = PP.LookupFile( clang::SourceLocation(),
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

    Include* parse_include( Database& database )
    {
        Include* pResult = nullptr;

        // include name( file );
        // optional identifier
        ScopedIdentifier* pIdentifier = nullptr;
        if( Tok.is( clang::tok::identifier ) )
        {
            pIdentifier = parse_scopedIdentifier( database );
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
            const bool bIsAngled = strFile[ 0 ] == '<';
            if( bIsAngled )
            {
                VERIFY_RTE( strFile.back() == '>' );
                strFile = std::string( strFile.begin() + 1, strFile.end() - 1 );

                if( pIdentifier )
                {
                    MEGA_PARSER_ERROR( "System include has identifier" );
                }
                pResult = database.construct< SystemInclude >( SystemInclude::Args{ Include::Args{}, strFile } );
            }
            else
            {
                const boost::filesystem::path filePath = resolveFilePath( strFile );
                if( filePath.extension() == mega::io::megaFilePath::extension() )
                {
                    if( pIdentifier )
                        pResult = database.construct< MegaIncludeNested >(
                            MegaIncludeNested::Args{ MegaInclude::Args{ Include::Args{}, filePath }, pIdentifier } );
                    else
                        pResult = database.construct< MegaIncludeInline >(
                            MegaIncludeInline::Args{ MegaInclude::Args{ Include::Args{}, filePath } } );
                }
                else
                {
                    if( pIdentifier )
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

        while( !Tok.is( clang::tok::r_paren ) )
        {
            endLoc = Tok.getEndLoc();
            ConsumeAnyToken();
        }

        std::string strDependency;
        VERIFY_RTE( getSourceText( startLoc, endLoc, strDependency ) );
        strDependency.erase( std::remove( strDependency.begin(), strDependency.end(), '\"' ), strDependency.end() );
        strDependency.erase( std::remove( strDependency.begin(), strDependency.end(), ' ' ), strDependency.end() );

        T.consumeClose();

        parse_semicolon();

        return database.construct< Dependency >( Dependency::Args{ strDependency } );
    }

    Part* parse_part( Database& database )
    {
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
                    clang::SourceLocation startLoc = Tok.getLocation();
                    clang::SourceLocation endLoc   = Tok.getEndLoc();
                    ConsumeAnyToken();

                    while( !isEofOrEom() && !Tok.is( clang::tok::r_paren ) )
                    {
                        endLoc = Tok.getEndLoc();
                        ConsumeAnyToken();
                    }
                    if( !getSourceText( startLoc, endLoc, strArguments ) )
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
        return database.construct< Part >( Part::Args{ names } );
    }

    ContextDef::Args defaultBody( ScopedIdentifier* pScopedIdentifier ) const
    {
        ContextDef::Args body( pScopedIdentifier,
                               std::vector< ContextDef* >{},
                               std::vector< Dimension* >{},
                               std::vector< Link* >{},
                               std::vector< Include* >{},
                               std::vector< Dependency* >{},
                               std::vector< Part* >{},
                               std::nullopt );
        return body;
    }

    NamespaceDef* parse_namespace( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
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
        Size* pSize = parse_size( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
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

        return database.construct< AbstractDef >( AbstractDef::Args{ body, pSize, pInheritance } );
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
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
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

        return database.construct< EventDef >( EventDef::Args{ body, pSize, pInheritance } );
    }

    InteruptDef* parse_interupt( Database& database, bool bIsRequirement )
    {
        ScopedIdentifier* pScopedIdentifier = nullptr;
        if( Tok.is( clang::tok::identifier ) )
        {
            pScopedIdentifier = parse_scopedIdentifier( database );
        }
        else
        {
            pScopedIdentifier = generate_unamedScopeIdentifier( database );
        }

        parse_comment();
        ArgumentList* pArgumentList = parse_argumentList( database );
        parse_comment();
        Transition* pTransition = parse_transition( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
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
        if( bIsRequirement )
        {
            return database.construct< RequirementDef >(
                RequirementDef::Args{ InteruptDef::Args{ body, pArgumentList, pTransition } } );
        }
        else
        {
            return database.construct< InteruptDef >( InteruptDef::Args{ body, pArgumentList, pTransition } );
        }
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
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
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

        return database.construct< FunctionDef >( FunctionDef::Args{ body, pArgumentList, pReturnType } );
    }

    ObjectDef* parse_object( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Size* pSize = parse_size( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
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

        return database.construct< ObjectDef >( ObjectDef::Args{ body, pSize, pInheritance } );
    }

    StateDef* parse_state( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Size* pSize = parse_size( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();
        Transition* pTransition = parse_transition( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
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

        return database.construct< StateDef >( StateDef::Args{ body, pSize, pInheritance, pTransition } );
    }

    ActionDef* parse_action( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Size* pSize = parse_size( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();
        Transition* pTransition = parse_transition( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
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

        return database.construct< ActionDef >(
            ActionDef::Args{ StateDef::Args{ body, pSize, pInheritance, pTransition } } );
    }

    ComponentDef* parse_component( Database& database )
    {
        ScopedIdentifier* pScopedIdentifier = parse_scopedIdentifier( database );
        parse_comment();
        Size* pSize = parse_size( database );
        parse_comment();
        Inheritance* pInheritance = parse_inheritance( database );
        parse_comment();
        Transition* pTransition = parse_transition( database );
        parse_comment();

        ContextDef::Args body = defaultBody( pScopedIdentifier );
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                body = parse_context_body( database, pScopedIdentifier );
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

        return database.construct< ComponentDef >(
            ComponentDef::Args{ StateDef::Args{ body, pSize, pInheritance, pTransition } } );
    }

    ContextDef::Args parse_context_body( Database& database, ScopedIdentifier* pScopedIdentifier )
    {
        ContextDef::Args bodyArgs = defaultBody( pScopedIdentifier );

        braceStack.push_back( BraceCount );

        bool bIsBodyDefinition = false;

        while( !isEofOrEom() )
        {
            if( Tok.is( clang::tok::kw_namespace ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_namespace( database ) );
            }
            else if( Tok.is( clang::tok::kw_interface ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_abstract( database ) );
            }
            else if( Tok.is( clang::tok::kw_event ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_event( database ) );
            }
            else if( Tok.is( clang::tok::kw_interupt ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_interupt( database, false ) );
            }
            else if( Tok.is( clang::tok::kw_requirement ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_interupt( database, true ) );
            }
            else if( Tok.is( clang::tok::kw_function ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_function( database ) );
            }
            else if( Tok.is( clang::tok::kw_action ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_action( database ) );
            }
            else if( Tok.is( clang::tok::kw_state ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_state( database ) );
            }
            else if( Tok.is( clang::tok::kw_component ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_component( database ) );
            }
            else if( Tok.is( clang::tok::kw_object ) )
            {
                ConsumeToken();
                bodyArgs.children.value().push_back( parse_object( database ) );
            }
            else if( ( Tok.is( clang::tok::kw_const ) && NextToken().is( clang::tok::kw_dim ) )
                     || Tok.is( clang::tok::kw_dim ) )
            {
                bool bIsConst = false;
                if( Tok.is( clang::tok::kw_const ) )
                {
                    bIsConst = true;
                    ConsumeToken();
                }
                ConsumeToken();
                Dimension* pDimension = parse_dimension( database, bIsConst );
                bodyArgs.dimensions.value().push_back( pDimension );
            }
            else if( Tok.is( clang::tok::kw_link ) )
            {
                ConsumeToken();
                Link* pLink = parse_link( database, false );
                bodyArgs.links.value().push_back( pLink );
            }
            else if( Tok.is( clang::tok::kw_owns ) )
            {
                ConsumeToken();
                Link* pLink = parse_link( database, true );
                bodyArgs.links.value().push_back( pLink );
            }
            else if( Tok.is( clang::tok::kw_include ) )
            {
                ConsumeToken();
                Include* pInclude = parse_include( database );
                bodyArgs.includes.value().push_back( pInclude );
            }
            else if( Tok.is( clang::tok::kw_dependency ) )
            {
                ConsumeToken();
                Dependency* pDependency = parse_dependency( database );
                bodyArgs.dependencies.value().push_back( pDependency );
            }
            else if( Tok.is( clang::tok::kw_part ) )
            {
                ConsumeToken();
                Part* pPart = parse_part( database );
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
                            clang::tok::kw_interface,
                            clang::tok::kw_interupt,
                            clang::tok::kw_requirement,
                            clang::tok::kw_dim,
                            clang::tok::kw_link,
                            clang::tok::kw_owns,
                            clang::tok::kw_include,
                            clang::tok::kw_dependency,
                            clang::tok::kw_part,
                            clang::tok::kw_state,
                            clang::tok::kw_component
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
                        bodyArgs.body = database.construct< Body >( Body::Args{
                            strBodyPart, sm.getFilename( startLoc ).str(), sm.getSpellingLineNumber( startLoc ) } );
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
        ScopedIdentifier* pID         = database.construct< ScopedIdentifier >(
            ScopedIdentifier::Args{ std::vector< Identifier* >{}, strLocation, 0U } );
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

        MegaParser parser( stuff, pDiagnosticsEngine );
        parser.ConsumeAnyToken( true );

        return parser.parse_file( database );
    }
};

extern "C" BOOST_SYMBOL_EXPORT EG_PARSER_IMPL g_parserSymbol;
MEGA_PARSER_EXPORT EG_PARSER_IMPL             g_parserSymbol;

/*
std::unique_ptr< EG_PARSER_INTERFACE > getParser()
{
    return std::make_unique< EG_PARSER_IMPL >();
}*/
