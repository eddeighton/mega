
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

#include "mega/tag_parser.hpp"

#include "common/string.hpp"

#include <boost/algorithm/string/detail/formatter.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
// #include <boost/spirit/include/support_istream_iterator.hpp>

#include <boost/spirit/include/support_multi_pass.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/object.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/variant/detail/apply_visitor_unary.hpp>

#include <string>
#include <iostream>
#include <utility>

// clang-format off
BOOST_FUSION_ADAPT_STRUCT(  mega::XMLTag,
                           ( std::string,                                   key )
                           ( std::optional< mega::AddressTable::Index >,    indexOpt )
                           ( std::vector< mega::XMLTag >,                   children ) 
                        )

// clang-format on

namespace mega
{
namespace
{

template < typename Iterator >
struct ParseResult
{
    bool     bSuccess;
    Iterator iterReached;
};

template < typename Iterator >
struct error_handler
{
    template < typename, typename, typename, typename >
    struct result
    {
        using type = void;
    };

    std::ostream& errorStream;
    error_handler( std::ostream& _errorStream )
        : errorStream( _errorStream )
    {
    }

    void operator()( Iterator /*first*/, Iterator /*last*/, Iterator err_pos, boost::spirit::info const& what ) const
    {
        // Iterator eol = err_pos;
        // int line = calculateLineNumber( first, err_pos );
        int line = boost::spirit::get_line( err_pos );
        if( line != -1 )
            errorStream << '(' << line << ')';
        else
            errorStream << "( unknown )";
        errorStream << " : Error! Expected " << what << std::endl;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Skip grammar for handling c++ style comments and white space
template < typename Iterator >
class SkipGrammar : public boost::spirit::qi::grammar< Iterator >
{
public:
    SkipGrammar()
        : SkipGrammar::base_type( start )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;
        start = boost::spirit::ascii::space; // C comment
    }

private:
    boost::spirit::qi::rule< Iterator > start;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// parser helper function

template < class GrammarType, typename ResultType, typename Iterator >
inline ParseResult< Iterator >
parse_impl( std::pair< Iterator, Iterator >& iterPair, ResultType& result, std::ostream& errorStream )
{
    GrammarType grammar;
    {
        using namespace boost::phoenix;
        function< error_handler< Iterator > > const error_handler( errorStream );
        boost::spirit::qi::on_error< boost::spirit::qi::fail >(
            grammar.m_main_rule,
            error_handler(
                boost::spirit::qi::_1, boost::spirit::qi::_2, boost::spirit::qi::_3, boost::spirit::qi::_4 ) );
    }

    ParseResult< Iterator > resultPair{ false, iterPair.first };

    resultPair.bSuccess = boost::spirit::qi::phrase_parse(
        resultPair.iterReached, iterPair.second, grammar, SkipGrammar< Iterator >(), result );

    return resultPair;
}

class Identifier : public std::string
{
public:
    class Compare
    {
    public:
        inline bool operator()( const Identifier& left, const Identifier& right ) const
        {
            return std::lexicographical_compare( left.begin(), left.end(), right.begin(), right.end() );
        }
    };

    Identifier() {}
    Identifier( const std::string& str )
        : std::string( str )
    {
    }
    Identifier( const char* pszStr )
        : std::string( pszStr )
    {
    }
};

template < typename Iterator >
class IdentifierGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Identifier() >
{
public:
    IdentifierGrammar()
        : IdentifierGrammar::base_type( m_main_rule, "identifier" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;
        m_main_rule = lexeme[ char_( "a-zA-Z0-9" )[ push_back( _val, qi::_1 ) ]
                              >> *( char_( "a-zA-Z0-9_*" )[ push_back( _val, qi::_1 ) ] ) ];
    }

    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Identifier() > m_main_rule;
};

template < typename Iterator >
class IndexAttributeGrammar
    : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, mega::AddressTable::Index() >
{
public:
    IndexAttributeGrammar()
        : IndexAttributeGrammar::base_type( m_main_rule, "index_attribute" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;
        m_main_rule = lit( "index" ) >> lit( "=" ) >> lexeme[ lit( '"' ) >> int_[ _val = qi::_1 ] >> lit( '"' ) ];
    }
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, mega::AddressTable::Index() > m_main_rule;
};

template < typename Iterator >
class StartTagGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, XMLTag() >
{
public:
    StartTagGrammar()
        : StartTagGrammar::base_type( m_main_rule, "start_tag" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        // clang-format off
        m_main_rule = 
        
            *( char_ - "<" ) >>

            lit( "<" ) >> 
            (
                m_identifier_grammar[   at_c< 0 >( _val ) = qi::_1 ] >>
                -( m_index_rule )[      at_c< 1 >( _val ) = qi::_1 ]
            ) >> 
            lit( ">" )
            ;
        // clang-format on
    }

    IdentifierGrammar< Iterator >                                          m_identifier_grammar;
    IndexAttributeGrammar< Iterator >                                      m_index_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, XMLTag() > m_main_rule;
};

template < typename Iterator >
ParseResult< Iterator >
parseStartTag( std::pair< Iterator, Iterator >& iterPair, XMLTag& tag, std::ostream& errorStream )
{
    return parse_impl< StartTagGrammar< Iterator > >( iterPair, tag, errorStream );
}

template < typename Iterator >
class EndTagGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, XMLTag() >
{
public:
    EndTagGrammar()
        : EndTagGrammar::base_type( m_main_rule, "end_tag" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        // clang-format off
        m_main_rule = 
        
            *( char_ - "<" ) >>

            lit( "</" ) >> 
            (
                m_identifier_grammar[ at_c< 0 >( _val ) = qi::_1 ]
            ) >> 
            lit( ">" )
            ;
        // clang-format on
    }

    IdentifierGrammar< Iterator >                                          m_identifier_grammar;
    IndexAttributeGrammar< Iterator >                                      m_index_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, XMLTag() > m_main_rule;
};

template < typename Iterator >
ParseResult< Iterator > parseEndTag( std::pair< Iterator, Iterator >& iterPair, XMLTag& tag, std::ostream& errorStream )
{
    return parse_impl< EndTagGrammar< Iterator > >( iterPair, tag, errorStream );
}

template < typename Iterator >
class TagGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, XMLTag() >
{
public:
    TagGrammar()
        : TagGrammar::base_type( m_main_rule, "tag" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        // clang-format off

        //m_index_rule = lit( "index" ) >> lit( "=" ) >> lexeme[ lit( '"' ) >> int_[ _val = qi::_1 ] >> lit( '"' ) ];

        m_main_rule = 
            lit( "<" ) >> 
            (
                m_identifier_grammar[   at_c< 0 >( _val ) = qi::_1 ] >>
                -( m_index_rule )[      at_c< 1 >( _val ) = qi::_1 ]
            ) >> 
            lit( ">" ) >>

            *( char_ - "<" ) >>

            *m_main_rule[ push_back( at_c< 2 >( _val ), qi::_1 ) ] >>

            *( char_ - "<" ) >>

            lit( "</" ) >> 
            (
                m_identifier_grammar //[ at_c< 0 >( _val ) == qi::_1 ]
            ) >> 
            lit( ">" )

            ;
        // clang-format on
    }

    IndexAttributeGrammar< Iterator >                                      m_index_rule;
    IdentifierGrammar< Iterator >                                          m_identifier_grammar;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, XMLTag() > m_main_rule;
};

template < typename Iterator >
ParseResult< Iterator > parse( std::pair< Iterator, Iterator >& iterPair, XMLTag& tag, std::ostream& errorStream )
{
    return parse_impl< TagGrammar< Iterator > >( iterPair, tag, errorStream );
}

} // anonymous namespace

XMLTag parse( std::istream& is )
{
    using BaseIterType       = std::istreambuf_iterator< char >;
    using UnderlyingIterType = boost::spirit::multi_pass< BaseIterType >;
    using IteratorType       = boost::spirit::line_pos_iterator< UnderlyingIterType >;
    using IteratorTypePair   = std::pair< IteratorType, IteratorType >;

    IteratorTypePair iterPair( boost::spirit::make_default_multi_pass( BaseIterType( is ) ), IteratorType{} );

    std::ostringstream osError;
    XMLTag             tag;
    const ParseResult  result = parse( iterPair, tag, osError );
    VERIFY_RTE_MSG( result.bSuccess, "Failed to parse tags with error: " << osError.str() );
    return tag;
}

void consumeStart( std::istream& is, const std::string& strTag )
{
    using BaseIterType       = std::istreambuf_iterator< char >;
    using UnderlyingIterType = boost::spirit::multi_pass< BaseIterType >;
    using IteratorType       = boost::spirit::line_pos_iterator< UnderlyingIterType >;
    using IteratorTypePair   = std::pair< IteratorType, IteratorType >;

    IteratorTypePair iterPair( boost::spirit::make_default_multi_pass( BaseIterType( is ) ), IteratorType{} );

    std::ostringstream osError;
    XMLTag             tag;
    const ParseResult  result = parseStartTag( iterPair, tag, osError );
    VERIFY_RTE_MSG( result.bSuccess && tag.key == strTag,
                    "Failed to consume start tag: " << strTag << " got: " << tag.key << osError.str() );
}

void consumeEnd( std::istream& is, const std::string& strTag )
{
    using BaseIterType       = std::istreambuf_iterator< char >;
    using UnderlyingIterType = boost::spirit::multi_pass< BaseIterType >;
    using IteratorType       = boost::spirit::line_pos_iterator< UnderlyingIterType >;
    using IteratorTypePair   = std::pair< IteratorType, IteratorType >;

    IteratorTypePair iterPair( boost::spirit::make_default_multi_pass( BaseIterType( is ) ), IteratorType{} );

    std::ostringstream osError;
    XMLTag             tag;
    const ParseResult  result = parseEndTag( iterPair, tag, osError );
    if( !result.bSuccess )
    {
        const ParseResult startResult = parseStartTag( iterPair, tag, osError );
        if( startResult.bSuccess )
        {
            THROW_RTE( "Got start tag: " << tag.key << " instead of end tag: " << strTag );
        }
        else
        {
            THROW_RTE( "Fail to parse any tag for tag: " << strTag );
        }
    }
    else if( tag.key != strTag )
    {
        THROW_RTE( "Failed to consume matching end tag: " << strTag << " got: " << tag.key );
    }
}

} // namespace mega
