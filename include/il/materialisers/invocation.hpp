
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

#ifndef GUARD_2024_January_11_invocation
#define GUARD_2024_January_11_invocation

#include "mega/values/compilation/invocation_id.hpp"

#include "mega/values/runtime/pointer.hpp"

#include "runtime/function_ptr.hpp"

namespace mega::materialiser
{

struct Invocation
{
    struct Name
    {
        inline std::string operator()( mega::InvocationID invocationID ) const
        {
            std::ostringstream os;
            os << invocationID;
            return os.str();
        }
    };

    class Functions
    {
        void*                     Read( mega::runtime::Pointer& ref );
        mega::runtime::Pointer    Write( mega::runtime::Pointer& ref, void* pData );
        void*                     LinkRead( mega::runtime::Pointer& ref );
        mega::runtime::Pointer    LinkAdd( mega::runtime::Pointer& ref, mega::runtime::Pointer& target );
        mega::runtime::Pointer    LinkRemove( mega::runtime::Pointer& ref, mega::runtime::Pointer& target );
        mega::runtime::Pointer    LinkClear( mega::runtime::Pointer& ref );
        mega::runtime::CallResult Call( mega::runtime::Pointer& ref );
        mega::runtime::Pointer    Start( mega::runtime::Pointer& ref );
        mega::runtime::Pointer    Move( mega::runtime::Pointer& ref );
        mega::runtime::Pointer    Get( mega::runtime::Pointer& ref );
    };
};

} // namespace mega::materialiser

#endif // GUARD_2024_January_11_invocation
