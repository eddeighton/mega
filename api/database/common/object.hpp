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

#include <utility>
#include <vector>
#include <map>
#include <memory>

#include "object_info.hpp"

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

        virtual ~Object();

        virtual void load( Loader& loader ) = 0;
        virtual void load_post( Loader& loader ) = 0;
        virtual void store( Storer& storer ) const = 0;

        Object( const ObjectInfo& objectInfo )
            : m_objectInfo( objectInfo )
        {
        }

        inline ObjectInfo::Type   getType() const { return m_objectInfo.getType(); }
        inline ObjectInfo::FileID getFileID() const { return m_objectInfo.getFileID(); }
        inline ObjectInfo::Index  getIndex() const { return m_objectInfo.getIndex(); }

        const ObjectInfo& getObjectInfo() const { return m_objectInfo; }

    private:
        ObjectInfo m_objectInfo;
    };

    struct CompareIndexedObjects
    {
        inline bool operator()( const Object* pLeft, const Object* pRight ) const
        {
            VERIFY_RTE( pLeft && pRight );
            return ( pLeft->getFileID() != pRight->getFileID() ) ? ( pLeft->getFileID() < pRight->getFileID() ) : ( pLeft->getIndex() < pRight->getIndex() );
        }
    };
    
} // namespace io
} // namespace mega

#endif // INDEXED_FILE_18_04_2019
