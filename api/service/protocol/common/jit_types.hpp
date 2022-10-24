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

#ifndef JIT_TYPES_21_OCT_2022
#define JIT_TYPES_21_OCT_2022

#include "mega/native_types.hpp"
#include "mega/default_traits.hpp"

namespace mega::network
{

using JITModuleName  = U64;
using JITFunctionPtr = U64;
using JITMemoryPtr   = U64;

inline const char*   convert( JITModuleName moduleName ) { return reinterpret_cast< const char* >( moduleName ); }
inline JITModuleName convert( const char* pszModuleName ) { return reinterpret_cast< JITModuleName >( pszModuleName ); }

template < typename TFunctionPtrType >
inline TFunctionPtrType* convert( U64 functionPtr )
{
    return reinterpret_cast< TFunctionPtrType* >( functionPtr );
}
template < typename TFunctionPtrType >
inline U64 convert( TFunctionPtrType* ppFunction )
{
    return reinterpret_cast< U64 >( ppFunction );
}

struct MemoryBaseReference
{
    U64       base;
    reference machineRef;

    MemoryBaseReference() = default;
    MemoryBaseReference( void* pBaseAddress, const reference& machine )
        : base( reinterpret_cast< U64 >( pBaseAddress ) )
        , machineRef( machine )
    {
    }
    void* getBaseAddress() const { return reinterpret_cast< void* >( base ); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& base;
        archive& machineRef;
    }
};

struct SizeAlignment
{
    U64 shared_size      = 0U;
    U64 shared_alignment = 0U;
    U64 heap_size        = 0U;
    U64 heap_alignment   = 0U;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& shared_size;
        archive& shared_alignment;
        archive& heap_size;
        archive& heap_alignment;
    }
};

} // namespace mega::network

#endif // JIT_TYPES_21_OCT_2022
