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

#include "service/mpo_context.hpp"

namespace mega::runtime
{

MPO getThisMPO()
{
    return getMPOContext()->getThisMPO();
}
void* base()
{
    return getMPOContext()->base();
}
void* read( reference& ref )
{
    return getMPOContext()->read( ref );
}
void* write( reference& ref )
{
    return getMPOContext()->write( ref );
}

#define FUNCTION_ARG_0( return_type, name ) \
    return_type name()                      \
    {                                       \
        return getMPOContext()->name();     \
    }

#define FUNCTION_ARG_1( return_type, name, arg1_type, arg1_name ) \
    return_type name( arg1_type arg1_name )                       \
    {                                                             \
        return getMPOContext()->name( arg1_name );                \
    }

#define FUNCTION_ARG_2( return_type, name, arg1_type, arg1_name, arg2_type, arg2_name ) \
    return_type name( arg1_type arg1_name, arg2_type arg2_name )                        \
    {                                                                                   \
        return getMPOContext()->name( arg1_name, arg2_name );                           \
    }

#define FUNCTION_ARG_3( return_type, name, arg1_type, arg1_name, arg2_type, arg2_name, arg3_type, arg3_name ) \
    return_type name( arg1_type arg1_name, arg2_type arg2_name, arg3_type arg3_name )                         \
    {                                                                                                         \
        return getMPOContext()->name( arg1_name, arg2_name, arg3_name );                                      \
    }

#include "service/jit_interface.hxx"

#undef FUNCTION_ARG_0
#undef FUNCTION_ARG_1
#undef FUNCTION_ARG_2
#undef FUNCTION_ARG_3

} // namespace mega::runtime
