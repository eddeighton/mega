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

#include "mega/reference.hpp"
#include "service/protocol/common/header.hpp"

#include "log/record.hpp"

#include <utility>
#include <vector>
#include <ostream>
#include <optional>

namespace mega::network
{

class Status
{
public:
    using StatusVector = std::vector< Status >;

    Status() = default;

    Status( StatusVector children )
        : m_childStatus( std::move( children ) )
    {
    }

    const std::optional< MachineID >&             getMachineID() const { return m_machineID; }
    const std::optional< MP >&                    getMP() const { return m_mp; }
    const std::optional< MPO >&                   getMPO() const { return m_mpo; }
    const std::vector< network::ConversationID >& getConversations() const { return m_conversationIDs; }
    const std::optional< log::IndexRecord >&      getLogIterator() const { return m_logIterator; }

    const std::optional< std::vector< std::pair< MPO, TimeStamp > > >& getReads() const { return m_reads; }
    const std::optional< std::vector< std::pair< MPO, TimeStamp > > >& getWrites() const { return m_writes; }
    const std::optional< std::vector< MPO > >&                         getReaders() const { return m_readers; }
    const std::optional< MPO >&                                        getWriter() const { return m_writer; }

    const std::string&  getDescription() const { return m_description; }
    const StatusVector& getChildren() const { return m_childStatus; }

    void setMachineID( MachineID machineID ) { m_machineID = machineID; }
    void setMP( MP mp ) { m_mp = mp; }
    void setMPO( MPO mpo ) { m_mpo = mpo; }
    void setConversationID( const std::vector< network::ConversationID >& conversations )
    {
        m_conversationIDs = conversations;
    }
    void setLogIterator( const log::IndexRecord& iterator ) { m_logIterator = iterator; }

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
        archive& m_conversationIDs;
        archive& m_logIterator;

        archive& m_reads;
        archive& m_writes;
        archive& m_readers;
        archive& m_writer;

        archive& m_description;
        archive& m_childStatus;
    }

private:
    std::optional< MachineID >             m_machineID;
    std::optional< MP >                    m_mp;
    std::optional< MPO >                   m_mpo;
    std::vector< network::ConversationID > m_conversationIDs;
    std::optional< log::IndexRecord >      m_logIterator;

    std::optional< std::vector< std::pair< MPO, TimeStamp > > > m_reads;
    std::optional< std::vector< std::pair< MPO, TimeStamp > > > m_writes;
    std::optional< std::vector< MPO > >                         m_readers;
    std::optional< MPO >                                        m_writer;

    std::string  m_description;
    StatusVector m_childStatus;
};

std::ostream& operator<<( std::ostream& os, const Status& conversationID );

} // namespace mega::network

#endif // STATUS_23_SEPT_2022
