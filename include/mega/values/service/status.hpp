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

#ifndef STATUS_23_SEPT_2022
#define STATUS_23_SEPT_2022

#include "mega/values/runtime/reference.hpp"

#include "mega/values/service/logical_thread_id.hpp"
#include "service/protocol/common/sender_ref.hpp"

#include "log/index_record.hpp"

#include <utility>
#include <vector>
#include <ostream>
#include <optional>

namespace mega::network
{

struct AllocatorStatus
{
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& total;
        archive& blockSize;
        archive& allocations;
        archive& free;
    }

    U64 total;
    U64 blockSize;
    U64 allocations;
    U64 free;
};

struct MemoryStatus
{
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_heap;
        archive& m_object;
        archive& m_allocators;
    }

    U64 m_heap   = 0;
    U64 m_object = 0;

    struct TypedAllocatorStatus
    {
        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& typeID;
            archive& status;
        }
        TypeID          typeID;
        AllocatorStatus status;
    };
    using TypedAllocatorStatusArray = std::vector< TypedAllocatorStatus >;
    TypedAllocatorStatusArray m_allocators;
};

struct ComponentMgrStatus
{
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_interfaceComponents;
        archive& m_pythonComponents;
    }

    U64 m_interfaceComponents = 0;
    U64 m_pythonComponents    = 0;
};

struct JITStatus
{
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_functionPointers;
        archive& m_allocators;
        archive& m_relations;
        archive& m_invocations;
        archive& m_operators;
        archive& m_decisions;
    }

    U64 m_functionPointers = 0;
    U64 m_allocators       = 0;
    U64 m_relations        = 0;
    U64 m_invocations      = 0;
    U64 m_operators        = 0;
    U64 m_decisions        = 0;

    ComponentMgrStatus m_componentManagerStatus;
};

class Status
{
    inline MPO toMPO() const
    {
        MPO mpoIsh;
        if( getMPO().has_value() )
        {
            mpoIsh = getMPO().value();
        }
        else if( getMP().has_value() )
        {
            mpoIsh = MPO( getMP().value(), OwnerID{} );
        }
        else if( getMachineID().has_value() )
        {
            mpoIsh = MPO{ getMachineID().value(), ProcessID{}, OwnerID{} };
        }
        else
        {
            mpoIsh = MPO{ 0, 0, 0 };
        }
        return mpoIsh;
    }

public:
    using StatusVector = std::vector< Status >;

    Status() = default;

    Status( StatusVector children )
        : m_childStatus( std::move( children ) )
    {
    }

    inline void sort()
    {
        for( auto& child : m_childStatus )
            child.sort();
        std::sort( m_childStatus.begin(), m_childStatus.end(),
                   []( const Status& left, const Status& right ) -> bool { return left.toMPO() < right.toMPO(); } );
    }

    const std::optional< MachineID >&              getMachineID() const { return m_machineID; }
    const std::optional< MP >&                     getMP() const { return m_mp; }
    const std::optional< MPO >&                    getMPO() const { return m_mpo; }
    const std::vector< network::LogicalThreadID >& getLogicalThreads() const { return m_logicalthreadIDs; }
    const std::optional< log::IndexRecord >&       getLogIterator() const { return m_logIterator; }
    const std::optional< std::string >&            getLogFolder() const { return m_strLogFolder; }
    const std::optional< network::MemoryStatus >&  getMemory() const { return m_memory; }

    const std::optional< std::vector< std::pair< MPO, TimeStamp > > >& getReads() const { return m_reads; }
    const std::optional< std::vector< std::pair< MPO, TimeStamp > > >& getWrites() const { return m_writes; }
    const std::optional< std::vector< MPO > >&                         getReaders() const { return m_readers; }
    const std::optional< MPO >&                                        getWriter() const { return m_writer; }

    const std::string&  getDescription() const { return m_description; }
    const StatusVector& getChildren() const { return m_childStatus; }

    void setMachineID( MachineID machineID ) { m_machineID = machineID; }
    void setMP( MP mp ) { m_mp = mp; }
    void setMPO( MPO mpo ) { m_mpo = mpo; }
    void setLogicalThreadID( const std::vector< network::LogicalThreadID >& logicalthreads )
    {
        m_logicalthreadIDs = logicalthreads;
    }
    void setLogIterator( const log::IndexRecord& iterator ) { m_logIterator = iterator; }
    void setLogFolder( const std::string& strLogFolder ) { m_strLogFolder = strLogFolder; }
    void setMemory( network::MemoryStatus memoryStatus ) { m_memory = memoryStatus; }

    void setReads( const std::optional< std::vector< std::pair< MPO, TimeStamp > > >& value ) { m_reads = value; }
    void setWrites( const std::optional< std::vector< std::pair< MPO, TimeStamp > > >& value ) { m_writes = value; }
    void setReaders( const std::optional< std::vector< MPO > >& value ) { m_readers = value; }
    void setWriter( const std::optional< MPO >& value ) { m_writer = value; }

    void setDescription( const std::string& strDescription ) { m_description = strDescription; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_machineID;
        archive& m_mp;
        archive& m_mpo;
        archive& m_logicalthreadIDs;
        archive& m_logIterator;
        archive& m_strLogFolder;
        archive& m_memory;

        archive& m_reads;
        archive& m_writes;
        archive& m_readers;
        archive& m_writer;

        archive& m_description;
        archive& m_childStatus;
    }

private:
    std::optional< MachineID >              m_machineID;
    std::optional< MP >                     m_mp;
    std::optional< MPO >                    m_mpo;
    std::vector< network::LogicalThreadID > m_logicalthreadIDs;
    std::optional< log::IndexRecord >       m_logIterator;
    std::optional< std::string >            m_strLogFolder;
    std::optional< network::MemoryStatus >  m_memory;

    std::optional< std::vector< std::pair< MPO, TimeStamp > > > m_reads;
    std::optional< std::vector< std::pair< MPO, TimeStamp > > > m_writes;
    std::optional< std::vector< MPO > >                         m_readers;
    std::optional< MPO >                                        m_writer;

    std::string  m_description;
    StatusVector m_childStatus;
};

} // namespace mega::network

#endif // STATUS_23_SEPT_2022
