
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
        void* read( mega::Pointer& ref );
        mega::Pointer write( mega::Pointer& ref, void* pData );
        void* LinkRead( mega::Pointer& ref );
        mega::Pointer LinkAdd( mega::Pointer& ref, mega::Pointer& target );
        mega::Pointer LinkRemove( mega::Pointer& ref, mega::Pointer& target );
        mega::Pointer LinkClear( mega::Pointer& ref );
        mega::runtime::CallResult Call( mega::Pointer& ref );
        mega::Pointer Start( mega::Pointer& ref );
        mega::Pointer Move( mega::Pointer& ref );
        mega::Pointer Get( mega::Pointer& ref );
    };
};

}

#endif //GUARD_2024_January_11_invocation
