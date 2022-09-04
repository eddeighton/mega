
#ifndef ROOT_22_AUG_2022
#define ROOT_22_AUG_2022

#include "common.hpp"
//#include "result_type.hpp"

#include <type_traits>

template< typename Context, typename TypePath, typename Operation, typename... Args >
static void invoke_impl_void( Context context, Args... args );

template< typename ResultType, typename Context, typename TypePath, typename Operation, typename... Args >
static ResultType invoke_impl( Context context, Args... args );

// warning: ‘clang::eg_type’ scoped attribute directive ignored [-Wattributes]
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
class [[clang::eg_type( mega::ROOT_TYPE_ID )]]Root
{
public:
    enum : mega::TypeID
    {
        ID = mega::ROOT_TYPE_ID
    };
    mega::reference data;
    inline Root()
    {
        data.value = mega::INVALID_ADDRESS;
    }
    inline Root( const mega::reference& reference )
        :   data( reference )
    {
    }
/*
    template< typename TypePath, typename Operation, typename... Args >
    typename mega::result_type< Root, TypePath, Operation >::Type invoke( Args... args ) const
    {
        using ResultType = typename mega::result_type< Root, TypePath, Operation >::Type;
        if constexpr ( std::is_same< ResultType, void >::value )
        {
            invoke_impl_void< Root, TypePath, Operation, Args... >( *this, args... );
        }
        if constexpr ( !std::is_same< ResultType, void >::value )
        {
            return invoke_impl< ResultType, Root, TypePath, Operation, Args... >( *this, args... );
        }
    }
    */

};



#pragma GCC diagnostic pop

#endif //ROOT_22_AUG_2022
