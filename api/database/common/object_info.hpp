#ifndef OBJECT_INFO_30_MAR_2022
#define OBJECT_INFO_30_MAR_2022

#include <cstdint>
#include <type_traits>

namespace mega
{
namespace io
{
class Loader;
class Storer;

class ObjectInfo
{
public:
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

    inline Type   getType() const { return m_type; }
    inline FileID getFileID() const { return m_fileID; }
    inline Index  getIndex() const { return m_index; }

    void setFileID( FileID fileID ) { m_fileID = fileID; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_type;
        archive& m_fileID;
        archive& m_index;
    }

    ObjectInfo() {}

    ObjectInfo( Type type, FileID fileID, Index index )
        : m_type( type )
        , m_fileID( fileID )
        , m_index( index )
    {
    }

    inline bool operator<( const ObjectInfo& cmp ) const
    {
        return m_type != cmp.m_type       ? m_type < cmp.m_type
               : m_fileID != cmp.m_fileID ? m_fileID < cmp.m_fileID
               : m_index != cmp.m_index   ? m_index < cmp.m_index
                                          : false;
    }

    inline bool operator==( const ObjectInfo& cmp ) const
    {
        return ( m_type == cmp.m_type ) && ( m_fileID == cmp.m_fileID ) && ( m_index == cmp.m_index );
    }

private:
    Type   m_type   = NO_TYPE;
    FileID m_fileID = NO_FILE;
    Index  m_index  = NO_INDEX;
};

} // namespace io
} // namespace mega

#endif // OBJECT_INFO_30_MAR_2022
