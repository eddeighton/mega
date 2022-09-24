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

std::string typeToString( const schema::Type& type )
{
    std::ostringstream os;
    os << type;
    return os.str();
}

nlohmann::json writeParameter( const schema::Parameter& param )
{
    nlohmann::json parameter( { { "name", param.m_name }, { "type", typeToString( param.m_type ) } } );
    return parameter;
}

} // namespace

void toJSON( const boost::filesystem::path& dataDir, const std::string& strFileName, const schema::Schema& schema )
{
    std::ostringstream osGuard;
    osGuard << "PROTOCOL_" << strFileName << "_GUARD";

    nlohmann::json data( { { "guard", osGuard.str() },
                           { "filename", strFileName },
                           { "requests", nlohmann::json::array() },
                           { "responses", nlohmann::json::array() } } );

    for ( const schema::Transaction& transaction : schema.m_transactions )
    {
        std::optional< schema::Parameter > returnParam;

        {
            nlohmann::json responseData( { { "name", transaction.m_name }, { "params", nlohmann::json::array() } } );
            VERIFY_RTE_MSG( transaction.m_response.m_parameters.size() <= 1U,
                            "Non singular response parameters in: " << transaction.m_name );
            for ( const schema::Parameter& parameter : transaction.m_response.m_parameters )
            {
                if ( !returnParam )
                    returnParam = parameter;
                responseData[ "params" ].push_back( writeParameter( parameter ) );
            }
            data[ "responses" ].push_back( responseData );
        }

        nlohmann::json requestData( { { "return_type", "void" },
                                      { "name", transaction.m_name },
                                      { "params", nlohmann::json::array() },
                                      { "return_msg_member", "" } } );
        if ( returnParam.has_value() )
        {
            requestData[ "return_type" ]       = typeToString( returnParam.value().m_type );
            requestData[ "return_msg_member" ] = returnParam.value().m_name;
        }

        struct Visitor
        {
            const schema::Transaction&                transaction;
            const std::optional< schema::Parameter >& returnParam;
            nlohmann::json&                           data;
            Visitor( const schema::Transaction& transaction, const std::optional< schema::Parameter >& returnParam,
                     nlohmann::json& data )
                : transaction( transaction )
                , returnParam( returnParam )
                , data( data )
            {
            }
            void operator()( const protocol::schema::PointToPointRequest& request ) const
            {
                for ( const schema::Parameter& parameter : request.m_parameters )
                {
                    data[ "params" ].push_back( writeParameter( parameter ) );
                }
            }
            void operator()( const protocol::schema::BroadcastRequest& request ) const
            {
                for ( const schema::Parameter& parameter : request.m_parameters )
                {
                    data[ "params" ].push_back( writeParameter( parameter ) );
                }
                VERIFY_RTE_MSG( returnParam, "Broadcast: " << transaction.m_name << " has no return type" );
                std::ostringstream osType;
                osType << "std::vector< " << typeToString( returnParam.value().m_type ) << " >";
                nlohmann::json broadcastParam( { { "name", returnParam.value().m_name }, { "type", osType.str() } } );
                data[ "params" ].push_back( broadcastParam );
            }
        } visitor( transaction, returnParam, requestData );
        boost::apply_visitor( visitor, transaction.m_request );

        data[ "requests" ].push_back( requestData );
    }

    std::ostringstream os;
    os << strFileName << ".json";
    writeJSON( dataDir / os.str(), data );
}

void toMessagesJSON( const boost::filesystem::path& dataDir, const std::map< std::string, schema::Schema >& schemas )
{
    nlohmann::json data( { { "messages", nlohmann::json::array() } } );

    int idCounter = 0; // must match the variant index

    {
        nlohmann::json errorResponse( { { "has_namespace", false },
                                        { "name", "MSG_Error_Dispatch" },
                                        { "is_request", false },
                                        { "is_broadcast", false },
                                        { "id", idCounter++ },
                                        { "namespaces", nlohmann::json::array() },
                                        { "members", nlohmann::json::array() } } );

        nlohmann::json parameter( { { "name", "what" }, { "type", "std::string" } } );
        errorResponse[ "members" ].push_back( parameter );
        data[ "messages" ].push_back( errorResponse );
    }
    {
        nlohmann::json errorResponse( { { "has_namespace", false },
                                        { "name", "MSG_Error_Response" },
                                        { "is_request", false },
                                        { "is_broadcast", false },
                                        { "id", idCounter++ },
                                        { "namespaces", nlohmann::json::array() },
                                        { "members", nlohmann::json::array() } } );

        nlohmann::json parameter( { { "name", "what" }, { "type", "std::string" } } );
        errorResponse[ "members" ].push_back( parameter );
        data[ "messages" ].push_back( errorResponse );
    }

    for ( const auto& [ strName, schema ] : schemas )
    {
        for ( const schema::Transaction& transaction : schema.m_transactions )
        {
            {
                std::ostringstream osName;
                osName << "MSG_" << transaction.m_name << "_Request";
                nlohmann::json request( { { "has_namespace", true },
                                          { "name", osName.str() },
                                          { "is_request", true },
                                          { "is_broadcast", false },
                                          { "id", idCounter++ },
                                          { "namespaces", { strName } },
                                          { "members", nlohmann::json::array() } } );

                struct Visitor
                {
                    const schema::Transaction& transaction;
                    nlohmann::json&            data;
                    Visitor( const schema::Transaction& transaction, nlohmann::json& data )
                        : transaction( transaction )
                        , data( data )
                    {
                    }
                    void operator()( const protocol::schema::PointToPointRequest& request ) const
                    {
                        for ( const schema::Parameter& parameter : request.m_parameters )
                        {
                            data[ "members" ].push_back( writeParameter( parameter ) );
                        }
                    }
                    void operator()( const protocol::schema::BroadcastRequest& request ) const
                    {
                        data[ "is_broadcast" ] = true;
                        {
                            std::ostringstream osName;
                            osName << "MSG_" << transaction.m_name << "_Response";
                            data[ "response_name" ] = osName.str();
                        }
                        for ( const schema::Parameter& parameter : request.m_parameters )
                        {
                            data[ "members" ].push_back( writeParameter( parameter ) );
                        }

                        const auto&        returnParam = transaction.m_response.m_parameters.back();
                        std::ostringstream osType;
                        osType << "std::vector< " << typeToString( returnParam.m_type ) << " >";
                        nlohmann::json broadcastParam( { { "name", returnParam.m_name }, { "type", osType.str() } } );
                        data[ "members" ].push_back( broadcastParam );
                        data[ "response_member" ] = returnParam.m_name;
                    }
                } visitor( transaction, request );
                boost::apply_visitor( visitor, transaction.m_request );

                data[ "messages" ].push_back( request );
            }

            {
                std::ostringstream osName;
                osName << "MSG_" << transaction.m_name << "_Response";
                nlohmann::json responseData( { { "has_namespace", true },
                                               { "name", osName.str() },
                                               { "is_request", false },
                                               { "is_broadcast", false },
                                               { "id", idCounter++ },
                                               { "namespaces", { strName } },
                                               { "members", nlohmann::json::array() } } );

                for ( const schema::Parameter& parameter : transaction.m_response.m_parameters )
                {
                    responseData[ "members" ].push_back( writeParameter( parameter ) );
                }

                data[ "messages" ].push_back( responseData );
            }
        }
    }

    std::ostringstream os;
    os << "messages"
       << ".json";
    writeJSON( dataDir / os.str(), data );
}
} // namespace jsonconv
} // namespace protocol
