
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

#ifndef GUARD_2023_March_26_defaults
#define GUARD_2023_March_26_defaults

#include "mega/values/compilation/interface/type_id.hpp"

#include "common/assert_verify.hpp"
#include "common/serialisation.hpp"

#include <unordered_map>
#include <map>
#include <variant>
#include <vector>
#include <string>
#include <any>

namespace mega
{

class Defaults
{
    // clang-format off
    using Variant = std::variant
    < 
        int, 
        float, 
        bool, 
        std::string, 
        std::vector< int >, 
        std::vector< float >,
        std::vector< std::string > 
    >;
    // clang-format on
    // using Table = std::unordered_map< TypeID, Variant, TypeID::Hash >;
    using Table = std::map< interface::TypeID, Variant >;

public:
    template < typename T >
    void set( interface::TypeID typeID, const T& value )
    {
        VERIFY_RTE_MSG(
            m_table.find( typeID ) == m_table.end(), "Attempted to override existing element at: " << typeID );
        m_table[ typeID ] = value;
    }

    template < typename T >
    const T& get( interface::TypeID typeID )
    {
        auto iFind = m_table.find( typeID );
        VERIFY_RTE_MSG( iFind != m_table.end(), "Failed to locate default value for typeID: " << typeID );
        auto p = boost::get< T >( &iFind->second );
        VERIFY_RTE_MSG( p, "Failed to locate default value for typeID: " << typeID );
        return *p;
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& boost::serialization::make_nvp( "table", m_table );
    }

private:
    Table m_table;
};

class DefaultsProvider
{
public:
    virtual ~DefaultsProvider()                    = default;
    virtual void getDefaults( Defaults& defaults ) = 0;
};

} // namespace mega

#endif // GUARD_2023_March_26_defaults
