
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

#include "mega/values/compilation/interface/type_id.hpp"

#include "mega/values/compilation/concrete/sub_object_id_instance.hpp"

#include "mega/values/runtime/pointer.hpp"
#include "mega/values/runtime/any.hpp"

#include "runtime/function_ptr.hpp"

#include "mega/bin_archive.hpp"
#include "mega/iterator.hpp"

#include <iomanip>

namespace mega::materialiser
{

struct Object
{
    struct Name
    {
        inline std::string operator()( mega::interface::TypeID typeID ) const
        {
            std::ostringstream os;
            os << std::hex << std::setfill( '0' ) << std::setw( 8 ) << typeID.getValue();
            return os.str();
        }
    };

    class Functions
    {
        void                        constructor( void* pMemory );
        void                        destructor( const mega::runtime::Pointer& ref, void* Buffer, bool LinkReset );
        void                        binaryLoad( void* pMemory, mega::BinLoadArchive& archive );
        void                        binarySave( void* pMemory, mega::BinSaveArchive& archive );
        runtime::TypeErasedFunction getFunction();
        void                        unParent( const mega::runtime::Pointer& ref );
        void                        traverse( Iterator& iterator );
        mega::U64                   linkSize( const mega::runtime::Pointer& ref );
        mega::runtime::Pointer      linkGet( const mega::runtime::Pointer& ref, mega::U64 index );
        mega::runtime::Any          anyRead( const mega::runtime::Pointer& ref );
        void                        anyWrite( const mega::runtime::Pointer& ref, mega::runtime::Any value );
        mega::concrete::SubObjectIDInstance enumerate( const mega::runtime::Pointer& ref, mega::U32& iterator );
    };
};

} // namespace mega::materialiser

#endif // GUARD_2023_December_05_object
