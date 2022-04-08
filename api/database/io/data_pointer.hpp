#ifndef DATA_POINTER_7_APRIL_2022
#define DATA_POINTER_7_APRIL_2022

#include "object_info.hpp"

namespace data
{

    template < typename T >
    class Ptr
    {
    public:
        Ptr( const mega::io::ObjectInfo& objectInfo )
            : m_objectInfo( objectInfo )
        {
        }
        Ptr( T* pObjectPart )
            : m_objectInfo( pObjectPart->getObjectInfo() )
            , m_pObjectPart( pObjectPart )
        {
        }

    private:
        mega::io::ObjectInfo m_objectInfo;
        T*                   m_pObjectPart = nullptr;
    };

} // namespace data

#endif // DATA_POINTER_7_APRIL_2022
