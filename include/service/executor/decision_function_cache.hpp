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

#ifndef GUARD_2023_November_10_decision_function_cache
#define GUARD_2023_November_10_decision_function_cache

#include "service/protocol/common/context.hpp"
#include "service/protocol/common/jit_base.hpp"

#include "mega/coroutine.hpp"

#include <unordered_map>

namespace mega::service
{

class DecisionFunctionCache
{
    using DecisionFunctionPtr = void ( * )( const mega::reference* );
    using DecisionFunctionMap = std::unordered_map< TypeID, DecisionFunctionPtr, TypeID::Hash >;

public:
    const DecisionFunctionPtr& getDecisionFunction( TypeID concreteTypeID )
    {
        auto iter = m_decisionFunctions.find( concreteTypeID );
        if( iter != m_decisionFunctions.end() )
        {
            if( iter->second != nullptr )
            {
                return iter->second;
            }
        }
        else
        {
            iter = m_decisionFunctions.insert( { concreteTypeID, nullptr } ).first;
        }

        {
            mega::runtime::JITFunctor functor( [ concreteTypeID, &iter ]( mega::runtime::JITBase& jit, void* pLLVMCompiler )
                                               { jit.getDecisionFunction( pLLVMCompiler, concreteTypeID, ( void** )&iter->second ); } );
            mega::Context::get()->jit( functor );
        }

        return iter->second;
    }

private:
    DecisionFunctionMap m_decisionFunctions;
};

} // namespace mega::service

#endif // GUARD_2023_November_10_decision_function_cache
