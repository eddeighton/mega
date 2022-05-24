#include "grammar.hpp"

#include "common/string.hpp"

#include <boost/algorithm/string/detail/formatter.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "boost/phoenix/core.hpp"
#include "boost/phoenix/stl.hpp"
#include "boost/phoenix/fusion.hpp"
#include "boost/phoenix/operator.hpp"
#include "boost/phoenix/object.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/variant/detail/apply_visitor_unary.hpp>

#include <string>
#include <iostream>

namespace protocol
{
namespace schema
{
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

    void operator()( Iterator first, Iterator last, Iterator err_pos, boost::spirit::info const& what ) const
    {
        Iterator eol = err_pos;
        // int line = calculateLineNumber( first, err_pos );
        int line = boost::spirit::get_line( err_pos );
        if ( line != -1 )
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
        m_main_rule = lexeme[ char_( "a-zA-Z" )[ push_back( _val, qi::_1 ) ]
                              >> *( char_( "a-zA-Z0-9_" )[ push_back( _val, qi::_1 ) ] ) ];
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
} // namespace schema
} // namespace protocol

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Type,
    ( protocol::schema::IdentifierList, m_idList )
    ( std::vector< protocol::schema::Type >, m_children ) )
// clang-format on

namespace protocol
{
namespace schema
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
} // namespace schema
} // namespace protocol

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Parameter,
    ( protocol::schema::Type, m_type )
    ( protocol::schema::Identifier, m_name ) )
// clang-format on

namespace protocol
{
namespace schema
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
} // namespace schema
} // namespace protocol

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Request,
    ( std::vector< protocol::schema::Parameter >, m_parameters ) )

BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Event,
    ( std::vector< protocol::schema::Parameter >, m_parameters ) )

BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Response,
    ( std::vector< protocol::schema::Parameter >, m_parameters ) )

// clang-format on

namespace protocol
{
namespace schema
{

enum MessageKeywordType
{
    eREQUEST,
    eEVENT,
    eRESPONSE
};
static const char* MessageKeywordNames[] = { "request", "event", "response" };

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

ParseResult parse( const std::string& strInput, Request& request, std::ostream& errorStream )
{
    return parse_impl< MessageGrammar< Request, IteratorType, eREQUEST > >( strInput, request, errorStream );
}
ParseResult parse( const std::string& strInput, Event& event, std::ostream& errorStream )
{
    return parse_impl< MessageGrammar< Event, IteratorType, eEVENT > >( strInput, event, errorStream );
}
ParseResult parse( const std::string& strInput, Response& response, std::ostream& errorStream )
{
    return parse_impl< MessageGrammar< Response, IteratorType, eRESPONSE > >( strInput, response, errorStream );
}

} // namespace schema
} // namespace protocol

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Msg,
                           ( protocol::schema::Identifier,              m_name )
                           ( protocol::schema::Request,                 m_request )
                           ( std::vector< protocol::schema::Event >,    m_events )
                           ( std::vector< protocol::schema::Response >, m_responses ) 
                        )

BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Cast,
                           ( protocol::schema::Identifier,              m_name )
                           ( protocol::schema::Request,                 m_request )
                           ( std::vector< protocol::schema::Event >,    m_events )
                           ( std::vector< protocol::schema::Response >, m_responses ) 
                        )
// clang-format on

namespace protocol
{
namespace schema
{

enum TransactionKeywordType
{
    eMSG,
    eCAST
};
static const char* TransactionKeywordNames[] = { "msg", "cast" };

template < typename TransactionType, typename Iterator, TransactionKeywordType transactionKeywordType >
class TransactionGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, TransactionType() >
{
public:
    TransactionGrammar()
        : TransactionGrammar::base_type( m_main_rule, TransactionKeywordNames[ transactionKeywordType ] )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        // clang-format off
        m_main_rule = lit( TransactionKeywordNames[ transactionKeywordType ] ) >> 
            m_grammar_identifier[ at_c< 0 >( _val ) = qi::_1 ]
            >>
            lit( '{' ) >> 
                            m_grammar_request   [ at_c< 1 >( _val ) = qi::_1 ]              >> lit( ';' ) >>
                        *(  m_grammar_event     [ push_back( at_c< 2 >( _val ), qi::_1 ) ]  >> lit( ';' ) ) >>
                        *(  m_grammar_response  [ push_back( at_c< 3 >( _val ), qi::_1 ) ]  >> lit( ';' ) ) >
            lit( '}' )
            ;
        // clang-format on
    }

    IdentifierGrammar< Iterator >                                                   m_grammar_identifier;
    MessageGrammar< Request, Iterator, eREQUEST >                                   m_grammar_request;
    MessageGrammar< Event, Iterator, eEVENT >                                       m_grammar_event;
    MessageGrammar< Response, Iterator, eRESPONSE >                                 m_grammar_response;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, TransactionType() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Msg& msg, std::ostream& errorStream )
{
    return parse_impl< TransactionGrammar< Msg, IteratorType, eMSG > >( strInput, msg, errorStream );
}
ParseResult parse( const std::string& strInput, Cast& cast, std::ostream& errorStream )
{
    return parse_impl< TransactionGrammar< Cast, IteratorType, eCAST > >( strInput, cast, errorStream );
}

} // namespace schema
} // namespace protocol
// clang-format off
BOOST_FUSION_ADAPT_STRUCT( protocol::schema::Schema,
    ( std::vector< protocol::schema::TransactionVariant >, m_transactions )  )
// clang-format on

namespace protocol
{
namespace schema
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
        m_main_rule = *( m_grammar_msg | m_grammar_cast )[ push_back( at_c< 0 >( _val ), qi::_1 ) ];
        // clang-format on
    }

    TransactionGrammar< Msg, Iterator, eMSG >                              m_grammar_msg;
    TransactionGrammar< Cast, Iterator, eCAST >                            m_grammar_cast;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Schema() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Schema& schema, std::ostream& errorStream )
{
    return parse_impl< SchemaGrammar< IteratorType > >( strInput, schema, errorStream );
}
} // namespace schema
} // namespace protocol

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
    if ( !type.m_children.empty() )
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

std::ostream& operator<<( std::ostream& os, const protocol::schema::Request& message )
{
    using namespace protocol::schema;
    os << protocol::schema::MessageKeywordNames[ protocol::schema::eREQUEST ] << '(';
    common::delimit( message.m_parameters.begin(), message.m_parameters.end(), ",", os );
    return os << ')';
}
std::ostream& operator<<( std::ostream& os, const protocol::schema::Event& message )
{
    using namespace protocol::schema;
    os << protocol::schema::MessageKeywordNames[ protocol::schema::eEVENT ] << '(';
    common::delimit( message.m_parameters.begin(), message.m_parameters.end(), ",", os );
    return os << ')';
}
std::ostream& operator<<( std::ostream& os, const protocol::schema::Response& message )
{
    using namespace protocol::schema;
    os << protocol::schema::MessageKeywordNames[ protocol::schema::eRESPONSE ] << '(';
    common::delimit( message.m_parameters.begin(), message.m_parameters.end(), ",", os );
    return os << ')';
}

std::ostream& operator<<( std::ostream& os, const protocol::schema::Msg& transaction )
{
    const std::string strDelim = "\n    ";
    using namespace protocol::schema;
    os << protocol::schema::TransactionKeywordNames[ protocol::schema::eMSG ] << ' ' << transaction.m_name << "\n{    "
       << transaction.m_request << strDelim;
    common::delimit( transaction.m_events.begin(), transaction.m_events.end(), strDelim, os );
    common::delimit( transaction.m_responses.begin(), transaction.m_responses.end(), strDelim, os );
    return os << '}';
}
std::ostream& operator<<( std::ostream& os, const protocol::schema::Cast& transaction )
{
    const std::string strDelim = "\n    ";
    using namespace protocol::schema;
    os << protocol::schema::TransactionKeywordNames[ protocol::schema::eCAST ] << ' ' << transaction.m_name << "\n{    "
       << transaction.m_request << strDelim;
    common::delimit( transaction.m_events.begin(), transaction.m_events.end(), strDelim, os );
    common::delimit( transaction.m_responses.begin(), transaction.m_responses.end(), strDelim, os );
    return os << '}';
}

std::ostream& operator<<( std::ostream& os, const protocol::schema::Schema& schema )
{
    using namespace protocol::schema;
    struct SchemaVariantPrinter : boost::static_visitor< void >
    {
        std::ostream& os;
        SchemaVariantPrinter( std::ostream& os )
            : os( os )
        {
        }
        void operator()( const Msg& msg ) const { os << msg; }
        void operator()( const Cast& cast ) const { os << cast; }
    };
    SchemaVariantPrinter printer( os );

    for ( const TransactionVariant& transaction : schema.m_transactions )
    {
        boost::apply_visitor( printer, transaction );
    }
    return os;
}
