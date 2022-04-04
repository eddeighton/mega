#ifndef DATABASE_GRAMMAR_4_APRIL_2022
#define DATABASE_GRAMMAR_4_APRIL_2022

#include "common/assert_verify.hpp"
#include "common/string.hpp"

#include <boost/algorithm/string/detail/formatter.hpp>
#include <boost/variant.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/variant/detail/apply_visitor_unary.hpp>
#include <string>
#include <iostream>
#include <optional>

namespace db
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

        void operator()( Iterator                   first,
                         Iterator                   last,
                         Iterator                   err_pos,
                         boost::spirit::info const& what ) const
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

    typedef std::string::const_iterator                            UnderlyingIterType;
    typedef boost::spirit::line_pos_iterator< UnderlyingIterType > IteratorType;

    struct ParseResult
    {
        bool         bSuccess;
        IteratorType iterReached;
    };

    template < template < typename IterType > class GrammarType, typename ResultType >
    inline ParseResult
    parse_impl( const std::string& strInput, ResultType& result, std::ostream& errorStream )
    {
        GrammarType< IteratorType > grammar;
        {
            using namespace boost::phoenix;
            function< error_handler< IteratorType > > const error_handler( errorStream );
            boost::spirit::qi::on_error< boost::spirit::qi::fail >(
                grammar.m_main_rule,
                error_handler( boost::spirit::qi::_1,
                               boost::spirit::qi::_2,
                               boost::spirit::qi::_3,
                               boost::spirit::qi::_4 ) );
        }

        ParseResult resultPair{ false, IteratorType( strInput.begin() ) };

        resultPair.bSuccess = boost::spirit::qi::phrase_parse( resultPair.iterReached,
                                                               IteratorType( strInput.end() ),
                                                               grammar,
                                                               SkipGrammar< IteratorType >(),
                                                               result );

        return resultPair;
    }

    template < class T >
    inline T parse( const std::string& strInput )
    {
        std::ostringstream osError;
        T                  resultType;
        const ParseResult  result = parse( strInput, resultType, osError );
        VERIFY_RTE_MSG( result.bSuccess && result.iterReached.base() == strInput.end(),
                        "Failed to parse string: " << strInput << " : " << osError.str() );
        return resultType;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Identifier
    class Identifier : public std::string
    {
    public:
        class Compare
        {
        public:
            inline bool operator()( const Identifier& left, const Identifier& right ) const
            {
                return std::lexicographical_compare(
                    left.begin(), left.end(), right.begin(), right.end() );
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

    inline ParseResult
    parse( const std::string& strInput, Identifier& code, std::ostream& errorStream )
    {
        return parse_impl< IdentifierGrammar >( strInput, code, errorStream );
    }

    using IdentifierList = std::vector< Identifier >;

    inline std::ostream& operator<<( std::ostream& os, const IdentifierList& idlist )
    {
        common::delimit( idlist.begin(), idlist.end(), "::", os );
        return os;
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
            m_main_rule = m_grammar_identifier[ push_back( _val, qi::_1 ) ] >> *(
                              string( "::" ) >> m_grammar_identifier[ push_back( _val, qi::_1 ) ] );
        }

        IdentifierGrammar< Iterator >                         m_grammar_identifier;
        boost::spirit::qi::rule< Iterator, IdentifierList() > m_main_rule;
    };

    inline ParseResult
    parse( const std::string& strInput, IdentifierList& idlist, std::ostream& errorStream )
    {
        return parse_impl< IdentifierListGrammar >( strInput, idlist, errorStream );
    }

    class Type
    {
    public:
        IdentifierList      m_idList;
        std::vector< Type > m_children;
    };

    inline std::ostream& operator<<( std::ostream& os, const Type& type )
    {
        os << type.m_idList;
        if ( !type.m_children.empty() )
        {
            os << '<';
            common::delimit( type.m_children.begin(), type.m_children.end(), ",", os );
            os << '>';
        }
        return os;
    }

} // namespace schema
} // namespace db

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Type,
    ( db::schema::IdentifierList, m_idList )
    ( std::vector< db::schema::Type >, m_children ) )
// clang-format on

namespace db
{
namespace schema
{

    template < typename Iterator >
    class TypeGrammar
        : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Type() >
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
                               >> *( lit( ',' ) >> ( m_main_rule )[ push_back( _val, qi::_1 ) ] )
                               > lit( '>' );

            m_main_rule = m_grammar_idlist[ at_c< 0 >( _val ) = qi::_1 ]
                          >> -m_type_list_rule[ at_c< 1 >( _val ) = qi::_1 ];
        }

        IdentifierListGrammar< Iterator > m_grammar_idlist;
        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, std::vector< Type >() >
                                                                             m_type_list_rule;
        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Type() > m_main_rule;
    };

    inline ParseResult parse( const std::string& strInput, Type& type, std::ostream& errorStream )
    {
        return parse_impl< TypeGrammar >( strInput, type, errorStream );
    }

    template < typename T >
    class StrongStringType : public std::string
    {
    public:
        class Compare
        {
        public:
            inline bool operator()( const StrongStringType< T >& left,
                                    const StrongStringType< T >& right ) const
            {
                return std::lexicographical_compare(
                    left.begin(), left.end(), right.begin(), right.end() );
            }
        };
        StrongStringType() {}
        StrongStringType( const std::string& str )
            : std::string( str )
        {
        }
        StrongStringType( const char* pszStr )
            : std::string( pszStr )
        {
        }
    };

    struct PerObjectFileTag;
    struct PerProgramFileTag;

    using PerObjectFile = StrongStringType< PerObjectFileTag >;
    using PerProgramFile = StrongStringType< PerProgramFileTag >;

    using FileVariant = boost::variant< PerObjectFile, PerProgramFile >;

    class Stage
    {
    public:
        Identifier                 m_name;
        std::vector< FileVariant > m_files;
    };

    inline std::ostream& operator<<( std::ostream& os, const Stage& stage )
    {
        os << "stage " << stage.m_name << "\n{\n";
        struct FileVariantPrinter : boost::static_visitor< void >
        {
            std::ostream& os;
            FileVariantPrinter( std::ostream& os )
                : os( os )
            {
            }
            void operator()( const PerObjectFile& file ) const
            {
                os << "    object " << file << "\n";
            }
            void operator()( const PerProgramFile& file ) const
            {
                os << "    program " << file << "\n";
            }
        };
        FileVariantPrinter printer( os );
        for ( const FileVariant& file : stage.m_files )
        {
            boost::apply_visitor( printer, file );
        }
        os << "}\n";
        return os;
    }
} // namespace schema
} // namespace db

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Stage,
    ( db::schema::Identifier, m_name )
    ( std::vector< db::schema::FileVariant >, m_files ) )
// clang-format on

namespace db
{
namespace schema
{

    template < typename Iterator >
    class StageGrammar
        : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Stage() >
    {
    public:
        StageGrammar()
            : StageGrammar::base_type( m_main_rule, "Stage" )
        {
            using namespace boost::spirit;
            using namespace boost::spirit::qi;
            using boost::spirit::qi::eoi;
            using namespace boost::phoenix;

            m_perObjectFile = string( "object" ) >> m_grammar_id[ _val = qi::_1 ];
            m_perProgramFile = string( "program" ) >> m_grammar_id[ _val = qi::_1 ];

            m_main_rule = string( "stage" ) >> m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ]
                          >> lit( '{' )
                          >> *( ( m_perObjectFile
                                  | m_perProgramFile )[ push_back( at_c< 1 >( _val ), qi::_1 ) ] )
                          >> lit( '}' );
        }

        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, PerObjectFile() >
            m_perObjectFile;
        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, PerProgramFile() >
            m_perProgramFile;

        IdentifierGrammar< Iterator >                                         m_grammar_id;
        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Stage() > m_main_rule;
    };

    inline ParseResult parse( const std::string& strInput, Stage& stage, std::ostream& errorStream )
    {
        return parse_impl< StageGrammar >( strInput, stage, errorStream );
    }

    class Property
    {
    public:
        Type                        m_type;
        Identifier                  m_name;
        std::optional< Identifier > m_optFile;
    };

    inline std::ostream& operator<<( std::ostream& os, const Property& property )
    {
        os << property.m_type << property.m_name;
        if ( property.m_optFile.has_value() )
            os << "->" << property.m_optFile.value();
        return os;
    }
} // namespace schema
} // namespace db

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Property,
    ( db::schema::Type, m_type )
    ( db::schema::Identifier, m_name )
    ( std::optional< db::schema::Identifier >, m_optFile ) )
// clang-format on

namespace db
{
namespace schema
{

    template < typename Iterator >
    class PropertyGrammar
        : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Property() >
    {
    public:
        PropertyGrammar()
            : PropertyGrammar::base_type( m_main_rule, "Property" )
        {
            using namespace boost::spirit;
            using namespace boost::spirit::qi;
            using boost::spirit::qi::eoi;
            using namespace boost::phoenix;

            m_main_rule = m_grammar_type[ at_c< 0 >( _val ) = qi::_1 ]
                          >> m_grammar_id[ at_c< 1 >( _val ) = qi::_1 ]
                          >> -( string( "->" ) >> m_grammar_id )[ at_c< 2 >( _val ) = qi::_1 ];
        }

        TypeGrammar< Iterator >                                                  m_grammar_type;
        IdentifierGrammar< Iterator >                                            m_grammar_id;
        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Property() > m_main_rule;
    };

    inline ParseResult
    parse( const std::string& strInput, Property& property, std::ostream& errorStream )
    {
        return parse_impl< PropertyGrammar >( strInput, property, errorStream );
    }

    class Object
    {
    public:
        Identifier              m_name;
        Identifier              m_file;
        std::vector< Property > m_properties;
    };

    inline std::ostream& operator<<( std::ostream& os, const Object& object )
    {
        os << object.m_name << "->" << object.m_file;
        os << "\n{\n";
        for ( const Property& property : object.m_properties )
        {
            os << property << "\n";
        }
        os << "\n}\n";
        return os;
    }
} // namespace schema
} // namespace db

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Object,
    ( db::schema::Identifier, m_name )
    ( db::schema::Identifier, m_file )
    ( std::vector< db::schema::Property >, m_properties ) )
// clang-format on

namespace db
{
namespace schema
{

    template < typename Iterator >
    class ObjectGrammar
        : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Object() >
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
        m_main_rule =   string( "object" ) >> m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ] >> 
                        string( "->" ) >> m_grammar_id[ at_c< 1 >( _val ) = qi::_1 ] >>
                        lit( '{' ) >> 
                            *m_grammar_property[ push_back( at_c< 2 >( _val ), qi::_1 ) ] 
                        >> lit( '}' );
            // clang-format on
        }

        PropertyGrammar< Iterator >                                            m_grammar_property;
        TypeGrammar< Iterator >                                                m_grammar_type;
        IdentifierGrammar< Iterator >                                          m_grammar_id;
        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Object() > m_main_rule;
    };

    inline ParseResult
    parse( const std::string& strInput, Object& object, std::ostream& errorStream )
    {
        return parse_impl< ObjectGrammar >( strInput, object, errorStream );
    }

    class Namespace;

    typedef boost::variant< boost::recursive_wrapper< Namespace >, Object > NamespaceVariant;

    class Namespace
    {
    public:
        Identifier                      m_name;
        std::vector< NamespaceVariant > m_elements;
    };

    inline std::ostream& operator<<( std::ostream& os, const Namespace& namespace_ )
    {
        struct NamespaceVariantPrinter : boost::static_visitor< void >
        {
            std::ostream& os;
            NamespaceVariantPrinter( std::ostream& os )
                : os( os )
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
} // namespace schema
} // namespace db

// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Namespace,
    ( db::schema::Identifier, m_name )
    ( std::vector< db::schema::NamespaceVariant >, m_elements ) )
// clang-format on

namespace db
{
namespace schema
{
    template < typename Iterator >
    class NamespaceGrammar
        : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Namespace() >
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
            = string( "namespace" ) > m_grammar_id[ at_c< 0 >( _val ) = qi::_1 ] 
                >> lit( '{' )
                    >> *( m_main_rule | m_grammar_object )[ push_back( at_c< 1 >( _val ), qi::_1 ) ]
                >> lit( '}' );
            // clang-format on
        }

        IdentifierGrammar< Iterator >                                             m_grammar_id;
        ObjectGrammar< Iterator >                                                 m_grammar_object;
        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Namespace() > m_main_rule;
    };

    inline ParseResult
    parse( const std::string& strInput, Namespace& namespace_, std::ostream& errorStream )
    {
        return parse_impl< NamespaceGrammar >( strInput, namespace_, errorStream );
    }

    typedef boost::variant< Stage, Namespace > SchemaVariant;

    class Schema
    {
    public:
        std::vector< SchemaVariant > m_elements;
    };

    inline std::ostream& operator<<( std::ostream& os, const Schema& schema )
    {
        struct SchemaVariantPrinter : boost::static_visitor< void >
        {
            std::ostream& os;
            SchemaVariantPrinter( std::ostream& os )
                : os( os )
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
} // namespace schema
} // namespace db
// clang-format off
BOOST_FUSION_ADAPT_STRUCT( db::schema::Schema,
    ( std::vector< db::schema::SchemaVariant >, m_elements )  )
// clang-format on

namespace db
{
namespace schema
{

    template < typename Iterator >
    class SchemaGrammar
        : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, Schema() >
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
        m_main_rule = *( m_grammar_stage | m_grammar_namespace )[ push_back( at_c< 0 >( _val ), qi::_1 )  ];
            // clang-format on
        }

        StageGrammar< Iterator >                                               m_grammar_stage;
        NamespaceGrammar< Iterator >                                           m_grammar_namespace;
        boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, Schema() > m_main_rule;
    };

    inline ParseResult
    parse( const std::string& strInput, Schema& schema, std::ostream& errorStream )
    {
        return parse_impl< SchemaGrammar >( strInput, schema, errorStream );
    }

} // namespace schema
} // namespace db

#endif // DATABASE_GRAMMAR_4_APRIL_2022
