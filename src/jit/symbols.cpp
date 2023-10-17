
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

#include "jit/symbol.hpp"

#include "mega/relation_io.hpp"
#include "mega/invocation_io.hpp"

#include "symbol_utils.hpp"

namespace mega::runtime
{
namespace
{
#ifdef _WIN32
//  1    0 00001000 ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z
//  (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *)) 2    1 00001000 ?foobar_None@@YAXXZ =
//  ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void
//  *)) 3    2 00001000 ?foobar_Ref@@YAXVreference@mega@@@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void
//  __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *)) 4    3 00001000
//  ?foobar_Ref_CVStar@@YAXVreference@mega@@PEBX@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl
//  foobar_ID_VStar_VStar(class mega::TypeID,void *,void *)) 5    4 00001000 ?foobar_Ref_Ref@@YAXVreference@mega@@0@Z =
//  ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void
//  *)) 6    5 00001000 ?foobar_Ref_VStar@@YAXVreference@mega@@PEAX@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z
//  (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *)) 7    6 00001000
//  ?foobar_Ref_VStar_U64@@YAXVreference@mega@@PEAX_K@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl
//  foobar_ID_VStar_VStar(class mega::TypeID,void *,void *)) 8    7 00001000
//  ?foobar_Ref_Wo_CVStar@@YAXVreference@mega@@W4WriteOperation@@PEBX@Z =
//  ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void
//  *)) 9    8 00001000 ?foobar_Ref_Wo_RefCR@@YAXVreference@mega@@W4WriteOperation@@AEBV12@@Z =
//  ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void
//  *))
// 10    9 00001000 ?foobar_VStar@@YAXPEAX@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl
// foobar_ID_VStar_VStar(class mega::TypeID,void *,void *)) 11    A 00001000 ?foobar_VStar_VStar@@YAXPEAX0@Z =
// ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void
// *))

const char* toStr( Symbol::Parameters params )
{
    switch( params )
    {
        case Symbol::None:
            return "@@YAXXZ";
        case Symbol::VStar:
            return "@@YAXPEAX@Z";
        case Symbol::VStar_VStar:
            return "@@YAXPEAX0@Z";
        case Symbol::Ref:
            return "@@YAXVreference@mega@@@Z";
        case Symbol::Ref_Ref:
            return "@@YAXVreference@mega@@0@Z";
        case Symbol::Ref_VStar:
            return "@@YAXVreference@mega@@PEAX@Z";
        case Symbol::Ref_VStar_Bool:
            THROW_TODO;
        case Symbol::Ref_CVStar:
            return "@@YAXVreference@mega@@PEBX@Z";
        case Symbol::ID_VStar_VStar:
            return "@@YAXVTypeID@mega@@PEAX1@Z";
        case Symbol::Ref_VStar_U64:
            return "@@YAXVreference@mega@@PEAX_K@Z";
        case Symbol::Ref_U64:
            THROW_TODO;
        case Symbol::Ref_Any:
            THROW_TODO;
        default:
            THROW_RTE( "Unsupported params type" );
    }
}

std::string toSymbolName( const std::string& strName, Symbol::Parameters params )
{
    std::ostringstream os;
    os << "?" << strName << toStr( params );
    return os.str();
}

#elif __gnu_linux__
const char* toStr( Symbol::Parameters params )
{
    // clang-format off
    switch( params )
    {
        case Symbol::None               : return "v";
        case Symbol::VStar              : return "Pv";
        case Symbol::VStar_VStar        : return "PvS_";
        case Symbol::Ref                : return "N4mega9referenceE";
        case Symbol::Ref_Ref            : return "N4mega9referenceES0_";
        case Symbol::Ref_Ref_U64        : return "N4mega9referenceES0_m";
        case Symbol::Ref_VStar          : return "N4mega9referenceEPv";
        case Symbol::Ref_VStar_Bool     : return "N4mega9referenceEPvb";
        case Symbol::Ref_CVStar         : return "N4mega9referenceEPKv";
        case Symbol::ID_VStar_VStar     : return "N4mega6TypeIDEPvS1_";
        case Symbol::Ref_VStar_U64      : return "N4mega9referenceEPvm";
        case Symbol::Ref_U64            : return "N4mega9referenceEm";
        case Symbol::Ref_Any            : return "N4mega9referenceENS_3AnyE";
        case Symbol::RefR               : return "RN4mega9referenceE";
        case Symbol::RefR_U32R          : return "RN4mega9referenceERj";
        case Symbol::RefCR_U32R         : return "KRN4mega9referenceERj";
        case Symbol::RefR_RefR          : return "RN4mega9referenceES1_";
        case Symbol::RefR_VStar         : return "RN4mega9referenceEPv";
        case Symbol::RefR_CVStar        : return "RN4mega9referenceEPKv";
        default:
            THROW_RTE( "Unsupported params type" );
    }
    // clang-format on
}

// // _Z17enumerate_0x10000KRN4mega9referenceERj
//
//
//

std::string toSymbolName( const std::string& strName, Symbol::Parameters params )
{
    std::ostringstream os;
    os << "_Z" << strName.size() << strName << toStr( params );
    return os.str();
}
#else
static_assert( false, "Symbol mangling not supported" );
#endif
} // namespace

Symbol::Symbol( const char* prefix, Symbol::Parameters params )
{
    std::ostringstream osTypeID;
    osTypeID << prefix;
    m_symbolName = toSymbolName( osTypeID.str(), params );
}
Symbol::Symbol( const char* prefix, RelationID relationID, Symbol::Parameters params )
{
    using ::           operator<<;
    std::ostringstream osTypeID;
    osTypeID << prefix << relationID;
    m_symbolName = toSymbolName( osTypeID.str(), params );
}
Symbol::Symbol( const char* prefix, mega::TypeID objectTypeID, Symbol::Parameters params )
{
    using ::           operator<<;
    std::ostringstream osTypeID;
    osTypeID << prefix << printTypeID( objectTypeID );
    m_symbolName = toSymbolName( osTypeID.str(), params );
}
Symbol::Symbol( const mega::InvocationID& invocationID, Symbol::Parameters params )
{
    using ::           operator<<;
    std::ostringstream osTypeID;
    osTypeID << invocationID;
    m_symbolName = toSymbolName( osTypeID.str(), params );
}
Symbol::Symbol( const char* prefix, mega::TypeID objectTypeID, mega::TypeID objectTypeID2 )
{
    using ::           operator<<;
    std::ostringstream osTypeID;
    osTypeID << prefix << printTypeID( objectTypeID ) << '_' << printTypeID( objectTypeID2 );
    m_symbolName = toSymbolName( osTypeID.str(), None );
}
} // namespace mega::runtime
