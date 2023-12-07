
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

#ifndef GUARD_2023_December_05_object
#define GUARD_2023_December_05_object

#include "mega/values/compilation/type_id.hpp"
#include "mega/values/compilation/sub_type_instance.hpp"

#include "mega/values/runtime/reference.hpp"
#include "mega/values/runtime/any.hpp"

#include "runtime/function_ptr.hpp"

#include "mega/bin_archive.hpp"
#include "mega/iterator.hpp"

namespace mega::materialiser
{

template < mega::TypeID typeID >
struct Object
{
    void                        constructor( void* pMemory );
    void                        destructor( const mega::reference& ref, void* Buffer, bool LinkReset );
    void                        binaryLoad( void* pMemory, mega::BinLoadArchive& archive );
    void                        binarySave( void* pMemory, mega::BinSaveArchive& archive );
    runtime::TypeErasedFunction getFunction();
    void                        unParent( const mega::reference& ref );
    void                        traverse( Iterator& iterator );
    mega::U64                   linkSize( const mega::reference& ref );
    mega::reference             linkGet( const mega::reference& ref, mega::U64 index );
    mega::Any                   anyRead( const mega::reference& ref );
    void                        anyWrite( const mega::reference& ref, mega::Any value );
    mega::SubTypeInstance       enumerate( const mega::reference& ref, mega::U32& iterator );
};

} // namespace mega::materialiser

#endif // GUARD_2023_December_05_object
