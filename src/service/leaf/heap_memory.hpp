
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

#ifndef GUARD_2022_October_22_heap_memory
#define GUARD_2022_October_22_heap_memory

#include "mega/shared_memory_header.hpp"

#include "service/protocol/common/jit_types.hpp"

#include "service/network/log.hpp"

#include "jit/functions.hpp"
#include "jit/jit.hpp"

#include <unordered_map>
#include <map>
#include <memory>

namespace mega::service
{

class HeapMemory
{
    using HeapBufferPtr = std::unique_ptr< char[] >;
    using CtorMap       = std::map< TypeID, mega::runtime::HeapCtorFunction >;
    using DtorMap       = std::map< TypeID, mega::runtime::HeapDtorFunction >;
    // NOTE: once inserted the function pointer value must remain at same address
    using HeapMap = std::map< reference, HeapBufferPtr >;

public:
    HeapMemory( U8 thisProcessID )
        : m_thisProcessID( thisProcessID )
    {
    }
    void allocate( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit, void* pSharedMemoryBuffer,
                   reference& ref )
    {
        const network::SizeAlignment size = jit.getSize( ref.type );
        if( size.heap_size )
        {
            SPDLOG_TRACE( "HeapMemory allocate size: {} alignment: {} at: {} in ref: {}", size.heap_size,
                          size.heap_alignment, pSharedMemoryBuffer, ref );

            HeapBufferPtr pHeapBuffer;
            pHeapBuffer.reset( new( std::align_val_t( size.heap_alignment ) ) char[ size.heap_size ] );

            setHeap( m_thisProcessID, getSharedHeader( pSharedMemoryBuffer ), pHeapBuffer.get() );
            getCtor( compiler, jit, ref.type )( pHeapBuffer.get() );
            m_heapBuffers.insert( { ref, std::move( pHeapBuffer ) } );
        }
    }

    void ensureAllocated( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                          void* pSharedMemoryBuffer, reference& ref )
    {
        ASSERT( ref.isMachine() );
        auto iFind = m_heapBuffers.find( ref );
        if( iFind == m_heapBuffers.end() )
        {
            allocate( compiler, jit, pSharedMemoryBuffer, ref );
        }
    }

    void ensureAllocated( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                          network::MemoryBaseReference& ref )
    {
        ASSERT( ref.machineRef.isMachine() );
        auto iFind = m_heapBuffers.find( ref.machineRef );
        if( iFind == m_heapBuffers.end() )
        {
            allocate(
                compiler, jit, fromProcessAddress( ref.getBaseAddress(), ref.machineRef.pointer ), ref.machineRef );
        }
    }

    // NOTE: free DOES NOT reset the heap pointer!
    void free( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit, reference& ref )
    {
        auto iFind = m_heapBuffers.find( ref );
        if( iFind != m_heapBuffers.end() )
        {
            getDtor( compiler, jit, ref.type )( iFind->second.get() );
            m_heapBuffers.erase( iFind );
        }
    }

    void freeAll( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit, MPO mpo )
    {
        auto i = m_heapBuffers.lower_bound( reference( TypeInstance( 0, 0 ), mpo, std::numeric_limits< U64 >::min() ) );
        auto iEnd = m_heapBuffers.lower_bound(
            reference( TypeInstance( 0, 0 ), MPO( mpo.getMachineID(), mpo.getProcessID(), mpo.getOwnerID() + 1 ),
                       std::numeric_limits< U64 >::max() ) );
        for( ; i != iEnd; ++i )
        {
            getDtor( compiler, jit, i->first.type )( i->second.get() );
        }
        m_heapBuffers.erase( i, iEnd );
    }

private:
    mega::runtime::HeapCtorFunction getCtor( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                                             TypeID typeID )
    {
        auto iFind = m_constructors.find( typeID );
        if( iFind == m_constructors.end() )
        {
            iFind = m_constructors.insert( { typeID, mega::runtime::HeapCtorFunction{} } ).first;
        }
        if( iFind->second == nullptr )
        {
            jit.getObjectHeapAlloc( compiler, "leaf", typeID, &iFind->second );
        }
        return iFind->second;
    }
    mega::runtime::HeapDtorFunction getDtor( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                                             TypeID typeID )
    {
        auto iFind = m_destructors.find( typeID );
        if( iFind == m_destructors.end() )
        {
            iFind = m_destructors.insert( { typeID, mega::runtime::HeapDtorFunction{} } ).first;
        }
        if( iFind->second == nullptr )
        {
            jit.getObjectHeapDel( compiler, "leaf", typeID, &iFind->second );
        }
        return iFind->second;
    }

private:
    U8      m_thisProcessID;
    CtorMap m_constructors;
    DtorMap m_destructors;
    HeapMap m_heapBuffers;
};

} // namespace mega::service

#endif // GUARD_2022_October_22_heap_memory
