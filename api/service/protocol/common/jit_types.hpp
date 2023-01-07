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
#include "mega/reference.hpp"
#include "mega/snapshot.hpp"

#include <optional>

namespace mega::network
{

template < typename TFunctionPtrType >
inline TFunctionPtrType* type_reify( U64 erasedTypePtr )
{
    return reinterpret_cast< TFunctionPtrType* >( erasedTypePtr );
}

template < typename TFunctionPtrType >
inline U64 type_erase( TFunctionPtrType* ppFunction )
{
    return reinterpret_cast< U64 >( ppFunction );
}

struct MemoryBaseReference
{
    reference                 machineRef;
    std::optional< Snapshot > snapshotOpt;

    MemoryBaseReference() = default;
    MemoryBaseReference( const reference& machine )
        : machineRef( machine )
    {
    }
    
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        THROW_RTE( "Unexpected serialisation of MemoryBaseReference" );
        archive& machineRef;
        archive& snapshotOpt;
    }
};

struct SizeAlignment
{
    U64 size      = 0U;
    U64 alignment = 0U;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& size;
        archive& alignment;
    }
};

} // namespace mega::network

#endif // JIT_TYPES_21_OCT_2022
