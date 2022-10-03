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

#include <vector>
#include <ostream>
#include <optional>

namespace mega::network
{

class Status
{
public:
    using StatusVector = std::vector< Status >;

    Status() {}
    Status( const StatusVector& children )
        : m_childStatus( children )
    {
    }

    const std::optional< mega::MP >&              getMP() const { return m_mp; }
    const std::optional< mega::MPO >&             getMPO() const { return m_mpo; }
    const std::vector< network::ConversationID >& getConversations() const { return m_conversationIDs; }
    const std::string&                            getDescription() const { return m_description; }
    const StatusVector&                           getChildren() const { return m_childStatus; }

    void setMP( mega::MP mp ) { m_mp = mp; }
    void setMPO( mega::MPO mpo ) { m_mpo = mpo; }
    void setConversationID( const std::vector< network::ConversationID >& conversations )
    {
        m_conversationIDs = conversations;
    }
    void setDescription( const std::string& strDescription ) { m_description = strDescription; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_mp;
        archive& m_mpo;
        archive& m_conversationIDs;
        archive& m_description;
        archive& m_childStatus;
    }

private:
    std::optional< mega::MP >              m_mp;
    std::optional< mega::MPO >             m_mpo;
    std::vector< network::ConversationID > m_conversationIDs;
    std::string                            m_description;
    StatusVector                           m_childStatus;
};

std::ostream& operator<<( std::ostream& os, const Status& conversationID );

} // namespace mega::network

#endif // STATUS_23_SEPT_2022
