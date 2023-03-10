
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

#ifndef GUARD_2023_January_13_memory
#define GUARD_2023_January_13_memory

#include "mega/native_types.hpp"

#ifdef _WIN32
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace mega
{

struct SizeAlignment
{
    U64 size      = 0U;
    U64 alignment = 0U;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& size;
        archive& alignment;
    }
};

inline void* malloc( const SizeAlignment& sizeAlignment )
{
#ifdef _WIN32
    return _aligned_malloc( sizeAlignment.size, sizeAlignment.alignment );
#else
    return std::aligned_alloc( sizeAlignment.alignment, sizeAlignment.size );
#endif
}

inline void free( void* p )
{
#ifdef _WIN32
    _aligned_free( p );
#else
    std::free( p );
#endif 
}

class HeapBufferPtr
{
    void* m_pStorage;
public:
    inline HeapBufferPtr( const SizeAlignment& sizeAlignment )
        :   m_pStorage( mega::malloc( sizeAlignment ) )
    {
    }
    inline ~HeapBufferPtr()
    {
        if( m_pStorage )
        {
            mega::free( m_pStorage );
        }
    }
    inline HeapBufferPtr( HeapBufferPtr&& other )
    {
        m_pStorage = other.m_pStorage;
        other.m_pStorage = nullptr;
    }
    HeapBufferPtr( HeapBufferPtr& ) = delete;
    HeapBufferPtr& operator=( HeapBufferPtr& ) = delete;
    inline void* get() const { return m_pStorage; }
};

}

#endif //GUARD_2023_January_13_memory
