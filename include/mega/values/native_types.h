
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

#ifndef GUARD_2023_December_07_native_types_h
#define GUARD_2023_December_07_native_types_h

#include "mega/values/runtime.h"

C_RUNTIME_START

typedef void c_void;
typedef char c_char;
typedef unsigned char c_bool;

typedef unsigned char  c_u8;
typedef unsigned short c_u16;
typedef unsigned int   c_u32;
typedef signed char    c_i8;
typedef signed short   c_i16;
typedef signed int     c_i32;
typedef float          c_f32;
typedef double         c_f64;

#ifdef _WIN64
typedef unsigned long long c_u64;
typedef signed long long   c_i64;
#else
typedef unsigned long int c_u64;
typedef signed long int   c_i64;
#endif

C_RUNTIME_END

#endif // GUARD_2023_December_07_native_types_h
