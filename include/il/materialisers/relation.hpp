
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

#ifndef GUARD_2023_December_07_relation
#define GUARD_2023_December_07_relation

#include "mega/values/compilation/interface/relation_id.hpp"

#include "mega/values/runtime/pointer.hpp"

#include <iomanip>

namespace mega::materialiser
{

struct Relation
{
    struct Name
    {
        inline std::string operator()( mega::interface::RelationID relationID ) const
        {
            std::ostringstream os;
            os << std::hex << std::setfill( '0' ) << std::setw( 16 ) << relationID.getValue();
            return os.str();
        }
    };
    
    class Functions
    {
        void Make( mega::Pointer& source, mega::Pointer& target );
        void Break( mega::Pointer& source, mega::Pointer& target );
        void Reset( mega::Pointer& ref );
        void Size( mega::Pointer& ref );
        void Get( mega::Pointer& ref );
    };
};

} // namespace mega::materialiser
#endif //GUARD_2023_December_07_relation
