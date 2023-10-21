
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

#include "mega/values/runtime/reference.hpp"
#include "mega/values/native_types.hpp"
#include "mega/address_table.hpp"

#include <unordered_map>
#include <vector>

namespace mega
{

class Snapshot
{
public:
    using Buffer      = std::vector< char >;
    using IndexVector = std::vector< AddressTable::Index >;

    Snapshot()
        : m_timeStamp( 0U )
    {
    }
    Snapshot( mega::TimeStamp timeStamp )
        : m_timeStamp( timeStamp )
    {
    }

    TimeStamp           getTimeStamp() const { return m_timeStamp; }
    const Buffer&       getBuffer() const { return m_buffer; }
    const IndexVector&  getObjects() const { return m_objects; }
    const AddressTable& getTable() const { return m_table; }

    AddressTable& getTable() { return m_table; }
    void setTimeStamp( TimeStamp timeStamp ) { m_timeStamp = timeStamp; }
    void setBuffer( const Buffer& buffer ) { m_buffer = buffer; }

    void beginObject( const reference& ref ) { m_objects.push_back( m_table.refToIndex( ref ) ); }

    const AddressTable::Index& refToIndex( const reference& ref ) { return m_table.refToIndex( ref ); }
    void remap( const AddressTable::Index& index, const reference& ref ) { m_table.remap( index, ref ); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_table;
        archive& m_timeStamp;
        archive& m_objects;
        archive& m_buffer;
    }

private:
    AddressTable m_table;
    TimeStamp    m_timeStamp;
    IndexVector  m_objects;
    Buffer       m_buffer;
};

} // namespace mega

#endif // GUARD_2022_November_02_snapshot
