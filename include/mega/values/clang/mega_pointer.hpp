
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

#include "mega/values/runtime/pointer.hpp"

#include "mega/values/clang/result_type.hpp"
#include "mega/values/clang/attribute_id.hpp"

template< typename Context, typename TypePath, typename Operation, typename... Args >
static void invoke_impl_void( Context& context, Args... args );

template< typename ResultType, typename Context, typename TypePath, typename Operation, typename... Args >
static ResultType invoke_impl( Context& context, Args... args );

template< mega::interface::TypeID::ValueType... InterfaceTypeID >
class [[clang::eg_type( mega::id_MegaPointer )]] __mega_ptr : public mega::runtime::Pointer
{
public: 
    template< typename TypePath, typename Operation, typename... Args >
    inline typename mega::result_type< __mega_ptr< InterfaceTypeID... >, TypePath, Operation >::Type __mega_invoke( Args... args ) const
    {
        using ThisType = __mega_ptr< InterfaceTypeID... >;
        using ResultType = typename mega::result_type< ThisType, TypePath, Operation >::Type;
        
        if constexpr ( std::is_same< ResultType, void >::value )
        {
            invoke_impl_void< ThisType, TypePath, Operation, Args... >( const_cast< ThisType& >( *this ), args... );
        }

        if constexpr ( !std::is_same< ResultType, void >::value )
        {
            return invoke_impl< ResultType, ThisType, TypePath, Operation, Args... >( const_cast< ThisType& >( *this ), args... );
        }
    }
};

#endif //GUARD_2023_December_26_typed_pointer
