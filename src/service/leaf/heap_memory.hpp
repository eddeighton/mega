
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

#include "database/types/shared_memory_header.hpp"

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
    using HeapMap = std::unordered_map< reference, HeapBufferPtr, reference::Hash >;

public:
    void allocate( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                   const network::SizeAlignment& size, void* pSharedMemoryBuffer, reference& ref, U8 processID )
    {
        if ( size.heap_size )
        {
            SPDLOG_TRACE( "HeapMemory allocate size: {} alignment: {} at: {} in ref: {} processID: {}", size.heap_size,
                          size.heap_alignment, pSharedMemoryBuffer, ref, processID );

            HeapBufferPtr pHeapBuffer;
            pHeapBuffer.reset( new ( std::align_val_t( size.heap_alignment ) ) char[ size.heap_size ] );

            setHeap( processID, getSharedHeader( pSharedMemoryBuffer ), pHeapBuffer.get() );
            getCtor( compiler, jit, ref.type )( pHeapBuffer.get() );
            m_heapBuffers.insert( { ref, std::move( pHeapBuffer ) } );
        }
    }

    // NOTE: free DOES NOT reset the heap pointer!
    void free( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit, reference& ref )
    {
        auto iFind = m_heapBuffers.find( ref );
        if ( iFind != m_heapBuffers.end() )
        {
            getDtor( compiler, jit, ref.type )( iFind->second.get() );
            m_heapBuffers.erase( iFind );
        }
    }

private:
    mega::runtime::HeapCtorFunction getCtor( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                                             TypeID typeID )
    {
        auto iFind = m_constructors.find( typeID );
        if ( iFind == m_constructors.end() )
        {
            iFind = m_constructors.insert( { typeID, mega::runtime::HeapCtorFunction{} } ).first;
        }
        if ( iFind->second == nullptr )
        {
            jit.getObjectHeapAlloc( compiler, "leaf", typeID, &iFind->second );
        }
        return iFind->second;
    }
    mega::runtime::HeapDtorFunction getDtor( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                                             TypeID typeID )
    {
        auto iFind = m_destructors.find( typeID );
        if ( iFind == m_destructors.end() )
        {
            iFind = m_destructors.insert( { typeID, mega::runtime::HeapDtorFunction{} } ).first;
        }
        if ( iFind->second == nullptr )
        {
            jit.getObjectHeapDel( compiler, "leaf", typeID, &iFind->second );
        }
        return iFind->second;
    }

private:
    CtorMap m_constructors;
    DtorMap m_destructors;
    HeapMap m_heapBuffers;
};

} // namespace mega::service

#endif // GUARD_2022_October_22_heap_memory
