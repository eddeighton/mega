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

#include "base_task.hpp"

#include "database/model/MemoryStage.hxx"

#include "database/types/sources.hpp"

#include "common/file.hpp"

#include "mega/common.hpp"
#include "mega/allocator.hpp"
#include "mega/default_traits.hpp"

#include <common/stash.hpp>

#include <optional>
#include <vector>
#include <string>

namespace
{
mega::U64 getBitmask32AllocatorSize( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
        case 2:  return sizeof( mega::Bitmask32Allocator< 2 > ); break;
        case 3:  return sizeof( mega::Bitmask32Allocator< 3 > ); break;
        case 4:  return sizeof( mega::Bitmask32Allocator< 4 > ); break;
        case 5:  return sizeof( mega::Bitmask32Allocator< 5 > ); break;
        case 6:  return sizeof( mega::Bitmask32Allocator< 6 > ); break;
        case 7:  return sizeof( mega::Bitmask32Allocator< 7 > ); break;
        case 8:  return sizeof( mega::Bitmask32Allocator< 8 > ); break;
        case 9:  return sizeof( mega::Bitmask32Allocator< 9 > ); break;
        case 10: return sizeof( mega::Bitmask32Allocator< 10 > ); break;
        case 11: return sizeof( mega::Bitmask32Allocator< 11 > ); break;
        case 12: return sizeof( mega::Bitmask32Allocator< 12 > ); break;
        case 13: return sizeof( mega::Bitmask32Allocator< 13 > ); break;
        case 14: return sizeof( mega::Bitmask32Allocator< 14 > ); break;
        case 15: return sizeof( mega::Bitmask32Allocator< 15 > ); break;
        case 16: return sizeof( mega::Bitmask32Allocator< 16 > ); break;
        case 17: return sizeof( mega::Bitmask32Allocator< 17 > ); break;
        case 18: return sizeof( mega::Bitmask32Allocator< 18 > ); break;
        case 19: return sizeof( mega::Bitmask32Allocator< 19 > ); break;
        case 20: return sizeof( mega::Bitmask32Allocator< 20 > ); break;
        case 21: return sizeof( mega::Bitmask32Allocator< 21 > ); break;
        case 22: return sizeof( mega::Bitmask32Allocator< 22 > ); break;
        case 23: return sizeof( mega::Bitmask32Allocator< 23 > ); break;
        case 24: return sizeof( mega::Bitmask32Allocator< 24 > ); break;
        case 25: return sizeof( mega::Bitmask32Allocator< 25 > ); break;
        case 26: return sizeof( mega::Bitmask32Allocator< 26 > ); break;
        case 27: return sizeof( mega::Bitmask32Allocator< 27 > ); break;
        case 28: return sizeof( mega::Bitmask32Allocator< 28 > ); break;
        case 29: return sizeof( mega::Bitmask32Allocator< 29 > ); break;
        case 30: return sizeof( mega::Bitmask32Allocator< 30 > ); break;
        case 31: return sizeof( mega::Bitmask32Allocator< 31 > ); break;
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}
mega::U64 getBitmask32AllocatorAlignment( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
        case 2:  return alignof( mega::Bitmask32Allocator< 2 > ); break;
        case 3:  return alignof( mega::Bitmask32Allocator< 3 > ); break;
        case 4:  return alignof( mega::Bitmask32Allocator< 4 > ); break;
        case 5:  return alignof( mega::Bitmask32Allocator< 5 > ); break;
        case 6:  return alignof( mega::Bitmask32Allocator< 6 > ); break;
        case 7:  return alignof( mega::Bitmask32Allocator< 7 > ); break;
        case 8:  return alignof( mega::Bitmask32Allocator< 8 > ); break;
        case 9:  return alignof( mega::Bitmask32Allocator< 9 > ); break;
        case 10: return alignof( mega::Bitmask32Allocator< 10 > ); break;
        case 11: return alignof( mega::Bitmask32Allocator< 11 > ); break;
        case 12: return alignof( mega::Bitmask32Allocator< 12 > ); break;
        case 13: return alignof( mega::Bitmask32Allocator< 13 > ); break;
        case 14: return alignof( mega::Bitmask32Allocator< 14 > ); break;
        case 15: return alignof( mega::Bitmask32Allocator< 15 > ); break;
        case 16: return alignof( mega::Bitmask32Allocator< 16 > ); break;
        case 17: return alignof( mega::Bitmask32Allocator< 17 > ); break;
        case 18: return alignof( mega::Bitmask32Allocator< 18 > ); break;
        case 19: return alignof( mega::Bitmask32Allocator< 19 > ); break;
        case 20: return alignof( mega::Bitmask32Allocator< 20 > ); break;
        case 21: return alignof( mega::Bitmask32Allocator< 21 > ); break;
        case 22: return alignof( mega::Bitmask32Allocator< 22 > ); break;
        case 23: return alignof( mega::Bitmask32Allocator< 23 > ); break;
        case 24: return alignof( mega::Bitmask32Allocator< 24 > ); break;
        case 25: return alignof( mega::Bitmask32Allocator< 25 > ); break;
        case 26: return alignof( mega::Bitmask32Allocator< 26 > ); break;
        case 27: return alignof( mega::Bitmask32Allocator< 27 > ); break;
        case 28: return alignof( mega::Bitmask32Allocator< 28 > ); break;
        case 29: return alignof( mega::Bitmask32Allocator< 29 > ); break;
        case 30: return alignof( mega::Bitmask32Allocator< 30 > ); break;
        case 31: return alignof( mega::Bitmask32Allocator< 31 > ); break;
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}

mega::U64 getBitmask64AllocatorSize( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
        case 32: return sizeof( mega::Bitmask64Allocator< 32 > ); break;
        case 33: return sizeof( mega::Bitmask64Allocator< 33 > ); break;
        case 34: return sizeof( mega::Bitmask64Allocator< 34 > ); break;
        case 35: return sizeof( mega::Bitmask64Allocator< 35 > ); break;
        case 36: return sizeof( mega::Bitmask64Allocator< 36 > ); break;
        case 37: return sizeof( mega::Bitmask64Allocator< 37 > ); break;
        case 38: return sizeof( mega::Bitmask64Allocator< 38 > ); break;
        case 39: return sizeof( mega::Bitmask64Allocator< 39 > ); break;
        case 40: return sizeof( mega::Bitmask64Allocator< 40 > ); break;
        case 41: return sizeof( mega::Bitmask64Allocator< 41 > ); break;
        case 42: return sizeof( mega::Bitmask64Allocator< 42 > ); break;
        case 43: return sizeof( mega::Bitmask64Allocator< 43 > ); break;
        case 44: return sizeof( mega::Bitmask64Allocator< 44 > ); break;
        case 45: return sizeof( mega::Bitmask64Allocator< 45 > ); break;
        case 46: return sizeof( mega::Bitmask64Allocator< 46 > ); break;
        case 47: return sizeof( mega::Bitmask64Allocator< 47 > ); break;
        case 48: return sizeof( mega::Bitmask64Allocator< 48 > ); break;
        case 49: return sizeof( mega::Bitmask64Allocator< 49 > ); break;
        case 50: return sizeof( mega::Bitmask64Allocator< 50 > ); break;
        case 51: return sizeof( mega::Bitmask64Allocator< 51 > ); break;
        case 52: return sizeof( mega::Bitmask64Allocator< 52 > ); break;
        case 53: return sizeof( mega::Bitmask64Allocator< 53 > ); break;
        case 54: return sizeof( mega::Bitmask64Allocator< 54 > ); break;
        case 55: return sizeof( mega::Bitmask64Allocator< 55 > ); break;
        case 56: return sizeof( mega::Bitmask64Allocator< 56 > ); break;
        case 57: return sizeof( mega::Bitmask64Allocator< 57 > ); break;
        case 58: return sizeof( mega::Bitmask64Allocator< 58 > ); break;
        case 59: return sizeof( mega::Bitmask64Allocator< 59 > ); break;
        case 60: return sizeof( mega::Bitmask64Allocator< 60 > ); break;
        case 61: return sizeof( mega::Bitmask64Allocator< 61 > ); break;
        case 62: return sizeof( mega::Bitmask64Allocator< 62 > ); break;
        case 63: return sizeof( mega::Bitmask64Allocator< 63 > ); break;
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}

mega::U64 getBitmask64AllocatorAlignment( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
        case 32: return alignof( mega::Bitmask64Allocator< 32 > ); break;
        case 33: return alignof( mega::Bitmask64Allocator< 33 > ); break;
        case 34: return alignof( mega::Bitmask64Allocator< 34 > ); break;
        case 35: return alignof( mega::Bitmask64Allocator< 35 > ); break;
        case 36: return alignof( mega::Bitmask64Allocator< 36 > ); break;
        case 37: return alignof( mega::Bitmask64Allocator< 37 > ); break;
        case 38: return alignof( mega::Bitmask64Allocator< 38 > ); break;
        case 39: return alignof( mega::Bitmask64Allocator< 39 > ); break;
        case 40: return alignof( mega::Bitmask64Allocator< 40 > ); break;
        case 41: return alignof( mega::Bitmask64Allocator< 41 > ); break;
        case 42: return alignof( mega::Bitmask64Allocator< 42 > ); break;
        case 43: return alignof( mega::Bitmask64Allocator< 43 > ); break;
        case 44: return alignof( mega::Bitmask64Allocator< 44 > ); break;
        case 45: return alignof( mega::Bitmask64Allocator< 45 > ); break;
        case 46: return alignof( mega::Bitmask64Allocator< 46 > ); break;
        case 47: return alignof( mega::Bitmask64Allocator< 47 > ); break;
        case 48: return alignof( mega::Bitmask64Allocator< 48 > ); break;
        case 49: return alignof( mega::Bitmask64Allocator< 49 > ); break;
        case 50: return alignof( mega::Bitmask64Allocator< 50 > ); break;
        case 51: return alignof( mega::Bitmask64Allocator< 51 > ); break;
        case 52: return alignof( mega::Bitmask64Allocator< 52 > ); break;
        case 53: return alignof( mega::Bitmask64Allocator< 53 > ); break;
        case 54: return alignof( mega::Bitmask64Allocator< 54 > ); break;
        case 55: return alignof( mega::Bitmask64Allocator< 55 > ); break;
        case 56: return alignof( mega::Bitmask64Allocator< 56 > ); break;
        case 57: return alignof( mega::Bitmask64Allocator< 57 > ); break;
        case 58: return alignof( mega::Bitmask64Allocator< 58 > ); break;
        case 59: return alignof( mega::Bitmask64Allocator< 59 > ); break;
        case 60: return alignof( mega::Bitmask64Allocator< 60 > ); break;
        case 61: return alignof( mega::Bitmask64Allocator< 61 > ); break;
        case 62: return alignof( mega::Bitmask64Allocator< 62 > ); break;
        case 63: return alignof( mega::Bitmask64Allocator< 63 > ); break;
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}

mega::U64 getRingAllocatorSize( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
        case 64:  return sizeof(    mega::RingAllocator< mega::Instance, 64 >   ); break;
        case 80:  return sizeof(    mega::RingAllocator< mega::Instance, 80 >   ); break;
        case 96:  return sizeof(    mega::RingAllocator< mega::Instance, 96 >   ); break;
        case 112: return sizeof(    mega::RingAllocator< mega::Instance, 112 >  ); break;
        case 128: return sizeof(    mega::RingAllocator< mega::Instance, 128 >  ); break;
        case 144: return sizeof(    mega::RingAllocator< mega::Instance, 144 >  ); break;
        case 160: return sizeof(    mega::RingAllocator< mega::Instance, 160 >  ); break;
        case 176: return sizeof(    mega::RingAllocator< mega::Instance, 176 >  ); break;
        case 192: return sizeof(    mega::RingAllocator< mega::Instance, 192 >  ); break;
        case 208: return sizeof(    mega::RingAllocator< mega::Instance, 208 >  ); break;
        case 224: return sizeof(    mega::RingAllocator< mega::Instance, 224 >  ); break;
        case 240: return sizeof(    mega::RingAllocator< mega::Instance, 240 >  ); break;
        case 256: return sizeof(    mega::RingAllocator< mega::Instance, 256 >  ); break;
        case 272: return sizeof(    mega::RingAllocator< mega::Instance, 272 >  ); break;
        case 288: return sizeof(    mega::RingAllocator< mega::Instance, 288 >  ); break;
        case 304: return sizeof(    mega::RingAllocator< mega::Instance, 304 >  ); break;
        case 320: return sizeof(    mega::RingAllocator< mega::Instance, 320 >  ); break;
        case 336: return sizeof(    mega::RingAllocator< mega::Instance, 336 >  ); break;
        case 352: return sizeof(    mega::RingAllocator< mega::Instance, 352 >  ); break;
        case 368: return sizeof(    mega::RingAllocator< mega::Instance, 368 >  ); break;
        case 384: return sizeof(    mega::RingAllocator< mega::Instance, 384 >  ); break;
        case 400: return sizeof(    mega::RingAllocator< mega::Instance, 400 >  ); break;
        case 416: return sizeof(    mega::RingAllocator< mega::Instance, 416 >  ); break;
        case 432: return sizeof(    mega::RingAllocator< mega::Instance, 432 >  ); break;
        case 448: return sizeof(    mega::RingAllocator< mega::Instance, 448 >  ); break;
        case 464: return sizeof(    mega::RingAllocator< mega::Instance, 464 >  ); break;
        case 480: return sizeof(    mega::RingAllocator< mega::Instance, 480 >  ); break;
        case 496: return sizeof(    mega::RingAllocator< mega::Instance, 496 >  ); break;
        case 512: return sizeof(    mega::RingAllocator< mega::Instance, 512 >  ); break;
        case 528: return sizeof(    mega::RingAllocator< mega::Instance, 528 >  ); break;
        case 544: return sizeof(    mega::RingAllocator< mega::Instance, 544 >  ); break;
        case 560: return sizeof(    mega::RingAllocator< mega::Instance, 560 >  ); break;
        case 576: return sizeof(    mega::RingAllocator< mega::Instance, 576 >  ); break;
        case 592: return sizeof(    mega::RingAllocator< mega::Instance, 592 >  ); break;
        case 608: return sizeof(    mega::RingAllocator< mega::Instance, 608 >  ); break;
        case 624: return sizeof(    mega::RingAllocator< mega::Instance, 624 >  ); break;
        case 640: return sizeof(    mega::RingAllocator< mega::Instance, 640 >  ); break;
        case 656: return sizeof(    mega::RingAllocator< mega::Instance, 656 >  ); break;
        case 672: return sizeof(    mega::RingAllocator< mega::Instance, 672 >  ); break;
        case 688: return sizeof(    mega::RingAllocator< mega::Instance, 688 >  ); break;
        case 704: return sizeof(    mega::RingAllocator< mega::Instance, 704 >  ); break;
        case 720: return sizeof(    mega::RingAllocator< mega::Instance, 720 >  ); break;
        case 736: return sizeof(    mega::RingAllocator< mega::Instance, 736 >  ); break;
        case 752: return sizeof(    mega::RingAllocator< mega::Instance, 752 >  ); break;
        case 768: return sizeof(    mega::RingAllocator< mega::Instance, 768 >  ); break;
        case 784: return sizeof(    mega::RingAllocator< mega::Instance, 784 >  ); break;
        case 800: return sizeof(    mega::RingAllocator< mega::Instance, 800 >  ); break;
        case 816: return sizeof(    mega::RingAllocator< mega::Instance, 816 >  ); break;
        case 832: return sizeof(    mega::RingAllocator< mega::Instance, 832 >  ); break;
        case 848: return sizeof(    mega::RingAllocator< mega::Instance, 848 >  ); break;
        case 864: return sizeof(    mega::RingAllocator< mega::Instance, 864 >  ); break;
        case 880: return sizeof(    mega::RingAllocator< mega::Instance, 880 >  ); break;
        case 896: return sizeof(    mega::RingAllocator< mega::Instance, 896 >  ); break;
        case 912: return sizeof(    mega::RingAllocator< mega::Instance, 912 >  ); break;
        case 928: return sizeof(    mega::RingAllocator< mega::Instance, 928 >  ); break;
        case 944: return sizeof(    mega::RingAllocator< mega::Instance, 944 >  ); break;
        case 960: return sizeof(    mega::RingAllocator< mega::Instance, 960 >  ); break;
        case 976: return sizeof(    mega::RingAllocator< mega::Instance, 976 >  ); break;
        case 992: return sizeof(    mega::RingAllocator< mega::Instance, 992 >  ); break;
        case 1008: return sizeof(   mega::RingAllocator< mega::Instance, 1008 > ); break;
        case 1024: return sizeof(   mega::RingAllocator< mega::Instance, 1024 > ); break;
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}

mega::U64 getRingAllocatorAlignment( mega::U64 szLocalDomainSize )
{
    // clang-format off
    switch( szLocalDomainSize )
    {
        case 64:   return alignof( mega::RingAllocator< mega::Instance, 64 > ); break;
        case 80:   return alignof( mega::RingAllocator< mega::Instance, 80 > ); break;
        case 96:   return alignof( mega::RingAllocator< mega::Instance, 96 > ); break;
        case 112:  return alignof( mega::RingAllocator< mega::Instance, 112 > ); break;
        case 128:  return alignof( mega::RingAllocator< mega::Instance, 128 > ); break;
        case 144:  return alignof( mega::RingAllocator< mega::Instance, 144 > ); break;
        case 160:  return alignof( mega::RingAllocator< mega::Instance, 160 > ); break;
        case 176:  return alignof( mega::RingAllocator< mega::Instance, 176 > ); break;
        case 192:  return alignof( mega::RingAllocator< mega::Instance, 192 > ); break;
        case 208:  return alignof( mega::RingAllocator< mega::Instance, 208 > ); break;
        case 224:  return alignof( mega::RingAllocator< mega::Instance, 224 > ); break;
        case 240:  return alignof( mega::RingAllocator< mega::Instance, 240 > ); break;
        case 256:  return alignof( mega::RingAllocator< mega::Instance, 256 > ); break;
        case 272:  return alignof( mega::RingAllocator< mega::Instance, 272 > ); break;
        case 288:  return alignof( mega::RingAllocator< mega::Instance, 288 > ); break;
        case 304:  return alignof( mega::RingAllocator< mega::Instance, 304 > ); break;
        case 320:  return alignof( mega::RingAllocator< mega::Instance, 320 > ); break;
        case 336:  return alignof( mega::RingAllocator< mega::Instance, 336 > ); break;
        case 352:  return alignof( mega::RingAllocator< mega::Instance, 352 > ); break;
        case 368:  return alignof( mega::RingAllocator< mega::Instance, 368 > ); break;
        case 384:  return alignof( mega::RingAllocator< mega::Instance, 384 > ); break;
        case 400:  return alignof( mega::RingAllocator< mega::Instance, 400 > ); break;
        case 416:  return alignof( mega::RingAllocator< mega::Instance, 416 > ); break;
        case 432:  return alignof( mega::RingAllocator< mega::Instance, 432 > ); break;
        case 448:  return alignof( mega::RingAllocator< mega::Instance, 448 > ); break;
        case 464:  return alignof( mega::RingAllocator< mega::Instance, 464 > ); break;
        case 480:  return alignof( mega::RingAllocator< mega::Instance, 480 > ); break;
        case 496:  return alignof( mega::RingAllocator< mega::Instance, 496 > ); break;
        case 512:  return alignof( mega::RingAllocator< mega::Instance, 512 > ); break;
        case 528:  return alignof( mega::RingAllocator< mega::Instance, 528 > ); break;
        case 544:  return alignof( mega::RingAllocator< mega::Instance, 544 > ); break;
        case 560:  return alignof( mega::RingAllocator< mega::Instance, 560 > ); break;
        case 576:  return alignof( mega::RingAllocator< mega::Instance, 576 > ); break;
        case 592:  return alignof( mega::RingAllocator< mega::Instance, 592 > ); break;
        case 608:  return alignof( mega::RingAllocator< mega::Instance, 608 > ); break;
        case 624:  return alignof( mega::RingAllocator< mega::Instance, 624 > ); break;
        case 640:  return alignof( mega::RingAllocator< mega::Instance, 640 > ); break;
        case 656:  return alignof( mega::RingAllocator< mega::Instance, 656 > ); break;
        case 672:  return alignof( mega::RingAllocator< mega::Instance, 672 > ); break;
        case 688:  return alignof( mega::RingAllocator< mega::Instance, 688 > ); break;
        case 704:  return alignof( mega::RingAllocator< mega::Instance, 704 > ); break;
        case 720:  return alignof( mega::RingAllocator< mega::Instance, 720 > ); break;
        case 736:  return alignof( mega::RingAllocator< mega::Instance, 736 > ); break;
        case 752:  return alignof( mega::RingAllocator< mega::Instance, 752 > ); break;
        case 768:  return alignof( mega::RingAllocator< mega::Instance, 768 > ); break;
        case 784:  return alignof( mega::RingAllocator< mega::Instance, 784 > ); break;
        case 800:  return alignof( mega::RingAllocator< mega::Instance, 800 > ); break;
        case 816:  return alignof( mega::RingAllocator< mega::Instance, 816 > ); break;
        case 832:  return alignof( mega::RingAllocator< mega::Instance, 832 > ); break;
        case 848:  return alignof( mega::RingAllocator< mega::Instance, 848 > ); break;
        case 864:  return alignof( mega::RingAllocator< mega::Instance, 864 > ); break;
        case 880:  return alignof( mega::RingAllocator< mega::Instance, 880 > ); break;
        case 896:  return alignof( mega::RingAllocator< mega::Instance, 896 > ); break;
        case 912:  return alignof( mega::RingAllocator< mega::Instance, 912 > ); break;
        case 928:  return alignof( mega::RingAllocator< mega::Instance, 928 > ); break;
        case 944:  return alignof( mega::RingAllocator< mega::Instance, 944 > ); break;
        case 960:  return alignof( mega::RingAllocator< mega::Instance, 960 > ); break;
        case 976:  return alignof( mega::RingAllocator< mega::Instance, 976 > ); break;
        case 992:  return alignof( mega::RingAllocator< mega::Instance, 992 > ); break;
        case 1008: return alignof( mega::RingAllocator< mega::Instance, 1008 > ); break;
        case 1024: return alignof( mega::RingAllocator< mega::Instance, 1024 > ); break;
        default:
            THROW_RTE( "Unsupported allocator type" );
    }
    // clang-format on
}
} // namespace

namespace mega::compiler
{

template < typename TContext >
inline U64 getSizeTraitSize( const TContext* pInterfaceContext )
{
    using namespace MemoryStage;
    U64                                    allocationSize = 1U;
    std::optional< Interface::SizeTrait* > sizeTraitOpt   = pInterfaceContext->get_size_trait();
    if ( sizeTraitOpt.has_value() )
    {
        allocationSize = sizeTraitOpt.value()->get_size();
        VERIFY_RTE_MSG( allocationSize > 0U, "Invalid size for: " << pInterfaceContext->get_identifier() );
    }
    return allocationSize;
}

class Task_Allocators : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Allocators( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    void createallocators( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                           MemoryStage::Concrete::Context* pContext, U64 szTotalSize,
                           std::vector< MemoryStage::Concrete::Dimensions::Allocator* >& allocatorDimensions );

    struct Parts
    {
        using PartVector = std::vector< MemoryStage::MemoryLayout::Part* >;
        PartVector simpleParts, nonSimpleParts, gpuParts;
    };

    void createParts( MemoryStage::Database& database, MemoryStage::Concrete::Context* pContext, Parts* pParts );
    void createBuffers( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                        MemoryStage::Concrete::Context* pContext, Parts* pParts );

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath concrete = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        const mega::io::CompilationFilePath compilationFile
            = m_environment.MemoryStage_MemoryLayout( m_sourceFilePath );

        start( taskProgress, "Task_Allocators", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( concrete ) } );

        if ( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace MemoryStage;

        Database database( m_environment, m_sourceFilePath );

        Concrete::Root* pConcreteRoot = database.one< Concrete::Root >( m_sourceFilePath );
        for ( Concrete::ContextGroup* pContextGroup : pConcreteRoot->get_children() )
        {
            if ( auto pContext = db_cast< Concrete::Context >( pContextGroup ) )
            {
                std::vector< Concrete::Dimensions::Allocator* > nestedAllocatorDimensions;
                createallocators( database, nullptr, pContext, 1, nestedAllocatorDimensions );
                VERIFY_RTE( nestedAllocatorDimensions.empty() );
            }
        }
        for ( Concrete::ContextGroup* pContextGroup : pConcreteRoot->get_children() )
        {
            if ( auto pContext = db_cast< Concrete::Context >( pContextGroup ) )
            {
                createBuffers( database, nullptr, pContext, nullptr );
            }
        }

        const task::FileHash fileHashCode = database.save_MemoryLayout_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Allocators( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Allocators >( taskArguments, sourceFilePath );
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// createallocators
void Task_Allocators::createallocators(
    MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
    MemoryStage::Concrete::Context* pContext, U64 szTotalSize,
    std::vector< MemoryStage::Concrete::Dimensions::Allocator* >& allocatorDimensions )
{
    using namespace MemoryStage;
    using namespace MemoryStage::Concrete;

    U64 szDomainSize = 0U;
    if ( auto pNamespace = db_cast< Namespace >( pContext ) )
    {
    }
    else if ( auto pAction = db_cast< Action >( pContext ) )
    {
        szDomainSize = getSizeTraitSize( pAction->get_interface_action() );
        szTotalSize *= szDomainSize;

        pAction = database.construct< Action >( Action::Args{ pAction, szDomainSize, szTotalSize } );
    }
    else if ( auto pEvent = db_cast< Event >( pContext ) )
    {
        szDomainSize = getSizeTraitSize( pEvent->get_interface_event() );
        szTotalSize *= szDomainSize;

        pEvent = database.construct< Event >( Event::Args{ pEvent, szDomainSize, szTotalSize } );
    }
    else if ( auto pFunction = db_cast< Function >( pContext ) )
    {
    }
    else if ( auto pObject = db_cast< Object >( pContext ) )
    {
    }
    else if ( auto pLink = db_cast< Link >( pContext ) )
    {
        HyperGraph::Relation* pRelation = pLink->get_link()->get_relation();

        Concrete::Dimensions::LinkReference* pLinkRef = nullptr;
        if ( pRelation->get_source_interface()->get_link_trait()->get_cardinality().maximum().isMany() )
        {
            // range
            pLinkRef = database.construct< Concrete::Dimensions::LinkMany >( Concrete::Dimensions::LinkMany::Args{
                Concrete::Dimensions::LinkReference::Args{ pParentContext, pLink } } );
        }
        else
        {
            // singular
            pLinkRef = database.construct< Concrete::Dimensions::LinkSingle >( Concrete::Dimensions::LinkSingle::Args{
                Concrete::Dimensions::LinkReference::Args{ pParentContext, pLink } } );
        }

        pLink = database.construct< Link >( Link::Args{ pLink, szTotalSize, pLinkRef } );
    }
    else if ( auto pBuffer = db_cast< Buffer >( pContext ) )
    {
        szDomainSize = 1U;
        pBuffer      = database.construct< Buffer >( Buffer::Args{ pBuffer, szTotalSize } );
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    std::vector< Dimensions::Allocator* > nestedAllocatorDimensions;

    for ( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
    {
        if ( auto pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
        {
            createallocators( database, pContext, pChildContext, szTotalSize, nestedAllocatorDimensions );
        }
    }

    {
        using namespace MemoryStage::Allocators;
        Allocator* pAllocator                = nullptr;
        bool       bCreateAllocatorDimension = true;
        if ( szDomainSize == 0 )
        {
            pAllocator = database.construct< Nothing >( Nothing::Args{ Allocator::Args{ std::nullopt, pContext } } );
            bCreateAllocatorDimension = false;
        }
        else if ( szDomainSize == 1 )
        {
            pAllocator
                = database.construct< Singleton >( Singleton::Args{ Allocator::Args{ pParentContext, pContext } } );
        }
        else if ( szDomainSize <= 32 )
        {
            pAllocator = database.construct< Range32 >(
                Range32::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext } } } );
        }
        else if ( szDomainSize <= 64 )
        {
            pAllocator = database.construct< Range64 >(
                Range64::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext } } } );
        }
        else
        {
            pAllocator = database.construct< RangeAny >(
                RangeAny::Args{ Range::Args{ Allocator::Args{ pParentContext, pContext } } } );
        }
        VERIFY_RTE( pAllocator );

        if ( bCreateAllocatorDimension )
        {
            Dimensions::Allocator* pAllocatorDim = database.construct< Dimensions::Allocator >(
                Dimensions::Allocator::Args{ Dimensions::Allocation::Args{ pContext }, pAllocator } );
            pAllocator->set_dimension( pAllocatorDim );
            allocatorDimensions.push_back( pAllocatorDim );
        }
        else
        {
            pAllocator->set_dimension( std::nullopt );
        }
        pContext = database.construct< Context >( Context::Args{ pContext, pAllocator, nestedAllocatorDimensions } );
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// createParts

static U64 padToAlignment( U64 alignment, U64 offset )
{
    const U64 diff = offset % alignment;
    if ( diff )
    {
        return offset + ( alignment - diff );
    }
    else
    {
        return offset;
    }
}

using namespace MemoryStage;
using namespace MemoryStage::Concrete;

struct SizeAlignment
{
    U64 size = 0U, alignment = 1U;
};

struct PartDimensions
{
    std::vector< Concrete::Dimensions::User* >          user;
    std::vector< Concrete::Dimensions::LinkReference* > link;
    std::vector< Concrete::Dimensions::Allocation* >    alloc;

    bool empty() const { return user.empty() && link.empty() && alloc.empty(); }

    U64 getLocalDomainSize( MemoryStage::Concrete::Context* pContext ) const
    {
        using namespace MemoryStage;

        if ( auto pObject = db_cast< Concrete::Object >( pContext ) )
        {
            return 1;
        }
        else if ( auto pEvent = db_cast< Concrete::Event >( pContext ) )
        {
            return pEvent->get_local_size();
        }
        else if ( auto pAction = db_cast< Concrete::Action >( pContext ) )
        {
            return pAction->get_local_size();
        }
        else if ( auto pLink = db_cast< Concrete::Link >( pContext ) )
        {
            return 1;
        }
        else
        {
            return 1;
        }
    }

    SizeAlignment calculatePartLayout( MemoryStage::Database& database, MemoryLayout::Part* pPart )
    {
        SizeAlignment result;
        for ( auto p : user )
        {
            const U64 szAlign = p->get_interface_dimension()->get_alignment();
            const U64 szSize  = p->get_interface_dimension()->get_size();
            result.alignment  = std::max( result.alignment, szAlign );
            result.size       = padToAlignment( szAlign, result.size );
            database.construct< Concrete::Dimensions::User >(
                Concrete::Dimensions::User::Args{ p, result.size, pPart } );
            result.size += szSize;
        }
        for ( auto p : link )
        {
            p->set_part( pPart );

            if ( db_cast< Concrete::Dimensions::LinkSingle >( p ) )
            {
                const U64 szAlign = alignof( mega::reference );
                const U64 szSize  = sizeof( mega::reference );
                result.alignment  = std::max( result.alignment, szAlign );
                result.size       = padToAlignment( szAlign, result.size );
                p->set_offset( result.size );
                result.size += szSize;
            }
            else if ( db_cast< Concrete::Dimensions::LinkMany >( p ) )
            {
                const U64 szAlign = mega::DimensionTraits< mega::ReferenceVector >::Alignment;
                const U64 szSize  = mega::DimensionTraits< mega::ReferenceVector >::Size;
                result.alignment  = std::max( result.alignment, szAlign );
                result.size       = padToAlignment( szAlign, result.size );
                p->set_offset( result.size );
                result.size += szSize;
                static_assert( mega::DimensionTraits< mega::ReferenceVector >::Size == 32U,
                               "Something is wrong with mega::DimensionTraits< mega::ReferenceVector >::Size" );
            }
            else
            {
                THROW_RTE( "Unknown link reference type" );
            }
        }
        for ( Concrete::Dimensions::Allocation* p : alloc )
        {
            p->set_part( pPart );

            auto pAllocationDimension = db_cast< Concrete::Dimensions::Allocator >( p );
            VERIFY_RTE( pAllocationDimension );

            if ( auto pAlloc = db_cast< Allocators::Nothing >( pAllocationDimension->get_allocator() ) )
            {
                THROW_RTE( "Unreachable" );
            }
            else if ( auto pAlloc = db_cast< Allocators::Singleton >( pAllocationDimension->get_allocator() ) )
            {
                const U64 szAlign = alignof( bool );
                const U64 szSize  = sizeof( bool );
                result.alignment  = std::max( result.alignment, szAlign );
                result.size       = padToAlignment( szAlign, result.size );
                p->set_offset( result.size );
                result.size += szSize;
            }
            else if ( auto pAlloc = db_cast< Allocators::Range32 >( pAllocationDimension->get_allocator() ) )
            {
                const U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );
                const U64 szSize            = getBitmask32AllocatorSize( szLocalDomainSize );
                const U64 szAlign           = getBitmask32AllocatorAlignment( szLocalDomainSize );
                result.alignment            = std::max( result.alignment, szAlign );
                result.size                 = padToAlignment( szAlign, result.size );
                p->set_offset( result.size );
                result.size += szSize;
            }
            else if ( auto pAlloc = db_cast< Allocators::Range64 >( pAllocationDimension->get_allocator() ) )
            {
                const U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );
                const U64 szSize            = getBitmask64AllocatorSize( szLocalDomainSize );
                const U64 szAlign           = getBitmask64AllocatorAlignment( szLocalDomainSize );
                result.alignment            = std::max( result.alignment, szAlign );
                result.size                 = padToAlignment( szAlign, result.size );
                p->set_offset( result.size );
                result.size += szSize;
            }
            else if ( auto pAlloc = db_cast< Allocators::RangeAny >( pAllocationDimension->get_allocator() ) )
            {
                const U64 szLocalDomainSize = getLocalDomainSize( pAlloc->get_allocated_context() );
                const U64 szSize            = getRingAllocatorSize( szLocalDomainSize );
                const U64 szAlign           = getRingAllocatorAlignment( szLocalDomainSize );
                result.alignment            = std::max( result.alignment, szAlign );
                result.size                 = padToAlignment( szAlign, result.size );
                p->set_offset( result.size );
                result.size += szSize;
            }
            else
            {
                THROW_RTE( "Unknown allocator type" );
            }
        }

        // ensure the entire part is padded to its own alignment to allow array of the part
        result.size = padToAlignment( result.alignment, result.size );
        return result;
    }

    MemoryLayout::Part* createPart( MemoryStage::Database& database, MemoryStage::Concrete::Context* pContext,
                                    U64 szTotalDomainSize )
    {
        MemoryLayout::Part* pContextPart = database.construct< MemoryLayout::Part >(
            MemoryLayout::Part::Args{ szTotalDomainSize, pContext, user, link, alloc } );
        const SizeAlignment sizeAlignment = calculatePartLayout( database, pContextPart );
        pContextPart->set_size( sizeAlignment.size );
        pContextPart->set_alignment( sizeAlignment.alignment );
        return pContextPart;
    }
};

void Task_Allocators::createParts( MemoryStage::Database& database, Concrete::Context* pContext, Parts* pParts )
{
    using namespace MemoryStage;
    using namespace MemoryStage::Concrete;

    PartDimensions simple, nonSimple, gpu;

    U64 szTotalDomainSize;
    {
        if ( Concrete::Namespace* pNamespace = db_cast< Concrete::Namespace >( pContext ) )
        {
            szTotalDomainSize = 1U;
            for ( Concrete::Dimensions::User* pDim : pNamespace->get_dimensions() )
            {
                if ( pDim->get_interface_dimension()->get_simple() )
                {
                    simple.user.push_back( pDim );
                }
                else
                {
                    nonSimple.user.push_back( pDim );
                }
            }
        }
        else if ( Concrete::Object* pObject = db_cast< Concrete::Object >( pContext ) )
        {
            szTotalDomainSize = 1U;
            for ( Concrete::Dimensions::User* pDim : pObject->get_dimensions() )
            {
                if ( pDim->get_interface_dimension()->get_simple() )
                {
                    simple.user.push_back( pDim );
                }
                else
                {
                    nonSimple.user.push_back( pDim );
                }
            }
        }
        else if ( Concrete::Action* pAction = db_cast< Concrete::Action >( pContext ) )
        {
            szTotalDomainSize = pAction->get_total_size();
            for ( Concrete::Dimensions::User* pDim : pAction->get_dimensions() )
            {
                if ( pDim->get_interface_dimension()->get_simple() )
                {
                    simple.user.push_back( pDim );
                }
                else
                {
                    nonSimple.user.push_back( pDim );
                }
            }
        }
        else if ( Concrete::Event* pEvent = db_cast< Concrete::Event >( pContext ) )
        {
            szTotalDomainSize = pEvent->get_total_size();

            for ( Concrete::Dimensions::User* pDim : pEvent->get_dimensions() )
            {
                if ( pDim->get_interface_dimension()->get_simple() )
                {
                    simple.user.push_back( pDim );
                }
                else
                {
                    nonSimple.user.push_back( pDim );
                }
            }
        }
        else if ( Concrete::Link* pLink = db_cast< Concrete::Link >( pContext ) )
        {
            szTotalDomainSize = pLink->get_total_size();
            simple.link.push_back( pLink->get_link_reference() );
        }
        else if ( Concrete::Buffer* pBuffer = db_cast< Concrete::Buffer >( pContext ) )
        {
            szTotalDomainSize = pBuffer->get_total_size();
        }
        else if ( Concrete::Function* pFunction = db_cast< Concrete::Function >( pContext ) )
        {
            szTotalDomainSize = 1U;
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    for ( Concrete::Dimensions::Allocation* pAllocation : pContext->get_allocation_dimensions() )
    {
        simple.alloc.push_back( pAllocation );
    }

    if ( !simple.empty() )
    {
        MemoryLayout::Part* pContextPart = simple.createPart( database, pContext, szTotalDomainSize );
        pParts->simpleParts.push_back( pContextPart );
    }
    if ( !nonSimple.empty() )
    {
        MemoryLayout::Part* pContextPart = nonSimple.createPart( database, pContext, szTotalDomainSize );
        pParts->nonSimpleParts.push_back( pContextPart );
    }
    if ( !gpu.empty() )
    {
        MemoryLayout::Part* pContextPart = gpu.createPart( database, pContext, szTotalDomainSize );
        pParts->gpuParts.push_back( pContextPart );
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// createBuffers
void Task_Allocators::createBuffers( MemoryStage::Database& database, MemoryStage::Concrete::Context* pParentContext,
                                     MemoryStage::Concrete::Context* pContext, Parts* pParts )
{
    using namespace MemoryStage;
    using namespace MemoryStage::Concrete;

    if ( Object* pObject = db_cast< Object >( pContext ) )
    {
        Parts parts;
        {
            createParts( database, pObject, &parts );
            for ( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
            {
                if ( Concrete::Context* pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
                {
                    createBuffers( database, pContext, pChildContext, &parts );
                }
            }
        }

        std::vector< MemoryLayout::Buffer* > objectBuffers;
        if ( !parts.simpleParts.empty() )
        {
            U64 szOffset    = 0U;
            U64 szAlignment = 1U;
            for ( auto p : parts.simpleParts )
            {
                szOffset = padToAlignment( p->get_alignment(), szOffset );
                p->set_offset( szOffset );
                szOffset += ( p->get_size() * p->get_total_domain_size() );
                szAlignment = std::max( szAlignment, p->get_alignment() );
            }

            MemoryLayout::SimpleBuffer* pSimpleBuffer
                = database.construct< MemoryLayout::SimpleBuffer >( MemoryLayout::SimpleBuffer::Args{
                    MemoryLayout::Buffer::Args{ parts.simpleParts, szOffset, szAlignment } } );
            objectBuffers.push_back( pSimpleBuffer );
        }
        if ( !parts.nonSimpleParts.empty() )
        {
            U64 szOffset    = 0U;
            U64 szAlignment = 1U;
            for ( auto p : parts.nonSimpleParts )
            {
                szOffset = padToAlignment( p->get_alignment(), szOffset );
                p->set_offset( szOffset );
                szOffset += ( p->get_size() * p->get_total_domain_size() );
                szAlignment = std::max( szAlignment, p->get_alignment() );
            }

            MemoryLayout::NonSimpleBuffer* pNonSimpleBuffer
                = database.construct< MemoryLayout::NonSimpleBuffer >( MemoryLayout::NonSimpleBuffer::Args{
                    MemoryLayout::Buffer::Args{ parts.nonSimpleParts, szOffset, szAlignment } } );
            objectBuffers.push_back( pNonSimpleBuffer );
        }
        if ( !parts.gpuParts.empty() )
        {
            U64 szOffset    = 0U;
            U64 szAlignment = 1U;
            for ( auto p : parts.gpuParts )
            {
                szOffset = padToAlignment( p->get_alignment(), szOffset );
                p->set_offset( szOffset );
                szOffset += ( p->get_size() * p->get_total_domain_size() );
                szAlignment = std::max( szAlignment, p->get_alignment() );
            }

            MemoryLayout::GPUBuffer* pGPUBuffer = database.construct< MemoryLayout::GPUBuffer >(
                MemoryLayout::GPUBuffer::Args{ MemoryLayout::Buffer::Args{ parts.gpuParts, szOffset, szAlignment } } );
            objectBuffers.push_back( pGPUBuffer );
        }

        pObject = database.construct< Object >( Object::Args{ pObject, objectBuffers } );
    }
    else
    {
        if ( pParts )
        {
            if ( auto pNamespace = db_cast< Namespace >( pContext ) )
            {
            }
            else if ( auto pAction = db_cast< Action >( pContext ) )
            {
                createParts( database, pAction, pParts );
            }
            else if ( auto pEvent = db_cast< Event >( pContext ) )
            {
                createParts( database, pEvent, pParts );
            }
            else if ( auto pFunction = db_cast< Function >( pContext ) )
            {
            }
            else if ( Link* pLink = db_cast< Link >( pContext ) )
            {
                createParts( database, pLink, pParts );
            }
            else if ( auto pBuffer = db_cast< Buffer >( pContext ) )
            {
            }
            else
            {
                THROW_RTE( "Unknown context type" );
            }
        }

        for ( Concrete::ContextGroup* pContextGroup : pContext->get_children() )
        {
            if ( auto pChildContext = db_cast< Concrete::Context >( pContextGroup ) )
            {
                createBuffers( database, pContext, pChildContext, pParts );
            }
        }
    }
}

} // namespace mega::compiler
