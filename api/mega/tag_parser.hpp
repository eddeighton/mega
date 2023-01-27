
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

#ifndef GUARD_2023_TAG_PARSER_25_JAN
#define GUARD_2023_TAG_PARSER_25_JAN

#include "mega/address_table.hpp"

#include <vector>
#include <list>
#include <string>
#include <optional>
#include <istream>

namespace mega
{

struct XMLTag
{
    using Vector = std::vector< XMLTag >;
    using Stack  = std::list< Vector::const_iterator >;

    std::string                          key;
    std::optional< AddressTable::Index > indexOpt;
    Vector                               children;
};

XMLTag parse( std::istream& is );

void consumeStart( std::istream& is, const std::string& strTag );
void consumeEnd( std::istream& is, const std::string& strTag );

} // namespace mega

#endif // GUARD_2023_TAG_PARSER_25_JAN
