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

#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>

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

public:
    runtime::ManagedSharedMemory& get( const mega::MPO& mpo, const std::string& strMemory )
    {
        auto iFind = m_memory.find( mpo );
        if ( iFind != m_memory.end() )
        {
            return iFind->second->get();
        }
        else
        {
            SharedMemory::Ptr             pNewMemory = std::make_unique< SharedMemory >( mpo, strMemory );
            runtime::ManagedSharedMemory& memory     = pNewMemory->get();
            m_memory.insert( { mpo, std::move( pNewMemory ) } );
            return memory;
        }
    }

    void release( const mega::MPO& mpo )
    {
        auto iFind = m_memory.find( mpo );
        if ( iFind != m_memory.end() )
        {
            m_memory.erase( iFind );
        }
    }

private:
    SharedMemoryMap m_memory;
};

} // namespace mega::service

#endif // SHARED_MEMORY_21_OCT_2022
