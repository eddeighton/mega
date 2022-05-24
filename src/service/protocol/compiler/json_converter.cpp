#include "json_converter.hpp"

#include "nlohmann/json.hpp"
#include "service/protocol/compiler/grammar.hpp"

#include <algorithm>
#include <fstream>
#include <memory>
#include <set>

namespace protocol
{
namespace jsonconv
{
namespace
{
void writeJSON( const boost::filesystem::path& filePath, const nlohmann::json& data )
{
    std::ofstream os( filePath.native(), std::ios_base::trunc | std::ios_base::out );
    os << data;
}

nlohmann::json writeMessage( const schema::Message& message )
{
    nlohmann::json messageData( { { "params", nlohmann::json::array() } } );

    for ( const auto& param : message.m_parameters )
    {
        std::ostringstream os;
        os << param.m_type;

        nlohmann::json parameter( { { "name", param.m_name }, { "type", os.str() } } );
        messageData[ "params" ].push_back( parameter );
    }

    return messageData;
}

struct SchemaVariantPrinter : boost::static_visitor< void >
{
    nlohmann::json& data;
    SchemaVariantPrinter( nlohmann::json& data )
        : data( data )
    {
    }
    void operator()( const schema::Msg& msg ) const
    {
        nlohmann::json messageData( { { "name", msg.m_name },
                                      { "request", writeMessage( msg.m_request ) },
                                      { "events", nlohmann::json::array() },
                                      { "responses", nlohmann::json::array() } } );
        for ( const auto& message : msg.m_events )
        {
            messageData[ "events" ].push_back( writeMessage( message ) );
        }
        for ( const auto& message : msg.m_responses )
        {
            messageData[ "responses" ].push_back( writeMessage( message ) );
        }
        data[ "messages" ].push_back( messageData );
    }
    void operator()( const schema::Cast& cast ) const
    {
        nlohmann::json messageData( { { "name", cast.m_name },
                                      { "request", writeMessage( cast.m_request ) },
                                      { "events", nlohmann::json::array() },
                                      { "responses", nlohmann::json::array() } } );
        for ( const auto& message : cast.m_events )
        {
            messageData[ "events" ].push_back( writeMessage( message ) );
        }
        for ( const auto& message : cast.m_responses )
        {
            messageData[ "responses" ].push_back( writeMessage( message ) );
        }
        data[ "broadcasts" ].push_back( messageData );
    }
};

} // namespace

void toJSON( const boost::filesystem::path& dataDir, const std::string& strFileName, const schema::Schema& schema )
{
    std::ostringstream osGuard;
    osGuard << "DATABASE_VIEW_" << strFileName << "_GUARD";

    nlohmann::json data( { { "guard", osGuard.str() }, { "filename", strFileName } } );

    SchemaVariantPrinter printer( data );

    for ( const schema::TransactionVariant& transaction : schema.m_transactions )
    {
        boost::apply_visitor( printer, transaction );
    }

    std::ostringstream os;
    os << strFileName << ".json";
    writeJSON( dataDir / os.str(), data );
}

} // namespace jsonconv
} // namespace protocol
