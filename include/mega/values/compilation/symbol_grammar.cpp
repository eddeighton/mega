
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

#include "symbol_grammar.hpp"

#include "common/string.hpp"
#include "common/assert_verify.hpp"

#include <boost/algorithm/string/detail/formatter.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/object.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

using MegaSymbol                         = mega::Symbol;
using MegaSymbolVariant                  = mega::SymbolVariant;
using MegaSymbolVariantPath              = mega::SymbolVariantPath;
using MegaSymbolVariantPathSequence      = mega::SymbolVariantPathSequence;
using MegaNamedSymbolVariantPath         = mega::NamedSymbolVariantPath;
using MegaNamedSymbolVariantPathSequence = mega::NamedSymbolVariantPathSequence;

BOOST_FUSION_ADAPT_STRUCT( MegaSymbol, ( std::string, m_identifier ) )

BOOST_FUSION_ADAPT_STRUCT( MegaSymbolVariant, ( std::vector< MegaSymbol >, m_symbols ) )

BOOST_FUSION_ADAPT_STRUCT( MegaSymbolVariantPath, ( std::vector< MegaSymbolVariant >, m_symbolVariants ) )

BOOST_FUSION_ADAPT_STRUCT( MegaSymbolVariantPathSequence,
                           ( std::vector< MegaSymbolVariantPath >, m_symbolVariantPaths ) )

BOOST_FUSION_ADAPT_STRUCT( MegaNamedSymbolVariantPath,
                           ( MegaSymbolVariantPath, m_symbolVariantPath ),
                           ( std::optional< MegaSymbol >, m_name ) )

BOOST_FUSION_ADAPT_STRUCT( MegaNamedSymbolVariantPathSequence,
                           ( std::vector< MegaNamedSymbolVariantPath >, m_symbolVariantPaths ) )

namespace mega
{

namespace
{

template < typename IteratorType >
struct ParseResult
{
    bool         bSuccess;
    IteratorType iterReached;
};

template < typename Iterator >
struct error_handler
{
    template < typename, typename, typename, typename >
    struct result
    {
        typedef void type;
    };

    std::ostream& errorStream;
    error_handler( std::ostream& _errorStream )
        : errorStream( _errorStream )
    {
    }

    void operator()( Iterator, Iterator, Iterator err_pos, boost::spirit::info const& what ) const
    {
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
        start = boost::spirit::ascii::space | "//" >> *( char_ - '\n' ) >> '\n' // C++ comment
                | "/*" >> *( char_ - "*/" ) >> "*/";                            // C comment
    }

private:
    boost::spirit::qi::rule< Iterator > start;
};

template < typename Iterator >
class SymbolGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Symbol() >
{
public:
    using ResultType = Symbol;

    SymbolGrammar()
        : SymbolGrammar::base_type( m_main_rule, "Symbol" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;
        m_main_rule = lexeme[ +char_( "a-zA-Z0-9_" )[ push_back( at_c< 0 >( _val ), qi::_1 ) ] ];
    }

    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Symbol() > m_main_rule;
};

template < typename Iterator >
class SymbolVariantGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, SymbolVariant() >
{
public:
    using ResultType = SymbolVariant;

    SymbolVariantGrammar()
        : SymbolVariantGrammar::base_type( m_main_rule, "SymbolVariant" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        m_main_rule
            = ( m_symbol_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ] )
              | ( lit( '<' ) >> m_symbol_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ]
                  >> +( lit( '.' ) > m_symbol_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ] ) >> lit( '>' ) );
    }

    SymbolGrammar< Iterator >                                                     m_symbol_grammar;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, SymbolVariant() > m_main_rule;
};

template < typename Iterator >
class SymbolVariantPathGrammar
    : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, SymbolVariantPath() >
{
public:
    using ResultType = SymbolVariantPath;

    SymbolVariantPathGrammar()
        : SymbolVariantPathGrammar::base_type( m_main_rule, "SymbolVariantPath" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        m_main_rule = m_symbol_variant_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ]
                      >> *( lit( '.' ) > m_symbol_variant_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ] );
    }

    SymbolVariantGrammar< Iterator >                                                  m_symbol_variant_grammar;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, SymbolVariantPath() > m_main_rule;
};

template < typename Iterator >
class SymbolVariantPathSequenceGrammar
    : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, SymbolVariantPathSequence() >
{
public:
    using ResultType = SymbolVariantPathSequence;

    SymbolVariantPathSequenceGrammar()
        : SymbolVariantPathSequenceGrammar::base_type( m_main_rule, "SymbolVariantPathSequence" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        m_main_rule = m_symbol_variant_path_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ]
                      >> *( lit( ',' ) > m_symbol_variant_path_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ] );
    }

    SymbolVariantPathGrammar< Iterator > m_symbol_variant_path_grammar;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, SymbolVariantPathSequence() > m_main_rule;
};

template < typename Iterator >
class NamedSymbolVariantPathGrammar
    : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, NamedSymbolVariantPath() >
{
public:
    using ResultType = NamedSymbolVariantPath;

    NamedSymbolVariantPathGrammar()
        : NamedSymbolVariantPathGrammar::base_type( m_main_rule, "NamedSymbolVariantPath" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        m_main_rule = m_symbol_variant_path_grammar[ at_c< 0 >( _val ) = qi::_1 ]
                      >> -m_symbol_grammar[ at_c< 1 >( _val ) = qi::_1 ];
    }

    SymbolVariantPathGrammar< Iterator > m_symbol_variant_path_grammar;
    SymbolGrammar< Iterator >            m_symbol_grammar;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, NamedSymbolVariantPath() > m_main_rule;
};

template < typename Iterator >
class NamedSymbolVariantPathSequenceGrammar
    : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, NamedSymbolVariantPathSequence() >
{
public:
    using ResultType = NamedSymbolVariantPathSequence;

    NamedSymbolVariantPathSequenceGrammar()
        : NamedSymbolVariantPathSequenceGrammar::base_type( m_main_rule, "NamedSymbolVariantPathSequence" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        m_main_rule = m_named_symbol_variant_path_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ] >> *(
                          lit( ',' ) > m_named_symbol_variant_path_grammar[ push_back( at_c< 0 >( _val ), qi::_1 ) ] );
    }

    NamedSymbolVariantPathGrammar< Iterator > m_named_symbol_variant_path_grammar;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, NamedSymbolVariantPathSequence() > m_main_rule;
};

template < typename Iterator >
class NamedSymbolVariantPathSequenceOptGrammar
    : public boost::spirit::qi::
          grammar< Iterator, SkipGrammar< Iterator >, std::optional< NamedSymbolVariantPathSequence >() >
{
public:
    using ResultType = std::optional< NamedSymbolVariantPathSequence >;

    NamedSymbolVariantPathSequenceOptGrammar()
        : NamedSymbolVariantPathSequenceOptGrammar::base_type( m_main_rule, "OptionalNamedSymbolVariantPathSequence" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        m_main_rule = -m_named_symbol_variant_path_sequence_grammar[ _val = qi::_1 ];
    }

    NamedSymbolVariantPathSequenceGrammar< Iterator > m_named_symbol_variant_path_sequence_grammar;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, std::optional< NamedSymbolVariantPathSequence >() >
        m_main_rule;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// parser helper function

template < typename IteratorType, template < typename IterType > class GrammarType, typename ResultType >
inline ParseResult< IteratorType >
parse_impl( IteratorType iBegin, IteratorType iEnd, ResultType& result, std::ostream& errorStream )
{
    GrammarType< IteratorType > grammar;
    {
        using namespace boost::phoenix;
        function< error_handler< IteratorType > > const error_handler( errorStream );
        boost::spirit::qi::on_error< boost::spirit::qi::fail >(
            grammar.m_main_rule,
            error_handler(
                boost::spirit::qi::_1, boost::spirit::qi::_2, boost::spirit::qi::_3, boost::spirit::qi::_4 ) );
    }

    ParseResult< IteratorType > resultPair{ false, iBegin };

    resultPair.bSuccess = boost::spirit::qi::phrase_parse(
        resultPair.iterReached, iEnd, grammar, SkipGrammar< IteratorType >(), result );

    return resultPair;
}

template < template < typename IterType > class GrammarType >
struct ParserString
{
    using IteratorType = std::string::const_iterator;
    using ResultType   = typename GrammarType< IteratorType >::ResultType;

    static void parse( const std::string& str, ResultType& value )
    {
        std::ostringstream osError;
        auto               result = parse_impl< IteratorType, GrammarType >( str.begin(), str.end(), value, osError );
        if( !result.bSuccess )
        {
            THROW( SymbolParseException, "Parsing of: " << str << " failed: " << osError.str() );
        }
        else if( result.iterReached != str.end() )
        {
            THROW( SymbolParseException, "Parsing of: " << str << " incomplete: " << osError.str() );
        }
    }
};

} // namespace

Symbol Symbol::parse( const std::string& str )
{
    Symbol value;
    ParserString< SymbolGrammar >::parse( str, value );
    return value;
}

std::string Symbol::str() const
{
    return m_identifier;
}

SymbolVariant SymbolVariant::parse( const std::string& str )
{
    SymbolVariant value;
    ParserString< SymbolVariantGrammar >::parse( str, value );
    return value;
}

std::string SymbolVariant::str() const
{
    VERIFY_RTE( !m_symbols.empty() );
    std::ostringstream os;
    if( m_symbols.size() == 1 )
    {
        os << m_symbols.front();
    }
    else
    {
        os << "<";
        common::delimit( m_symbols.begin(), m_symbols.end(), ".", os );
        os << ">";
    }
    return os.str();
}   

SymbolVariantPath SymbolVariantPath::parse( const std::string& str )
{
    SymbolVariantPath value;
    ParserString< SymbolVariantPathGrammar >::parse( str, value );
    return value;
}

std::string SymbolVariantPath::str() const
{
    VERIFY_RTE( !m_symbolVariants.empty() );
    std::ostringstream os;
    common::delimit( m_symbolVariants.begin(), m_symbolVariants.end(), ".", os );
    return os.str();
}

SymbolVariantPathSequence SymbolVariantPathSequence::parse( const std::string& str )
{
    SymbolVariantPathSequence value;
    ParserString< SymbolVariantPathSequenceGrammar >::parse( str, value );
    return value;
}

std::string SymbolVariantPathSequence::str() const
{
    VERIFY_RTE( !m_symbolVariantPaths.empty() );
    std::ostringstream os;
    common::delimit( m_symbolVariantPaths.begin(), m_symbolVariantPaths.end(), ",", os );
    return os.str();
}

NamedSymbolVariantPath NamedSymbolVariantPath::parse( const std::string& str )
{
    NamedSymbolVariantPath value;
    ParserString< NamedSymbolVariantPathGrammar >::parse( str, value );
    return value;
}

std::string NamedSymbolVariantPath::str() const
{
    std::ostringstream os;
    os << m_symbolVariantPath.str();
    if( m_name.has_value() )
    {
        os << ' ' << m_name.value().str();
    }
    return os.str();
}

NamedSymbolVariantPathSequence NamedSymbolVariantPathSequence::parse( const std::string& str )
{
    NamedSymbolVariantPathSequence value;
    ParserString< NamedSymbolVariantPathSequenceGrammar >::parse( str, value );
    return value;
}

std::optional< NamedSymbolVariantPathSequence > NamedSymbolVariantPathSequence::parseOptional( const std::string& str )
{
    std::optional< NamedSymbolVariantPathSequence > value;
    ParserString< NamedSymbolVariantPathSequenceOptGrammar >::parse( str, value );
    return value;
}

std::string NamedSymbolVariantPathSequence::str() const
{
    VERIFY_RTE( !m_symbolVariantPaths.empty() );
    std::ostringstream os;
    common::delimit( m_symbolVariantPaths.begin(), m_symbolVariantPaths.end(), ",", os );
    return os.str();
}

std::ostream& operator<<( std::ostream& os, const mega::Symbol& value )
{
    return os << value.str();
}
std::ostream& operator<<( std::ostream& os, const mega::SymbolVariant& value )
{
    return os << value.str();
}
std::ostream& operator<<( std::ostream& os, const mega::SymbolVariantPath& value )
{
    return os << value.str();
}
std::ostream& operator<<( std::ostream& os, const mega::SymbolVariantPathSequence& value )
{
    return os << value.str();
}
std::ostream& operator<<( std::ostream& os, const mega::NamedSymbolVariantPath& value )
{
    return os << value.str();
}
std::ostream& operator<<( std::ostream& os, const mega::NamedSymbolVariantPathSequence& value )
{
    return os << value.str();
}

} // namespace mega
