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

namespace mega::runtime
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
            if ( db_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
            {
                VERIFY_RTE( m_szSizeShared == 0U );
                m_szSizeShared  = pBuffer->get_size();
                m_szAlignShared = pBuffer->get_alignment();
            }
            else if ( db_cast< MemoryLayout::NonSimpleBuffer >( pBuffer ) )
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
            symbolPrefix( "shared_ctor_", objectTypeID, os );
            os << "PvS_";
            m_pSharedCtor = pModule->get< SharedCtorFunction >( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "shared_dtor_", objectTypeID, os );
            os << "Pv";
            m_pSharedDtor = pModule->get< SharedDtorFunction >( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "heap_ctor_", objectTypeID, os );
            os << "Pv";
            m_pHeapCtor = pModule->get< HeapCtorFunction >( os.str() );
        }
        {
            std::ostringstream os;
            symbolPrefix( "heap_dtor_", objectTypeID, os );
            os << "Pv";
            m_pHeapDtor = pModule->get< HeapDtorFunction >( os.str() );
        }
    }
    
}


} // namespace mega::runtime
