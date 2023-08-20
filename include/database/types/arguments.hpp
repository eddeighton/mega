
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

#ifndef GUARD_2023_August_20_arguments
#define GUARD_2023_August_20_arguments

#include "database/common/api.hpp"
#include "database/common/serialisation.hpp"

#include <vector>
#include <string>
#include <optional>

namespace mega
{

struct EGDB_EXPORT Argument
{
    using Vector = std::vector< Argument >;

    Argument() = default;
    Argument( std::string type, std::optional< std::string > name )
        : m_type( std::move( type ) )
        , m_name( std::move( name ) )
    {
    }

    bool operator==( const Argument& ) const = default;

    const std::string&                  getType() const { return m_type; }
    const std::optional< std::string >& getName() const { return m_name; }

    std::string str() const;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_type;
        archive& m_name;
    }

//private: - uses BOOST_FUSION_ADAPT_STRUCT in cpp
    std::string                  m_type;
    std::optional< std::string > m_name;
};

// EGDB_EXPORT
EGDB_EXPORT void parse( const std::string& str, Argument::Vector& args );
EGDB_EXPORT std::ostream& operator<<( std::ostream& os, const mega::Argument::Vector& arguments );

} // namespace mega

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::Argument& argument )
{
    if( argument.getName().has_value() )
    {
        j = nlohmann::json{ { "type", argument.getType() }, { "name", argument.getName().value() } };
    }
    else
    {
        j = nlohmann::json{ { "type", argument.getType() } };
    }
}
} // namespace mega

#endif // GUARD_2023_August_20_arguments
