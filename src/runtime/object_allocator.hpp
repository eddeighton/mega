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

#ifndef OBJECT_ALLOCATOR_SEPT_19_2022
#define OBJECT_ALLOCATOR_SEPT_19_2022

#include "runtime/functions.hpp"

#include "indexed_buffer_allocator.hpp"

#include <memory>
#include <unordered_map>

namespace mega::runtime
{

class Runtime;

class ObjectTypeAllocator : public Allocator
{
public:
    using Ptr         = std::shared_ptr< ObjectTypeAllocator >;
    using IndexPtr    = std::shared_ptr< IndexedBufferAllocator >;
    using IndexPtrMap = std::unordered_map< MPO, IndexPtr, MPO::Hash >;

    ObjectTypeAllocator( Runtime& runtime, TypeID objectTypeID );

    IndexPtr  getIndex( MPO mpo );
    reference get( MachineAddress machineAddress );
    reference allocate( MPO mpo );
    void      deAllocate( MachineAddress machineAddress );

    // Allocator
    virtual void* get_shared( mega::MachineAddress machineAddress );
    virtual void* get_heap( mega::MachineAddress machineAddress );

    GetHeapFunction   getHeapGetter();
    GetSharedFunction getSharedGetter();

private:
    Runtime&     m_runtime;
    mega::TypeID m_objectTypeID;
    mega::U64    m_szSizeShared, m_szSizeHeap;
    mega::U64    m_szAlignShared, m_szAlignHeap;
    IndexPtrMap  m_indexTable;

    SetAllocatorFunction m_pSetAllocator = nullptr;
    SharedCtorFunction   m_pSharedCtor   = nullptr;
    SharedDtorFunction   m_pSharedDtor   = nullptr;
    HeapCtorFunction     m_pHeapCtor     = nullptr;
    HeapDtorFunction     m_pHeapDtor     = nullptr;
    GetHeapFunction      m_pHeapGetter   = nullptr;
    GetSharedFunction    m_pSharedGetter = nullptr;
};

} // namespace mega::runtime

#endif // OBJECT_ALLOCATOR_SEPT_19_2022
