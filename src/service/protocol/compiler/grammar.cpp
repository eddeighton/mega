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

#include "grammar.hpp"

#include "common/string.hpp"

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

#include <boost/variant/detail/apply_visitor_unary.hpp>

#include <string>
#include <iostream>

namespace protocol::schema
{

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
        start = boost::spirit::ascii::space | "//" >> *( char_ - '\n' ) >> '\n' // C++ comment
                | "/*" >> *( char_ - "*/" ) >> "*/";                            // C comment
    }

private:
    boost::spirit::qi::rule< Iterator > start;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// parser helper function

template < class GrammarType, typename ResultType >
inline ParseResult parse_impl( const std::string& strInput, ResultType& result, std::ostream& errorStream )
{
    GrammarType grammar;
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
        m_main_rule = lexeme[ char_( "a-zA-Z0-9" )[ push_back( _val, qi::_1 ) ]
                              >> *( char_( "a-zA-Z0-9_*" )[ push_back( _val, qi::_1 ) ] ) ];
    }

    boost::spirit::qi::rule< Iterator, Identifier() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Identifier& code, std::ostream& errorStream )
{
    return parse_impl< IdentifierGrammar< IteratorType > >( strInput, code, errorStream );
}

template < typename Iterator >
class IdentifierListGrammar : public boost::spirit::qi::grammar< Iterator, IdentifierList() >
{
public:
    IdentifierListGrammar()
        : IdentifierListGrammar::base_type( m_main_rule, "identifierlist" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;
        m_main_rule = m_grammar_identifier[ push_back( _val, qi::_1 ) ]
                      >> *( lit( "::" ) >> m_grammar_identifier[ push_back( _val, qi::_1 ) ] );
    }

    IdentifierGrammar< Iterator >                         m_grammar_identifier;
    boost::spirit::qi::rule< Iterator, IdentifierList() > m_main_rule;
};

ParseResult parse( const std::string& strInput, IdentifierList& idlist, std::ostream& errorStream )
{
    return parse_impl< IdentifierListGrammar< IteratorType > >( strInput, idlist, errorStream );
}
} // namespace protocol::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Type,
    ( protocol::schema::IdentifierList, m_idList )
    ( std::vector< protocol::schema::Type >, m_children ) )
// clang-format on

namespace protocol::schema
{
template < typename Iterator >
class TypeGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Type() >
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

        m_main_rule = m_grammar_idlist[ at_c< 0 >( _val ) = qi::_1 ] >> -m_type_list_rule[ at_c< 1 >( _val ) = qi::_1 ];
    }

    IdentifierListGrammar< Iterator >                                                   m_grammar_idlist;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, std::vector< Type >() > m_type_list_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Type() >                m_main_rule;
};

ParseResult parse( const std::string& strInput, Type& type, std::ostream& errorStream )
{
    return parse_impl< TypeGrammar< IteratorType > >( strInput, type, errorStream );
}
} // namespace protocol::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Parameter,
    ( protocol::schema::Type, m_type )
    ( protocol::schema::Identifier, m_name ) )
// clang-format on

namespace protocol::schema
{
template < typename Iterator >
class ParameterGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Parameter() >
{
public:
    ParameterGrammar()
        : ParameterGrammar::base_type( m_main_rule, "Parameter" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        m_main_rule
            = m_grammar_type[ at_c< 0 >( _val ) = qi::_1 ] >> m_grammar_identifier[ at_c< 1 >( _val ) = qi::_1 ];
    }

    TypeGrammar< Iterator >                                                   m_grammar_type;
    IdentifierGrammar< Iterator >                                             m_grammar_identifier;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Parameter() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Parameter& parameter, std::ostream& errorStream )
{
    return parse_impl< ParameterGrammar< IteratorType > >( strInput, parameter, errorStream );
}
} // namespace protocol::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::PointToPointRequest,
    ( std::vector< protocol::schema::Parameter >, m_parameters ) )

BOOST_FUSION_ADAPT_STRUCT( protocol::schema::BroadcastRequest,
    ( std::vector< protocol::schema::Parameter >, m_parameters ) )

BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Response,
    ( std::vector< protocol::schema::Parameter >, m_parameters ) )

// clang-format on

namespace protocol::schema
{

enum MessageKeywordType
{
    eREQUEST,
    eBROADCAST,
    eRESPONSE
};
static const char* MessageKeywordNames[] = { "request", "broadcast", "response" };

template < typename MessageType, typename Iterator, MessageKeywordType messageKeywordType >
class MessageGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, MessageType() >
{
public:
    MessageGrammar()
        : MessageGrammar::base_type( m_main_rule, MessageKeywordNames[ messageKeywordType ] )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        // clang-format off
        //
        m_main_rule = lit( MessageKeywordNames[ messageKeywordType ]  ) >> 
            lit( '(' ) >> 
                        -(
                            m_grammar_parameter[ push_back( at_c< 0 >( _val ), qi::_1 ) ]
                            >> *( lit( ',' ) >> m_grammar_parameter[ push_back( at_c< 0 >( _val ), qi::_1 ) ] )
                        )
            >
            lit( ')' );
        // clang-format on
    }

    IdentifierGrammar< Iterator >                                               m_grammar_identifier;
    ParameterGrammar< Iterator >                                                m_grammar_parameter;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, MessageType() > m_main_rule;
};

ParseResult parse( const std::string& strInput, PointToPointRequest& request, std::ostream& errorStream )
{
    return parse_impl< MessageGrammar< PointToPointRequest, IteratorType, eREQUEST > >(
        strInput, request, errorStream );
}
ParseResult parse( const std::string& strInput, BroadcastRequest& broadcast, std::ostream& errorStream )
{
    return parse_impl< MessageGrammar< BroadcastRequest, IteratorType, eBROADCAST > >(
        strInput, broadcast, errorStream );
}
ParseResult parse( const std::string& strInput, Response& response, std::ostream& errorStream )
{
    return parse_impl< MessageGrammar< Response, IteratorType, eRESPONSE > >( strInput, response, errorStream );
}

} // namespace protocol::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Transaction,
                           ( protocol::schema::Identifier,      m_name )
                           ( protocol::schema::RequestVariant,  m_request )
                           ( protocol::schema::Response,        m_response ) 
                        )

// clang-format on

namespace protocol::schema
{

static const char* TRANSACTION_KEYWORD = "msg";

template < typename Iterator >
class TransactionGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Transaction() >
{
public:
    TransactionGrammar()
        : TransactionGrammar::base_type( m_main_rule, "transaction" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        m_grammar_request = m_grammar_point2point_request | m_grammar_broadcast_request;

        // clang-format off
        m_main_rule = lit( TRANSACTION_KEYWORD ) >> 
            m_grammar_identifier[ at_c< 0 >( _val ) = qi::_1 ]
            >>
            lit( '{' ) >> 
                            m_grammar_request   [ at_c< 1 >( _val ) = qi::_1 ] >> lit( ';' ) >>
                            m_grammar_response  [ at_c< 2 >( _val ) = qi::_1 ] >> lit( ';' ) >
            lit( '}' )
            ;
        // clang-format on
    }

    IdentifierGrammar< Iterator >                                                  m_grammar_identifier;
    MessageGrammar< PointToPointRequest, Iterator, eREQUEST >                      m_grammar_point2point_request;
    MessageGrammar< BroadcastRequest, Iterator, eBROADCAST >                       m_grammar_broadcast_request;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, RequestVariant() > m_grammar_request;
    MessageGrammar< Response, Iterator, eRESPONSE >                                m_grammar_response;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Transaction() >    m_main_rule;
};

ParseResult parse( const std::string& strInput, Transaction& msg, std::ostream& errorStream )
{
    return parse_impl< TransactionGrammar< IteratorType > >( strInput, msg, errorStream );
}

} // namespace protocol::schema
// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Schema,
    ( std::vector< protocol::schema::Transaction >, m_transactions )  )
// clang-format on

namespace protocol::schema
{
template < typename Iterator >
class SchemaGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Schema() >
{
public:
    SchemaGrammar()
        : SchemaGrammar::base_type( m_main_rule, "Schema" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        // clang-format off
        m_main_rule = *( m_grammar_msg )[ push_back( at_c< 0 >( _val ), qi::_1 ) ];
        // clang-format on
    }

    TransactionGrammar< Iterator >                                         m_grammar_msg;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Schema() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Schema& schema, std::ostream& errorStream )
{
    return parse_impl< SchemaGrammar< IteratorType > >( strInput, schema, errorStream );
}

std::ostream& operator<<( std::ostream& os, const protocol::schema::Identifier& identifier )
{
    using namespace protocol::schema;
    return os << identifier.c_str();
}

std::ostream& operator<<( std::ostream& os, const protocol::schema::IdentifierList& idlist )
{
    using namespace protocol::schema;
    common::delimit( idlist.begin(), idlist.end(), "::", os );
    return os;
}
std::ostream& operator<<( std::ostream& os, const protocol::schema::Type& type )
{
    using namespace protocol::schema;
    os << type.m_idList;
    if( !type.m_children.empty() )
    {
        os << '<';
        common::delimit( type.m_children.begin(), type.m_children.end(), ",", os );
        os << '>';
    }
    return os;
}
std::ostream& operator<<( std::ostream& os, const protocol::schema::Parameter& parameter )
{
    using namespace protocol::schema;
    return os << parameter.m_type << " " << parameter.m_name;
}

std::ostream& operator<<( std::ostream& os, const protocol::schema::PointToPointRequest& message )
{
    using namespace protocol::schema;
    os << protocol::schema::MessageKeywordNames[ protocol::schema::eREQUEST ] << '(';
    common::delimit( message.m_parameters.begin(), message.m_parameters.end(), ",", os );
    return os << ')';
}
std::ostream& operator<<( std::ostream& os, const protocol::schema::BroadcastRequest& message )
{
    using namespace protocol::schema;
    os << protocol::schema::MessageKeywordNames[ protocol::schema::eBROADCAST ] << '(';
    common::delimit( message.m_parameters.begin(), message.m_parameters.end(), ",", os );
    return os << ')';
}
std::ostream& operator<<( std::ostream& os, const protocol::schema::RequestVariant& message )
{
    using namespace protocol::schema;
    struct Visitor
    {
        std::ostream& os;
        Visitor( std::ostream& os_ )
            : os( os_ )
        {
        }
        void operator()( const protocol::schema::PointToPointRequest& message ) const { os << message << "\n"; }
        void operator()( const protocol::schema::BroadcastRequest& message ) const { os << message << "\n"; }
    } visitor( os );
    boost::apply_visitor( visitor, message );
    return os;
}
std::ostream& operator<<( std::ostream& os, const protocol::schema::Response& message )
{
    using namespace protocol::schema;
    os << protocol::schema::MessageKeywordNames[ protocol::schema::eRESPONSE ] << '(';
    common::delimit( message.m_parameters.begin(), message.m_parameters.end(), ",", os );
    return os << ')';
}

std::ostream& operator<<( std::ostream& os, const protocol::schema::Transaction& transaction )
{
    const std::string strDelim = "\n    ";
    using namespace protocol::schema;
    os << TRANSACTION_KEYWORD << ' ' << transaction.m_name << "\n{    " << transaction.m_request << strDelim
       << transaction.m_response << strDelim << '}';
    return os;
}

std::ostream& operator<<( std::ostream& os, const protocol::schema::Schema& schema )
{
    using namespace protocol::schema;
    for( const Transaction& transaction : schema.m_transactions )
    {
        os << transaction << "\n";
    }
    return os;
}
} // namespace protocol::schema
