
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

#ifndef GUARD_2022_October_21_shared_header
#define GUARD_2022_October_21_shared_header

#include "mega/native_types.hpp"
#include "mega/reference.hpp"

namespace mega
{

struct SharedHeader
{
    void*     m_heap[ MAX_PROCESS_PER_MACHINE ] = { nullptr };
    TimeStamp m_timestamp                       = 0U;
};

inline SharedHeader& getSharedHeader( void* pMemory ) { return *reinterpret_cast< SharedHeader* >( pMemory ); }

inline void* getHeap( U8 processID, SharedHeader& sharedHeader ) { return sharedHeader.m_heap[ processID ]; }

inline void setHeap( U8 processID, SharedHeader& sharedHeader, void* pHeap )
{
    sharedHeader.m_heap[ processID ] = pHeap;
}

} // namespace mega

#endif // GUARD_2022_October_21_shared_header
