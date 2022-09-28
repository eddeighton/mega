

#ifndef INDEXED_FILE_18_04_2019
#define INDEXED_FILE_18_04_2019

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>

#include <utility>
#include <vector>
#include <map>
#include <memory>

#include "object_info.hpp"

#include "nlohmann/json.hpp"

namespace data
{
    class ObjectPartLoader;
}
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

        virtual bool test_inheritance_pointer( ::data::ObjectPartLoader &loader ) const = 0;
        virtual void set_inheritance_pointer() = 0;
        virtual void load( Loader& loader ) = 0;
        virtual void store( Storer& storer ) const = 0;
        virtual void to_json( nlohmann::json& data ) const = 0;

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
