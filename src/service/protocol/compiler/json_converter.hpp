#ifndef JSON_CONVERTER_4_APRIL_2022
#define JSON_CONVERTER_4_APRIL_2022

#include "grammar.hpp"

#include "boost/filesystem/path.hpp"

namespace protocol
{
namespace jsonconv
{
    void toJSON( const boost::filesystem::path& dataDir, const std::string& strFileName, const schema::Schema& schema );

}
} // namespace db

#endif // JSON_CONVERTER_4_APRIL_2022