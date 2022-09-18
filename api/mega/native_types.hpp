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

using U64 = unsigned long int;
static_assert( sizeof( U64 ) == 8U, "Invalid U64 Size" );

using I64 = signed long int;
static_assert( sizeof( I64 ) == 8U, "Invalid I64 Size" );

using F32 = float;
static_assert( sizeof( F32 ) == 4U, "Invalid F32 Size" );

using F64 = double;
static_assert( sizeof( F64 ) == 8U, "Invalid F64 Size" );

} // namespace mega

#endif // MEGA_NATIVE_TYPES_18_SEPT_2022
