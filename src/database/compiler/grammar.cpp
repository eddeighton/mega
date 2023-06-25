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

namespace db::schema
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

    void operator()( Iterator, Iterator, Iterator err_pos, boost::spirit::info const& what ) const
    {
        const int line = boost::spirit::get_line( err_pos );
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
        m_main_rule = lexeme[ char_( "a-zA-Z" )[ push_back( _val, qi::_1 ) ]
                              >> *( char_( "a-zA-Z0-9_" )[ push_back( _val, qi::_1 ) ] ) ];
    }

    boost::spirit::qi::rule< Iterator, Identifier() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Identifier& code, std::ostream& errorStream )
{
    return parse_impl< IdentifierGrammar >( strInput, code, errorStream );
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
    return parse_impl< IdentifierListGrammar >( strInput, idlist, errorStream );
}
} // namespace db::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Type,
    ( db::schema::IdentifierList, m_idList )
    ( std::vector< db::schema::Type >, m_children ) )
// clang-format on

namespace db::schema
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
    return parse_impl< TypeGrammar >( strInput, type, errorStream );
}
} // namespace db::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::File,
    ( db::schema::Identifier, m_id ) )

BOOST_FUSION_ADAPT_STRUCT( db::schema::Source,
    ( db::schema::Identifier, m_id ) )

BOOST_FUSION_ADAPT_STRUCT( db::schema::GlobalAccessor,
    ( db::schema::Type, m_type ) )

BOOST_FUSION_ADAPT_STRUCT( db::schema::PerSourceAccessor,
    ( db::schema::Type, m_type ) )

BOOST_FUSION_ADAPT_STRUCT( db::schema::Dependency,
    ( db::schema::Identifier, m_id ) )

BOOST_FUSION_ADAPT_STRUCT( db::schema::Stage,
    ( db::schema::Identifier, m_name )
    
    ( std::vector< db::schema::StageElementVariant >, m_elements )  )

// clang-format on

namespace db::schema
{
template < typename Iterator >
class StageGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Stage() >
{
public:
    StageGrammar()
        : StageGrammar::base_type( m_main_rule, "Stage" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        m_file_rule       = lit( "file" ) >> m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ];
        m_dependency_rule = lit( "dependency" ) >> m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ];
        m_source_rule     = lit( "source" ) >> lexeme[ lit( "." ) >> m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ] ];
        m_per_source_accessor_rule
            = lit( "persource" ) >> lit( "accessor" ) >> m_grammar_type[ at_c< 0 >( _val ) = qi::_1 ];
        m_global_accessor_rule = lit( "global" ) >> lit( "accessor" ) >> m_grammar_type[ at_c< 0 >( _val ) = qi::_1 ];
        m_main_rule            = lit( "stage" ) >> m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ] >> lit( '{' )
                      >> *( ( m_file_rule | m_source_rule | m_per_source_accessor_rule | m_global_accessor_rule
                              | m_dependency_rule )[ push_back( at_c< 1 >( _val ), qi::_1 ) ]
                            >> lit( ';' ) )
                      >> lit( '}' );
    }

    IdentifierGrammar< Iterator >                                                     m_grammar_id;
    TypeGrammar< Iterator >                                                           m_grammar_type;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, GlobalAccessor() >    m_global_accessor_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, PerSourceAccessor() > m_per_source_accessor_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, File() >              m_file_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Dependency() >        m_dependency_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Source() >            m_source_rule;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Stage() >             m_main_rule;
};

ParseResult parse( const std::string& strInput, Stage& stage, std::ostream& errorStream )
{
    return parse_impl< StageGrammar >( strInput, stage, errorStream );
}

} // namespace db::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Property,
    ( db::schema::Type, m_type )
    ( db::schema::Identifier, m_name )
    ( std::optional< db::schema::IdentifierList >, m_optFile ) )
// clang-format on

namespace db::schema
{
template < typename Iterator >
class PropertyGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Property() >
{
public:
    PropertyGrammar()
        : PropertyGrammar::base_type( m_main_rule, "Property" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        // clang-format off
                m_main_rule
                    = m_grammar_type[ at_c< 0 >( _val ) = qi::_1 ]
                      >> m_grammar_id[ at_c< 1 >( _val ) = qi::_1 ]
                      >> -( lit( "->" ) >> m_grammar_idlist )[ at_c< 2 >( _val ) = qi::_1 ];
        // clang-format on
    }

    TypeGrammar< Iterator >                                                  m_grammar_type;
    IdentifierGrammar< Iterator >                                            m_grammar_id;
    IdentifierListGrammar< Iterator >                                        m_grammar_idlist;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Property() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Property& property, std::ostream& errorStream )
{
    return parse_impl< PropertyGrammar >( strInput, property, errorStream );
}

} // namespace db::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Object,
    ( db::schema::Identifier,                       m_name )
    ( std::optional< db::schema::IdentifierList >,  m_optInheritance )
    ( std::vector< db::schema::IdentifierList >,    m_files )
    ( std::vector< db::schema::Property >,          m_properties ) )
// clang-format on

namespace db::schema
{
template < typename Iterator >
class ObjectGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Object() >
{
public:
    ObjectGrammar()
        : ObjectGrammar::base_type( m_main_rule, "Object" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        // clang-format off
        m_idlist_list_rule = m_grammar_idlist[ push_back( _val, qi::_1 ) ] >>
            *( lit( "," ) >> m_grammar_idlist[ push_back( _val, qi::_1 ) ] );

        m_main_rule =   
            lit( "object" ) >> m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ] >> 
                -( lit( ':' ) >> m_grammar_idlist )[ at_c< 1 >( _val ) = qi::_1 ] >> 
                lit( "->" ) >> m_idlist_list_rule[ at_c< 2 >( _val ) = qi::_1 ] >> 
                lit( '{' ) >> 
                    *( m_grammar_property[ push_back( at_c< 3 >( _val ), qi::_1 ) ] >> lit( ';' ) )
                >> lit( '}' );
        // clang-format on
    }

    IdentifierListGrammar< Iterator > m_grammar_idlist;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, std::vector< db::schema::IdentifierList >() >
        m_idlist_list_rule;

    PropertyGrammar< Iterator >                                            m_grammar_property;
    TypeGrammar< Iterator >                                                m_grammar_type;
    IdentifierGrammar< Iterator >                                          m_grammar_id;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Object() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Object& object, std::ostream& errorStream )
{
    return parse_impl< ObjectGrammar >( strInput, object, errorStream );
}

} // namespace db::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Namespace,
    ( db::schema::Identifier, m_name )
    ( std::vector< db::schema::NamespaceVariant >, m_elements ) )
// clang-format on

namespace db::schema
{
template < typename Iterator >
class NamespaceGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Namespace() >
{
public:
    NamespaceGrammar()
        : NamespaceGrammar::base_type( m_main_rule, "Namespace" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using boost::spirit::qi::eoi;
        using namespace boost::phoenix;

        // clang-format off
                m_main_rule
                    = lit( "namespace" ) > m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ] 
                        >> lit( '{' )
                            >> *( m_main_rule | m_grammar_object )[ push_back( at_c< 1 >( _val ), qi::_1 ) ]
                        >> lit( '}' );
        // clang-format on
    }

    IdentifierGrammar< Iterator >                                             m_grammar_id;
    ObjectGrammar< Iterator >                                                 m_grammar_object;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Namespace() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Namespace& namespace_, std::ostream& errorStream )
{
    return parse_impl< NamespaceGrammar >( strInput, namespace_, errorStream );
}

} // namespace db::schema

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Schema,
    ( std::vector< db::schema::SchemaVariant >, m_elements )  )
// clang-format on

namespace db::schema
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
                m_main_rule = *( m_grammar_stage | m_grammar_namespace )[ push_back( at_c< 0 >( _val ), qi::_1 ) ];
        // clang-format on
    }

    StageGrammar< Iterator >                                               m_grammar_stage;
    NamespaceGrammar< Iterator >                                           m_grammar_namespace;
    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Schema() > m_main_rule;
};

ParseResult parse( const std::string& strInput, Schema& schema, std::ostream& errorStream )
{
    return parse_impl< SchemaGrammar >( strInput, schema, errorStream );
}
} // namespace db::schema

std::ostream& operator<<( std::ostream& os, const db::schema::Identifier& identifier )
{
    using namespace db::schema;
    return os << identifier.c_str();
}

std::ostream& operator<<( std::ostream& os, const db::schema::IdentifierList& idlist )
{
    using namespace db::schema;
    common::delimit( idlist.begin(), idlist.end(), "::", os );
    return os;
}
std::ostream& operator<<( std::ostream& os, const db::schema::Type& type )
{
    using namespace db::schema;
    os << type.m_idList;
    if ( !type.m_children.empty() )
    {
        os << '<';
        common::delimit( type.m_children.begin(), type.m_children.end(), ",", os );
        os << '>';
    }
    return os;
}
std::ostream& operator<<( std::ostream& os, const db::schema::Stage& stage )
{
    using namespace db::schema;
    os << "stage " << stage.m_name << "\n{\n";
    struct StageElementPrinter : boost::static_visitor< void >
    {
        std::ostream& os;
        StageElementPrinter( std::ostream& _os )
            : os( _os )
        {
        }
        void operator()( const db::schema::File& file ) const { os << "    file " << file.m_id << "\n"; }
        void operator()( const db::schema::Source& source ) const { os << "    source ." << source.m_id << "\n"; }
        void operator()( const db::schema::Dependency& dependency ) const
        {
            os << "    dependency " << dependency.m_id << "\n";
        }
        void operator()( const db::schema::GlobalAccessor& accessor ) const
        {
            os << "   global accessor " << accessor.m_type << "\n";
        }
        void operator()( const db::schema::PerSourceAccessor& accessor ) const
        {
            os << "    persource accessor " << accessor.m_type << "\n";
        }
    };
    StageElementPrinter printer( os );
    for ( const db::schema::StageElementVariant& file : stage.m_elements )
    {
        boost::apply_visitor( printer, file );
    }
    os << "}\n";
    return os;
}
std::ostream& operator<<( std::ostream& os, const db::schema::Property& property )
{
    using namespace db::schema;
    os << property.m_type << property.m_name;
    if ( property.m_optFile.has_value() )
        os << " -> " << property.m_optFile.value();
    return os;
}
std::ostream& operator<<( std::ostream& os, const db::schema::Object& object )
{
    using namespace db::schema;
    os << object.m_name;
    if ( object.m_optInheritance.has_value() )
        os << " : " << object.m_optInheritance.value();
    os << " -> ";
    common::delimit( object.m_files.begin(), object.m_files.end(), ",", os );
    os << "\n{\n";
    for ( const Property& property : object.m_properties )
    {
        os << property << "\n";
    }
    os << "\n}\n";
    return os;
}
std::ostream& operator<<( std::ostream& os, const db::schema::Namespace& namespace_ )
{
    using namespace db::schema;
    struct NamespaceVariantPrinter : boost::static_visitor< void >
    {
        std::ostream& os;
        NamespaceVariantPrinter( std::ostream& _os )
            : os( _os )
        {
        }
        void operator()( const Namespace& namespace_ ) const { os << namespace_; }
        void operator()( const Object& object ) const { os << object; }
    };
    NamespaceVariantPrinter printer( os );

    os << namespace_.m_name;
    os << "\n{\n";
    for ( const NamespaceVariant& element : namespace_.m_elements )
    {
        boost::apply_visitor( printer, element );
    }
    os << "\n}\n";
    return os;
}
std::ostream& operator<<( std::ostream& os, const db::schema::Schema& schema )
{
    using namespace db::schema;
    struct SchemaVariantPrinter : boost::static_visitor< void >
    {
        std::ostream& os;
        SchemaVariantPrinter( std::ostream& _os )
            : os( _os )
        {
        }
        void operator()( const Stage& stage ) const { os << stage; }
        void operator()( const Namespace& namespace_ ) const { os << namespace_; }
    };
    SchemaVariantPrinter printer( os );

    for ( const SchemaVariant& element : schema.m_elements )
    {
        boost::apply_visitor( printer, element );
    }
    return os;
}
