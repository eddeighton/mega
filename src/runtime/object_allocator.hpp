#ifndef OBJECT_ALLOCATOR_SEPT_19_2022
#define OBJECT_ALLOCATOR_SEPT_19_2022

#include "runtime/runtime_functions.hpp"

#include "indexed_buffer_allocator.hpp"

#include <memory>
#include <unordered_map>

namespace mega
{
namespace runtime
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

} // namespace runtime
} // namespace mega

#endif // OBJECT_ALLOCATOR_SEPT_19_2022
