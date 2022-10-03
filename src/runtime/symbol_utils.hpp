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

#ifndef SYMBOL_UTILS_SEPT_19_2022
#define SYMBOL_UTILS_SEPT_19_2022

#include "mega/reference.hpp"
#include "database/types/invocation_id.hpp"

#include "common/assert_verify.hpp"

#include <ostream>
#include <sstream>

namespace mega::runtime
{

static inline void symbolPrefix( const char* prefix, mega::TypeID objectTypeID, std::ostream& os )
{
    std::ostringstream osTypeID;
    osTypeID << prefix << objectTypeID;
    os << "_Z" << osTypeID.str().size() << osTypeID.str();
}

static inline void symbolPrefix( const mega::InvocationID& invocationID, std::ostream& os )
{
    std::ostringstream osTypeID;
    osTypeID << invocationID;
    os << "_Z" << osTypeID.str().size() << osTypeID.str();
}

static std::string megaMangle( const std::string& strCanonicalTypeName )
{
    std::ostringstream os;
    for ( auto c : strCanonicalTypeName )
    {
        if ( std::isspace( c ) )
            continue;
        else if ( std::isalnum( c ) )
            os << c;
        else
        {
            switch ( c )
            {
                case ':':
                    os << '0';
                    break;
                case '(':
                    os << '1';
                    break;
                case ')':
                    os << '2';
                    break;
                case '<':
                    os << '3';
                    break;
                case '>':
                    os << '4';
                    break;
                case '[':
                    os << '5';
                    break;
                case ']':
                    os << '6';
                    break;
                case '.':
                    os << '7';
                    break;
                case '*':
                    os << '8';
                    break;
                case '&':
                    os << '9';
                    break;
                    break;
                default:
                    THROW_RTE( "Unexpected character in typename: " << strCanonicalTypeName );
            }
        }
    }
    return os.str();
}

} // namespace mega::runtime

#endif // SYMBOL_UTILS_SEPT_19_2022
