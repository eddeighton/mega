
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

#ifndef GUARD_2022_November_07_address_table
#define GUARD_2022_November_07_address_table

#include "mega/values/runtime/reference.hpp"
#include "mega/values/native_types.hpp"
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>

#include "common/assert_verify.hpp"

#include <unordered_map>
#include <vector>
#include <optional>

namespace mega
{
class AddressTable
{
public:
    using Index           = U64;
    using IndexTable      = std::unordered_map< reference, Index, reference::Hash >;
    using ReferenceVector = std::vector< reference >;

    inline const IndexTable& getTable() const { return m_table; }

    // NOTE: ensure the object address is always mapped to index
    // besides underlying contexts of the object
    inline const Index& refToIndex( const reference& maybeNetAddress )
    {
        const auto net = maybeNetAddress.getNetworkAddress();
        const auto obj = net.getObjectAddress();
        if( obj != net )
        {
            refToIndex( obj );
        }

        auto iFind = m_table.find( net );
        if( iFind == m_table.end() )
        {
            const Index index = m_table.size();
            m_references.push_back( net );
            iFind = m_table.insert( { net, index } ).first;
        }
        return iFind->second;
    }

    // if the object has an index then store and index for the reference even if
    // it is a context of the object
    inline std::optional< Index > refToIndexIfObjectExist( const reference& maybeNetAddress )
    {
        const auto net = maybeNetAddress.getNetworkAddress();
        {
            auto iFind = m_table.find( net );
            if( iFind != m_table.end() )
            {
                return iFind->second;
            }
        }
        {
            const auto obj   = net.getObjectAddress();
            auto       iFind = m_table.find( obj );
            if( iFind != m_table.end() )
            {
                return refToIndex( net );
            }
        }
        return {};
    }

    inline const reference& indexToRef( Index index ) const
    {
        ASSERT( index < m_references.size() );
        return m_references[ index ];
    }

    inline void remap( const Index& index, const reference& ref )
    {
        const auto existing = indexToRef( index );
        auto       iFind    = m_table.find( existing );
        ASSERT( iFind != m_table.end() );
        m_table.erase( iFind );
        m_table[ ref ]        = index;
        m_references[ index ] = ref;
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_table;
        archive& m_references;
    }

private:
    IndexTable      m_table;
    ReferenceVector m_references;
};
} // namespace mega

#endif // GUARD_2022_November_07_address_table
