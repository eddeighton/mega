
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

#ifndef GUARD_2022_November_02_snapshot
#define GUARD_2022_November_02_snapshot

#include "mega/reference.hpp"
#include "mega/reference_io.hpp"
#include "mega/native_types.hpp"

#include "common/assert_verify.hpp"

#include <boost/serialization/split_member.hpp>
#include <boost/serialization/array_wrapper.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>

#include <unordered_map>
#include <optional>

namespace mega
{

class Snapshot
{
public:
    using Index           = U64;
    using IndexTable      = std::unordered_map< reference, Index, reference::Hash >;
    using ReferenceVector = std::vector< reference >;
    using Buffer          = std::vector< char >;

    Snapshot()
        : m_timeStamp( 0U )
    {
    }
    Snapshot( mega::TimeStamp timeStamp )
        : m_timeStamp( timeStamp )
    {
    }

    TimeStamp              getTimeStamp() const { return m_timeStamp; }
    const Buffer&          getBuffer() const { return m_buffer; }
    const ReferenceVector& getObjects() const { return m_objects; }
    /*const reference&       getRoot() const
    {
        std::optional< const reference* > root;
        for ( const auto& ref : m_objects )
        {
            if ( ref.type == ROOT_TYPE_ID )
            {
                VERIFY_RTE_MSG( !root.has_value(), "Duplicate root references found in snapshot" );
                root = &ref;
            }
        }
        VERIFY_RTE_MSG( root.has_value(), "Failed to locate root object in snapshot" );
        return *root.value();
    }*/

    void setTimeStamp( TimeStamp timeStamp ) { m_timeStamp = timeStamp; }
    void setBuffer( const Buffer& buffer ) { m_buffer = buffer; }

    void beginObject( const reference& ref )
    {
        m_objects.push_back( ref );
        refToIndex( ref );
    }

    const Index& refToIndex( const reference& ref )
    {
        auto iFind = m_table.find( ref );
        if ( iFind == m_table.end() )
        {
            const Index index = m_table.size();
            m_references.push_back( ref );
            iFind = m_table.insert( { ref, index } ).first;
        }
        return iFind->second;
    }

    const reference& indexToRef( Index index ) const
    {
        ASSERT( index < m_references.size() );
        return m_references[ index ];
    }

    void remap( const Index& index, const reference& ref )
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
        archive& m_timeStamp;
        archive& m_table;
        archive& m_references;
        archive& m_objects;
        archive& m_buffer;
    }

private:
    TimeStamp       m_timeStamp;
    IndexTable      m_table;
    ReferenceVector m_references;
    ReferenceVector m_objects;
    Buffer          m_buffer;
};

} // namespace mega

#endif // GUARD_2022_November_02_snapshot
