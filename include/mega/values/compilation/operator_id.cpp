
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

namespace
{

using OperatortringArray = std::array< std::string, mega::HIGHEST_OPERATOR_TYPE >;

static const OperatortringArray g_pszOperatorStrings = {
    std::string( "New" ),
    std::string( "Delete" ),
    std::string( "Cast" ),
    std::string( "Active" ),
    std::string( "Enabled" )
};

}

std::ostream& operator<<( std::ostream& os, const mega::OperatorID& operatorID )
{
    return os << g_pszOperatorStrings[ operatorID.m_operator ] << '.' << operatorID.m_typeID;
}

#endif //GUARD_2023_October_03_operator_io
