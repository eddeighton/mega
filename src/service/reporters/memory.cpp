
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

#ifndef GUARD_2023_October_19_memory
#define GUARD_2023_October_19_memory

#include "service/reporters.hpp"

namespace mega::reports
{
/*
MemoryReporter::MemoryReporter( mega::runtime::MemoryManager& memoryManager, runtime::MPODatabase& database )
    : m_memoryManager( memoryManager )
    , m_database( database )
{
}

const mega::reports::ReporterID MemoryReporter::ID = "memory";

mega::reports::Container MemoryReporter::generate( const mega::reports::URL& url )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Table test{ { { "Type Name"s },
                  { "Allocation ID"s },
                  { "Heap Address"s },
                  { "Network Address"s },
                  { "SizeAlignment"s },
                  { "Ref Count"s },
                  { "Lock Cycle"s } } };

    for( auto i = m_memoryManager.begin(), iEnd = m_memoryManager.end(); i != iEnd; ++i )
    {
        const reference& ref = i->first;
        VERIFY_RTE( ref.isHeapAddress() );

        auto pHeader = reinterpret_cast< runtime::ObjectHeader* >( ref.getHeap() );

        // clang-format off
        test.m_rows.push_back(
            ContainerVector
            {
                Line( { m_database.getConcreteFullType( ref.getType() ) } ),
                Line( { std::to_string( ref.getAllocationID() ) } ),
                Line( { ref } ),
                Line( { ref.getNetworkAddress() } ),
                Line( { pHeader->m_pAllocator->getSizeAlignment() } ),
                Line( { std::to_string( ref.getRefCount() ) } ),
                Line( { std::to_string( ref.getLockCycle() ) } )
            }
        );
        // clang-format on
    }

    return test;
}
*/
} // namespace mega::reports

#endif // GUARD_2023_October_19_memory
