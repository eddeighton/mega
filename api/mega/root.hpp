
#ifndef ROOT_22_AUG_2022
#define ROOT_22_AUG_2022

#include "common.hpp"
//#include "result_type.hpp"

#include <type_traits>

template< typename Context, typename TypePath, typename Operation, typename... Args >
static void invoke_impl_void( Context context, Args... args );

template< typename ResultType, typename Context, typename TypePath, typename Operation, typename... Args >
static ResultType invoke_impl( Context context, Args... args );

#endif //ROOT_22_AUG_2022
