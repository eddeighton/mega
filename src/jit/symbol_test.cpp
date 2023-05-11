
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

#include "mega/reference.hpp"
#include "mega/relation_id.hpp"
#include "mega/invocation_id.hpp"
#include "mega/write_operation.hpp"

#if defined( _WIN32 )
#    define EXPORT_TEST_SYMBOL __declspec( dllexport )
#elif defined( __GNUC__ )
#    define EXPORT_TEST_SYMBOL __attribute__( ( visibility( "default" ) ) )
#endif

EXPORT_TEST_SYMBOL void foobar_None            ( ) {}
EXPORT_TEST_SYMBOL void foobar_VStar           ( void* ) {}
EXPORT_TEST_SYMBOL void foobar_VStar_VStar     ( void*, void* ) {}
EXPORT_TEST_SYMBOL void foobar_Ref             ( mega::reference ) {}
EXPORT_TEST_SYMBOL void foobar_Ref_Ref         ( mega::reference, mega::reference) {}
EXPORT_TEST_SYMBOL void foobar_Ref_VStar       ( mega::reference, void*) {}
EXPORT_TEST_SYMBOL void foobar_Ref_CVStar      ( mega::reference, const void* ) {}
EXPORT_TEST_SYMBOL void foobar_Ref_Wo_RefCR    ( mega::reference, WriteOperation, const mega::reference& ) {}
EXPORT_TEST_SYMBOL void foobar_Ref_Wo_CVStar   ( mega::reference, WriteOperation, const void* ) {}
EXPORT_TEST_SYMBOL void foobar_ID_VStar_VStar  ( mega::TypeID, void*, void* ) {}
EXPORT_TEST_SYMBOL void foobar_Ref_VStar_U64   ( mega::reference, void*, mega::U64 ) {}

/*
Dump of file symbol_test.dll

File Type: DLL

  Section contains the following exports for symbol_test.dll

    00000000 characteristics
    FFFFFFFF time date stamp
        0.00 version
           1 ordinal base
          11 number of functions
          11 number of names

    ordinal hint RVA      name

          1    0 00001000 ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
          2    1 00001000 ?foobar_None@@YAXXZ = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
          3    2 00001000 ?foobar_Ref@@YAXVreference@mega@@@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
          4    3 00001000 ?foobar_Ref_CVStar@@YAXVreference@mega@@PEBX@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
          5    4 00001000 ?foobar_Ref_Ref@@YAXVreference@mega@@0@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
          6    5 00001000 ?foobar_Ref_VStar@@YAXVreference@mega@@PEAX@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
          7    6 00001000 ?foobar_Ref_VStar_U64@@YAXVreference@mega@@PEAX_K@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
          8    7 00001000 ?foobar_Ref_Wo_CVStar@@YAXVreference@mega@@W4WriteOperation@@PEBX@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
          9    8 00001000 ?foobar_Ref_Wo_RefCR@@YAXVreference@mega@@W4WriteOperation@@AEBV12@@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
         10    9 00001000 ?foobar_VStar@@YAXPEAX@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))
         11    A 00001000 ?foobar_VStar_VStar@@YAXPEAX0@Z = ?foobar_ID_VStar_VStar@@YAXVTypeID@mega@@PEAX1@Z (void __cdecl foobar_ID_VStar_VStar(class mega::TypeID,void *,void *))

  Summary

        1000 .data
        1000 .pdata
        1000 .rdata
        1000 .reloc
        1000 .rsrc
        1000 .text

*/
