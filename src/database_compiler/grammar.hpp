#ifndef DATABASE_GRAMMAR_4_APRIL_2022
#define DATABASE_GRAMMAR_4_APRIL_2022

#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/variant.hpp>

#include <string>
#include <vector>
#include <optional>
#include <ostream>

namespace db
{
    namespace schema
    {
        using UnderlyingIterType = std::string::const_iterator;
        using IteratorType = boost::spirit::line_pos_iterator< UnderlyingIterType >;

        struct ParseResult
        {
            bool         bSuccess;
            IteratorType iterReached;
        };

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

        ParseResult
        parse( const std::string& strInput, Identifier& code, std::ostream& errorStream );

        using IdentifierList = std::vector< Identifier >;

        ParseResult
        parse( const std::string& strInput, IdentifierList& idlist, std::ostream& errorStream );

        class Type
        {
        public:
            IdentifierList      m_idList;
            std::vector< Type > m_children;
        };

        ParseResult parse( const std::string& strInput, Type& type, std::ostream& errorStream );

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

        ParseResult parse( const std::string& strInput, Stage& stage, std::ostream& errorStream );

        class Property
        {
        public:
            Type                        m_type;
            Identifier                  m_name;
            std::optional< Identifier > m_optFile;
        };

        ParseResult
        parse( const std::string& strInput, Property& property, std::ostream& errorStream );

        class Object
        {
        public:
            Identifier              m_name;
            Identifier              m_file;
            std::vector< Property > m_properties;
        };

        ParseResult parse( const std::string& strInput, Object& object, std::ostream& errorStream );

        class Namespace;

        using NamespaceVariant = boost::variant< boost::recursive_wrapper< Namespace >, Object >;

        class Namespace
        {
        public:
            Identifier                      m_name;
            std::vector< NamespaceVariant > m_elements;
        };

        ParseResult
        parse( const std::string& strInput, Namespace& namespace_, std::ostream& errorStream );

        ParseResult
        parse( const std::string& strInput, Namespace& namespace_, std::ostream& errorStream );

        using SchemaVariant = boost::variant< Stage, Namespace >;
        class Schema
        {
        public:
            std::vector< SchemaVariant > m_elements;
        };

        ParseResult parse( const std::string& strInput, Schema& schema, std::ostream& errorStream );

    } // namespace schema
} // namespace db

std::ostream& operator<<( std::ostream& os, const db::schema::Identifier& identifier );
std::ostream& operator<<( std::ostream& os, const db::schema::IdentifierList& idlist );
std::ostream& operator<<( std::ostream& os, const db::schema::Type& type );
std::ostream& operator<<( std::ostream& os, const db::schema::Stage& stage );
std::ostream& operator<<( std::ostream& os, const db::schema::Property& property );
std::ostream& operator<<( std::ostream& os, const db::schema::Object& object );
std::ostream& operator<<( std::ostream& os, const db::schema::Namespace& namespace_ );
std::ostream& operator<<( std::ostream& os, const db::schema::Schema& schema );

#endif // DATABASE_GRAMMAR_4_APRIL_2022
