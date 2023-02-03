
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

#ifndef GUARD_2023_February_03_reference_limits
#define GUARD_2023_February_03_reference_limits

#include "reference.hpp"

#include <limits>

namespace mega
{
static constexpr auto max_net_ref = mega::reference{
    mega::TypeInstance{ std::numeric_limits< mega::Instance >::max(), std::numeric_limits< mega::TypeID >::max() },
    mega::MPO{ std::numeric_limits< mega::MachineID >::max(), std::numeric_limits< mega::ProcessID >::max(),
               std::numeric_limits< mega::OwnerID >::max() },
    std::numeric_limits< mega::ObjectID >::max() };

static constexpr auto min_net_ref = mega::reference{
    mega::TypeInstance{ std::numeric_limits< mega::Instance >::min(), std::numeric_limits< mega::TypeID >::min() },
    mega::MPO{ std::numeric_limits< mega::MachineID >::min(), std::numeric_limits< mega::ProcessID >::min(),
               std::numeric_limits< mega::OwnerID >::min() },
    std::numeric_limits< mega::ObjectID >::min() };

static const auto max_heap_ref = mega::reference{
    mega::TypeInstance{ std::numeric_limits< mega::Instance >::max(), std::numeric_limits< mega::TypeID >::max() },
    0, // std::numeric_limits< mega::OwnerID >::max(),
    reinterpret_cast< void* >( std::numeric_limits< mega::U64 >::max() ) };

static const auto min_heap_ref = mega::reference{
    mega::TypeInstance{ std::numeric_limits< mega::Instance >::min(), std::numeric_limits< mega::TypeID >::min() },
    0, // std::numeric_limits< mega::OwnerID >::min(),
    reinterpret_cast< void* >( std::numeric_limits< mega::U64 >::min() ) };
} // namespace mega

#endif // GUARD_2023_February_03_reference_limits
