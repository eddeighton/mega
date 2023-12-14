
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

#include "mega/values/compilation/sub_type_instance.hpp"

#include <istream>
#include <ostream>
#include <sstream>

namespace mega
{
    /*
std::ostream& operator<<( std::ostream& os, const mega::SubTypeInstance& subTypeInstance )
{
    return os << '<' << std::hex <<

           std::setw( 4 ) << std::setfill( '0' ) << static_cast< mega::U32 >( subTypeInstance.getSubType() )

              << '.' <<

           std::setw( 4 ) << std::setfill( '0' ) << static_cast< mega::U32 >( subTypeInstance.getInstance() )

              << std::dec << '>';
}

std::istream& operator>>( std::istream& is, mega::SubTypeInstance& subTypeInstance )
{
    mega::U32 subType, instance;
    char      c;
    is >> c >> std::hex >> subType >> c >> instance >> c;
    subTypeInstance = mega::SubTypeInstance( subType, instance );
    return is;
}*/
} // namespace mega