
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

#ifndef GUARD_2023_November_27_pointer_h
#define GUARD_2023_November_27_pointer_h

#include "mega/values/runtime.h"
#include "mega/values/compilation/concrete/inline.h"

#include "mega/values/runtime/allocation_id.h"
#include "mega/values/runtime/machine_id.h"
#include "mega/values/runtime/process_id.h"
#include "mega/values/runtime/owner_id.h"

C_RUNTIME_START

#pragma pack( 1 )
typedef struct _c_pointer_heap
{
    void*                       m_heap;   // 8
    c_u8                        _padding; // 1
    c_u8                        m_flags;  // 1
    c_concrete_type_id_instance m_type;   // 6
} c_pointer_heap;
#pragma pack()
#ifndef MEGAJIT
static_assert( sizeof( c_pointer_heap ) == 16U, "Invalid c_pointer_heap Size" );
#endif

#pragma pack( 1 )
typedef struct _c_pointer_net
{
    c_allocation_id             m_allocationID; // 2
    c_machine_id                m_machineID;    // 4
    c_process_id                m_processID;    // 2
    c_owner_id                  m_ownerID;      // 1
    c_u8                        m_flags;        // 1
    c_concrete_type_id_instance m_type;         // 6
} c_pointer_net;
#pragma pack()
#ifndef MEGAJIT
static_assert( sizeof( c_pointer_heap ) == 16U, "Invalid c_pointer_heap Size" );
#endif

#pragma pack( 1 )
typedef union _pointer_union
{
    c_pointer_heap heap;
    c_pointer_net  net;
} c_pointer_union;
#pragma pack()

#ifndef MEGAJIT
static_assert( sizeof( c_pointer_union ) == 16U, "Invalid pointer_union Size" );
#endif

#pragma pack( 1 )
typedef struct _c_pointer
{
    c_pointer_union value;
} c_pointer;
#pragma pack()

#ifndef MEGAJIT
static_assert( sizeof( c_pointer ) == 16U, "Invalid c_pointer Size" );
#endif

C_RUNTIME_END

#endif // GUARD_2023_November_27_pointer_h
