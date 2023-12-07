
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

#ifndef GUARD_2023_February_04_scheduler
#define GUARD_2023_February_04_scheduler

#include "service/protocol/common/context.hpp"
#include "runtime/function_provider.hpp"

#include "mega/coroutine.hpp"

#include <unordered_map>

namespace mega::service
{
/*
class ActionFunctionCache
{
    using ActionFunctionPtr = mega::ActionCoroutine ( * )( mega::reference* );
    using ActionFunctionMap = std::unordered_map< TypeID, ActionFunctionPtr, TypeID::Hash >;

public:
    const ActionFunctionPtr& getActionFunction( TypeID typeID )
    {
        auto iter = m_actionFunctions.find( typeID );
        if( iter != m_actionFunctions.end() )
        {
            if( iter->second != nullptr )
            {
                return iter->second;
            }
        }
        else
        {
            iter = m_actionFunctions.insert( { typeID, nullptr } ).first;
        }

        {
            mega::runtime::RuntimeFunctor functor( [ typeID, &iter ]( mega::runtime::FunctionProvider& jit, void* )
                                               { jit.getActionFunction( typeID, ( void** )&iter->second ); } );
            // NOTE: call to jit MAY return with THIS coroutine resumed in DIFFERENT thread!
            mega::Context::get()->jit( functor );
        }

        return iter->second;
    }

private:
    ActionFunctionMap m_actionFunctions;
};
*/
} // namespace mega::service

#endif // GUARD_2023_February_04_scheduler
