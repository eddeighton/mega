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

#ifndef SHARED_MEMORY_21_OCT_2022
#define SHARED_MEMORY_21_OCT_2022

#include "mega/reference.hpp"
#include "mega/default_traits.hpp"

#include "jit/functions.hpp"
#include "jit/jit.hpp"

#include "common/assert_verify.hpp"

#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <map>
#include <optional>

namespace mega::service
{

class SharedMemoryAccess
{
    class SharedMemory
    {
    public:
        using Ptr = std::unique_ptr< SharedMemory >;

        SharedMemory( const mega::MPO& mpo, const std::string& strName )
            : m_memory( boost::interprocess::open_only, strName.c_str() )
        {
        }

        runtime::ManagedSharedMemory& get() { return m_memory; }

    private:
        runtime::ManagedSharedMemory m_memory;
    };

    using SharedMemoryMap = std::unordered_map< mega::MPO, SharedMemory::Ptr, mega::MPO::Hash >;
    using SharedBufferSet = std::set< mega::reference >;

    using CtorMap = std::map< TypeID, mega::runtime::SharedCtorFunction >;
    using DtorMap = std::map< TypeID, mega::runtime::SharedDtorFunction >;

public:
    using MemoryAccessor = std::function< std::string( mega::MPO ) >;

    runtime::ManagedSharedMemory& get( const MPO& mpo, const MemoryAccessor& memoryAccess )
    {
        auto iFind = m_memory.find( mpo );
        if ( iFind == m_memory.end() )
        {
            const std::string             strMemory  = memoryAccess( mpo );
            SharedMemory::Ptr             pNewMemory = std::make_unique< SharedMemory >( mpo, strMemory );
            runtime::ManagedSharedMemory& memory     = pNewMemory->get();
            m_memory.insert( { mpo, std::move( pNewMemory ) } );
            return memory;
        }
        else
        {
            return iFind->second->get();
        }
    }

    void* construct( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit, const reference& ref,
                    const MemoryAccessor& memoryAccess )
    {
        runtime::ManagedSharedMemory& sharedMemory = get( ref, memoryAccess );
        void* pSharedMemoryBuffer                  = fromProcessAddress( sharedMemory.get_address(), ref.pointer );
        getCtor( compiler, jit, ref.type )( pSharedMemoryBuffer, sharedMemory.get_segment_manager() );
        m_buffers.insert( ref );
        return pSharedMemoryBuffer;
    }

    network::MemoryBaseReference getOrConstruct( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                                                const reference& ref, const MemoryAccessor& memoryAccess )
    {
        auto iFind = m_buffers.find( ref );
        if ( iFind == m_buffers.end() )
        {
            void* pSharedBuffer = construct( compiler, jit, ref, memoryAccess );
            void* pMemoryBase   = get( ref, memoryAccess ).get_address();
            return { pMemoryBase, reference( ref, ref, toProcessAddress( pMemoryBase, pSharedBuffer ) ) };
        }
        else
        {
            return { get( ref, memoryAccess ).get_address(), ref };
        }
    }

    void free( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit, const reference& ref,
               const MemoryAccessor& memoryAccess )
    {
        runtime::ManagedSharedMemory& sharedMemory = get( ref, memoryAccess );
        void* pSharedMemoryBuffer                  = fromProcessAddress( sharedMemory.get_address(), ref.pointer );
        getDtor( compiler, jit, ref.type )( pSharedMemoryBuffer );
        m_buffers.erase( ref );
    }

    void freeAll( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit, MPO mpo,
                  const MemoryAccessor& memoryAccess )
    {
        auto i    = m_buffers.lower_bound( reference( TypeInstance( 0, 0 ), mpo ) );
        auto iEnd = m_buffers.lower_bound(
            reference( TypeInstance( 0, 0 ), MPO( mpo.getMachineID(), mpo.getProcessID(), mpo.getOwnerID() + 1 ) ) );
        for ( ; i != iEnd; ++i )
        {
            runtime::ManagedSharedMemory& sharedMemory = get( *i, memoryAccess );
            void* pSharedMemoryBuffer                  = fromProcessAddress( sharedMemory.get_address(), i->pointer );
            getDtor( compiler, jit, i->type )( pSharedMemoryBuffer );
        }
        m_buffers.erase( i, iEnd );
        m_memory.erase( mpo );
    }

private:
    mega::runtime::SharedCtorFunction getCtor( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                                               TypeID typeID )
    {
        auto iFind = m_constructors.find( typeID );
        if ( iFind == m_constructors.end() )
        {
            iFind = m_constructors.insert( { typeID, mega::runtime::SharedCtorFunction{} } ).first;
        }
        if ( iFind->second == nullptr )
        {
            jit.getObjectSharedAlloc( compiler, "leaf", typeID, &iFind->second );
        }
        return iFind->second;
    }
    mega::runtime::SharedDtorFunction getDtor( const runtime::CodeGenerator::LLVMCompiler& compiler, runtime::JIT& jit,
                                               TypeID typeID )
    {
        auto iFind = m_destructors.find( typeID );
        if ( iFind == m_destructors.end() )
        {
            iFind = m_destructors.insert( { typeID, mega::runtime::SharedDtorFunction{} } ).first;
        }
        if ( iFind->second == nullptr )
        {
            jit.getObjectSharedDel( compiler, "leaf", typeID, &iFind->second );
        }
        return iFind->second;
    }

private:
    SharedMemoryMap m_memory;
    CtorMap         m_constructors;
    DtorMap         m_destructors;
    SharedBufferSet m_buffers;
};

} // namespace mega::service

#endif // SHARED_MEMORY_21_OCT_2022
