
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

#ifndef GUARD_2023_October_03_operator_io
#define GUARD_2023_October_03_operator_io

#include "mega/values/compilation/operator_id.hpp"

#include <ostream>
#include <array>
#include <string>

namespace mega
{
// clang-format off
const OperatorID::Array OperatorID::names =
{
    OperatorID::OperatorIDTypeName{ OperatorID::op_new,         std::string( "mega_new" ) },
    OperatorID::OperatorIDTypeName{ OperatorID::op_remote_new,  std::string( "mega_remote_new" ) },
    OperatorID::OperatorIDTypeName{ OperatorID::op_delete,      std::string( "mega_delete" ) },
    OperatorID::OperatorIDTypeName{ OperatorID::op_cast,        std::string( "mega_cast" ) },
    OperatorID::OperatorIDTypeName{ OperatorID::op_active,      std::string( "mega_active" ) },
    OperatorID::OperatorIDTypeName{ OperatorID::op_enabled,     std::string( "mega_enabled" ) }
};
// clang-format om

std::ostream& operator<<( std::ostream& os, const mega::OperatorID& operatorID )
{
    return os << mega::OperatorID::names[ operatorID.m_operator ].second << '.' << operatorID.m_typeID;
}

}

#endif //GUARD_2023_October_03_operator_io
