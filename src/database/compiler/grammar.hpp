#ifndef DATABASE_GRAMMAR_4_APRIL_2022
#define DATABASE_GRAMMAR_4_APRIL_2022

#include "common/assert_verify.hpp"

#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/variant.hpp>

#include <string>
#include <vector>
#include <optional>
#include <ostream>
#include <sstream>

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

        ParseResult parse( const std::string& strInput, Identifier& code, std::ostream& errorStream );

        using IdentifierList = std::vector< Identifier >;

        ParseResult parse( const std::string& strInput, IdentifierList& idlist, std::ostream& errorStream );

        struct Type
        {
            IdentifierList      m_idList;
            std::vector< Type > m_children;
        };

        ParseResult parse( const std::string& strInput, Type& type, std::ostream& errorStream );

        struct File
        {
            Identifier m_id;
        };
        struct Source
        {
            Identifier m_id;
        };
        struct GlobalAccessor
        {
            Type m_type;
        };
        struct PerSourceAccessor
        {
            Type m_type;
        };
        struct Dependency
        {
            Identifier m_id;
        };
        using StageElementVariant = boost::variant< File, Source, GlobalAccessor, PerSourceAccessor, Dependency >;

        struct Stage
        {
            Identifier                         m_name;
            Source                             m_source;
            std::vector< StageElementVariant > m_elements;
        };

        ParseResult parse( const std::string& strInput, Stage& stage, std::ostream& errorStream );

        struct Property
        {
            Type                            m_type;
            Identifier                      m_name;
            std::optional< IdentifierList > m_optFile;
        };

        ParseResult parse( const std::string& strInput, Property& property, std::ostream& errorStream );

        struct Object
        {
            Identifier                      m_name;
            std::optional< IdentifierList > m_optInheritance;
            IdentifierList                  m_file;
            std::vector< Property >         m_properties;
        };

        ParseResult parse( const std::string& strInput, Object& object, std::ostream& errorStream );

        struct Namespace;

        using NamespaceVariant = boost::variant< boost::recursive_wrapper< Namespace >, Object >;

        struct Namespace
        {
            Identifier                      m_name;
            std::vector< NamespaceVariant > m_elements;
        };

        ParseResult parse( const std::string& strInput, Namespace& namespace_, std::ostream& errorStream );

        ParseResult parse( const std::string& strInput, Namespace& namespace_, std::ostream& errorStream );

        using SchemaVariant = boost::variant< Stage, Namespace >;
        struct Schema
        {
        public:
            std::vector< SchemaVariant > m_elements;
        };

        ParseResult parse( const std::string& strInput, Schema& schema, std::ostream& errorStream );

        template < typename T >
        inline T parse( const std::string& strInput )
        {
            std::ostringstream osError;
            T                  resultType;
            const ParseResult  result = parse( strInput, resultType, osError );
            VERIFY_RTE_MSG( result.bSuccess && result.iterReached.base() == strInput.end(),
                            "Failed to parse string: " << strInput << " : " << osError.str() );
            return resultType;
        }
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
