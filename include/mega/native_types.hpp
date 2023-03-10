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




#ifndef MEGA_NATIVE_TYPES_18_SEPT_2022
#define MEGA_NATIVE_TYPES_18_SEPT_2022

namespace mega
{

using U8 = unsigned char;
static_assert( sizeof( U8 ) == 1U, "Invalid U8 Size" );

using I8 = signed char;
static_assert( sizeof( I8 ) == 1U, "Invalid I8 Size" );

using U16 = unsigned short;
static_assert( sizeof( U16 ) == 2U, "Invalid U16 Size" );

using I16 = signed short;
static_assert( sizeof( I16 ) == 2U, "Invalid I16 Size" );

using U32 = unsigned int;
static_assert( sizeof( U32 ) == 4U, "Invalid U32 Size" );

using I32 = signed int;
static_assert( sizeof( I32 ) == 4U, "Invalid I32 Size" );

#ifdef _WIN64
using U64 = unsigned long long;
#else
using U64 = unsigned long int;
#endif
static_assert( sizeof( U64 ) == 8U, "Invalid U64 Size" );

#ifdef _WIN64
using I64 = signed long long;
#else
using I64 = signed long int;
#endif
static_assert( sizeof( I64 ) == 8U, "Invalid I64 Size" );

using F32 = float;
static_assert( sizeof( F32 ) == 4U, "Invalid F32 Size" );

using F64 = double;
static_assert( sizeof( F64 ) == 8U, "Invalid F64 Size" );

using TimeStamp      = U32;

} // namespace mega

#endif // MEGA_NATIVE_TYPES_18_SEPT_2022
