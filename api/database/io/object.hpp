//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef INDEXED_FILE_18_04_2019
#define INDEXED_FILE_18_04_2019

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>

#include <cstddef>
#include <utility>
#include <vector>
#include <map>
#include <memory>

namespace mega
{
namespace io
{
    class Loader;
    class Storer;

    class Object
    {
    public:
        using Array = std::vector< Object* >;

        using Type = std::int32_t;
        enum : Type
        {
            NO_TYPE = ( Type )-1
        };

        using Index = std::int32_t;
        enum : Index
        {
            NO_INDEX = ( Index )-1
        };

        using FileID = std::int32_t;
        enum : FileID
        {
            NO_FILE = ( FileID )-1
        };

        virtual ~Object(){};

        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;

        inline Type   getType() const { return m_type; }
        inline FileID getFileID() const { return m_fileID; }
        inline Index  getIndex() const { return m_index; }

        Object() {}

        Object( Type type, FileID fileID, Index index )
            : m_type( type )
            , m_fileID( fileID )
            , m_index( index )
        {
        }

    private:
        Type   m_type = NO_TYPE;
        FileID m_fileID = NO_FILE;
        Index  m_index = NO_INDEX;
    };
} // namespace io
} // namespace mega

#endif // INDEXED_FILE_18_04_2019
