
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

#ifndef GUARD_2023_December_06_function_cache
#define GUARD_2023_December_06_function_cache

#include "runtime/function_provider.hpp"

#include "common/assert_verify.hpp"

namespace mega::runtime
{

template < typename TIDType, typename TFunctionType >
class FunctionCache
{
    struct IDTypeHash
    {
        std::size_t operator()( const TIDType& value ) const { return 0U; }
    };
    using FunctionPtr = TFunctionType;
    using FunctionMap = std::unordered_map< TIDType, FunctionPtr, IDTypeHash >;

public:
    const FunctionPtr& get( TIDType functionID )
    {
        THROW_TODO;
        /*auto iter = m_functions.find( functionID );
        if( iter != m_functions.end() )
        {
            if( iter->second != nullptr )
            {
                return iter->second;
            }
        }
        else
        {
            iter = m_functions.insert( { functionID, nullptr } ).first;
        }

        {
            mega::runtime::RuntimeFunctor functor( [ functionID, &iter ]( mega::runtime::FunctionProvider& jit, void*
        pLLVMCompiler ) { jit.getFunction( pStashProvider, functionID, ( void** )&iter->second ); } );
            mega::Context::get()->jit( functor );
        }

        return iter->second;*/
    }

private:
    FunctionMap m_functions;
};

} // namespace mega::runtime

#endif // GUARD_2023_December_06_function_cache
