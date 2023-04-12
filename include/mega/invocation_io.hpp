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

#ifndef INVOCATION_ID_7_JUNE_2022
#define INVOCATION_ID_7_JUNE_2022

#include "mega/invocation_id.hpp"
#include "mega/type_id_io.hpp"

#include "database/types/operation.hpp"

#include "nlohmann/json.hpp"

#include <vector>
#include <array>
#include <ostream>

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::TypeID& typeID )
{
    std::ostringstream os;
    using ::           operator<<;
    os << typeID;
    j = nlohmann::json{ { "typeID", os.str() } };
}

inline void to_json( nlohmann::json& j, const mega::InvocationID& invocationID )
{
    j = nlohmann::json{ { "context", invocationID.m_context },
                        { "type_path", invocationID.m_type_path },
                        { "operation", invocationID.m_operation } };
}
} // namespace mega

namespace detail
{
    template< class Iter >
    inline void delimit( Iter pBegin, Iter pEnd, const std::string& delimiter, std::ostream& os )
    {
        for( Iter p = pBegin, pNext = pBegin; p!=pEnd; ++p )
        {
            ++pNext;
            const mega::TypeID typeID = *p;
            if( typeID.isSymbolID() )
            {
                // symbol id
                os << 's' << std::dec << -typeID.getSymbolID() << std::dec;
            }
            else
            {
                // context id
                os << 't' << std::hex << std::setw( 4 ) << std::setfill( '0' ) << typeID.getObjectID() <<
                        std::hex << std::setw( 4 ) << std::setfill( '0' ) << typeID.getSubObjectID() << std::dec;
            }
            if( pNext != pEnd )
            {
                os << delimiter;
            }
        }
    }
}

inline std::ostream& operator<<( std::ostream& os, const mega::InvocationID& invocationID )
{
    os << "c";
    detail::delimit( invocationID.m_context.begin(), invocationID.m_context.end(), "_", os );
    os << 'p';
    detail::delimit( invocationID.m_type_path.begin(), invocationID.m_type_path.end(), "_", os );
    return os << mega::getOperationString( invocationID.m_operation );
}


#endif // INVOCATION_ID_7_JUNE_2022
