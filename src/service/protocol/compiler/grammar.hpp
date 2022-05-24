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

namespace protocol
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

        struct Parameter
        {
            Type m_type;
            Identifier m_name;
        };

        struct Message
        {
            std::vector< Parameter > m_parameters;
        };
        struct Request : public Message
        {
        };
        struct Event : public Message
        {
        };
        struct Response : public Message
        {
        };
        using MessageVariant = boost::variant< Request, Event, Response >;

        ParseResult parse( const std::string& strInput, Request& message, std::ostream& errorStream );
        ParseResult parse( const std::string& strInput, Event& message, std::ostream& errorStream );
        ParseResult parse( const std::string& strInput, Response& message, std::ostream& errorStream );

        struct Transaction
        {
            Identifier m_name;
            Request m_request;
            std::vector< Event > m_events;
            std::vector< Response > m_responses;
        };
        struct Msg : public Transaction
        {
        };
        struct Cast : public Transaction
        {
        };
        using TransactionVariant = boost::variant< Msg, Cast >;

        ParseResult parse( const std::string& strInput, Msg& transaction, std::ostream& errorStream );
        ParseResult parse( const std::string& strInput, Cast& transaction, std::ostream& errorStream );

        struct Schema
        {
        public:
            std::vector< TransactionVariant > m_transactions;
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
} // namespace protocol

std::ostream& operator<<( std::ostream& os, const protocol::schema::Identifier& identifier );
std::ostream& operator<<( std::ostream& os, const protocol::schema::IdentifierList& idlist );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Type& type );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Parameter& parameter );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Request& message );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Event& message );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Response& message );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Msg& transaction );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Cast& transaction );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Schema& schema );

#endif // DATABASE_GRAMMAR_4_APRIL_2022
