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

#ifndef RUNTIME_FUNCTIONS_17_AUG_2022
#define RUNTIME_FUNCTIONS_17_AUG_2022

#include "api.hpp"
#include "mega/reference.hpp"

namespace mega
{
namespace runtime
{

using TypeErasedFunction = const void*;

// full program functions
using LoadRecordFunction = void ( * )( reference, const void* );
using SaveRecordFunction = void ( * )( reference, const void* );
using LoadObjectRecordFunction = void ( * )( reference, const void* );

// allocator functions
using CtorFunction             = void ( * )( void* );
using DtorFunction             = void ( * )( void* );
using SaveObjectFunction       = void ( * )( void*, void* );
using LoadObjectFunction       = void ( * )( void*, void* );

// invocation functions
using AllocateFunction = reference ( * )( reference );
using ReadFunction     = void* ( * )( reference );
using WriteFunction    = reference ( * )( reference, const void* );
using StartFunction    = reference ( * )( reference );
using StopFunction     = reference ( * )( reference );
using SaveFunction     = void ( * )( reference, void* );
using LoadFunction     = void ( * )( reference, void* );

struct JIT_EXPORT CallResult
{
    TypeErasedFunction pFunction;
    reference          context;
};
using CallFunction = CallResult ( * )( reference );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_FUNCTIONS_17_AUG_2022
