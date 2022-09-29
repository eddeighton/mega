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


#include "object_allocator.hpp"

#include "runtime.hpp"
#include "database.hpp"
#include "symbol_utils.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace runtime
{

ObjectTypeAllocator::ObjectTypeAllocator( Runtime& runtime, TypeID objectTypeID )
    : m_runtime( runtime )
    , m_objectTypeID( objectTypeID )
    , m_szSizeShared( 0U )
    , m_szSizeHeap( 0U )
    , m_szAlignShared( 1U )
    , m_szAlignHeap( 1U )
{
    SPDLOG_TRACE( "ObjectTypeAllocator::ctor for {}", m_objectTypeID );
    {
        using namespace FinalStage;
        const Concrete::Object* pObject = m_runtime.m_database.getObject( m_objectTypeID );
        for ( auto pBuffer : pObject->get_buffers() )
        {
            if ( dynamic_database_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
            {
                VERIFY_RTE( m_szSizeShared == 0U );
                m_szSizeShared  = pBuffer->get_size();
                m_szAlignShared = pBuffer->get_alignment();
            }
            else if ( dynamic_database_cast< MemoryLayout::NonSimpleBuffer >( pBuffer ) )
            {
                VERIFY_RTE( m_szSizeHeap == 0U );
                m_szSizeHeap  = pBuffer->get_size();
                m_szAlignHeap = pBuffer->get_alignment();
            }
            else
            {
                THROW_RTE( "Unsupported buffer type" );
            }
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
            os << "PvS_";
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
        // TODO - this needs thread safety
        auto iFind = m_indexTable.find( mpo );
        if ( iFind != m_indexTable.end() )
        {
            //SPDLOG_TRACE( "ObjectTypeAllocator::getIndex cached {} {}", m_objectTypeID, mpo );
            pAllocator = iFind->second;
        }
        else
        {
            SPDLOG_TRACE( "ObjectTypeAllocator::getIndex created {} {}", m_objectTypeID, mpo );
            pAllocator
                = std::make_shared< IndexedBufferAllocator >( m_objectTypeID, m_runtime.getSharedMemoryManager( mpo ) );
            m_indexTable.insert( { mpo, pAllocator } );
        }
    }
    return pAllocator;
}

reference ObjectTypeAllocator::get( MachineAddress machineAddress )
{
    //SPDLOG_TRACE( "ObjectTypeAllocator::get {} {}", m_objectTypeID, machineAddress );
    VERIFY_RTE( machineAddress.isMachine() );
    auto  pIndex  = getIndex( machineAddress );
    void* pShared = pIndex->getShared( machineAddress.object );
    return reference( TypeInstance( 0, m_objectTypeID ), machineAddress, pShared );
}

reference ObjectTypeAllocator::allocate( MPO mpo )
{
    SPDLOG_TRACE( "ObjectTypeAllocator::allocate {} {}", m_objectTypeID, mpo );
    
    VERIFY_RTE( mpo.isMachine() );

    auto pIndex = getIndex( mpo );

    const IndexedBufferAllocator::AllocationResult result
        = pIndex->allocate( m_szSizeShared, m_szAlignShared, m_szSizeHeap, m_szAlignHeap );

    if ( result.pShared )
    {
        m_pSharedCtor( result.pShared, pIndex->getSegmentManager() );
    }
    if ( result.pHeap )
    {
        m_pHeapCtor( result.pHeap );
    }

    return reference( TypeInstance( 0, m_objectTypeID ), MachineAddress{ result.object, mpo }, result.pShared );
}

void ObjectTypeAllocator::deAllocate( MachineAddress machineAddress )
{
    SPDLOG_TRACE( "ObjectTypeAllocator::deAllocate {} {}", m_objectTypeID, machineAddress );
    
    auto pIndex = getIndex( machineAddress );
    if ( void* pAddress = pIndex->getShared( machineAddress.object ) )
    {
        m_pSharedDtor( pAddress );
    }
    if ( void* pHeap = pIndex->getHeap( machineAddress.object ) )
    {
        m_pHeapDtor( pHeap );
    }
    pIndex->deallocate( machineAddress.object );
}

// Allocator
void* ObjectTypeAllocator::get_shared( mega::MachineAddress machineAddress )
{
    auto  pIndex   = getIndex( machineAddress );
    void* pAddress = pIndex->getShared( machineAddress.object );
    //SPDLOG_TRACE( "ObjectTypeAllocator::get_shared {} {} {}", m_objectTypeID, machineAddress, pAddress );
    return pAddress;
}

void* ObjectTypeAllocator::get_heap( mega::MachineAddress machineAddress )
{
    auto  pIndex   = getIndex( machineAddress );
    void* pAddress = pIndex->getHeap( machineAddress.object );
    //SPDLOG_TRACE( "ObjectTypeAllocator::get_heap {} {} {}", m_objectTypeID, machineAddress, pAddress );
    return pAddress;
}

GetHeapFunction   ObjectTypeAllocator::getHeapGetter() { return m_pHeapGetter; }
GetSharedFunction ObjectTypeAllocator::getSharedGetter() { return m_pSharedGetter; }

} // namespace runtime
} // namespace mega