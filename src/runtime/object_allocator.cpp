
#include "object_allocator.hpp"

#include "runtime.hpp"
#include "database.hpp"
#include "symbol_utils.hpp"

namespace mega
{
namespace runtime
{

ObjectTypeAllocator::ObjectTypeAllocator( Runtime& runtime, TypeID objectTypeID )
    : m_runtime( runtime )
    , m_objectTypeID( objectTypeID )
    , m_szSizeShared( 0U )
    , m_szSizeHeap( 0U )
{
    const FinalStage::Concrete::Object* pObject = m_runtime.m_database.getObject( m_objectTypeID );
    using namespace FinalStage;
    for ( auto pBuffer : pObject->get_buffers() )
    {
        if ( dynamic_database_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
        {
            VERIFY_RTE( m_szSizeShared == 0U );
            m_szSizeShared = pBuffer->get_size();
        }
        else if ( dynamic_database_cast< MemoryLayout::NonSimpleBuffer >( pBuffer ) )
        {
            VERIFY_RTE( m_szSizeHeap == 0U );
            m_szSizeHeap = pBuffer->get_size();
        }
        else
        {
            THROW_RTE( "Unsupported buffer type" );
        }
    }

    JITCompiler::Module::Ptr pModule = runtime.get_allocation( m_objectTypeID );
    {
        {
            std::ostringstream os;
            symbolPrefix( "set_allocator_", objectTypeID, os );
            os << "PN4mega7runtime9AllocatorE";
            m_pSetAllocator = pModule->getSetAllocator( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "get_heap_", objectTypeID, os );
            os << "N4mega14MachineAddressE";
            m_pHeapGetter = pModule->getGetHeap( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "get_shared_", objectTypeID, os );
            os << "N4mega14MachineAddressE";
            m_pSharedGetter = pModule->getGetShared( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "shared_ctor_", objectTypeID, os );
            os << "Pv";
            m_pSharedCtor = pModule->getSharedCtor( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "shared_dtor_", objectTypeID, os );
            os << "Pv";
            m_pSharedDtor = pModule->getSharedDtor( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "heap_ctor_", objectTypeID, os );
            os << "Pv";
            m_pHeapCtor = pModule->getHeapCtor( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "heap_dtor_", objectTypeID, os );
            os << "Pv";
            m_pHeapDtor = pModule->getHeapDtor( os.str() );
        }
    }
    m_pSetAllocator( this );
}

ObjectTypeAllocator::IndexPtr ObjectTypeAllocator::getIndex( MPO mpo )
{
    IndexPtr pAllocator;
    {
        auto iFind = m_index.find( mpo );
        if ( iFind != m_index.end() )
        {
            pAllocator = iFind->second;
        }
        else
        {
            mega::runtime::ManagedSharedMemory& managedSharedMemory = m_runtime.getSharedMemoryManager( mpo );
            pAllocator = std::make_shared< IndexedBufferAllocator >( managedSharedMemory.get_segment_manager() );
            m_index.insert( { mpo, pAllocator } );
        }
    }
    return pAllocator;
}

reference ObjectTypeAllocator::get( MachineAddress machineAddress )
{
    VERIFY_RTE( machineAddress.isMachine() );
    void* pShared = getIndex( machineAddress )->getShared( machineAddress.object ).get();
    return reference( TypeInstance( 0, m_objectTypeID ), machineAddress, pShared );
}

reference ObjectTypeAllocator::allocate( MPO mpo )
{
    VERIFY_RTE( mpo.isMachine() );
    const IndexedBufferAllocator::AllocationResult result = getIndex( mpo )->allocate( m_szSizeShared, m_szSizeHeap );
    m_pSharedCtor( result.pShared );
    m_pHeapCtor( result.pHeap );
    return reference( TypeInstance( 0, m_objectTypeID ), MachineAddress{ result.object, mpo }, result.pShared );
}

void ObjectTypeAllocator::deAllocate( MachineAddress machineAddress )
{
    auto pIndex = getIndex( machineAddress );
    m_pSharedDtor( pIndex->getShared( machineAddress.object ).get() );
    m_pHeapDtor( pIndex->getHeap( machineAddress.object ) );
    pIndex->deallocate( machineAddress.object );
}

// Allocator
void* ObjectTypeAllocator::get_shared( mega::MachineAddress machineAddress )
{
    auto pIndex = getIndex( machineAddress );
    return pIndex->getShared( machineAddress.object ).get();
}

void* ObjectTypeAllocator::get_heap( mega::MachineAddress machineAddress )
{
    auto pIndex = getIndex( machineAddress );
    return pIndex->getHeap( machineAddress.object );
}

GetHeapFunction   ObjectTypeAllocator::getHeapGetter() { return m_pHeapGetter; }
GetSharedFunction ObjectTypeAllocator::getSharedGetter() { return m_pSharedGetter; }

} // namespace runtime
} // namespace mega