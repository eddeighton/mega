#ifndef DATA_POINTER_7_APRIL_2022
#define DATA_POINTER_7_APRIL_2022

#include "object_info.hpp"

#include "common/assert_verify.hpp"

#include <variant>

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

        inline T& operator*() const
        {
            return *m_pObjectPart;
        }
        inline T* operator->() const
        {
            return m_pObjectPart;
        }
    private:
        mega::io::ObjectInfo m_objectInfo;
        T*                   m_pObjectPart = nullptr;
    };

    template < typename TTo, typename TFrom >
    inline Ptr< TTo > convert( const Ptr< TFrom >& from )
    {
        THROW_RTE( "Invalid conversion" );
    }

    template < typename TTo, typename... TFromTypes >
    inline Ptr< TTo > convert( const std::variant< TFromTypes... >& from )
    {
        return std::visit( []( auto&& arg ) -> Ptr< TTo > { return convert< TTo >( arg ); }, from );
    }

} // namespace data

#endif // DATA_POINTER_7_APRIL_2022
