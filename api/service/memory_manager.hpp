
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

#ifndef GUARD_2023_January_07_memory_manager
#define GUARD_2023_January_07_memory_manager

#include "jit/allocator.hpp"
#include "jit/object_header.hpp"

#include <functional>
#include <memory>

namespace mega::runtime
{

class MemoryManager
{
    using HeapBufferPtr = std::unique_ptr< char[] >;
    using HeapMap       = std::map< reference, HeapBufferPtr >;

public:
    using GetAllocatorFPtr = std::function< Allocator::Ptr( TypeID ) >;

    template < typename TFunctor >
    MemoryManager( MPO mpo, TFunctor&& getAllocatorFunction )
        : m_mpo( mpo )
        , m_getAllocatorFPtr( std::move( getAllocatorFunction ) )
    {
    }

    reference New( TypeID typeID )
    {
        Allocator::Ptr pAllocator = m_getAllocatorFPtr( typeID );

        const network::SizeAlignment sizeAlignment = pAllocator->getSizeAlignment();

        HeapBufferPtr pHeapBuffer( new( std::align_val_t( sizeAlignment.alignment ) ) char[ sizeAlignment.size ] );

        // establish the header including the network address, lock timestamp and shared ownership of allocator
        {
            static ObjectID objectID = 0U;
            TimeStamp lockTime = 0U;
            reference networkAddress{ TypeInstance{ 0, typeID }, m_mpo, ++objectID };
            new( pHeapBuffer.get() ) ObjectHeader{ networkAddress, lockTime, pAllocator };
        }

        // invoke the constructor
        pAllocator->getCtor()( pHeapBuffer.get() );

        reference heapAddress{ TypeInstance( 0, typeID ), m_mpo.getOwnerID(), pHeapBuffer.get() };

        m_buffers.insert( { heapAddress, std::move( pHeapBuffer ) } );

        return heapAddress;
    }

    void Delete( reference& ref ) 
    {
        auto iFind = m_buffers.find( ref );
        VERIFY_RTE_MSG( iFind != m_buffers.end(), "Failed to locate reference heap buffer: " << ref );

        auto pHeader = reinterpret_cast< ObjectHeader* >( iFind->second.get() );
        pHeader->m_pAllocator->getDtor()( pHeader );
        delete pHeader;
        m_buffers.erase( iFind );
        ref = reference{ ref.getTypeInstance(), ref.getOwnerID(), NULL_ADDRESS };
    }


private:
    MPO              m_mpo;
    GetAllocatorFPtr m_getAllocatorFPtr;
    HeapMap          m_buffers;
};

} // namespace mega::runtime

#endif // GUARD_2023_January_07_memory_manager
