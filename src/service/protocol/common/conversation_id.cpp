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

#include "service/protocol/common/conversation_id.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace mega::network
{

std::ostream& operator<<( std::ostream& os, const ConversationID& conversationID )
{
    return os << conversationID.getID() << "_" << conversationID.getConnectionID();
}

std::istream& operator>>( std::istream& is, ConversationID& conversationID )
{
    std::string str;
    is >> str;

    auto iter = std::find( str.begin(), str.end(), '_' );

    VERIFY_RTE( iter != str.end() );

    const std::string strCon( iter + 1, str.end() );

    ConversationID::ID id;
    {
        std::string        strInt( str.begin(), iter );
        std::istringstream isInt( strInt );
        isInt >> id;
    }

    conversationID = ConversationID{ id, strCon };

    return is;
}

} // namespace mega::network
