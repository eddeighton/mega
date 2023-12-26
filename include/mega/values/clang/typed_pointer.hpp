
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

#ifndef GUARD_2023_December_26_typed_pointer
#define GUARD_2023_December_26_typed_pointer

#include "mega/values/compilation/interface/type_id.hpp"
#include "mega/values/compilation/interface/symbol_id.hpp"

#include "mega/values/runtime/pointer.hpp"

#include "mega/values/clang/result_type.hpp"

template< mega::interface::TypeID... ContextID >
class TypedPtr : public mega::Pointer
{
public:
    template< typename TypePath, typename Operation, typename... Args >
    inline typename mega::result_type< TypedPtr< ContextID... >, TypePath, Operation >::Type invoke( Args... args ) const
    {
        using ThisType = TypedPtr< ContextID... >;
        using ResultType = typename mega::result_type< ThisType, TypePath, Operation >::Type;
        
        if constexpr ( std::is_same< ResultType, void >::value )
        {
            invoke_impl_void< ThisType, TypePath, Operation, Args... >( const_cast< ThisType& >( *this ), args... );
        }

        if constexpr ( !std::is_same< ResultType, void >::value )
        {
            return invoke_impl< ResultType,ThisType, TypePath, Operation, Args... >( const_cast< ThisType& >( *this ), args... );
        }
    }
};

#endif //GUARD_2023_December_26_typed_pointer
