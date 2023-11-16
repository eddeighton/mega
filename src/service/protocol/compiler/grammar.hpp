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

#ifndef DATABASE_GRAMMAR_4_APRIL_2022
#define DATABASE_GRAMMAR_4_APRIL_2022

#include "common/assert_verify.hpp"

#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/variant.hpp>

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <ostream>
#include <sstream>

namespace protocol::schema
{
using UnderlyingIterType = std::string::const_iterator;
using IteratorType       = boost::spirit::line_pos_iterator< UnderlyingIterType >;

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
    Type       m_type;
    Identifier m_name;
};

struct Message
{
    std::vector< Parameter > m_parameters;
};
struct PointToPointRequest : public Message
{
};
struct BroadcastRequest : public Message
{
};
struct Response : public Message
{
};
using MessageVariant = boost::variant< PointToPointRequest, BroadcastRequest, Response >;

ParseResult parse( const std::string& strInput, PointToPointRequest& message, std::ostream& errorStream );
ParseResult parse( const std::string& strInput, BroadcastRequest& message, std::ostream& errorStream );
ParseResult parse( const std::string& strInput, Response& message, std::ostream& errorStream );

using RequestVariant = boost::variant< PointToPointRequest, BroadcastRequest >;

struct Transaction
{
    Identifier     m_name;
    RequestVariant m_request;
    Response       m_response;
};

ParseResult parse( const std::string& strInput, Transaction& transaction, std::ostream& errorStream );

struct Schema
{
public:
    std::vector< Transaction > m_transactions;
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

std::ostream& operator<<( std::ostream& os, const protocol::schema::Identifier& identifier );
std::ostream& operator<<( std::ostream& os, const protocol::schema::IdentifierList& idlist );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Type& type );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Parameter& parameter );
std::ostream& operator<<( std::ostream& os, const protocol::schema::PointToPointRequest& message );
std::ostream& operator<<( std::ostream& os, const protocol::schema::BroadcastRequest& message );
std::ostream& operator<<( std::ostream& os, const protocol::schema::RequestVariant& message );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Response& message );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Transaction& transaction );
std::ostream& operator<<( std::ostream& os, const protocol::schema::Schema& schema );

} // namespace protocol::schema

#endif // DATABASE_GRAMMAR_4_APRIL_2022
