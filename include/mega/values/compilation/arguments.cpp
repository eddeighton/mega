
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

#include "mega/values/compilation/arguments.hpp"

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

namespace mega
{

using Identifier = std::string;

namespace
{
using UnderlyingIterType = std::string::const_iterator;
using IteratorType       = UnderlyingIterType;

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

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// parser helper function

template < template < typename IterType > class GrammarType, typename ResultType >
inline ParseResult parse_impl( const std::string& strInput, ResultType& result, std::ostream& errorStream )
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

    ParseResult resultPair{ false, IteratorType( strInput.begin() ) };

    resultPair.bSuccess = boost::spirit::qi::phrase_parse(
        resultPair.iterReached, IteratorType( strInput.end() ), grammar, SkipGrammar< IteratorType >(), result );

    return resultPair;
}

template < typename Iterator >
class IdentifierGrammar : public boost::spirit::qi::grammar< Iterator, Identifier() >
{
public:
    IdentifierGrammar()
        : IdentifierGrammar::base_type( m_main_rule, "identifier" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;
        m_main_rule = lexeme[ char_( "a-zA-Z0-9_:." )[ push_back( _val, qi::_1 ) ]
                              >> *( char_( "a-zA-Z0-9_:." )[ push_back( _val, qi::_1 ) ] ) ];
    }

    boost::spirit::qi::rule< Iterator, Identifier() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Identifier& code, std::ostream& errorStream )
{
    return parse_impl< IdentifierGrammar >( strInput, code, errorStream );
}

} // namespace

struct _Type
{
    Identifier           id;
    std::vector< _Type > args;
};

std::ostream& operator<<( std::ostream& os, const _Type& type )
{
    os << type.id;
    if( !type.args.empty() )
    {
        os << "< ";
        bool bFirst = true;
        for( const auto& a : type.args )
        {
            if( bFirst )
                bFirst = false;
            else
                os << ", ";
            os << a;
        }
        os << " >";
    }
    return os;
}
} // namespace mega

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( mega::_Type,
    ( mega::Identifier, id )
    ( std::vector< mega::_Type >, args ) )
// clang-format on

namespace mega
{
namespace
{

template < typename Iterator >
class TypeGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, _Type() >
{
public:
    TypeGrammar()
        : TypeGrammar::base_type( m_main_rule, "Type" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        m_type_list_rule = lit( '<' ) > ( m_main_rule )[ push_back( _val, qi::_1 ) ]
                           >> *( lit( ',' ) >> ( m_main_rule )[ push_back( _val, qi::_1 ) ] ) > lit( '>' );

        m_main_rule = m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ] >> -m_type_list_rule[ at_c< 1 >( _val ) = qi::_1 ];
    }

    IdentifierGrammar< Iterator >                                                        m_grammar_id;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, std::vector< _Type >() > m_type_list_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, _Type() >                m_main_rule;
};
} // namespace

struct _TypeName
{
    _Type                       type;
    std::optional< Identifier > name;

    using Vector = std::vector< _TypeName >;
};
} // namespace mega

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( mega::_TypeName,
    ( mega::_Type, type )
    ( std::optional< mega::Identifier >, name ) )

// clang-format on

namespace mega
{
namespace
{

template < typename Iterator >
class TypeNameListGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, _TypeName::Vector() >
{
public:
    TypeNameListGrammar()
        : TypeNameListGrammar::base_type( m_main_rule, "TypeNameList" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        m_arg_rule = m_grammar_type[ at_c< 0 >( _val ) = qi::_1 ] >> -m_grammar_id[ at_c< 1 >( _val ) = qi::_1 ];
        m_main_rule
            = -m_arg_rule[ push_back( _val, qi::_1 ) ] >> *( lit( ',' ) >> m_arg_rule[ push_back( _val, qi::_1 ) ] );
    }

    IdentifierGrammar< Iterator > m_grammar_id;
    TypeGrammar< Iterator >       m_grammar_type;

    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, _TypeName() >         m_arg_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, _TypeName::Vector() > m_main_rule;
};
} // namespace

void parse( const std::string& str, TypeName::Vector& args )
{
    _TypeName::Vector ast;
    {
        std::ostringstream osError;
        const ParseResult  result = parse_impl< TypeNameListGrammar >( str, ast, osError );
        VERIFY_RTE_MSG( result.bSuccess && ( result.iterReached == str.end() ), "Failed to parse arguments: " << str );
    }
    for( const auto& a : ast )
    {
        std::ostringstream os;
        os << a.type;
        args.push_back( TypeName( os.str(), a.name ) );
    }
}

void parse( const std::string& str, Type::Vector& args )
{
    _TypeName::Vector ast;
    {
        std::ostringstream osError;
        const ParseResult  result = parse_impl< TypeNameListGrammar >( str, ast, osError );
        VERIFY_RTE_MSG( result.bSuccess && ( result.iterReached == str.end() ), "Failed to parse arguments: " << str );
    }
    for( const auto& a : ast )
    {
        VERIFY_RTE_MSG( !a.name.has_value(), "Type has a name" );
        std::ostringstream os;
        os << a.type;
        args.push_back( Type( os.str() ) );
    }
}

std::ostream& operator<<( std::ostream& os, const mega::Type& arg )
{
    return os << arg.get();
}

std::ostream& operator<<( std::ostream& os, const mega::TypeName& arg )
{
    os << arg.getType();
    if( arg.getName().has_value() )
    {
        os << " " << arg.getName().value();
    }
    return os;
}

std::ostream& operator<<( std::ostream& os, const mega::TypeName::Vector& arguments )
{
    bool bFirst = true;
    for( const auto& arg : arguments )
    {
        if( bFirst )
            bFirst = false;
        else
            os << ", ";
        os << arg;
    }
    return os;
}

std::ostream& operator<<( std::ostream& os, const mega::Type::Vector& arguments )
{
    bool bFirst = true;
    for( const auto& arg : arguments )
    {
        if( bFirst )
            bFirst = false;
        else
            os << ", ";
        os << arg;
    }
    return os;
}

std::string TypeName::str() const
{
    using ::           operator<<;
    std::ostringstream os;
    os << *this;
    return os.str();
}

} // namespace mega