#ifndef SYMBOL_UTILS_SEPT_19_2022
#define SYMBOL_UTILS_SEPT_19_2022

#include "mega/reference.hpp"
#include "database/types/invocation_id.hpp"

#include "common/assert_verify.hpp"

#include <ostream>
#include <sstream>

namespace mega
{
namespace runtime
{
static inline void symbolPrefix( const char* prefix, mega::TypeID objectTypeID, std::ostream& os )
{
    std::ostringstream osTypeID;
    osTypeID << prefix << objectTypeID;
    os << "_Z" << osTypeID.str().size() << osTypeID.str();
}

static inline void symbolPrefix( const mega::InvocationID& invocationID, std::ostream& os )
{
    std::ostringstream osTypeID;
    osTypeID << invocationID;
    os << "_Z" << osTypeID.str().size() << osTypeID.str();
}

static std::string megaMangle( const std::string& strCanonicalTypeName )
{
    std::ostringstream os;
    for ( auto c : strCanonicalTypeName )
    {
        if ( std::isspace( c ) )
            continue;
        else if ( std::isalnum( c ) )
            os << c;
        else
        {
            switch ( c )
            {
                case ':':
                    os << '0';
                    break;
                case '(':
                    os << '1';
                    break;
                case ')':
                    os << '2';
                    break;
                case '<':
                    os << '3';
                    break;
                case '>':
                    os << '4';
                    break;
                case '[':
                    os << '5';
                    break;
                case ']':
                    os << '6';
                    break;
                case '.':
                    os << '7';
                    break;
                case '*':
                    os << '8';
                    break;
                case '&':
                    os << '9';
                    break;
                    break;
                default:
                    THROW_RTE( "Unexpected character in typename: " << strCanonicalTypeName );
            }
        }
    }
    return os.str();
}

} // namespace runtime
} // namespace mega

#endif // SYMBOL_UTILS_SEPT_19_2022
