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
    osGuard << "DATABASE_VIEW_" << strFileName << "_GUARD";

    nlohmann::json data( { { "guard", osGuard.str() },
                           { "filename", strFileName },
                           { "requests", nlohmann::json::array() },
                           { "responses", nlohmann::json::array() } } );

    for ( const schema::Transaction& transaction : schema.m_transactions )
    {
        std::optional< schema::Parameter > returnParam;
        for ( const schema::Response& response : transaction.m_responses )
        {
            nlohmann::json responseData( { { "name", transaction.m_name }, { "params", nlohmann::json::array() } } );

            for ( const schema::Parameter& parameter : response.m_parameters )
            {
                if ( !returnParam )
                    returnParam = parameter;
                responseData[ "params" ].push_back( writeParameter( parameter ) );
            }

            data[ "responses" ].push_back( responseData );
            break;
        }

        nlohmann::json requestData( { { "return_type", "void" },
                                      { "name", transaction.m_name },
                                      { "params", nlohmann::json::array() },
                                      { "return_msg_member", "" } } );

        if( returnParam.has_value() )
        {
            requestData[ "return_type" ] = typeToString( returnParam.value().m_type );
            requestData[ "return_msg_member" ] = returnParam.value().m_name;
        }

        {
            for ( const schema::Parameter& parameter : transaction.m_request.m_parameters )
            {
                requestData[ "params" ].push_back( writeParameter( parameter ) );
            }
        }

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
        nlohmann::json completionRequest( { { "has_namespace", false },
                                            { "name", "MSG_Complete_Request" },
                                            { "is_request", true },
                                            { "id", idCounter++ },
                                            { "namespaces", nlohmann::json::array() },
                                            { "members", nlohmann::json::array() } } );
        data[ "messages" ].push_back( completionRequest );
    }
    {
        nlohmann::json errorResponse( { { "has_namespace", false },
                                        { "name", "MSG_Error_Response" },
                                        { "is_request", true },
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
                                          { "id", idCounter++ },
                                          { "namespaces", { strName } },
                                          { "members", nlohmann::json::array() } } );

                for ( const schema::Parameter& parameter : transaction.m_request.m_parameters )
                {
                    request[ "members" ].push_back( writeParameter( parameter ) );
                }

                data[ "messages" ].push_back( request );
            }

            for ( const schema::Response& response : transaction.m_responses )
            {
                std::ostringstream osName;
                osName << "MSG_" << transaction.m_name << "_Response";
                nlohmann::json responseData( { { "has_namespace", true },
                                               { "name", osName.str() },
                                               { "is_request", false },
                                               { "id", idCounter++ },
                                               { "namespaces", { strName } },
                                               { "members", nlohmann::json::array() } } );

                for ( const schema::Parameter& parameter : response.m_parameters )
                {
                    responseData[ "members" ].push_back( writeParameter( parameter ) );
                }

                data[ "messages" ].push_back( responseData );

                break;
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
